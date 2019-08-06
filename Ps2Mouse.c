#include "Type.h"
#include "CH559.h"

#include "System.h"
#include "PinDefine.h"
#include "Protocol.h"

#include "Ps2Mouse.h"

#ifdef COUNT_FACTOR
	#undef COUNT_FACTOR
#endif

#ifdef USE_PLL
	#define COUNT_FACTOR    4
#else
	#define COUNT_FACTOR    3
#endif

#define STANDARD_MOUSE_LEN      3
#define INTELLI_MOUSE_LEN       4

//鼠标数据长度
static UINT8 s_ps2MouseDataLen = STANDARD_MOUSE_LEN;

void DisablePs2MousePort()
{
	CLR_GPIO_BIT(PIN_MS_CLK);
	SET_GPIO_OUTPUT(PIN_MS_CLK);	
}

static BOOL SendPs2MouseByte(UINT8 c)
{
	UINT8 parity = 0x00;
	UINT8 i;
	UINT16 j;
	
	CLR_GPIO_BIT(PIN_MS_CLK);
	SET_GPIO_OUTPUT(PIN_MS_CLK);
	mDelayuS(105);
	
	CLR_GPIO_BIT(PIN_MS_DAT);
	SET_GPIO_OUTPUT(PIN_MS_DAT);		
	mDelayuS(20);

	SET_GPIO_BIT(PIN_MS_CLK);
	SET_GPIO_INPUT(PIN_MS_CLK);
	
	j = 3000 * COUNT_FACTOR;
	do 
	{
		j--;
		if (j == 0)
		{
			return FALSE;
		}
	} while (GET_GPIO_BIT(PIN_MS_CLK)); //等待时钟变低
	
	for (i = 0; i < 8; i++)
	{
		if (c & 0x01)
		{
			SET_GPIO_BIT(PIN_MS_DAT);
		}
		else
		{
			CLR_GPIO_BIT(PIN_MS_DAT);
		}
		
		j = 500 * COUNT_FACTOR;
		do
		{
			j--;
			if (j == 0)
			{
				return FALSE;
			}
		} while (!GET_GPIO_BIT(PIN_MS_CLK));
				
		j = 500 * COUNT_FACTOR;
		do
		{
			j--;
			if (j == 0)
			{
				return FALSE;
			}
		} while (GET_GPIO_BIT(PIN_MS_CLK));
			
		parity ^= (c & 0x01);
		c >>= 1;
	}
	
	parity ^= 0x01;
	//奇校验位
	if (parity & 0x01)
	{
		SET_GPIO_BIT(PIN_MS_DAT);
	}
	else
	{
		CLR_GPIO_BIT(PIN_MS_DAT);
	}
	
	j = 500 * COUNT_FACTOR;
	do
	{
		j--;
		if (j == 0)
		{
			return FALSE;
		}
	} while (!GET_GPIO_BIT(PIN_MS_CLK));
	
	j = 500 * COUNT_FACTOR;
	do 
	{
		j--;
		if (j == 0)
		{
			return FALSE;
		}
	} while (GET_GPIO_BIT(PIN_MS_CLK));
	
	//停止位
	SET_GPIO_BIT(PIN_MS_DAT);
	
	j = 500 * COUNT_FACTOR;
	do
	{
		j--;
		if (j == 0)
		{
			return FALSE;
		}
	} while (!GET_GPIO_BIT(PIN_MS_CLK));
	
	SET_GPIO_INPUT(PIN_MS_DAT);
	
	j = 500 * COUNT_FACTOR;
	do 
	{
		j--;
		if (j == 0)
		{
			return FALSE;
		}
	} while (GET_GPIO_BIT(PIN_MS_CLK));
		
	//等待ack
	j = 500 * COUNT_FACTOR;
	do
	{
		j--;
		if (j == 0)
		{
			return FALSE;
		}
	} while (!GET_GPIO_BIT(PIN_MS_CLK));
	
	return TRUE;
}

static BOOL ReceivePs2MouseByte(UINT8 *pData)
{
	UINT8 dat = 0x00;
	UINT8 parity = 0x00;
	UINT8 parityTemp;
	UINT16 j;
	UINT8 i;

	SET_GPIO_BIT(PIN_MS_CLK);
	SET_GPIO_INPUT(PIN_MS_CLK);
	
	SET_GPIO_BIT(PIN_MS_DAT);
	SET_GPIO_INPUT(PIN_MS_DAT);
	
	if (GET_GPIO_BIT(PIN_MS_DAT))
	{
		return FALSE;
	}
		
	if (!GET_GPIO_BIT(PIN_MS_CLK) && !GET_GPIO_BIT(PIN_MS_DAT))
	{
		j = 500 * COUNT_FACTOR;
		do 
		{
			j--;
			if (j == 0)
			{
				return FALSE;
			}
		} while (!GET_GPIO_BIT(PIN_MS_CLK));
		
		j = 500 * COUNT_FACTOR;
		do
		{
			j--;
			if (j == 0)
			{
				return FALSE;
			}
		} while (GET_GPIO_BIT(PIN_MS_CLK));
			
		//获取数据 
		for (i = 0; i < 8; i++)
		{
			if (!GET_GPIO_BIT(PIN_MS_CLK))
			{
				dat >>= 1;
				if (GET_GPIO_BIT(PIN_MS_DAT))
				{
					dat |= 0x80;
				}
				else
				{
					dat &= 0x7f;
				}
					
				parity ^= GET_GPIO_BIT(PIN_MS_DAT);
					
				j = 500 * COUNT_FACTOR;
				do
				{
					j--;
					if (j == 0)
					{
						return FALSE;
					}
				} while (!GET_GPIO_BIT(PIN_MS_CLK));
					
				j = 500 * COUNT_FACTOR;
				do
				{
					j--;
					if (j == 0)
					{
						return FALSE;
					}
				} while (GET_GPIO_BIT(PIN_MS_CLK));
			}
		}
		
		if (!GET_GPIO_BIT(PIN_MS_CLK))
		{
			parityTemp = GET_GPIO_BIT(PIN_MS_DAT);
			
			j = 500 * COUNT_FACTOR;
			do
			{
				j--;
				if (j == 0)
				{
					return FALSE;
				}
			} while (!GET_GPIO_BIT(PIN_MS_CLK));
			
			j = 500 * COUNT_FACTOR;
			do
			{
				j--;
				if (j == 0)
				{
					return FALSE;
				}
			} while (GET_GPIO_BIT(PIN_MS_CLK));	
		
			parity ^= 0x01;
		}
		else
		{
			return FALSE;
		}
		
		if (parityTemp != parity)
		{
			return FALSE;
		}
		
		if (!GET_GPIO_BIT(PIN_MS_CLK))
		{
			j = 500 * COUNT_FACTOR;
			do
			{
				j--;
				if (j == 0)
				{
					return FALSE;
				}
			} while (!GET_GPIO_BIT(PIN_MS_CLK));
		
			*pData = dat;
	
			return TRUE;
		}
		else
		{
			return FALSE;
		}
	}
	
	return FALSE;
}

UINT8 TransceivePs2MousePort(UINT8 *pPs2)
{
	UINT16 i;
	UINT8 dat;
	UINT8 ptr = 0;
	
	UINT8 j;
	
	UINT8 header;
	
	UINT8 res = PS2_MS_NONE;
	
	i = 800 /** COUNT_FACTOR*/;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		
		if (i == 0)
		{
			return res;
		}
	}
	
	pPs2[ptr] = dat;
	ptr++;
		
	if (dat == 0xaa)
	{
		res = PS2_MS_AA;
			
		return res;
	}
		
	header = dat;
			
	for (j = 0; j < s_ps2MouseDataLen - 1; j++)
	{
		i = 1000 * COUNT_FACTOR;
		while (!ReceivePs2MouseByte(&dat))
		{
			i--;
			if (i == 0)
			{
				res = PS2_MS_INVALID_DATA;
				return res;
			}
		}
			
		pPs2[ptr] = dat;
		ptr++;
	}
		
	if (s_ps2MouseDataLen == STANDARD_MOUSE_LEN)
	{
		pPs2[ptr] = 0;
			
		ptr++;
	}
		
	if (header & 0xc0)
	{
		//有溢出的数据
		res = PS2_MS_INVALID_DATA;
	}
	else
	{
		res = PS2_MS_VALID_DATA;
	}
		
	return res;
}

BOOL InitPs2Mouse()
{
	UINT16 i;
	
	UINT8 dat;
	
	UINT8 k;
	
	//复位
	if (!SendPs2MouseByte(0xff))
	{
		return FALSE;
	}
		
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		if (i == 0)
		{
			return FALSE;
		}
	}
		
	k = 3 * COUNT_FACTOR;
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
			
		if (i == 0)
		{
			k--;
			if (k == 0)
			{
				return FALSE;
			}
		}
	}
		
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		if (i == 0)
		{
			return FALSE;
		}
	}
	
	//0xf3
	if (!SendPs2MouseByte(0xf3))
	{
		return FALSE;
	}
	
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		if (i == 0)
		{
			return FALSE;
		}
	}
	
	//0xc8
	if (!SendPs2MouseByte(0xc8))
	{
		return FALSE;
	}
	
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		if (i == 0)
		{
			return FALSE;
		}
	}	
	
	//0xf3
	if (!SendPs2MouseByte(0xf3))
	{
		return FALSE;
	}
	
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		if (i == 0)
		{
			return FALSE;
		}
	}
	
	//0x64
	if (!SendPs2MouseByte(0x64))
	{
		return FALSE;
	}
	
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		if (i == 0)
		{
			return FALSE;
		}
	}	
	
	//0xf3
	if (!SendPs2MouseByte(0xf3))
	{
		return FALSE;
	}
	
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		if (i == 0)
		{
			return FALSE;
		}
	}
	
	//0x50
	if (!SendPs2MouseByte(0x50))
	{
		return FALSE;
	}
	
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		if (i == 0)
		{
			return FALSE;
		}
	}	
	
	//0xf2
	if (!SendPs2MouseByte(0xf2))
	{
		return FALSE;
	}
	
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		if (i == 0)
		{
			return FALSE;
		}
	}
	
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		if (i == 0)
		{
			return FALSE;
		}
	}
	
	if (dat == 0x00)
	{
		//标准3字节PS2
		s_ps2MouseDataLen = STANDARD_MOUSE_LEN;
	}
	else
	{
		//4字节PS2
		s_ps2MouseDataLen = INTELLI_MOUSE_LEN;
	}
	
	//0xe8
	if (!SendPs2MouseByte(0xe8))
	{
		return FALSE;
	}
	
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		if (i == 0)
		{
			return FALSE;
		}
	}
	
	//0x03
	if (!SendPs2MouseByte(0x03))
	{
		return FALSE;
	}
	
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		if (i == 0)
		{
			return FALSE;
		}
	}
	
	//0xe6
	if (!SendPs2MouseByte(0xe6))
	{
		return FALSE;
	}
	
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		if (i == 0)
		{
			return FALSE;
		}
	}
	
	//0xf3
	if (!SendPs2MouseByte(0xf3))
	{
		return FALSE;
	}
	
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		if (i == 0)
		{
			return FALSE;
		}
	}
	
	//0x28
	if (!SendPs2MouseByte(0x28))
	{
		return FALSE;
	}
	
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		if (i == 0)
		{
			return FALSE;
		}
	}
	
	//0xf4
	if (!SendPs2MouseByte(0xf4))
	{
		return FALSE;
	}
	
	i = 1000 * COUNT_FACTOR;
	while (!ReceivePs2MouseByte(&dat))
	{
		i--;
		if (i == 0)
		{
			return FALSE;
		}
	}

	return TRUE;
}



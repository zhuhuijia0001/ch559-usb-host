#include "Type.h"
#include "CH559.h"

#include "PinDefine.h"
#include "System.h"
#include "Protocol.h"

#include "Ps2Keyboard.h"

#ifdef COUNT_FACTOR
	#undef COUNT_FACTOR
#endif

#ifdef USE_PLL
	#define COUNT_FACTOR    4
#else
	#define COUNT_FACTOR    3
#endif

void DisablePs2KeyboardPort()
{
	CLR_GPIO_BIT(PIN_KB_CLK);
	SET_GPIO_OUTPUT(PIN_KB_CLK);
}

static BOOL SendPs2KeyboardByte(UINT8 c)
{
	UINT8 parity = 0x00;
	UINT8 i;
	UINT16 j;
	
	CLR_GPIO_BIT(PIN_KB_CLK);
	SET_GPIO_OUTPUT(PIN_KB_CLK);
	mDelayuS(105);
	
	CLR_GPIO_BIT(PIN_KB_DAT);
	SET_GPIO_OUTPUT(PIN_KB_DAT);	
	mDelayuS(20);
	
	SET_GPIO_BIT(PIN_KB_CLK);
	SET_GPIO_INPUT(PIN_KB_CLK);
	
	j = 3000 * COUNT_FACTOR;
	do 
	{
		j--;
		if (j == 0)
		{
			return FALSE;
		}
	} while (GET_GPIO_BIT(PIN_KB_CLK)); //等待时钟变低
	
	for (i = 0; i < 8; i++)
	{
	    if (c & 0x01)
	    {
            SET_GPIO_BIT(PIN_KB_DAT);
	    }
	    else
	    {
            CLR_GPIO_BIT(PIN_KB_DAT);
	    }
		
		j = 500 * COUNT_FACTOR;
		do
		{
			j--;
			if (j == 0)
			{
				return FALSE;
			}
		} while (!GET_GPIO_BIT(PIN_KB_CLK));
				
		j = 500 * COUNT_FACTOR;
		do
		{
			j--;
			if (j == 0)
			{
				return FALSE;
			}
		} while (GET_GPIO_BIT(PIN_KB_CLK));
			
		parity ^= (c & 0x01);
		c >>= 1;
	}
	
	parity ^= 0x01;
	//奇校验位
	if (parity & 0x01)
	{
		SET_GPIO_BIT(PIN_KB_DAT);
	}
	else
	{
		CLR_GPIO_BIT(PIN_KB_DAT);
	}
	
	j = 500 * COUNT_FACTOR;
	do
	{
		j--;
		if (j == 0)
		{
			return FALSE;
		}
	} while (!GET_GPIO_BIT(PIN_KB_CLK));
	
	j = 500 * COUNT_FACTOR;
	do 
	{
		j--;
		if (j == 0)
		{
			return FALSE;
		}
	} while (GET_GPIO_BIT(PIN_KB_CLK));
	
	//停止位
	SET_GPIO_BIT(PIN_KB_DAT);
	
	j = 500 * COUNT_FACTOR;
	do
	{
		j--;
		if (j == 0)
		{
			return FALSE;
		}
	} while (!GET_GPIO_BIT(PIN_KB_CLK));
	
	SET_GPIO_INPUT(PIN_KB_DAT);
	
	j = 500 * COUNT_FACTOR;
	do 
	{
		j--;
		if (j == 0)
		{
			return FALSE;
		}
	} while (GET_GPIO_BIT(PIN_KB_CLK));
		
	//等待ack
	j = 500 * COUNT_FACTOR;
	do
	{
		j--;
		if (j == 0)
		{
			return FALSE;
		}
	} while (!GET_GPIO_BIT(PIN_KB_CLK));
	
	return TRUE;
}

static BOOL ReceivePs2KeyboardByte(UINT8 *pData)
{
	UINT8 dat = 0x00;
	UINT8 parity = 0x00;
	UINT8 parityTemp;
	UINT16 j;
	UINT8 i;

	SET_GPIO_BIT(PIN_KB_CLK);
	SET_GPIO_INPUT(PIN_KB_CLK);
	
	SET_GPIO_BIT(PIN_KB_DAT);
	SET_GPIO_INPUT(PIN_KB_DAT);
	
	if (GET_GPIO_BIT(PIN_KB_DAT))
	{
		return FALSE;
	}
		
	if (!GET_GPIO_BIT(PIN_KB_CLK) && !GET_GPIO_BIT(PIN_KB_DAT))
	{
		j = 500 * COUNT_FACTOR;
		do 
		{
			j--;
			if (j == 0)
			{
				return FALSE;
			}
		} while (!GET_GPIO_BIT(PIN_KB_CLK));
		
		j = 500 * COUNT_FACTOR;
		do
		{
			j--;
			if (j == 0)
			{
				return FALSE;
			}
		} while (GET_GPIO_BIT(PIN_KB_CLK));
			
		//获取数据 
		for (i = 0; i < 8; i++)
		{
			if (!GET_GPIO_BIT(PIN_KB_CLK))
			{
				dat >>= 1;
				if (GET_GPIO_BIT(PIN_KB_DAT))
				{
					dat |= 0x80;
				}
				else
				{
					dat &= 0x7f;
				}
					
				parity ^= GET_GPIO_BIT(PIN_KB_DAT);
					
				j = 500 * COUNT_FACTOR;
				do
				{
					j--;
					if (j == 0)
					{
						return FALSE;
					}
				} while (!GET_GPIO_BIT(PIN_KB_CLK));
					
				j = 500 * COUNT_FACTOR;
				do
				{
					j--;
					if (j == 0)
					{
						return FALSE;
					}
				} while (GET_GPIO_BIT(PIN_KB_CLK));
			}
		}
		
		if (!GET_GPIO_BIT(PIN_KB_CLK))
		{
			parityTemp = PIN_KB_DAT;
			
			j = 500 * COUNT_FACTOR;
			do
			{
				j--;
				if (j == 0)
				{
					return FALSE;
				}
			} while (!GET_GPIO_BIT(PIN_KB_CLK));
			
			j = 500 * COUNT_FACTOR;
			do
			{
				j--;
				if (j == 0)
				{
					return FALSE;
				}
			} while (GET_GPIO_BIT(PIN_KB_CLK));	
		
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
		
		if (!GET_GPIO_BIT(PIN_KB_CLK))
		{
			j = 500 * COUNT_FACTOR;
			do
			{
				j--;
				if (j == 0)
				{
					return FALSE;
				}
			} while (!GET_GPIO_BIT(PIN_KB_CLK));
		
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

//收发键盘端口
UINT8 TransceivePs2KeyboardPort(UINT8 *pPs2)
{
	UINT16 i;
	UINT8 dat;
	UINT8 ptr = 0;
	
	UINT8 res = PS2_KB_NONE;
	
	i = 600 /** COUNT_FACTOR*/;
	while (!ReceivePs2KeyboardByte(&dat))
	{
		i--;
		
		if (i == 0)
		{
			return res;
		}
	}

	pPs2[ptr] = dat;
	ptr++;

	if (dat == 0xe0)
	{
		//接收e0打头的数据包
		i = 1000 * COUNT_FACTOR;
		while (!ReceivePs2KeyboardByte(&dat))
		{
			i--;
			if (i == 0)
			{
				res = PS2_KB_INVALID_DATA;
				return res;
			}
		}
			
		pPs2[ptr] = dat;
		ptr++;

		if (dat == 0xf0)
		{
			//断码
			i = 1000 * COUNT_FACTOR;
			while (!ReceivePs2KeyboardByte(&dat))
			{
				i--;
				if (i == 0)
				{
					res = PS2_KB_INVALID_DATA;
					return res;
				}
			}
				
			pPs2[ptr] = dat;
			ptr++;
				
			if (dat == 0x12) 
			{
				//11个特殊键后面跟的无效数据
				res = PS2_KB_INVALID_DATA;
				
				return res;
			}
			else
			{
				goto VALID_DATA;
			}
		}
		else if (dat == 0x12)
		{
			//11个特殊键前面的无效数据
			res = PS2_KB_INVALID_DATA;
			return res;
		}
		else
		{
			//通码					
			goto VALID_DATA;
		}
	}
	else if (dat == 0xe1)
	{
		//pause键
		i = 1000 * COUNT_FACTOR;
		while (!ReceivePs2KeyboardByte(&dat))
		{
			i--;
			if (i == 0)
			{
				res = PS2_KB_INVALID_DATA;
				return res;
			}
		}
			
		pPs2[ptr] = dat;
		ptr++;
			
		if (dat == 0x14)
		{
			i = 1000 * COUNT_FACTOR;
			while (!ReceivePs2KeyboardByte(&dat))
			{
				i--;
				if (i == 0)
				{
					res = PS2_KB_INVALID_DATA;
					return res;
				}
			}
			
			pPs2[ptr] = dat;
			ptr++;
		}
		else
		{
			res = PS2_KB_INVALID_DATA;
			return res;
		}
			
		if (dat == 0x77)
		{
			i = 1000 * COUNT_FACTOR;
			while (!ReceivePs2KeyboardByte(&dat))
			{
				i--;
				if (i == 0)
				{
					res = PS2_KB_INVALID_DATA;
					return res;
				}
			}
			
			pPs2[ptr] = dat;
			ptr++;
		}
		else
		{
			res = PS2_KB_INVALID_DATA;
			return res;
		}

		if (dat == 0xe1)
		{
			i = 1000 * COUNT_FACTOR;
			while (!ReceivePs2KeyboardByte(&dat))
			{
				i--;
				if (i == 0)
				{
					res = PS2_KB_INVALID_DATA;
					return res;
				}
			}
			
			pPs2[ptr] = dat;
			ptr++;
		}
		else
		{
			res = PS2_KB_INVALID_DATA;
			return res;
		}
		
		if (dat == 0xf0)
		{
			i = 1000 * COUNT_FACTOR;
			while (!ReceivePs2KeyboardByte(&dat))
			{
				i--;
				if (i == 0)
				{
					res = PS2_KB_INVALID_DATA;
					return res;
				}
			}
			
			pPs2[ptr] = dat;
			ptr++;
		}
		else
		{
			res = PS2_KB_INVALID_DATA;
			return res;
		}
			
		if (dat == 0x14)
		{
			i = 1000 * COUNT_FACTOR;
			while (!ReceivePs2KeyboardByte(&dat))
			{
				i--;
				if (i == 0)
				{
					res = PS2_KB_INVALID_DATA;
					return res;
				}
			}
			
			pPs2[ptr] = dat;
			ptr++;
		}
		else
		{
			res = PS2_KB_INVALID_DATA;
			return res;
		}

		if (dat == 0xf0)
		{
			i = 1000 * COUNT_FACTOR;
			while (!ReceivePs2KeyboardByte(&dat))
			{
				i--;
				if (i == 0)
				{
					res = PS2_KB_INVALID_DATA;
					return res;
				}
			}
			
			pPs2[ptr] = dat;
			ptr++;
		}
		else
		{
			res = PS2_KB_INVALID_DATA;
			return res;
		}

		if (dat == 0x77)
		{
			goto VALID_DATA;
		}
		else
		{
			res = PS2_KB_INVALID_DATA;
			return res;
		}	
	}
	else if (dat == 0xf0)
	{
		//常规断码
		i = 1000 * COUNT_FACTOR;
		while (!ReceivePs2KeyboardByte(&dat))
		{
			i--;
			if (i == 0)
			{
				res = PS2_KB_INVALID_DATA;
				return res;
			}
		}
			
		pPs2[ptr] = dat;
		ptr++;
			
		goto VALID_DATA;
	}
	else if (dat == 0xaa)
	{
		//应答数据，省略
		res = PS2_KB_AA;	
					
		return res;
	}
	else
	{	
		goto VALID_DATA;
	}
	
VALID_DATA:
	for ( ; ptr < KEYBOARD_LEN; ptr++)
	{
		pPs2[ptr] = 0x00;
	}

	res = PS2_KB_VALID_DATA;
	
	return res;	
}

BOOL SetPs2KeyboardLed(UINT8 c)
{
	UINT8 temp1, temp2;
	UINT16 i;
	UINT8 dat;
	
	temp1 = c & 0x04;
	temp2 = c & 0x03;
	c = (temp1 >> 2) | (temp2 << 1);

	if (SendPs2KeyboardByte(0xed))
	{	
		i = 1000 * COUNT_FACTOR;
		while (!ReceivePs2KeyboardByte(&dat))
		{
			i--;
			if (i == 0)
			{
				return FALSE;
			}
		}
	}
	else
	{
		return FALSE;
	}
		
	if (dat == 0xfa)
	{
		if (SendPs2KeyboardByte(c))
		{
			i = 1000 * COUNT_FACTOR;
			while (!ReceivePs2KeyboardByte(&dat))
			{
				i--;
				if (i == 0)
				{
					return FALSE;
				}
			}
		}
		else
		{
			return FALSE;
		}
	}
	else
	{	
		return FALSE;
	}
		
	if (dat == 0xfa)
	{		
		return TRUE;
	}				
	
	return FALSE;
}


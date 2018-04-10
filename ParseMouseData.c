#include "Type.h"

#include "UsbHost.h"
#include "ParseMouseData.h"

static UINT8 GetMouseFieldData(const UINT8 *pData, UINT8 start, UINT8 size)
{
	UINT8 index = start / 8;
	UINT8 mouseData = pData[index];
	UINT8 i;
		
	start -= index * 8;
	
	mouseData >>= start;
	
	if (size > 8 - start)
	{
		size = 8 - start;
	}
	
	for (i = size; i < 8; i++)
	{
		mouseData &= ~(0x01 << i);
	}
	
	return mouseData;
}

//--------------------------------------------------------------------------------------------------
// 函数名称: UsbMouseParse()
// 函数功能: 鼠标数据解析
//--------------------------------------------------------------------------------------------------
BOOL UsbMouseParse(const UINT8 *pUsb, UINT8 *pOut, const MOUSEDATA_STRUCT *pMouseDataStruct)
{
	UINT8 index;
	
	UINT16 datx, daty;
	
	//解析button位置
	index = pMouseDataStruct->buttonStart;
	
	if (index == 0xff)
	{
		pOut[0] = 0;
	}
	else
	{
		index >>= 3;
		
		pOut[0] = pUsb[index] & 0x07;
	}
		
	//解析x，y位置
	index = pMouseDataStruct->xStart;
	if (index == 0xff)
	{
		pOut[1] = 0;
		pOut[2] = 0;
	}
	else
	{
		index >>= 3;
	
		if (pMouseDataStruct->xSize == 16)
		{
			datx = pUsb[index] | (pUsb[index + 1] << 8);
			daty = pUsb[index + 2] | (pUsb[index + 3] << 8);
			
			pOut[1] = (UINT8)datx;
			pOut[2] = (UINT8)daty;
		}
		else if (pMouseDataStruct->xSize == 12)
		{
			datx = pUsb[index] | ((pUsb[index + 1] & 0x0f) << 8);
			daty = ((pUsb[index + 1] >> 4) & 0x0f) | (pUsb[index + 2] << 4);
		
			pOut[1] = (UINT8)datx;
			pOut[2] = (UINT8)daty;		
		}
		else
		{
			pOut[1] = pUsb[index];		
			pOut[2] = pUsb[index + 1];
		}
	}
	
	//解析wheel位置
	index = pMouseDataStruct->wheelStart;
	
	if (index == 0xff)
	{
		pOut[3] = 0x00;
	}
	else
	{
		if (pMouseDataStruct->wheelSize > 0)
		{
			pOut[3] = GetMouseFieldData(pUsb, pMouseDataStruct->wheelStart, pMouseDataStruct->wheelSize);
		
			if (pOut[3] & (0x01 << (pMouseDataStruct->wheelSize - 1)))
			{
				pOut[3] = 0xff;
			}
		}
		else
		{
			pOut[3] = 0x00;
		}	
	}

	return TRUE;
}



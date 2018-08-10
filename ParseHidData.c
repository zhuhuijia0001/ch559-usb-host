#include "Type.h"

#include "UsbHost.h"
#include "ParseHidData.h"

BOOL UsbKeyboardParse(const UINT8 *pUsb, UINT8 *pOut, const HID_SEG_STRUCT *pKeyboardSegStruct)
{
    UINT8 i;
    
    if (pKeyboardSegStruct->HIDSeg[HID_SEG_KEYBOARD_INDEX].segStart == 0xff)
    {
        return FALSE;
    }

    if (pOut != NULL)
    {
        pUsb += pKeyboardSegStruct->HIDSeg[HID_SEG_KEYBOARD_INDEX].segStart / 8;
        
        for (i = 0; i < 8; i++)
        {
            *pOut++ = *pUsb++;
        }
    }

    return TRUE;
}

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
BOOL UsbMouseParse(const UINT8 *pUsb, UINT8 *pOut, const HID_SEG_STRUCT *pMouseSegStruct)
{
	UINT8 index;
	
	UINT16 datx, daty;

	UINT8 size;
	
	//解析button位置
	index = pMouseSegStruct->HIDSeg[HID_SEG_BUTTON_INDEX].segStart;
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
	index = pMouseSegStruct->HIDSeg[HID_SEG_X_INDEX].segStart;
	if (index == 0xff)
	{
		pOut[1] = 0;
		pOut[2] = 0;
	}
	else
	{
		index >>= 3;

	    size = pMouseSegStruct->HIDSeg[HID_SEG_X_INDEX].segSize;
		if (size == 16)
		{
			datx = pUsb[index] | (pUsb[index + 1] << 8);
			daty = pUsb[index + 2] | (pUsb[index + 3] << 8);
			
			pOut[1] = (UINT8)datx;
			pOut[2] = (UINT8)daty;
		}
		else if (size == 12)
		{
			datx = pUsb[index] | ((pUsb[index + 1] & 0x0f) << 8);
			daty = ((pUsb[index + 1] >> 4) & 0x0f) | (pUsb[index + 2] << 4);
		
			pOut[1] = (UINT8)datx;
			pOut[2] = (UINT8)daty;		
		}
		else
		{
		    //size == 8
			pOut[1] = pUsb[index];		
			pOut[2] = pUsb[index + 1];
		}
	}
	
	//解析wheel位置
	index = pMouseSegStruct->HIDSeg[HID_SEG_WHEEL_INDEX].segStart;
	if (index == 0xff)
	{
		pOut[3] = 0x00;
	}
	else
	{
		if (pMouseSegStruct->HIDSeg[HID_SEG_WHEEL_INDEX].segSize > 0)
		{
			pOut[3] = GetMouseFieldData(pUsb, pMouseSegStruct->HIDSeg[HID_SEG_WHEEL_INDEX].segStart, pMouseSegStruct->HIDSeg[HID_SEG_WHEEL_INDEX].segSize);
		
			if (pOut[3] & (0x01 << (pMouseSegStruct->HIDSeg[HID_SEG_WHEEL_INDEX].segSize - 1)))
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



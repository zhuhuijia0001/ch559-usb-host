#include "Type.h"

#include "Protocol.h"
#include "UsbHost.h"
#include "ParseHidData.h"

#include "Trace.h"

static UINT8C lowest_bit_bitmap[] =
{
    /* 00 */ 0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    /* 10 */ 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    /* 20 */ 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    /* 30 */ 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    /* 40 */ 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    /* 50 */ 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    /* 60 */ 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    /* 70 */ 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    /* 80 */ 7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    /* 90 */ 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    /* A0 */ 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    /* B0 */ 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    /* C0 */ 6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    /* D0 */ 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    /* E0 */ 5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
    /* F0 */ 4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

static BOOL NewUsbKeyboardParse(const UINT8 *pUsb, UINT8 len, UINT8 *pOut, UINT8 maxOut)
{  
	UINT8 i;

	UINT8 offset = 0;

	UINT8 outputIndex = 0;

	for (i = 0; i < len; i++) 
	{
		UINT8 d = *pUsb++;

		while (d)
		{
			UINT8 lowestBit = lowest_bit_bitmap[d];

			d &= ~(1u << lowestBit);

			if (outputIndex < maxOut)
			{
				pOut[outputIndex++] = offset + lowestBit;
			}
			else 
			{
				return FALSE;
			}
		}

		offset += 8;
	}

	while (outputIndex < maxOut)
	{
		pOut[outputIndex++] = 0x00;
	}
	
	return TRUE;
}

BOOL UsbKeyboardParse(const UINT8 *pUsb, UINT8 *pOut, const HID_SEG_STRUCT *pKeyboardSegStruct)
{
    UINT8 i;
    
    if (pKeyboardSegStruct->HIDSeg[HID_SEG_KEYBOARD_MODIFIER_INDEX].start == 0xff)
    {
        return FALSE;
    }

    if (pOut != NULL)
    {
        UINT8 index = pKeyboardSegStruct->HIDSeg[HID_SEG_KEYBOARD_MODIFIER_INDEX].start / 8;
        pOut[0] = pUsb[index];
        pOut[1] = 0x00;

        index = pKeyboardSegStruct->HIDSeg[HID_SEG_KEYBOARD_VAL_INDEX].start / 8;
        if (pKeyboardSegStruct->HIDSeg[HID_SEG_KEYBOARD_VAL_INDEX].size > 1)
        {
			for (i = 0; i < 6; i++)
	        {
	            *pOut++ = *pUsb++;
	        }
        }
        else
        {
			//bit
			if (NewUsbKeyboardParse(&pUsb[index], pKeyboardSegStruct->HIDSeg[HID_SEG_KEYBOARD_VAL_INDEX].count / 8, &pOut[2], 6))
			{
#ifdef DEBUG
				TRACE("converted data:\r\n");
			    for (i = 0; i < 8; i++)
			    {
			        TRACE1("0x%02X ", (UINT16)pOut[i]);
			    }
			    TRACE("\r\n");
#endif
			}
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
	index = pMouseSegStruct->HIDSeg[HID_SEG_BUTTON_INDEX].start;
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
	index = pMouseSegStruct->HIDSeg[HID_SEG_X_INDEX].start;
	if (index == 0xff)
	{
		pOut[1] = 0;
		pOut[2] = 0;
	}
	else
	{
		index >>= 3;

	    size = pMouseSegStruct->HIDSeg[HID_SEG_X_INDEX].size;
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
	index = pMouseSegStruct->HIDSeg[HID_SEG_WHEEL_INDEX].start;
	if (index == 0xff)
	{
		pOut[3] = 0x00;
	}
	else
	{
		if (pMouseSegStruct->HIDSeg[HID_SEG_WHEEL_INDEX].size > 0)
		{
			pOut[3] = GetMouseFieldData(pUsb, pMouseSegStruct->HIDSeg[HID_SEG_WHEEL_INDEX].start, pMouseSegStruct->HIDSeg[HID_SEG_WHEEL_INDEX].size);
		
			if (pOut[3] & (0x01 << (pMouseSegStruct->HIDSeg[HID_SEG_WHEEL_INDEX].size - 1)))
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



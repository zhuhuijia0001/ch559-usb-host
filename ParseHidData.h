#ifndef _PARSELASERMOUSEDATA_H_
#define _PARSELASERMOUSEDATA_H_

extern BOOL UsbKeyboardParse(const UINT8 *pUsb, UINT8 *pOut, const HID_SEG_STRUCT *pKeyboardSegStruct);

extern BOOL UsbMouseParse(const UINT8 *pUsb, UINT8 *pOut, const HID_SEG_STRUCT *pMouseDataStruct);

#endif

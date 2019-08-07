#ifndef _PARSELASERMOUSEDATA_H_
#define _PARSELASERMOUSEDATA_H_

extern BOOL UsbKeyboardParse(const UINT8 *pUsb, UINT8 *pOut, const HID_SEG_STRUCT *pKeyboardSegStruct, KEYBOARD_PARSE_STRUCT *const pKeyboardParseStruct);

extern BOOL UsbMouseParse(const UINT8 *pUsb, UINT8 *pOut, const HID_SEG_STRUCT *pMouseDataStruct);

#endif

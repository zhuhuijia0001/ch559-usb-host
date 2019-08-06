#ifndef _PARSE_DESCRIPTOR_H_
#define _PARSE_DEXCRIPTOR_H_

extern BOOL ParseDeviceDescriptor(const USB_DEV_DESCR *const pDevDescr, UINT8 len, USB_DEVICE *const pUsbDevice);

extern BOOL ParseConfigDescriptor(const USB_CFG_DESCR *const pCfgDescr, UINT16 len, USB_DEVICE *const pUsbDevice);

extern BOOL ParseReportDescriptor(const UINT8 *pDescriptor, UINT16 len, HID_SEG_STRUCT *const pHidSegStruct);

#endif



#ifndef _USBUSER_H_
#define _USBUSER_H_

extern void SetKeyboardLedStatus(UINT8 led);
extern UINT8 GetKeyboardLedStatus(void);

extern void ProcessHIDData(INTERFACE *pInterface, const UINT8 *pData, UINT16 len);

#endif


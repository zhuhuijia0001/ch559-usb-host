#ifndef _IPPS2KEYBOARD_H_
#define _IPPS2KEYBOARD_H_

enum
{
	IP_PS2_KB_NONE         = 0x00,
	IP_PS2_KB_VALID_DATA   = 0x01,
	IP_PS2_KB_INVALID_DATA = 0x02,
	IP_PS2_KB_AA           = 0x03
};

extern void DisableIpPs2KeyboardPort();
extern UINT8 TransceiveIpPs2KeyboardPort(UINT8 *pPs2);
extern BOOL SetIpPs2KeyboardLed(UINT8 c);

#endif


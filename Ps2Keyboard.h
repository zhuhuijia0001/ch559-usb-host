#ifndef _PS2KEYBOARD_H_
#define _PS2KEYBOARD_H_

enum
{
	PS2_KB_NONE         = 0x00,
	PS2_KB_VALID_DATA   = 0x01,
	PS2_KB_INVALID_DATA = 0x02,
	PS2_KB_AA           = 0x03
};

extern void DisablePs2KeyboardPort();
extern UINT8 TransceivePs2KeyboardPort(UINT8 *pPs2);
extern BOOL SetPs2KeyboardLed(UINT8 c);

#endif


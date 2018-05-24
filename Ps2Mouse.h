#ifndef _PS2MOUSE_H_
#define _PS2MOUSE_H_

enum
{
	PS2_MS_NONE         = 0x00,
	PS2_MS_VALID_DATA   = 0x01,
	PS2_MS_INVALID_DATA = 0x02,
	PS2_MS_AA           = 0x03	
};

extern void DisablePs2MousePort(void);
extern UINT8 TransceivePs2MousePort(UINT8 *pPs2);
extern BOOL InitPs2Mouse(void);

#endif


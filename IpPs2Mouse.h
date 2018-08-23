#ifndef _IPPS2MOUSE_H_
#define _IPPS2MOUSE_H_

enum
{
	IP_PS2_MS_NONE         = 0x00,
	IP_PS2_MS_VALID_DATA   = 0x01,
	IP_PS2_MS_INVALID_DATA = 0x02,
	IP_PS2_MS_AA           = 0x03	
};

extern void DisableIpPs2MousePort(void);
extern UINT8 TransceiveIpPs2MousePort(UINT8 *pPs2);
extern BOOL InitIpPs2Mouse(void);

#endif


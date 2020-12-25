#ifndef _SENDBUFFER_H_
#define _SENDBUFFER_H_

extern void InitSendBuffer(void);

extern BOOL AppendSendBuffer(const UINT8 *pBuffer, UINT16 size);

extern void SendBufferIsr(void);

#endif


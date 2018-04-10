
#ifndef _RECVBUFFER_H_
#define _RECVBUFFER_H_

extern void InitRecvBuffer(void);

extern BOOL CheckRecvBuffer(void);

extern UINT8 *GetOutputBuffer(void);

extern void RecvBufferOneByte(UINT8 ch);

extern BOOL RecvBufferTimerout(void);

#endif


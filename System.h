
#ifndef	__DEBUG_H__
#define __DEBUG_H__

//定义函数返回值
#ifndef  SUCCESS
#define  SUCCESS  1
#endif
#ifndef  FAIL
#define  FAIL    0
#endif

//定义定时器起始
#ifndef  START
#define  START  1
#endif
#ifndef  STOP
#define  STOP    0
#endif

#define	 FREQ_SYS	48000000ul	  //系统主频48MHz

#ifndef  BUAD_RATE
#define  BUAD_RATE  100000ul
#endif

void CfgFsys(void);                        //CH559时钟选择和配置
void mDelayuS(UINT16 n);              // 以uS为单位延时
void mDelaymS(UINT16 n);              // 以mS为单位延时

#endif


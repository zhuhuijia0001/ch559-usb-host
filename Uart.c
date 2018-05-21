#include "Type.h"
#include "CH559.h" 
#include "System.h"
#include "Uart.h"

/*******************************************************************************
* Function Name  : CH559UART0Alter()
* Description    : CH559串口0引脚映射,串口映射到P0.2和P0.3
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void CH559UART0Alter()
{
    PORT_CFG |= bP0_OC;
    P0_DIR |= bTXD_;
    P0_PU |= bTXD_ | bRXD_;
    PIN_FUNC |= bUART0_PIN_X;                                                  //串口映射到P0.2和P0.3
}

/*******************************************************************************
* Function Name  : InitUART0()
* Description    : CH559串口0初始化,默认使用T1作UART0的波特率发生器,也可以使用T2
                   作为波特率发生器
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void InitUART0()
{
    UINT32 x;
    UINT8 x2; 

#ifdef DEBUG
    CH559UART0Alter();
#endif
    
    SM0 = 0;
    SM1 = 1;
    SM2 = 0;                                                                   //串口0使用模式1
                                                                               //使用Timer1作为波特率发生器
    RCLK = 0;                                                                  //UART0接收时钟
    TCLK = 0;                                                                  //UART0发送时钟
    PCON |= SMOD;
    x = 10 * FREQ_SYS / BUAD_RATE / 16;                                             //如果更改主频，注意x的值不要溢出                            
    x2 = x % 10;
    x /= 10;
    if ( x2 >= 5 ) x++;                                                       //四舍五入

    TMOD = TMOD & ~ bT1_GATE & ~ bT1_CT & ~ MASK_T1_MOD | bT1_M1;              //0X20，Timer1作为8位自动重载定时器
    T2MOD = T2MOD | bTMR_CLK | bT1_CLK;                                        //Timer1时钟选择
    TH1 = 0-x;                                                                 //12MHz晶振,buad/12为实际需设置波特率
    TR1 = 1;                                                                   //启动定时器1

	REN = 1;                                                                   //串口0接收使能
	
#ifdef DEBUG
	TI = 1;
#else
	TI = 0;

	ES = 1;
#endif
}

/*******************************************************************************
* Function Name  : CH559UART0RcvByte()
* Description    : CH559UART0接收一个字节
* Input          : None
* Output         : None
* Return         : SBUF
*******************************************************************************/
UINT8  CH559UART0RcvByte( )
{
    while(RI == 0);                                                            //查询接收，中断方式可不用
    RI = 0;
    return SBUF;
}

static BOOL volatile s_sent = FALSE;

void SetUart0Sent(void)
{
	s_sent = TRUE;
}

/*******************************************************************************
* Function Name  : CH559UART0SendByte(UINT8 SendDat)
* Description    : CH559UART0发送一个字节
* Input          : UINT8 SendDat；要发送的数据
* Output         : None
* Return         : None
*******************************************************************************/
void CH559UART0SendByte(UINT8 SendDat)
{
	s_sent = FALSE;
	SBUF = SendDat;
	while (!s_sent);
}

void CH559UART0SendData(const UINT8 *pData, UINT8 len)
{
	while (len-- > 0)
	{
		s_sent = FALSE;
		SBUF = *pData++;
		while (!s_sent);
	}
}

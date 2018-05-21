
#include "Type.h"

#include "PinDefine.h"

#include "Mcu.h"
#include "System.h"
#include "Gpio.h"
#include "Uart.h"
#include "Timer2.h"

#include "UsbHost.h"
#include "KeyboardLed.h"

#include "RecvBuffer.h"
#include "Protocol.h"

#include "Ps2Keyboard.h"
#include "Ps2Mouse.h"

#include "Task.h"

#include "Trace.h"

static UINT8 volatile s_5MsCounter = 0;

static BOOL volatile s_CheckUsbPort0 = FALSE;
static BOOL volatile s_CheckUsbPort1 = FALSE;

static UINT8X s_Ps2Data[KEYBOARD_LEN + 2];

void Uart0Isr(void) interrupt INT_NO_UART0 using 1
{	
	if (RI)
	{
		UINT8 ch;
		RI = 0;

		ch = SBUF;

		RecvBufferOneByte(ch);
	}

	if (TI)
	{
		TI = 0;

		SetUart0Sent();
	}
}

void Timer2Isr(void) interrupt INT_NO_TMR2 using 2
{
	if (TF2)
    { 	
        TF2 = 0; 
		
		//uart receive timeout
		RecvBufferTimerout();

		if (s_5MsCounter == 0)
		{
			s_CheckUsbPort0 = TRUE;
		}
		else if (s_5MsCounter == 1)
		{
			s_CheckUsbPort1 = TRUE;
		}
		
		s_5MsCounter++;
		if (s_5MsCounter == 2)
		{
			s_5MsCounter = 0;
		}
	}
}

static void InitTimer2()
{
	mTimer2Clk12DivFsys( ); 												//时钟选择Fsys定时器方式
	mTimer2Setup(0);														//定时器功能演示
	mTimer2Init(FREQ_SYS / 12 * 5 / 1000);									//定时器赋初值

	ET2 = 1;																//使能全局中断
	mTimer2RunCTL(1); 														//启动定时器
}

void InitSystem(void)
{
	CfgFsys();      //CH559时钟选择配置   
    mDelaymS(5);   

	InitRecvBuffer();

	DisablePs2MousePort();
	
	DisablePs2KeyboardPort();
	
    CH559GPIODrivCap(1, 1);
	CH559GPIOModeSelt(1, 2, 5);

	CLR_GPIO_BIT(PIN_TEST_LED);

	InitTimer2();
	
	InitUART0();

	InitUsbData();
	InitUsbHost();

	HAL_ENABLE_INTERRUPTS();

	TRACE("system init\r\n");
}

void ProcessUsbHostPort()
{	
	DealUsbPort();
	if (s_CheckUsbPort0)
	{
		s_CheckUsbPort0 = FALSE;
		
		InterruptProcessRootHubPort(0);
	}

	if (s_CheckUsbPort1)
	{
		s_CheckUsbPort1 = FALSE;

		InterruptProcessRootHubPort(1);
	}
}

void ProcessPs2Port()
{
	BOOL bDisabled = FALSE;
	UINT8 res;
	
	DisablePs2MousePort();

	res = TransceivePs2KeyboardPort(s_Ps2Data);
	if (res == PS2_KB_AA)
	{
		UINT8 led = GetKeyboardLedStatus();
		
		SetPs2KeyboardLed(led);	
	}
	else if (res == PS2_KB_VALID_DATA)
	{	
#ifdef DEBUG
		UINT16 i;
#endif

		DisablePs2KeyboardPort();

#ifdef DEBUG
		for (i = 0; i < KEYBOARD_LEN; i++)
		{
			TRACE1("0x%x ", s_Ps2Data[i]);
		}
		TRACE("\r\n");
#else

		//SendComData(s_ps2Data, KB_DATA_LEN);
#endif		
		bDisabled = TRUE;
	}
	
	if (!bDisabled)
	{
		DisablePs2KeyboardPort();
	}
	
	bDisabled = FALSE;
	res = TransceivePs2MousePort(s_Ps2Data);
	
	if (res == PS2_MS_AA)
	{
		InitPs2Mouse();
	}
	else if (res == PS2_MS_VALID_DATA)
	{
#ifdef DEBUG
		UINT16 i;
#endif

		DisablePs2MousePort();
		
#ifdef DEBUG
		for (i = 0; i < MOUSE_LEN; i++)
		{
			TRACE1("0x%x ", s_Ps2Data[i]);
		}
		TRACE("\r\n");
#else
		//SendComData(s_Ps2Data, MS_DATA_LEN);
#endif
		
		bDisabled = TRUE;
	}
	else if (res == PS2_KB_INVALID_DATA)
	{
		bDisabled = TRUE;
	}

	if (!bDisabled)
	{
		DisablePs2MousePort();
	}
}

void ProcessKeyboardLed()
{
	if (!IsRecvBufferEmpty())
	{
		UINT8 *packet = GetOutputBuffer();
		UINT8 id = packet[0];
		UINT8 *pData = &packet[1];

		switch (id)
		{
		case ID_LED_STATUS:
			{
				UINT8 led = pData[0];
				UINT8 ledSave = GetKeyboardLedStatus();
				if (led != ledSave)
				{
					SetPs2KeyboardLed(led);
					DisablePs2KeyboardPort();
		
					UpdateUsbKeyboardLed(led);

					SetKeyboardLedStatus(led);
				}

				CH559UART0SendData(&led, 1);
			}
			
			break;

		default:
			break;
		}
	}
}


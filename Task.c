
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

#include "IpPs2Keyboard.h"
#include "IpPs2Mouse.h"

#include "Packet.h"

#include "Task.h"

#include "Trace.h"

static UINT8 volatile s_5MsCounter = 0;

static BOOL volatile s_CheckUsbPort0 = FALSE;
static BOOL volatile s_CheckUsbPort1 = FALSE;

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

    #ifdef DEBUG
        if (GET_GPIO_BIT(PIN_TEST_LED))
        {
            CLR_GPIO_BIT(PIN_TEST_LED);
        }
        else
        {
            SET_GPIO_BIT(PIN_TEST_LED);
        }
    #endif
	}
}

static void InitPs2Port(void)
{
    CH559GPIOModeSelt(PORT_PIN_KB_CLK, 3, OFFSET_PIN_KB_CLK);
	CH559GPIOModeSelt(PORT_PIN_KB_DAT, 3, OFFSET_PIN_KB_DAT);

	CH559GPIOModeSelt(PORT_PIN_MS_CLK, 3, OFFSET_PIN_MS_CLK);
	CH559GPIOModeSelt(PORT_PIN_MS_DAT, 3, OFFSET_PIN_MS_DAT);

    //for ip ps2 pins 
	CH559P4Mode();
	
    DisablePs2MousePort();
	DisablePs2KeyboardPort();

	DisableIpPs2MousePort();
	DisableIpPs2KeyboardPort();
}

static void InitTimer2(void)
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

    CH559GPIODrivCap(3, 1);
	InitPs2Port();

#ifdef DEBUG
    CH559GPIOModeSelt(PORT_PIN_TEST_LED, 2, OFFSET_PIN_TEST_LED);

    SET_GPIO_BIT(PIN_TEST_LED);
#endif

	InitRecvBuffer();
	
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
	UINT8 res;
	UINT8 buffer[KEYBOARD_LEN];
	
	res = TransceivePs2KeyboardPort(buffer);
	DisablePs2KeyboardPort();

	if (res == PS2_KB_AA)
	{
		UINT8 led = GetKeyboardLedStatus();

		SetPs2KeyboardLed(led);
		DisablePs2KeyboardPort();
	}
	else if (res == PS2_KB_VALID_DATA)
	{
	    UINT8 output[KEYBOARD_LEN + 2];

	    UINT8 pktLen;
	    
#ifdef DEBUG
        UINT16 i;
        
		for (i = 0; i < KEYBOARD_LEN; i++)
		{
			TRACE1("0x%x ", buffer[i]);
		}
		TRACE("\r\n");
#endif
		if (BuildPs2KeyboardPacket(output, sizeof(output), &pktLen, buffer))
		{
			if (pktLen == KEYBOARD_LEN + 2)
			{
				CH559UART0SendData(output, pktLen);
			}
		}
	}
	else if (res == PS2_KB_INVALID_DATA)
	{
		TRACE("invalid kb data\r\n");
	}

	res = TransceivePs2MousePort(buffer);
	DisablePs2MousePort();
	
	if (res == PS2_MS_AA)
	{
		BOOL res = InitPs2Mouse();
		DisablePs2MousePort();
		if (res)
		{
			TRACE("mouse init ok\r\n");
		}
		else
		{
			TRACE("mouse init failed\r\n");
		}
	}
	else if (res == PS2_MS_VALID_DATA)
	{
	    UINT8 output[MOUSE_LEN + 2];
		UINT8 pktLen;
		
#ifdef DEBUG
        UINT16 i;
		for (i = 0; i < MOUSE_LEN; i++)
		{
			TRACE1("0x%x ", buffer[i]);
		}
		TRACE("\r\n");
#endif
        if (BuildPs2MousePacket(output, sizeof(output), &pktLen, buffer))
		{
			if (pktLen == MOUSE_LEN + 2)
			{
				CH559UART0SendData(output, pktLen);
			}
		}
	}
	else if (res == PS2_KB_INVALID_DATA)
	{
		TRACE("invalid ms data\r\n");
	}
}

void ProcessIpPs2Port()
{
	UINT8 res;
	UINT8 buffer[KEYBOARD_LEN];
	
	res = TransceiveIpPs2KeyboardPort(buffer);
	DisableIpPs2KeyboardPort();

	if (res == IP_PS2_KB_AA)
	{
		UINT8 led = GetKeyboardLedStatus();

		SetIpPs2KeyboardLed(led);
		DisableIpPs2KeyboardPort();
	}
	else if (res == IP_PS2_KB_VALID_DATA)
	{
	    UINT8 output[KEYBOARD_LEN + 2];

	    UINT8 pktLen;
	    
#ifdef DEBUG
        UINT16 i;
        
		for (i = 0; i < KEYBOARD_LEN; i++)
		{
			TRACE1("0x%x ", buffer[i]);
		}
		TRACE("\r\n");
#endif
		if (BuildPs2KeyboardPacket(output, sizeof(output), &pktLen, buffer))
		{
			if (pktLen == KEYBOARD_LEN + 2)
			{
				CH559UART0SendData(output, pktLen);
			}
		}
	}
	else if (res == IP_PS2_KB_INVALID_DATA)
	{
		TRACE("invalid kb data\r\n");
	}

	res = TransceiveIpPs2MousePort(buffer);
	DisableIpPs2MousePort();
	
	if (res == IP_PS2_MS_AA)
	{
		BOOL res = InitIpPs2Mouse();
		DisableIpPs2MousePort();
		if (res)
		{
			TRACE("mouse init ok\r\n");
		}
		else
		{
			TRACE("mouse init failed\r\n");
		}
	}
	else if (res == IP_PS2_MS_VALID_DATA)
	{
	    UINT8 output[MOUSE_LEN + 2];
		UINT8 pktLen;
		
#ifdef DEBUG
        UINT16 i;
		for (i = 0; i < MOUSE_LEN; i++)
		{
			TRACE1("0x%x ", buffer[i]);
		}
		TRACE("\r\n");
#endif
        if (BuildPs2MousePacket(output, sizeof(output), &pktLen, buffer))
		{
			if (pktLen == MOUSE_LEN + 2)
			{
				CH559UART0SendData(output, pktLen);
			}
		}
	}
	else if (res == IP_PS2_KB_INVALID_DATA)
	{
		TRACE("invalid ms data\r\n");
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

		            SetIpPs2KeyboardLed(led);
					DisableIpPs2KeyboardPort();
					
					UpdateUsbKeyboardLed(led);

					SetKeyboardLedStatus(led);
				}
			}
			
			break;

		default:
			break;
		}
	}
}


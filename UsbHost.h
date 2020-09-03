
#ifndef _USBHOST_H_
#define _USBHOST_H_

// 各子程序返回状态码
#define ERR_SUCCESS         0x00    // 操作成功
#define ERR_USB_CONNECT     0x15    /* 检测到USB设备连接事件,已经连接 */
#define ERR_USB_DISCON      0x16    /* 检测到USB设备断开事件,已经断开 */
#define ERR_USB_BUF_OVER    0x17    /* USB传输的数据有误或者数据太多缓冲区溢出 */
#define ERR_USB_DISK_ERR    0x1F    /* USB存储器操作失败,在初始化时可能是USB存储器不支持,在读写操作中可能是磁盘损坏或者已经断开 */
#define ERR_USB_TRANSFER    0x20    /* NAK/STALL等更多错误码在0x20~0x2F */
#define ERR_USB_UNSUPPORT   0xFB    /*不支持的USB设备*/
#define ERR_USB_UNKNOWN     0xFE    /*设备操作出错*/


/* 以下状态代码用于USB主机方式 */
/*   位4指示当前接收的数据包是否同步, 0=不同步, 1-同步 */
/*   位3-位0指示USB设备的应答: 0010=ACK, 1010=NAK, 1110=STALL, 0011=DATA0, 1011=DATA1, XX00=应答错误或者超时无应答 */
#ifndef	USB_INT_RET_ACK
#define	USB_INT_RET_ACK		     USB_PID_ACK		/* 错误:对于OUT/SETUP事务返回ACK */
#define	USB_INT_RET_NAK		     USB_PID_NAK		/* 错误:返回NAK */
#define	USB_INT_RET_STALL	     USB_PID_STALL	/* 错误:返回STALL */
#define	USB_INT_RET_DATA0	     USB_PID_DATA0	/* 错误:对于IN事务返回DATA0 */
#define	USB_INT_RET_DATA1	     USB_PID_DATA1	/* 错误:对于IN事务返回DATA1 */
#define	USB_INT_RET_TOUT	     0x00			/* 错误:应答错误或者超时无应答 */
#define	USB_INT_RET_TOUT1	     0x04			/* 错误:应答错误或者超时无应答 */
#define	USB_INT_RET_TOUT2	     0x08			/* 错误:应答错误或者超时无应答 */
#define	USB_INT_RET_TOUT3	     0x0C			/* 错误:应答错误或者超时无应答 */
#endif

//number of root hub ports
#define ROOT_HUB_PORT_NUM      2

//maximum number of external hub ports
#define MAX_EXHUB_PORT_NUM     4

//none external hub port
#define EXHUB_PORT_NONE        0xff

//maximum number of interfaces per devicve
#define MAX_INTERFACE_NUM      4
//maximum number of endpoints per interface
#define MAX_ENDPOINT_NUM       4
//maximum level of external hub
#define MAX_EXHUB_LEVEL        1

//in 
#define ENDPOINT_OUT           0
//out
#define ENDPOINT_IN            1

//endpoint struct
typedef struct _ENDPOINT
{
	UINT8 EndpointAddr;
	UINT16 MaxPacketSize;
	
	UINT8 EndpointDir : 1;
	UINT8 TOG : 1;
} ENDPOINT, *PENDPOINT;

//hid report define
#define HID_LOCAL_ITEM_TAG_USAGE         0x00

#define HID_GLOBAL_ITEM_TAG_USAGE_PAGE       0x00
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MINIMUM  0x01
#define HID_GLOBAL_ITEM_TAG_LOGICAL_MAXIMUM  0x02
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MINIMUM 0x03
#define HID_GLOBAL_ITEM_TAG_PHYSICAL_MAXIMUM 0x04
#define HID_GLOBAL_ITEM_TAG_REPORT_SIZE      0x07
#define HID_GLOBAL_ITEM_TAG_REPORT_ID        0x08
#define HID_GLOBAL_ITEM_TAG_REPORT_COUNT     0x09

#define HID_MAIN_ITEM_TAG_INPUT          0x08

#define TYPE_MAIN         0
#define TYPE_GLOBAL       1
#define TYPE_LOCAL        2

#define HID_ITEM_FORMAT_SHORT      0
#define HID_ITEM_FORMAT_LONG       1

#define HID_ITEM_TAG_LONG          15

//usage page
#define USAGE_PAGE_NONE            0x00
#define USAGE_PAGE_GENERIC_DESKTOP 0x01
#define USAGE_PAGE_KEYBOARD        0x07
#define USAGE_PAGE_BUTTON          0x09

//usage
#define USAGE_NONE         0x00
#define USAGE_X            0x30
#define USAGE_Y            0x31
#define USAGE_WHEEL        0x38

typedef struct _HID_ITEM_INFO
{
	unsigned int format;
	UINT8 size;
	UINT8 type;
	UINT8 tag;

	union
	{
		UINT8  u8;
		INT8   s8;
		UINT16 u16;
		INT16  s16;
		UINT32 u32;
		INT32  s32;

		const UINT8 *longdata;
	} value;
	
} HID_ITEM;

typedef struct _HID_GLOBAL
{
	UINT32   usagePage;
	INT32    logicalMinimum;
	INT32    logicalMaximum;
	INT32    physicalMinimum;
	INT32    physicalMaximum;
	UINT32   unitExponent;
	UINT32   unit;
	UINT32   reportID;
	UINT32   reportSize;
	UINT32   reportCount;
} HID_GLOBAL;

//hid segement define
#define HID_SEG_KEYBOARD_MODIFIER_INDEX  0
#define HID_SEG_KEYBOARD_VAL_INDEX       1

#define HID_SEG_BUTTON_INDEX             2
#define HID_SEG_X_INDEX                  3
#define HID_SEG_Y_INDEX                  4
#define HID_SEG_WHEEL_INDEX              5

#define HID_SEG_NUM                      6

//hid seg struct
typedef struct _HID_SEG_STRUCT
{
	UINT8 KeyboardReportId;
	UINT8 MouseReportId;
	
	struct
	{
		UINT8 start;
		UINT8 size;
		UINT8 count;
	} HIDSeg[HID_SEG_NUM];
} HID_SEG_STRUCT;

//keyboard parse struct
#define HID_KEYBOARD_VAL_LEN           6
#define MAX_HID_KEYBOARD_BIT_VAL_LEN   15

typedef struct _KEYBOARD_PARSE_STRUCT
{
	UINT8     KeyboardVal[HID_KEYBOARD_VAL_LEN];
	UINT8     KeyboardBitVal[MAX_HID_KEYBOARD_BIT_VAL_LEN]; //for bit value keyboard data
} KEYBOARD_PARSE_STRUCT;

#define MAX_USAGE_NUM      10

typedef struct _USAGE
{
	UINT32  usage;

	UINT8  usageLen;
}USAGE;

//interface struct
typedef struct _INTERFACE
{
	UINT8     InterfaceClass;
	UINT8     InterfaceProtocol;

	UINT16    ReportSize;
	
	UINT8     EndpointNum;   //number of endpoints in this interface
	ENDPOINT  Endpoint[MAX_ENDPOINT_NUM]; //endpoints
	
	HID_SEG_STRUCT  HidSegStruct;	

	KEYBOARD_PARSE_STRUCT KeyboardParseStruct;
} INTERFACE, *PINTERFACE;

//device struct
typedef struct _USB_DEVICE
{
	UINT8       DeviceClass;
	UINT8       MaxPacketSize0;
	
	UINT16      VendorID;
	UINT16      ProductID;
	UINT16      bcdDevice;

	UINT8       DeviceAddress;
	UINT8       DeviceSpeed;
	UINT8       InterfaceNum;
	INTERFACE   Interface[MAX_INTERFACE_NUM];
	
	UINT8       HubPortNum;
} USB_DEVICE, *PUSB_DEVICE;

//hub struct
#define  PORT_DEVICE_NONE         0
#define  PORT_DEVICE_INSERT       1
#define  PORT_DEVICE_ENUM_FAILED  2
#define  PORT_DEVICE_ENUM_SUCCESS 3

typedef struct _USB_HUB_PORT
{
	UINT8  HubPortStatus;
	USB_DEVICE UsbDevice;
	
} USB_HUB_PORT, *USB_PHUB_PORT;

#define  LOW_SPEED      0
#define  FULL_SPEED     1


extern void InitUsbHost(void);
extern void InitUsbData(void);
extern void DealUsbPort(void);
extern void InterruptProcessRootHubPort(UINT8 port_index);
extern void UpdateUsbKeyboardLed(UINT8 led);

#endif


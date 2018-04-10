#include "Type.h"
#include "CH559.h"

UINT8XV  UEP4_1_MOD  _at_ 0x2446;   // endpoint 4/1 mode
UINT8PV pUEP4_1_MOD  _at_ 0x2546;

UINT8XV  UEP2_3_MOD  _at_ 0x2447;   // endpoint 2/3 mode
UINT8PV pUEP2_3_MOD  _at_ 0x2547;

UINT8XV  UEP0_DMA_H  _at_ 0x2448;   // endpoint 0&4 buffer start address high byte
UINT8XV  UEP0_DMA_L  _at_ 0x2449;   // endpoint 0&4 buffer start address low byte
UINT8PV pUEP0_DMA_H  _at_ 0x2548;
UINT8PV pUEP0_DMA_L  _at_ 0x2549;

UINT8XV  UEP1_DMA_H  _at_ 0x244A;   // endpoint 1 buffer start address high byte
UINT8XV  UEP1_DMA_L  _at_ 0x244B;   // endpoint 1 buffer start address low byte
UINT8PV pUEP1_DMA_H  _at_ 0x254A;
UINT8PV pUEP1_DMA_L  _at_ 0x254B;

UINT8XV  UEP2_DMA_H  _at_ 0x244C;   // endpoint 2 buffer start address high byte
UINT8XV  UEP2_DMA_L  _at_ 0x244D;   // endpoint 2 buffer start address low byte
UINT8PV pUEP2_DMA_H  _at_ 0x254C;
UINT8PV pUEP2_DMA_L  _at_ 0x254D;

UINT8XV  UEP3_DMA_H  _at_ 0x244E;   // endpoint 3 buffer start address high byte
UINT8XV  UEP3_DMA_L  _at_ 0x244F;   // endpoint 3 buffer start address low byte
UINT8PV pUEP3_DMA_H  _at_ 0x254E;
UINT8PV pUEP3_DMA_L  _at_ 0x254F;

UINT8XV  LED_STAT    _at_ 0x2880;   // LED status
UINT8PV pLED_STAT    _at_ 0x2980;

UINT8XV  LED_CTRL    _at_ 0x2881;   // LED control
UINT8PV pLED_CTRL    _at_ 0x2981;

UINT8XV  LED_DATA    _at_ 0x2882;   // WriteOnly: data port
UINT8PV pLED_DATA    _at_ 0x2982;

UINT8XV  LED_CK_SE   _at_ 0x2883;   // clock divisor setting
UINT8PV pLED_CK_SE   _at_ 0x2983;

UINT8XV  LED_DMA_AH  _at_ 0x2884;   // DMA address high byte, automatic increasing after DMA
UINT8XV  LED_DMA_AL  _at_ 0x2885;   // DMA address low byte, automatic increasing after DMA
UINT8PV pLED_DMA_AH  _at_ 0x2984;
UINT8PV pLED_DMA_AL  _at_ 0x2985;

UINT8XV  LED_DMA_CN  _at_ 0x2886;   // DMA remainder word count, just main buffer and exclude aux buffer, automatic decreasing after DMA
UINT8PV pLED_DMA_CN  _at_ 0x2986;

UINT8XV  LED_DMA_XH  _at_ 0x2888;   // aux buffer DMA address high byte, automatic increasing after DMA
UINT8XV  LED_DMA_XL  _at_ 0x2889;   // aux buffer DMA address low byte, automatic increasing after DMA
UINT8PV pLED_DMA_XH  _at_ 0x2988;
UINT8PV pLED_DMA_XL  _at_ 0x2989;


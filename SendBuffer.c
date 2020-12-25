#include "Type.h"
#include "Ch559.h"
#include "Mcu.h"

#define REG_THR        SBUF

#define SEND_BUFFER_SIZE   256u

static UINT8 s_send_buffer[SEND_BUFFER_SIZE];

typedef struct
{
    uint8_t *          p_buf;           /**< Pointer to FIFO buffer memory.                      */
    uint16_t           buf_size_mask;   /**< Read/write index mask. Also used for size checking. */
    volatile uint16_t  read_pos;        /**< Next read position in the FIFO buffer.              */
    volatile uint16_t  write_pos;       /**< Next write position in the FIFO buffer.             */
} app_fifo_t;

static app_fifo_t s_send_fifo;

void InitSendBuffer(void)
{
	s_send_fifo.p_buf = s_send_buffer;
	s_send_fifo.buf_size_mask = sizeof(s_send_buffer) - 1;
	
	s_send_fifo.read_pos = 0;
	s_send_fifo.write_pos = 0;
}

BOOL AppendSendBuffer(const UINT8 *pBuffer, UINT16 size)
{
	UINT16 fifoLength;
	UINT16 availableCount;
	
	UINT16 i;
	
	halIntState_t level;
    HAL_ENTER_CRITICAL_SECTION(level);

	fifoLength = s_send_fifo.write_pos - s_send_fifo.read_pos;

	availableCount = s_send_fifo.buf_size_mask - fifoLength + 1;
	
	if (size > availableCount)
	{
		size = availableCount;
	}

	for (i = 0; i < size; i++)
	{
		s_send_fifo.p_buf[s_send_fifo.write_pos & s_send_fifo.buf_size_mask] = *pBuffer++;

		s_send_fifo.write_pos++;
	}

	if (fifoLength == 0)
	{
		UINT8 ch = s_send_fifo.p_buf[s_send_fifo.read_pos & s_send_fifo.buf_size_mask];
		s_send_fifo.read_pos++;

		REG_THR = ch;
	}

    HAL_EXIT_CRITICAL_SECTION(level);
    
	return TRUE;
}

void SendBufferIsr(void) using 1
{
	UINT16 fifoLength = s_send_fifo.write_pos - s_send_fifo.read_pos;

	if (fifoLength > 0)
	{
		UINT8 ch = s_send_fifo.p_buf[s_send_fifo.read_pos & s_send_fifo.buf_size_mask];
		s_send_fifo.read_pos++;

		REG_THR = ch;
	}
}


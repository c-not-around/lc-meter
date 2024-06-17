#include "common.h"
#include "delays.h"
#include "uart_defs.h"
#include "uart.h"


#define TIMEOUT_STEP_US		4
#define TIMEOUT_CYCLES		(1000/TIMEOUT_STEP_US)


void UartInitialization(const uint32_t baudrate, const UART_SETTINGS settings)
{
	uint16_t ubrr = (F_CPU >> 4) / baudrate - 1;
	
	UCON1.UBRR_HIGH = HIGH_BYTE(ubrr);
	UCON0.UBRR_LOW  = LOW_BYTE(ubrr);
	
	UCON1.ACCESS    = CONTROL;
	UCON1.SETTINGS  = settings;
	
	UCON0.TX_ENABLE = TRUE;
	UCON0.RX_ENABLE = TRUE;
}


inline BOOL UartAvailable()
{
	return UCON0.RX_COMPLETE ? TRUE : FALSE;
}

uint8_t UartReadByte()
{
	while (!UCON0.RX_COMPLETE);
	return UCON0.BUFFER;
}

BOOL UartReadBytes(uint16_t timeout, uint8_t *data, uint8_t size)
{
	uint8_t t = TIMEOUT_CYCLES;
	
	while (size && timeout)
	{
		if (UCON0.RX_COMPLETE)
		{
			*data++ = UCON0.BUFFER;
			size--;
		}
		else
		{
			DelayUs(TIMEOUT_STEP_US);
			
			if (--t == 0)
			{
				t = TIMEOUT_CYCLES;
				timeout--;
			}
		}
	}
	
	return size ? FALSE : TRUE;
}


void UartWriteByte(const uint8_t data)
{
	while (!UCON0.BUFFER_EMPTY);
	UCON0.BUFFER = data;
}

void UartWriteBytes(const uint8_t *data, uint8_t size)
{
	while (size--)
	{
		while (!UCON0.BUFFER_EMPTY);
		UCON0.BUFFER = *data++;
	}
}

void UartWriteByteHex(const uint8_t data)
{
	UartWriteByte(Hex(data >> 4));
	UartWriteByte(Hex(data & 0x0F));
}

void UartWriteBytesHex(const uint8_t *data, uint8_t size)
{
	data += size;
	while (size--)
	{
		UartWriteByteHex(*(--data));
	}
}

void UartWriteString(const char *text)
{
	while (*text)
	{
		UartWriteChar(*text++);
	}
}
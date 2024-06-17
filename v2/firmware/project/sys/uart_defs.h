#ifndef _UART_DEFS_H
#define _UART_DEFS_H


#include "types.h"
#include "sfr.h"
#include "uart_settings.h"


typedef struct
{
	/// UBRRL
	uint8_t UBRR_LOW;
	/// UCSRB
	BOOL TX_BIT8            : 1;
	const BOOL RX_BIT8      : 1;
	BOOL WORD_SIZE_9BIT     : 1;
	BOOL TX_ENABLE          : 1;
	BOOL RX_ENABLE          : 1;
	BOOL UDR_INT_ENABLE     : 1;
	BOOL TX_INT_ENABLE      : 1;
	BOOL RX_INT_ENABLE      : 1;
	/// UCSRA
	BOOL MPCM               : 1;
	BOOL SPEED2X            : 1;
	const BOOL PARITY_ERROR : 1;
	const BOOL BUF_OVERFLOW : 1;
	const BOOL FRAME_ERROR  : 1;
	const BOOL BUFFER_EMPTY : 1;
	const BOOL TX_COMPLETE  : 1;
	const BOOL RX_COMPLETE  : 1;
	/// UDR
	uint8_t BUFFER;
} UCON_0;

typedef union
{
	/// UBRRH
	struct
	{
		uint8_t UBRR_HIGH : 4;
		UNUSED_FIELD      : 4;
	};
	/// UCSRC
	union
	{
		struct
		{
			enum
			{
				DIRECT  = 0,
				INVERSE = 1
			} CLOCK_POLARITY : 1;
			enum
			{
				ASYNC = 0,
				SYNC  = 1
			} MODE           : 1;
			enum
			{
				BAUD_GEN = 0,
				CONTROL  = 1
			} ACCESS         : 1;
		};
		UART_SETTINGS SETTINGS;
	};
} UCON_1;


volatile SFR_REG(0x09) UCON_0 UCON0;
volatile SFR_REG(0x20) UCON_1 UCON1;


#endif
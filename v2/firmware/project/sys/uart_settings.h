#ifndef _UART_SETTINGS_H
#define _UART_SETTINGS_H


#define UART_5N1		{FIVE,NONE,ONE_BIT}
#define UART_5N2		{FIVE,NONE,TWO_BITS}
#define UART_5E1		{FIVE,EVEN,ONE_BIT}
#define UART_5E2		{FIVE,EVEN,TWO_BITS}
#define UART_5O1		{FIVE,ODD,ONE_BIT}
#define UART_5O2		{FIVE,ODD,TWO_BITS}
#define UART_6N1		{SIX,NONE,ONE_BIT}
#define UART_6N2		{SIX,NONE,TWO_BITS}
#define UART_6E1		{SIX,EVEN,ONE_BIT}
#define UART_6E2		{SIX,EVEN,TWO_BITS}
#define UART_6O1		{SIX,ODD,ONE_BIT}
#define UART_6O2		{SIX,ODD,TWO_BITS}
#define UART_7N1		{SEVEN,NONE,ONE_BIT}
#define UART_7N2		{SEVEN,NONE,TWO_BITS}
#define UART_7E1		{SEVEN,EVEN,ONE_BIT}
#define UART_7E2		{SEVEN,EVEN,TWO_BITS}
#define UART_7O1		{SEVEN,ODD,ONE_BIT}
#define UART_7O2		{SEVEN,ODD,TWO_BITS}
#define UART_8N1		{EIGHT,NONE,ONE_BIT}
#define UART_8N2		{EIGHT,NONE,TWO_BITS}
#define UART_8E1		{EIGHT,EVEN,ONE_BIT}
#define UART_8E2		{EIGHT,EVEN,TWO_BITS}
#define UART_8O1		{EIGHT,ODD,ONE_BIT}
#define UART_8O2		{EIGHT,ODD,TWO_BITS}


typedef struct
{
	UNUSED_FIELD   : 1;
	enum
	{
		FIVE  = 0,
		SIX   = 1,
		SEVEN = 2,
		EIGHT = 3
	} DATA_BITS    : 2;
	enum
	{
		ONE_BIT  = 0,
		TWO_BITS = 1
	} STOP_BITS    : 1;
	enum
	{
		NONE = 0,
		EVEN = 2,
		ODD  = 3
	} PARITY       : 2;
	UNUSED_FIELD   : 2;
} UART_SETTINGS;



#endif
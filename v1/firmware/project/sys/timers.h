#ifndef _TIMERS_H
#define _TIMERS_H


#include "types.h"
#include "sfr.h"


typedef enum
{
	T_DISABLE  = 0,
	T_DIV_1    = 1,
	T_DIV_8    = 2,
	T_DIV_64   = 3,
	T_DIV_256  = 4,
	T_DIV_1024 = 5,
	T_EXT_FALL = 6,
	T_EXT_RISE = 7
} T_CS;

typedef struct
{
	/// TCNT0
	uint8_t COUNTER;
	/// TCCR0
	struct
	{
		T_CS CLOCK_SOURCE : 3;
		UNUSED_FIELD      : 5;
	};
} TMR_0;

typedef	enum
{
	TO_OVERFLOW      = 0b00000,
	PWM_8BIT         = 0b00001,
	PWM_9BIT         = 0b00010,
	PWM_10BIT        = 0b00011,
	RESET_COMPARE    = 0b01000,
	PWM_8BIT_FAST    = 0b01001,
	PWM_9BIT_FAST    = 0b01010,
	PWM_10BIT_FAST   = 0b01011,
	PWM_CAPTURE_PFC  = 0b10000,
	PWM_COMPARE_PFC  = 0b10001,
	PWM_CAPTURE_PC   = 0b10010,
	PWM_COMPARE_PC   = 0b10011,
	RESET_CAPTURE    = 0b11000,
	PWM_CAPTURE_FAST = 0b11010,
	PWM_COMPARE_FAST = 0b11011
} T1_MODE;

typedef enum
{
	NORMAL = 0,
	TOGGLE = 1,
	CLEAR  = 2,
	SET    = 3
} ON_CMP;

typedef struct
{
	/// ICR1L
	uint16_t CAPTURE;
	/// OCR1BL
	uint16_t COMPARE_B;
	/// OCR1AL
	uint16_t COMPARE_A;
	/// TCNT1L
	uint16_t COUNTER;
	/// TCCR1B
	struct
	{
		T_CS CLOCK_SOURCE : 3;
		T1_MODE MODE_HIGH : 2;
		UNUSED_FIELD      : 1;
		EDGE CAPTURE_EDGE : 1;
		BOOL FILTER       : 1;
	};
	/// TCCR1A
	struct
	{
		T1_MODE MODE_LOW     : 2;
		BOOL FORCE_COMPARE_B : 1;
		BOOL FORCE_COMPARE_A : 1;
		ON_CMP ON_COMPARE_B  : 2;
		ON_CMP ON_COMPARE_A  : 2;
	};
} TMR_1;

typedef enum
{
	T2_DISABLE  = 0,
	T2_DIV_1    = 1,
	T2_DIV_8    = 2,
	T2_DIV_32   = 3,
	T2_DIV_64   = 4,
	T2_DIV_128  = 5,
	T2_DIV_256  = 6,
	T2_DIV_1024 = 7
} T2_CS;

typedef	enum
{
	TO_RESET   = 0b00000000,
	PWM_PC     = 0b01000000,
	TO_CAPTURE = 0b00001000,
	PWM_FAST   = 0b01001000
} T2_MODE;

typedef struct
{
	/// ASSR
	struct
	{
		BOOL CONTROL_REG_BUSY : 1;
		BOOL COMPARE_REG_BUSY : 1;
		BOOL COUNTER_REG_BUSY : 1;
		BOOL EXTERNAL_CLOCK   : 1;
		UNUSED_FIELD          : 4;
	};
	/// OCR2
	uint8_t COMPARE;
	/// TCNT2
	uint8_t COUNTER;
	/// TCCR2
	union
	{
		struct
		{
			T2_CS CLOCK_SOURCE : 3;
			UNUSED_FIELD       : 1;
			ON_CMP ON_COMPARE  : 2;
			UNUSED_FIELD       : 1;
			BOOL FORCE_COMPARE : 1;
		};
		struct
		{
			UNUSED_FIELD      : 3;
			T2_MODE MODE_HIGH : 1;
			UNUSED_FIELD      : 2;
			T2_MODE MODE_LOW  : 1;
			UNUSED_FIELD      : 1;
		};
	};
} TMR_2;

typedef struct
{
	/// TIFR
	struct
	{
		BOOL T0_OVERFLOW  : 1;
		UNUSED_FIELD            : 1;
		BOOL T1_OVERFLOW  : 1;
		BOOL T1_COMPARE_B : 1;
		BOOL T1_COMPARE_A : 1;
		BOOL T1_CAPTURE   : 1;
		BOOL T2_OVERFLOW  : 1;
		BOOL T2_COMPARE   : 1;
	};
	/// TIMSK
	struct
	{
		BOOL T0_OVF_ENABLE  : 1;
		UNUSED_FIELD        : 1;
		BOOL T1_OVF_ENABLE  : 1;
		BOOL T2_CMPB_ENABLE : 1;
		BOOL T2_CMPA_ENABLE : 1;
		BOOL T2_CAPT_ENABLE : 1;
		BOOL T2_OVF_ENABLE  : 1;
		BOOL T2_CMP_ENABLE  : 1;
	};
} TMR_I;


volatile SFR_REG(0x32) TMR_0 TIMER0;
volatile SFR_REG(0x26) TMR_1 TIMER1;
volatile SFR_REG(0x22) TMR_2 TIMER2;
volatile SFR_REG(0x38) TMR_I TMRINT;


#endif
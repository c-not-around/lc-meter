#include "sys/common.h"
#include "sys/ports.h"
#include "sys/timers.h"
#include "sys/interrupts.h"
#include "sys/ext_int.h"
#include "sys/delays.h"
#include "sys/uart.h"
#include "buttons.h"
#include "lcd1602.h"
#include <math.h>
#include <stdio.h>
#include <stdlib.h>


#define RELAY_1				PORTS.D.O4
#define RELAY_2				PORTS.D.O3
#define CE_DISCHARGE		PORTS.D.O6

#define ONE_MILLISECOND		1000UL // milliseconds per one second
#define MATH_PI				3.14159265358979d
#define C_CAL				3.291e-9d // measured using XJW-01
#define CE_CHARGE_R1		2916.0d  // measured using XJW-01
#define CE_CHARGE_R2		293.3d   // measured using XJW-01

#define LC_CAPT_TIMER_FSR	256
#define LC_CAPT_TIMER_DIV	64
#define LC_CAPT_TIMER_MOD	(uint8_t)(LC_CAPT_TIMER_FSR-F_CPU/(LC_CAPT_TIMER_DIV*ONE_MILLISECOND))
#define LC_CAPT_PERIOD_MS	250
#define LC_PERIODS_LSB		(ONE_MILLISECOND/LC_CAPT_PERIOD_MS) // periods per ONE second
#define CE_TIMER_DIV		8
#define CE_PERIOD_LSB		((double)CE_TIMER_DIV/F_CPU) // seconds per one timer tick

#define MODE_POWER_ON		'P'
#define MODE_CALIBRATE		'A'
#define MODE_CALIBRATED		'B'
#define MODE_PREPARE_CX		'C'
#define MODE_PREPARE_LX		'L'
#define MODE_MEASURE_CX		'F' //   1pF .. 1uF
#define MODE_MEASURE_LX		'H' // 100nH .. 5H
#define MODE_PREPARE_CE1	'1'
#define MODE_PREPARE_CE2	'2'
#define MODE_MEASURE_CE1	'D' //  5nF .. 1mF
#define MODE_MEASURE_CE2	'E' // 50nF .. 10mF

#define T_RELAY_SWITCH_MS	50
#define T_CE_DISCHARGE_MS	50 // not enough for large capacitors (~2mF). need to change the transistor to MOSFET


const uint8_t mu[] =
{
	_____,
	_____,
	X___X,
	X___X,
	X__XX,
	XXX_X,
	X____,
	X____
};


volatile uint16_t T1;     // timer1 extension (LC period)
volatile uint8_t  T2;     // timer2 extension (LC freq. capture period)
volatile uint32_t T;      // LC period
volatile BOOL     Done;   // LC period measure complete
volatile uint32_t Tce;    // timer0 extension (CE period)


inline double sqr(double x)
{
	return x * x;
}

void PrintQuantity(uint8_t column, uint8_t line, double value, char dimension)
{
	uint8_t n = 0;
	char text[9];
	
	if (signbit(value))
	{
		value = -value;
		text[n++] = '-';
	}
	
	uint8_t prefix = 0;
	if (value != 0.0)
	{
		while (value < 1.0 && prefix < 4)
		{
			value *= 1000.0;
			prefix++;
		}
		
		switch (prefix)
		{
			case 0: break;
			case 1: prefix = 'm';  break; // milli
			case 2: prefix = '\1'; break; // micro
			case 3: prefix = 'n';  break; // nano
			case 4: prefix = 'p';  break; // pico
			default: prefix = '_'; break; // wtf?
		}
	}
	
	char *format;
	if (value == 0.0)
	{
		format = "%1.1f";
	}
	else if (value < 10.0)
	{
		format = "%1.3f";
	}
	else if (value < 100.0)
	{
		format = "%2.2f";
	}
	else
	{
		format = "%3.1f";
	}
	
	n += sprintf(text + n, format, value);
	if (prefix)
	{
		text[n++] = prefix;
	}
	text[n++] = dimension;
	for (; n < 8; text[n++] = ' ');
	text[n] = '\0';
	
	Lcd1602SetCursor(column, line);
	Lcd1602WriteString(text);
}

void PrintFrequency(uint32_t freq)
{
	char text[9];
	for (int n = sprintf(text, "%luHz", freq); n < 8; text[n++] = ' ');
	text[8] = '\0';
	
	Lcd1602SetCursor(8, 0);
	Lcd1602WriteString(text);
}

void LCMeasureBegin()
{
	INT_DISABLE();
	
	TIMER1.CLOCK_SOURCE = T_DISABLE;
	TIMER2.CLOCK_SOURCE = T2_DISABLE;

	T    = 0;
	Done = FALSE;
	
	T1   = 0;
	T2   = 0;
	
	TIMER1.COUNTER = 0;
	TIMER2.COUNTER = LC_CAPT_TIMER_MOD;
	
	TMRINT.T1_OVERFLOW = TRUE; // interrupt flags reset by write '1' (sic!)
	TMRINT.T2_OVERFLOW = TRUE;
	
	TMRINT.T1_OVF_ENABLE = TRUE;
	TMRINT.T2_OVF_ENABLE = TRUE;
	
	TIMER1.CLOCK_SOURCE  = T_EXT_FALL;
	TIMER2.CLOCK_SOURCE  = T2_DIV_64;
	
	INT_ENABLE();
}

void CEMeasureBegin(uint16_t *timer)
{
	DelayMs(T_CE_DISCHARGE_MS);
	
	*timer = 20000; // 20k x 100us = 2s
	
	INT_DISABLE();
	
	Tce  = 0;
	Done = FALSE;
	
	TIMER0.COUNTER = 0;
	
	EXTINT0.CH0_REQUEST = TRUE; // interrupt flags reset by write '1' (sic!)
	TMRINT.T0_OVERFLOW  = TRUE; 
	
	EXTINT0.CH0_ENABLE   = TRUE;
	TMRINT.T0_OVF_ENABLE = TRUE;

	CE_DISCHARGE        = LOW;
	TIMER0.CLOCK_SOURCE = T_DIV_8;
	
	INT_ENABLE();
}

MAIN()
{
	INITIALIZATION()
	{
		/*
		 * PB7 - I - unused
		 * PB6 - I - unused
		 * PB5 - O - D7 HD44780
		 * PB4 - O - D6 HD44780
		 * PB3 - O - D5 HD44780
		 * PB2 - O - D4 HD44780
		 * PB1 - O - EN HD44780
		 * PB0 - O - RS HD44780 
		 */
		PORTS.B.OUT = 0b11000000;
		PORTS.B.DIR = 0b00111111;
	
		/*
		 * PC7 - X - not implemented
		 * PC6 - S - #RESET
		 * PC5 - I - button "Calibrate"
		 * PC4 - I - unused
		 * PC3 - I - unused
		 * PC2 - I - unused
		 * PC1 - I - unused
		 * PC0 - I - unused
		 */
		PORTS.C.OUT = 0b11111111;
		PORTS.C.DIR = 0b00000000;
	
		/*
		 * PD7 - I - button "Next Mode"
		 * PD6 - O - K3     CE discharge
		 * PD5 - I - F_IN   LC frequency input         
		 * PD4 - O - K1     relay1
		 * PD3 - O - K2     relay2
		 * PD2 - I - T_IN   CE charge period input
		 * PD1 - I - unused
		 * PD0 - I - unused
		 */
		PORTS.D.OUT = 0b11000111;
		PORTS.D.DIR = 0b01011000;
		
		TIMER1.MODE_HIGH  = TO_OVERFLOW;
		TIMER1.MODE_LOW   = TO_OVERFLOW;
		
		TIMER2.MODE_HIGH  = TO_RESET;
		TIMER2.MODE_LOW   = TO_RESET;
		
		EXTINT1.CH0_TRIGGER = EDGE_FALL;
		
		Lcd1602Initialization();
		Lcd1602LoadSymbol('\1', mu);
		
		//UART_SETTINGS settings = UART_8N1;
		//UartInitialization(9600UL, settings);
	}
	
	Lcd1602WriteString("  L-C-CE Meter\n   2021-07-23");
	while (ButtonsRead()); // wait charge debounce cap

	MAIN_LOOP()
	{
		static double f1;
		static double L;
		static double C;
		
		static uint16_t timeout;

		static char mode = MODE_POWER_ON;
		
		switch (mode)
		{
			case MODE_POWER_ON:
			{
				
			} break;
			
			case MODE_CALIBRATE:
			{
				RELAY_1 = LOW;
				RELAY_2 = LOW;
				DelayMs(6*T_RELAY_SWITCH_MS); // relay turning off is slower than turning on 
				
				LCMeasureBegin();
				
				Lcd1602Clear();
				Lcd1602WriteString("Calibration   0%");
				
				uint32_t F = 0;
				uint8_t progress = 0;
				while (progress < LCD1602_COLS)
				{
					if (Done)
					{
						INT_DISABLE();
						F += T;
						INT_ENABLE();
						
						if (++progress == (LCD1602_COLS/2))
						{
							RELAY_2 = HIGH;
							DelayMs(T_RELAY_SWITCH_MS);
							
							f1 = ((double)F) / ((double)(LCD1602_COLS/2));
							F = 0;
						}
						
						LCMeasureBegin();
						
						char text[5];
						sprintf(text, "%3.0f%%", (100.0*progress)/LCD1602_COLS);
						Lcd1602SetCursor(12, 0);
						Lcd1602WriteString(text);
						Lcd1602SetCursor(progress-1, 1);
						Lcd1602WriteChar('\xFF');
					}
				}
				
				RELAY_2 = LOW;
				DelayMs(T_RELAY_SWITCH_MS);
				
				LCMeasureBegin();
				
				double f2 = ((double)F) / ((double)(LCD1602_COLS/2));
				double q2 = sqr(f1 / f2) - 1.0d;
				
				C  = C_CAL / q2;
				L  = q2 / (sqr(2*MATH_PI*f1) * C_CAL);
				
				Lcd1602Clear();
				Lcd1602WriteString("[Cal] F=");
				PrintQuantity(0, 1, C, 'F');
				PrintQuantity(8, 1, L, 'H');
				
				mode = MODE_CALIBRATED;
			} break;
			
			case MODE_CALIBRATED:
			{
				if (Done)
				{
					INT_DISABLE();
					uint32_t F = T;
					INT_ENABLE();
					
					LCMeasureBegin();
					
					PrintFrequency(F);
				}
			} break;
			
			case MODE_PREPARE_CX:
			case MODE_PREPARE_LX:
			{
				RELAY_1 = mode == MODE_PREPARE_LX ? HIGH : LOW;
				RELAY_2 = LOW;
				DelayMs(T_RELAY_SWITCH_MS);
				
				LCMeasureBegin();
				
				char text[] =
				{
					'[', '-', mode, '-', ']', ' ', 'F', '\n',
					' ', ' ', mode, 'x', ' ', '=', ' ', '\0'
				};
				Lcd1602SetCursor(0, 0);
				Lcd1602WriteString(text);
				
				mode = mode == MODE_PREPARE_CX ? MODE_MEASURE_CX : MODE_MEASURE_LX;
				
				PrintQuantity(7, 1, 0.0, mode);
			} break;
			
			case MODE_MEASURE_CX:
			case MODE_MEASURE_LX:
			{
				if (Done)
				{
					INT_DISABLE();
					uint32_t F = T;
					INT_ENABLE();
					
					LCMeasureBegin();
					
					double f3 = (double)F;
					double Qx = sqr(f1 / f3) - 1.0d;
					
					if (mode == MODE_MEASURE_CX)
					{
						Qx *= C;
					}
					else if (mode == MODE_MEASURE_LX)
					{
						if (F < (1*KHZ))
						{
							Qx = 0.0;
						}
						else
						{
							Qx *= L;
						}
						//Qx *= F < 1*KHZ ? 0.0 : L;
					}
					
					PrintFrequency(F);
					PrintQuantity(7, 1, Qx, mode);
				}
			} break;
			
			case MODE_PREPARE_CE1:
			case MODE_PREPARE_CE2:
			{
				RELAY_1 = LOW;
				RELAY_2 = mode == MODE_PREPARE_CE2 ? HIGH : LOW;
				CE_DISCHARGE = HIGH;
				
				CEMeasureBegin(&timeout);
				
				char text[] =
				{
					'[', 'C', 'E', mode, ']', ' ', 'T', '\n',
					' ', 'C', 'E', '\0'
				};
				Lcd1602SetCursor(0, 0);
				Lcd1602WriteString(text);
				PrintQuantity(8, 0, 0.0, 's');
				PrintQuantity(7, 1, 0.0, 'F');
				
				mode = mode == MODE_PREPARE_CE1 ? MODE_MEASURE_CE1 : MODE_MEASURE_CE2;
			} break;
			
			case MODE_MEASURE_CE1:
			case MODE_MEASURE_CE2:
			{
				if (Done || timeout == 0)
				{
					INT_DISABLE();
					TIMER0.CLOCK_SOURCE = T_DISABLE;
					uint32_t Te = (Tce << 8) | TIMER0.COUNTER;
					INT_ENABLE();
					
					double t  = CE_PERIOD_LSB * ((double)Te);
					PrintQuantity(8, 0, t, 's');

					if (Done)
					{
						if (Te > 9)
						{
							double Ce = t / (mode == MODE_MEASURE_CE1 ? CE_CHARGE_R1 : CE_CHARGE_R2);
							PrintQuantity(7, 1, Ce, 'F');
						}
						else // under limit (10us <=> 5nF/50nF)
						{
							Lcd1602SetCursor(7, 1);
							Lcd1602WriteString("UL      "); 
						}
					}
					else // timedout -> over limit (2 seconds <=> 1mF/10mF)
					{
						CE_DISCHARGE = HIGH;

						Lcd1602SetCursor(7, 1);
						Lcd1602WriteString("OL      "); 
					}
					
					CEMeasureBegin(&timeout);
				}
				else
				{
					DelayUs(100-10.6); // 20k x 100us = 2s
					timeout--;
				}
			} break;
			
			default: break;
		}
		
		uint8_t buttons = ButtonsRead();
		if (buttons)
		{
			static char ret = MODE_PREPARE_CX;
			
			if (buttons & BUTTON_MODE)
			{
				switch (mode)
				{
					case MODE_POWER_ON:
					{
						mode = MODE_CALIBRATE;
					} break;
					
					case MODE_CALIBRATED:
					{
						mode = ret;
					} break;
					
					case MODE_MEASURE_CX:
					{
						mode = MODE_PREPARE_LX;
					} break;
					
					case MODE_MEASURE_LX:
					{
						TMRINT.T1_OVF_ENABLE = FALSE;
						TMRINT.T2_OVF_ENABLE = FALSE;
						
						mode = MODE_PREPARE_CE1;
					} break;
					
					case MODE_MEASURE_CE1:
					{
						mode = MODE_PREPARE_CE2;
					} break;
					
					case MODE_MEASURE_CE2:
					{
						EXTINT0.CH0_ENABLE   = FALSE;
						TMRINT.T0_OVF_ENABLE = FALSE;
						
						CE_DISCHARGE = HIGH;
						
						mode = MODE_PREPARE_CX;
					} break;
					
					default:
					{
						mode = MODE_POWER_ON;
					} break;
				}
			}
			else if (buttons & BUTTON_CALIB)
			{
				if (mode == MODE_MEASURE_CX || mode == MODE_MEASURE_LX)
				{
					ret = mode == MODE_MEASURE_LX ? MODE_PREPARE_LX : MODE_PREPARE_CX;
					mode = MODE_CALIBRATE;
				}
			}
		}
	}
}

INT_HANDLER(IV_TMR1_OVERLOW)
{
	T1++; 
}

INT_HANDLER(IV_TMR2_OVERLOW)
{
	TIMER2.COUNTER = LC_CAPT_TIMER_MOD;
	
	if (++T2 == LC_CAPT_PERIOD_MS)
	{
		TIMER1.CLOCK_SOURCE = T_DISABLE;
		TIMER2.CLOCK_SOURCE = T2_DISABLE;
		
		TMRINT.T1_OVF_ENABLE = FALSE;
		TMRINT.T2_OVF_ENABLE = FALSE;
		
		T    = LC_PERIODS_LSB * (((uint32_t)T1 << 16) | TIMER1.COUNTER);
		Done = TRUE;
	}
}

INT_HANDLER(IV_TMR0_OVERLOW)
{
	Tce++;
}

INT_HANDLER(IV_INT0_REQUEST)
{
	TIMER0.CLOCK_SOURCE = T_DISABLE;
	
	CE_DISCHARGE = HIGH;
	
	TMRINT.T0_OVF_ENABLE = FALSE;
	EXTINT0.CH0_ENABLE   = FALSE;
	
	Done = TRUE;
}
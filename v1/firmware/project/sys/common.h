#ifndef _COMMON_DEFS_H
#define _COMMON_DEFS_H


#include "types.h"


#define KHZ					1000UL
#define MHZ					1000000UL

#ifndef F_CPU
#define F_CPU				(8*MHZ)
#endif

#define BYTE_LSB			(1<<0)
#define BYTE_MSB			(1<<7)

#define LOW_BYTE(x)			(*((uint8_t*)&(x)+0))
#define HIGH_BYTE(x)		(*((uint8_t*)&(x)+1))
#define LOW_WORD(x)			(*((uint16_t*)&(x)+0))
#define HIGH_WORD(x)		(*((uint16_t*)&(x)+1))
#define BYTE(x,n)			(*((uint8_t*)&(x)+(n)))

#define Hex(x)				(((x)<10?'0':'7')+(x))

#define MAIN()				int main()
#define INITIALIZATION()	
#define MAIN_LOOP()			while (1)


#endif
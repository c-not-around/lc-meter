#ifndef _PORTS_H
#define _PORTS_H


#include "types.h"
#include "sfr.h"


typedef union
{
	struct
	{
		/// PINx
		LOGIC I0 : 1;
		LOGIC I1 : 1;
		LOGIC I2 : 1;
		LOGIC I3 : 1;
		LOGIC I4 : 1;
		LOGIC I5 : 1;
		LOGIC I6 : 1;
		LOGIC I7 : 1;
		/// DDRx
		LOGIC D0 : 1;
		LOGIC D1 : 1;
		LOGIC D2 : 1;
		LOGIC D3 : 1;
		LOGIC D4 : 1;
		LOGIC D5 : 1;
		LOGIC D6 : 1;
		LOGIC D7 : 1;
		/// PORTx
		LOGIC O0 : 1;
		LOGIC O1 : 1;
		LOGIC O2 : 1;
		LOGIC O3 : 1;
		LOGIC O4 : 1;
		LOGIC O5 : 1;
		LOGIC O6 : 1;
		LOGIC O7 : 1;
	};
	struct
	{
		uint8_t IN;  // PINx
		uint8_t DIR; // DDRx
		uint8_t OUT; // PORTx
	};
} GPIO;

typedef struct
{
	GPIO D;
	GPIO C;
	GPIO B;
} PORT;


volatile SFR_REG(0x10) PORT PORTS;


#endif
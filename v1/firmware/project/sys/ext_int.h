#ifndef _EXT_INT
#define _EXT_INT


#include "types.h"
#include "sfr.h"


typedef struct
{
	/// GIFR
	struct  
	{
		UNUSED_FIELD     : 6;
		BOOL CH0_REQUEST : 1;
		BOOL CH1_REQUEST : 1;
	};
	/// GICR
	struct 
	{
		BOOL REMAP_ENABLE  : 1;
		BOOL REMAP_TO_BOOT : 1;
		UNUSED_FIELD       : 4;
		BOOL CH0_ENABLE    : 1;
		BOOL CH1_ENABLE    : 1;
	};
} EXT_INT_0;

typedef enum
{
	LEVEL_LOW_   = 0,
	LEVEL_TOGGLE = 1,
	EDGE_FALL    = 2,
	EDGE_RISE    = 3
} EI_TRIG;

typedef struct
{
	EI_TRIG CH0_TRIGGER : 2;
	EI_TRIG CH1_TRIGGER : 2;
	UNUSED_FIELD      : 4;
} EXT_INT_1;


volatile SFR_REG(0x3A) EXT_INT_0 EXTINT0;
volatile SFR_REG(0x35) EXT_INT_1 EXTINT1;


#endif
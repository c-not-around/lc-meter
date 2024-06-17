#ifndef _DELAYS_H
#define _DELAYS_H


#include <util/delay.h>
#include "types.h"


#define DelayUs(t)	_delay_us(t)
#define DelayMs(t)	_delay_ms(t)


void DelayUsRunTime(uint16_t t);



#endif
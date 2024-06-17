#ifndef _BUTTONS_H
#define _BUTTONS_H


#include "sys/types.h"


#define BUTTON_MODE		(1 << 7)
#define BUTTON_CALIB	(1 << 0)


uint8_t ButtonsRead();


#endif
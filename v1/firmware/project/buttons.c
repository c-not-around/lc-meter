#include "sys/ports.h"
#include "buttons.h"


#define BUTTONS_PORT_1	PORTS.D.IN
#define BUTTONS_PORT_2	PORTS.C.IN
#define BUTTONS_MASK_1	BUTTON_MODE
#define BUTTONS_MASK_2	BUTTON_CALIB


uint8_t ButtonsRead()
{
	static uint8_t Triggers = BUTTONS_MASK_1 | BUTTONS_MASK_2;
	
	unsigned char Buttons = (BUTTONS_PORT_1 & BUTTONS_MASK_1) | (BUTTONS_PORT_1 & BUTTONS_MASK_1);
	unsigned char result  = ~Buttons & Triggers;
	Triggers = Buttons;
	
	return result;
}

#include "sys/ports.h"
#include "buttons.h"


#define BUTTONS_PORT	PORTS.D.IN
#define BUTTONS_MASK	(BUTTON_MODE | BUTTON_CALIB)


uint8_t ButtonsRead()
{
	static uint8_t Triggers = BUTTONS_MASK;
	
	unsigned char Buttons = BUTTONS_PORT & BUTTONS_MASK;
	unsigned char result  = ~Buttons & Triggers;
	Triggers = Buttons;
	
	return result;
}
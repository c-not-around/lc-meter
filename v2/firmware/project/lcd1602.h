#ifndef _LCD1602_H
#define _LCD1602_H


#include "sys/types.h"


#define LCD1602_COLS	16
#define LCD1602_ROWS	2

#define _____			0x00
#define ____X			0x01
#define ___X_			0x02
#define ___XX			0x03
#define __X__			0x04
#define __X_X			0x05
#define __XX_			0x06
#define __XXX			0x07
#define _X___			0x08
#define _X__X			0x09
#define _X_X_			0x0A
#define _X_XX			0x0B
#define _XX__			0x0C
#define _XX_X			0x0D
#define _XXX_			0x0E
#define _XXXX			0x0F
#define X____			0x10
#define X___X			0x11
#define X__X_			0x12
#define X__XX			0x13
#define X_X__			0x14
#define X_X_X			0x15
#define X_XX_			0x16
#define X_XXX			0x17
#define XX___			0x18
#define XX__X			0x19
#define XX_X_			0x1A
#define XX_XX			0x1B
#define XXX__			0x1C
#define XXX_X			0x1D
#define XXXX_			0x1E
#define XXXXX			0x1F

#define CASTOM_SYMBOL_1	'\1'
#define CASTOM_SYMBOL_2	'\2'
#define CASTOM_SYMBOL_3	'\3'
#define CASTOM_SYMBOL_4	'\4'
#define CASTOM_SYMBOL_5	'\5'
#define CASTOM_SYMBOL_6	'\6'
#define CASTOM_SYMBOL_7	'\7'


void Lcd1602Initialization();
void Lcd1602LoadSymbol(const uint8_t code, const uint8_t *image);

void Lcd1602Clear();
void Lcd1602SetCursor(const uint8_t x, const uint8_t y);

void Lcd1602WriteChar(const char c);
void Lcd1602WriteString(const char *text);


#endif
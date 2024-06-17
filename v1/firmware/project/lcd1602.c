#include "sys/common.h"
#include "sys/ports.h"
#include "sys/delays.h"
#include "lcd1602.h"


#define BITS_SERIAL			(1==1)

#define RS					PORTS.B.O0
#define EN					PORTS.B.O1
#if BITS_SERIAL
#define BUS					PORTS.B.OUT
#define BUS_SHIFT			2
#define BUS_MASK			(0xF0>>BUS_SHIFT)
#define HIGH_TETRADE(b)		((BUS&(~BUS_MASK))|((b>>BUS_SHIFT)&BUS_MASK))
#define LOW_TETRADE(b)		((BUS&(~BUS_MASK))|((b<<(4-BUS_SHIFT))&BUS_MASK))
#else
#define D4					PORTS.B.O4
#define D5					PORTS.B.O3
#define D6					PORTS.B.O2
#define D7					PORTS.B.O1
#endif

#define RS_COMMAND			LOW
#define RS_DATA				HIGH

#define CMD_SCREEN_CLEAR	0x01
#define CMD_CURSOR_RETURN	0x02
#define CMD_INPUT_SET		0x04
#define CMD_DISPLAY			0x08
#define CMD_SHIFT			0x10
#define CMD_FUNCTION_SET	0x20
#define CMD_SET_ADR_CGRAM	0x40
#define CMD_SET_ADR_DDRAM	0x80

#define INPUT_SET_INCREMENT	0x02
#define INPUT_SET_DECREMENT	0x00
#define INPUT_SET_SIFHT		0x01
#define INPUT_SET_NO_SIFHT	0x00

#define DISPLAY_ON			0x04
#define DISPLAY_OFF			0x00
#define DISPLAY_CURSOR_ON	0x02
#define DISPLAY_CURSOR_OFF	0x00
#define DISPLAY_BLINK_ON	0x01
#define DISPLAY_BLINK_OFF	0x00

#define SHIFT_ENABLE		0x08
#define SHIFT_DISABLE		0x00
#define SHIFT_RIGHT			0x04
#define SHIFT_LEFT			0x00

#define FUNCTION_SET_8BIT	0x10
#define FUNCTION_SET_4BIT	0x00
#define FUNCTION_SET_2ROWS	0x08
#define FUNCTION_SET_1ROWS	0x00
#define FUNCTION_SET_5X10	0x04
#define FUNCTION_SET_5X7	0x00

#define T_CMD_CLEAR_US		1600
#define T_CMD_CURSOR_US		1600
#define T_CMD_US			50

#define ROW1_ADDRESS		0x40
#define ROW2_ADDRESS		0x10


typedef struct
{
	uint8_t X : 5;
	uint8_t Y : 3;
} POINT;


static POINT Cursor = { 0, 0 };


void Lcd1602WriteByte(const uint8_t byte)
{
#if BITS_SERIAL
	EN  = HIGH;
	BUS = HIGH_TETRADE(byte);
	EN  = LOW;
	
	EN  = HIGH;
	BUS = LOW_TETRADE(byte);
	EN  = LOW;
#else
	EN = HIGH;
	D4 = byte & 0x10 ? HIGH : LOW;
	D5 = byte & 0x20 ? HIGH : LOW;
	D6 = byte & 0x40 ? HIGH : LOW;
	D7 = byte & 0x80 ? HIGH : LOW;
	EN = LOW;
	
	EN = HIGH;
	D4 = byte & 0x01 ? HIGH : LOW;
	D5 = byte & 0x02 ? HIGH : LOW;
	D6 = byte & 0x04 ? HIGH : LOW;
	D7 = byte & 0x08 ? HIGH : LOW;
	EN = LOW;
#endif
	DelayUs(T_CMD_US);
}

inline void Lcd1602WriteCommand(const uint8_t command)
{
	RS = RS_COMMAND;
	Lcd1602WriteByte(command);
}

inline void Lcd1602WriteData(const uint8_t data)
{
	RS = RS_DATA;
	Lcd1602WriteByte(data);
}


void Lcd1602Initialization()
{
	RS = RS_COMMAND;
	
	Lcd1602WriteByte(CMD_CURSOR_RETURN);
	DelayUs(T_CMD_CURSOR_US);
	
	Lcd1602WriteByte(CMD_FUNCTION_SET | FUNCTION_SET_4BIT | FUNCTION_SET_2ROWS | FUNCTION_SET_5X7);
	Lcd1602WriteByte(CMD_SHIFT | SHIFT_DISABLE | SHIFT_LEFT);
	Lcd1602WriteByte(CMD_INPUT_SET | INPUT_SET_INCREMENT | INPUT_SET_NO_SIFHT);
	Lcd1602WriteByte(CMD_DISPLAY | DISPLAY_ON | DISPLAY_CURSOR_OFF | DISPLAY_BLINK_OFF);
	
	Lcd1602Clear();
}

void Lcd1602LoadSymbol(const uint8_t code, const uint8_t *image)
{
	RS = RS_COMMAND;
	Lcd1602WriteByte(CMD_SET_ADR_CGRAM | (code << 3));
	
	RS = RS_DATA;
	for (uint8_t i = 0; i < 8; i++)
	{
		Lcd1602WriteByte(*image++);
	}
	
	Lcd1602SetCursor(Cursor.X, Cursor.Y);
}


void Lcd1602Clear()
{
	RS = RS_COMMAND;
	Lcd1602WriteByte(CMD_SCREEN_CLEAR);
	DelayUs(T_CMD_CLEAR_US);
	
	Lcd1602SetCursor(0, 0);
}

void Lcd1602SetCursor(const uint8_t x, const uint8_t y)
{
	uint8_t cmd = CMD_SET_ADR_DDRAM | x;
	
	if (y & 0x01)
	{
		cmd |= ROW1_ADDRESS;
	}
	if (y & 0x02)
	{
		cmd |= ROW2_ADDRESS;
	}
	
	RS = RS_COMMAND;
	Lcd1602WriteByte(cmd);
	
	Cursor.X = x;
	Cursor.Y = y;
}


void Lcd1602WriteChar(const char c)
{
	if (c != '\n')
	{
		RS = RS_DATA;
		Lcd1602WriteByte((uint8_t)c);
	}
	
	if (++Cursor.X == LCD1602_COLS || c == '\n')
	{
		Cursor.X = 0;
		
		if (++Cursor.Y == LCD1602_ROWS)
		{
			Cursor.Y = 0;
		}
		
		Lcd1602SetCursor(Cursor.X, Cursor.Y);
	}
}

void Lcd1602WriteString(const char *text)
{
	while (*text)
	{
		Lcd1602WriteChar(*text++);
	}
}

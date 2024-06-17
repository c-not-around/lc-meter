#ifndef _UART_H
#define _UART_H


#include "types.h"
#include "uart_settings.h"


#define UartWriteChar(c)	UartWriteByte((uint8_t)(c))
#define UartRead(t,v)		UartReadBytes(t,(uint8_t*)&(v),sizeof(v))
#define UartWrite(v)		UartWriteBytes((uint8_t*)&(v),sizeof(v))
#define UartWriteHex(v)		UartWriteBytesHex((uint8_t*)&(v),sizeof(v))


void UartInitialization(const uint32_t baudrate, const UART_SETTINGS settings);

BOOL UartAvailable();
uint8_t UartReadByte();
BOOL UartReadBytes(uint16_t timeout, uint8_t *data, uint8_t size);

void UartWriteByte(const uint8_t data);
void UartWriteBytes(const uint8_t *data, uint8_t size);
void UartWriteByteHex(const uint8_t data);
void UartWriteBytesHex(const uint8_t *data, uint8_t size);
void UartWriteString(const char *text);


#endif
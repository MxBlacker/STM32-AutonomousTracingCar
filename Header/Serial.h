#ifndef __SERIAL_KILLER
#define __SERIAL_KILLER

void USART1_Serial_Init(void);
void Serial_SendByte(uint8_t Byte);
void Serial_SendArray(uint8_t * Array, uint16_t Length);
void Serial_SendString(char * String);
void Serial_Printf(char * format, ...);
void Serial_SendPack(void);
uint8_t Serial_GetRxFlag(void);
int16_t Transfer_RxData(void);

#endif

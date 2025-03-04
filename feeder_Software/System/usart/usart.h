#ifndef __USART_H
#define __USART_H

#include "stdio.h"
#include "stm32f10x.h"
#include "OLED.h"
#include "string.h"
#include "ESP8266.h"

#define BUFFER_SIZE		200

extern uint8_t rx_buffer[BUFFER_SIZE];
extern uint8_t Auto_Feed_Global;

void _sys_exit(int x);
int fputc(int ch, FILE *f);
void uart3_init(u32 Baudrate);
void USAR_SendByte(uint8_t Byte);
void USAR_SendArray(uint8_t *Array, uint16_t Length);
void USAR_SendString(char *String);
uint32_t USAR_Pow(uint32_t X, uint32_t Y);
void USAR_SendNumber(uint32_t Number, uint8_t Length);
uint8_t USAR_GetRxFlag(void);
void Usart3_Send(u8 *buf,u8 len);

void checkOK(u8 *buf);
void Btn_Clicked(void);

#endif

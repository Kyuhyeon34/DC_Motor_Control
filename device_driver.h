#include "stm32f4xx.h"
#include "option.h"
#include "macro.h"
#include "malloc.h"

// Uart.c

extern void Uart2_Init(int baud);
extern void Uart2_Send_Byte(char data);
extern char Uart2_Receive_Byte(void);
extern char Uart2_Read_Byte(void);
extern void Uart2_RX_Interrupt_Enable(int enable);

extern void Uart1_Init(int baud);
extern void Uart1_Send_Byte(char data);
extern void Uart1_Send_String(char *pt);
extern char Uart1_Get_Char(void);
extern char Uart1_Get_Pressed(void);

// Clock.c

extern void Clock_Init(void);

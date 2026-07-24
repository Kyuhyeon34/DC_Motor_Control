#include "device_driver.h"
#include "key.h"
#include "timer.h"
#include <stdio.h>

extern volatile unsigned long Sys_Tick;
extern volatile int Uart_Data_In;
extern volatile unsigned char Uart_Data;

volatile unsigned char TIM2_Timeout_Flag = 0;

// 정의되지 않은 예외 발생 핸들러
void _Invalid_ISR(void)
{
    unsigned int r = Macro_Extract_Area(SCB->ICSR, 0x1ff, 0);
    printf("\nInvalid_Exception: %d!\n", r);
    printf("Invalid_ISR: %d!\n", r - 16);
    for (;;)
        ;
}

// EXTI15_10 ISR
void EXTI15_10_IRQHandler(void)
{
    EXTI->PR = (1U << 13U);
    NVIC_ClearPendingIRQ(EXTI15_10_IRQn);
}

// TIM2 ISR
void TIM2_IRQHandler(void)
{
    Macro_Clear_Bit(TIM2->SR, 0U);
    NVIC_ClearPendingIRQ(TIM2_IRQn);

    TIM2_Stop();
    TIM2_Timeout_Flag = 1; // 타임아웃 플래그 Set
}

// TIM4 ISR
void TIM4_IRQHandler(void)
{
    Macro_Clear_Bit(TIM4->SR, 0U);
    NVIC_ClearPendingIRQ(TIM4_IRQn);

    Sys_Tick++; // 1ms 시스템 틱 증가
}

// USART2 수신 ISR (1바이트 수신)
void USART2_IRQHandler(void)
{
    Uart_Data = (unsigned char)USART2->DR;
    Uart_Data_In = 1; // 수신 완료 플래그 Set

    NVIC_ClearPendingIRQ(USART2_IRQn);
}
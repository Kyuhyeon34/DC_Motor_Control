#include "device_driver.h"
#include "timer.h"

void Timer_Init(void)
{
    TIM2_Init();
    TIM4_Init();
    TIM5_Init();
}

void TIM2_Init(void)
{
    Macro_Set_Bit(RCC->APB1ENR, 0U);     // TIM2 클럭 활성화
    TIM2->CR1 = (1U << 4U) | (1U << 3U); // Down-counter, One-Pulse Mode
    TIM2->PSC = (unsigned int)(TIMXCLK / TIM4_FREQ + 0.5) - 1U;
}

void TIM2_Start(int delay_ms)
{
    TIM2->ARR = (unsigned int)(TIM4_1MS_PLS * delay_ms) - 1U;

    Macro_Set_Bit(TIM2->EGR, 0U);  // 섀도우 레지스터 동기화
    Macro_Clear_Bit(TIM2->SR, 0U); // 플래그 클리어
    Macro_Set_Bit(TIM2->CR1, 0U);  // 카운팅 시작
}

int TIM2_Check_Timeout(void)
{
    int is_timeout = Macro_Check_Bit_Set(TIM2->SR, 0U);
    if (is_timeout)
    {
        Macro_Clear_Bit(TIM2->SR, 0U);
    }
    return is_timeout;
}

void TIM2_Stop(void)
{
    Macro_Clear_Bit(TIM2->CR1, 0U);
}

void TIM4_Init(void)
{
    Macro_Set_Bit(RCC->APB1ENR, 2U);     // TIM4 클럭 활성화
    TIM4->CR1 = (1U << 4U) | (1U << 3U); // Down-counter, One-Pulse Mode
    TIM4->PSC = (unsigned int)(TIMXCLK / TIM4_FREQ + 0.5) - 1U;
}

void TIM4_Delay(int delay_ms)
{
    unsigned int total_pls = (unsigned int)(TIM4_1MS_PLS * delay_ms);
    unsigned int q = total_pls / TIM4_MAX_PLS;
    unsigned int r = total_pls % TIM4_MAX_PLS;
    unsigned int i;

    for (i = 0U; i < q; i++)
    {
        TIM4->ARR = TIM4_MAX_PLS - 1U;
        Macro_Set_Bit(TIM4->EGR, 0U);
        Macro_Clear_Bit(TIM4->SR, 0U);
        Macro_Set_Bit(TIM4->CR1, 0U);
        while (!Macro_Check_Bit_Set(TIM4->SR, 0U))
            ;
    }

    if (r > 0U)
    {
        TIM4->ARR = r - 1U;
        Macro_Set_Bit(TIM4->EGR, 0U);
        Macro_Clear_Bit(TIM4->SR, 0U);
        Macro_Set_Bit(TIM4->CR1, 0U);
        while (!Macro_Check_Bit_Set(TIM4->SR, 0U))
            ;
    }

    Macro_Clear_Bit(TIM4->CR1, 0U);
}

void TIM5_Init(void)
{
    // 1. GPIOA 및 TIM5 클럭 활성화
    Macro_Set_Bit(RCC->AHB1ENR, 0U); // GPIOA
    Macro_Set_Bit(RCC->APB1ENR, 3U); // TIM5

    // 2. PA0, PA1 핀 설정 (AF 모드 / AF02: TIM5)
    Macro_Write_Block(GPIOA->MODER, 0xFU, 0xAU, 0U);
    Macro_Write_Block(GPIOA->AFR[0], 0xFFU, 0x22U, 0U);

    // 3. Down-counter, Repeat Mode 설정
    TIM5->CR1 = (1U << 4U) | (0U << 3U);

    // 4. PWM 주파수 설정 (PSC: 분주비, ARR: 주기)
    TIM5->PSC = (unsigned int)(TIMXCLK / TIM5_CNT_FREQ) - 1U;
    TIM5->ARR = (unsigned int)(TIM5_CNT_FREQ / TIM5_PWM_FREQ) - 1U; // ARR = 99

    // 5. CH1, CH2 PWM Mode 1 + Preload 활성화
    Macro_Write_Block(TIM5->CCMR1, 0xFFFFU, 0x6868U, 0U);

    // 6. CH1, CH2 출력 활성화
    Macro_Set_Bit(TIM5->CCER, 0U); // CH1 ON
    Macro_Set_Bit(TIM5->CCER, 4U); // CH2 ON

    // 7. 레지스터 동기화 및 타이머 구동 시작
    Macro_Set_Bit(TIM5->EGR, 0U);
    Macro_Set_Bit(TIM5->CR1, 0U);
}

void TIM5_Set_Duty(int ch1_duty, int ch2_duty)
{
    unsigned int arr_val = TIM5->ARR + 1U;

    TIM5->CCR1 = (arr_val * ch1_duty + 50U) / 100U;
    TIM5->CCR2 = (arr_val * ch2_duty + 50U) / 100U;
}

void TIM5_Stop(void)
{
    Macro_Clear_Bit(TIM5->CR1, 0U);
}

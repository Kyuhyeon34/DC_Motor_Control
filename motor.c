#include "device_driver.h"
#include "motor.h"
#include "timer.h"

void Motor_Init(void)
{
    // GPIOA 클럭 활성화
    Macro_Set_Bit(RCC->AHB1ENR, 0U);

    // PA0, PA1 ODR 비트를 0V(Low)로 한 번만 초기화
    Macro_Clear_Bit(GPIOA->ODR, 0U);
    Macro_Clear_Bit(GPIOA->ODR, 1U);
}

void Motor_Stop(void)
{
    // PA0, PA1 둘 다 Output 모드
    Macro_Write_Block(GPIOA->MODER, 0xFU, 0x5U, 0U);

    TIM4_Delay(STOP_DELAY_MS);
}

void Motor_Forward(int duty)
{
    Motor_Stop();

    // kick start
    // TIM5_Set_Duty(0, 100);
    // Macro_Write_Block(GPIOA->MODER, 0xFU, 0x9U, 0U);
    // TIM4_Delay(50);

    // PWM 듀티 설정
    TIM5_Set_Duty(0, duty);

    // PA0: Output Mode, PA1: Alternate Function Mode
    Macro_Write_Block(GPIOA->MODER, 0xFU, 0x9U, 0U);
}

void Motor_Reverse(int duty)
{
    Motor_Stop();

    // kick start
    // TIM5_Set_Duty(100, 0);
    // Macro_Write_Block(GPIOA->MODER, 0xFU, 0x9U, 0U);
    // TIM4_Delay(50);

    // PWM 듀티 설정
    TIM5_Set_Duty(duty, 0);

    // PA0: Alternate Function Mode, PA1: Output Mode
    Macro_Write_Block(GPIOA->MODER, 0xFU, 0x6U, 0U);
}

void Motor_Update(const MotorController *motor)
{
    switch (motor->state)
    {
    case MOTOR_STATE_STOP:
        Motor_Stop();
        break;

    case MOTOR_STATE_FORWARD:
        Motor_Forward(motor->duty);
        break;

    case MOTOR_STATE_REVERSE:
        Motor_Reverse(motor->duty);
        break;

    default:
        Motor_Stop();
        break;
    }
}

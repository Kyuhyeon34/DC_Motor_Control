#include "device_driver.h"
#include "key.h"
#include "timer.h"

void Key_Init(void)
{
	// PC 클럭 활성화
	Macro_Set_Bit(RCC->AHB1ENR, 2U);

	// PC13(User Key) 입력 모드 설정
	Macro_Write_Block(GPIOC->MODER, 0x3U, 0x0U, 26U);
}

int Key_Is_Pressed(void)
{
	return Macro_Check_Bit_Clear(GPIOC->IDR, 13U);
}

KeyEvent Key_Get_Event(void)
{
	static unsigned char is_locked = 0;
	static unsigned char long_click_flag = 0;
	KeyEvent event = KEY_EVENT_NONE;

	unsigned char is_pressed = Key_Is_Pressed();

	// 버튼을 누른 시점 (Falling Edge)
	if (!is_locked && is_pressed)
	{
		TIM4_Delay(CHATTERING_MS);
		if (Key_Is_Pressed())
		{
			is_locked = 1;
			long_click_flag = 0;
			TIM2_Start(KEY_LONG_PRESS_MS);
		}
	}
	// 버튼을 뗀 시점 (Rising Edge)
	else if (is_locked && !is_pressed)
	{
		TIM4_Delay(CHATTERING_MS);
		if (!Key_Is_Pressed())
		{
			is_locked = 0;
			TIM2_Stop();

			if (!long_click_flag)
			{
				event = KEY_EVENT_SHORT_CLICK;
			}
		}
	}
	// 버튼을 계속 누르고 있는 중 (Holding)
	else if (is_locked && is_pressed)
	{
		if (!long_click_flag && TIM2_Check_Timeout())
		{
			long_click_flag = 1;
			event = KEY_EVENT_LONG_CLICK;
		}
	}

	return event;
}

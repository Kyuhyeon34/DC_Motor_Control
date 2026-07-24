#include "device_driver.h"
#include "key.h"
#include "timer.h"

extern volatile unsigned long Sys_Tick;
extern volatile unsigned char TIM2_Timeout_Flag;

// 스위치 GPIO 초기화 (PC13 입력)
void Key_Init(void)
{
	Macro_Set_Bit(RCC->AHB1ENR, 2U);				  // GPIOC 클럭 활성화
	Macro_Write_Block(GPIOC->MODER, 0x3U, 0x0U, 26U); // PC13 Input Mode
}

// 스위치 눌림 상태 확인 (Active Low)
int Key_Is_Pressed(void)
{
	return Macro_Check_Bit_Clear(GPIOC->IDR, 13U);
}

// 스위치 이벤트 처리 (상태 머신)
KeyEvent Key_Get_Event(void)
{
	static KeyState key_state = KEY_STATE_IDLE;
	static unsigned long last_chattering_tick = 0;

	KeyEvent event = KEY_EVENT_NONE;
	int is_pressed = Key_Is_Pressed();

	switch (key_state)
	{
	case KEY_STATE_IDLE:
		// 스위치 누름 감지
		if (is_pressed)
		{
			last_chattering_tick = Sys_Tick;
			key_state = KEY_STATE_PRESSED;
		}
		break;

	case KEY_STATE_PRESSED:
		// 디바운스 대기 및 상태 확정
		if ((Sys_Tick - last_chattering_tick) >= CHATTERING_MS)
		{
			if (is_pressed)
			{
				TIM2_Timeout_Flag = 0;
				TIM2_Start(KEY_LONG_PRESS_MS); // 롱클릭 타이머 가동
				key_state = KEY_STATE_HOLDING;
			}
			else
			{
				key_state = KEY_STATE_IDLE;
			}
		}
		break;

	case KEY_STATE_HOLDING:
		// 롱클릭 판정 (타임아웃 발생)
		if (TIM2_Timeout_Flag)
		{
			TIM2_Timeout_Flag = 0;
			event = KEY_EVENT_LONG_CLICK;
			key_state = KEY_STATE_RELEASED;
		}
		// 숏클릭 판정 (타임아웃 전 해제)
		else if (!is_pressed)
		{
			TIM2_Stop();
			last_chattering_tick = Sys_Tick;
			event = KEY_EVENT_SHORT_CLICK;
			key_state = KEY_STATE_IDLE;
		}
		break;

	case KEY_STATE_RELEASED:
		// 스위치를 뗄 때까지 대기
		if (!is_pressed)
		{
			key_state = KEY_STATE_IDLE;
		}
		break;

	default:
		key_state = KEY_STATE_IDLE;
		break;
	}

	return event;
}
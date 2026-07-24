#ifndef __KEY_H__
#define __KEY_H__

#define KEY_LONG_PRESS_MS (3000U) // 롱클릭 판정 시간 (3초)
#define CHATTERING_MS (20U)       // 디바운싱 대기 시간 (20ms)

// 스위치 내부 제어 상태
typedef enum
{
    KEY_STATE_IDLE = 0,
    KEY_STATE_PRESSED,
    KEY_STATE_HOLDING,
    KEY_STATE_RELEASED
} KeyState;

// 스위치 이벤트 종류
typedef enum
{
    KEY_EVENT_NONE = 0,
    KEY_EVENT_SHORT_CLICK,
    KEY_EVENT_LONG_CLICK
} KeyEvent;

void Key_Init(void);
int Key_Is_Pressed(void);
KeyEvent Key_Get_Event(void);

#endif // __KEY_H__
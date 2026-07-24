#ifndef __KEY_H__
#define __KEY_H__

#define KEY_LONG_PRESS_MS (3000U)
#define CHATTERING_MS (20U)

typedef enum
{
    KEY_EVENT_NONE = 0,
    KEY_EVENT_SHORT_CLICK,
    KEY_EVENT_LONG_CLICK
} KeyEvent;

void Key_Init(void);
int Key_Is_Pressed(void);
KeyEvent Key_Get_Event(void);

#endif /* __KEY_H__ */
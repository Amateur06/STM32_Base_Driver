#ifndef __JOYSTICK_DRIVER_H
#define __JOYSTICK_DRIVER_H

#include <stdint.h>

typedef enum
{
    KEY_EVENT_NONE = 0,
    KEY_EVENT_UP,
    KEY_EVENT_DOWN,
    KEY_EVENT_LEFT,
    KEY_EVENT_RIGHT,
    KEY_EVENT_OK,
} KeyEvent_t;

KeyEvent_t Joystick_Scan(void);

#endif

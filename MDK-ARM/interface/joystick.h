#ifndef __JOYSTICK_H
#define __JOYSTICK_H

#include "adc.h"
typedef struct
{
    int16_t thr;//油门
    int16_t yaw;
    int16_t roll;
    int16_t pitch;
} joystick;

void joystick_init(void);
void joystick_get_value(joystick*js);

#endif
#ifndef __KEY_H
#define __KEY_H
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
typedef enum
{
    key_none=0,
    key_up,
    key_down,
    key_left,
    key_right,
    key_left_x,
    key_right_x_short,
    key_right_x_long ,
}key_type;
key_type get_key(void);

#endif
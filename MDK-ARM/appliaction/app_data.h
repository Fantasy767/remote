#ifndef __APP_DATA_H
#define  __APP_DATA_H
#include "key.h"
#include "joystick.h"
typedef  struct
{
    int16_t thr;
    int16_t yaw;
    int16_t pitch;
    int16_t roll;
    uint8_t off_or_on;//开关机 ,1为关机 0开机
    uint8_t height; //0不切换  1

}remote_data;
void app_process_key_data(void);
void app_process_joystick_data(void);

#endif // __APP_DATA_H
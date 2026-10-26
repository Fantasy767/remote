#include "app_data.h"
#include "app_freertos_task.h"
#include "comment.h"
#include "joystick.h"
#include "key.h"
#include <stdint.h>

joystick joystick1={0};
int16_t key_pitch_offset=0;//向前 是+
int16_t key_roll_offset=0;//往右飞是+
remote_data  re_data={0};
//记录摇杆偏移量
int16_t  pitch_offset=0;
int16_t  roll_offset=0;
int16_t  yaw_offset=0;
int16_t  thr_offset=0;
int16_t limit(int16_t value,int16_t min,int16_t max)
{
    if(value<min)
    {
        return min;
    }
    else if(value>max)
    {
        return max;
    }
    else
    {
        return value;
    }
}

void app_joystick_jiaozhun(void)
{
    key_pitch_offset=0;
    key_roll_offset=0;
    int16_t thr_sum = 0;
    int16_t yaw_sum = 0;
    int16_t pitch_sum = 0;
    int16_t roll_sum = 0;

    key_pitch_offset = 0;
    key_roll_offset = 0;

    for (uint8_t i = 0; i < 10; i++)
    {
        app_process_joystick_data();

        thr_sum   += joystick1.thr;
        yaw_sum   += joystick1.yaw - 500;
        pitch_sum += joystick1.pitch - 500;
        roll_sum  += joystick1.roll - 500;

        vTaskDelay(pdMS_TO_TICKS(10));
    }

    thr_offset   += thr_sum / 10;
    yaw_offset   += yaw_sum / 10;
    pitch_offset += pitch_sum / 10;
    roll_offset  += roll_sum / 10;
}
void app_process_key_data(void)
{
     key_type  key=get_key();
    if (key != key_none)
    {
        debug_printf("key=%d\r\n", (int)key);
    }
    //根据按键的数值进行 记录
    if(key==key_up)
    {
        key_pitch_offset=key_pitch_offset+10;//向前
    }
    else if(key==key_down)
    {
        key_pitch_offset=key_pitch_offset-10;//向后
    }
    else  if(key==key_left)
    {
        key_roll_offset=key_roll_offset-10;//向右
    }
    else  if(key==key_right)
    {
        key_roll_offset=key_roll_offset+10;
    }
    else if(key==key_left_x)
    {
        re_data.off_or_on=1;
        debug_printf("power_off_request=1\r\n");
    }
    else if(key==key_right_x_short)
    {
        re_data.height=1;
    }
    else if(key==key_right_x_long)
    {
        //校准摇杆 thr=0,else=500
        app_joystick_jiaozhun();

    }
     /*   else if (key == key_right_x_long)
    {
        debug_printf("CAL START\r\n");
        app_joystick_jiaozhun();
        debug_printf("CAL DONE: %d,%d,%d,%d\r\n",
                    thr_offset, yaw_offset,
                    pitch_offset, roll_offset);
    }*/
}
void app_process_joystick_data(void)
{
    //获取摇杆 adc的数值
    taskENTER_CRITICAL();

    joystick_get_value(&joystick1);
    //处理范围和极性  想要的范围是0-1000 but  adc范围是0-4096
    joystick1.thr=1000-(joystick1.thr*1000/4095);
    joystick1.yaw=1000-(joystick1.yaw*1000/4095);
    joystick1.pitch=1000-(joystick1.pitch*1000/4095);
    joystick1.roll=1000-(joystick1.roll*1000/4095);

    //零偏校准
    joystick1.thr-=thr_offset;
    joystick1.yaw-=yaw_offset;
    joystick1.pitch-=pitch_offset;
    joystick1.roll-=roll_offset;

    joystick1.pitch+=key_pitch_offset;
    joystick1.roll+=key_roll_offset;

    joystick1.thr   = limit(joystick1.thr,   0, 1000);
    joystick1.yaw   = limit(joystick1.yaw,   0, 1000);
    joystick1.pitch = limit(joystick1.pitch, 0, 1000);
    joystick1.roll  = limit(joystick1.roll,  0, 1000);

    taskEXIT_CRITICAL();

    re_data.thr=joystick1.thr;
    re_data.yaw=joystick1.yaw;
    re_data.pitch=joystick1.pitch;
    re_data.roll=joystick1.roll;
    //debug_printf("%d,%d,%d,%d\n",joystick1.thr,joystick1.yaw,joystick1.pitch,joystick1.roll);
    
}

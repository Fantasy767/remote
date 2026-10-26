#include "app_freertos_task.h"
#include "SI24R1.h"
#include "app_data.h"
#include "comment.h"
#include "joystick.h"
#include <stdint.h>
#include <sys/cdefs.h>

  void powtask(void *args);
 #define powtask_stack_size 128
 #define powtask_priority 4//数值越小，中断优先级越低
 TaskHandle_t powtask_handle;

 //connect 
 void remote_task(void *args);
#define remote_stack_size 384
 #define remote_priority 3//数值越小，中断优先级越低
 TaskHandle_t remote_handle;
 #define remote_period  6

 //key
 void key_task(void *args);
#define key_stack_size 256
 #define key_priority 2//数值越小，中断优先级越低
 TaskHandle_t key_handle;
 #define key_period  20
 //adc
 void adc_task(void *args);
#define adc_stack_size 128
 #define adc_priority 2//数值越小，中断优先级越低
 TaskHandle_t adc_handle;
 #define adc_period  20
 //启动rtos

void app_freertos_task(void)
{
    //电源管理任务
    xTaskCreate(powtask, "powtask", powtask_stack_size, NULL, powtask_priority, &powtask_handle);
    //创建通讯任务
    xTaskCreate(remote_task, "remote_task", remote_stack_size, NULL, remote_priority, &remote_handle);
    //创建按键任务
    xTaskCreate(key_task, "key_task", key_stack_size, NULL, key_priority, &key_handle);
    //joystick
    xTaskCreate(adc_task, "adc_task", adc_stack_size, NULL, adc_priority, &adc_handle);

    //启动调度器
    vTaskStartScheduler();
}
void powtask(void *args)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();

    //task启动之后，不断执行的内容
    while (1)
    {
        
        //10s启动一次电源管理
        vTaskDelayUntil(&xLastWakeTime, 3000 );//延时1000ms，结束cpu占用，不是让cpu空转

        tp4336_init();//10s启动一次电源管理
    }
}
uint8_t remmote_buf[TX_PLOAD_WIDTH]={0};
 void remote_task(void *args)
 {
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    
    //task启动之后，不断执行的内容
    while (1)
    {
        /*
        //调用si24r1接口 发送数据
        SI24R1_TX_Mode();
        //6s启动一次远程通讯
        remmote_buf[0]='h';
        remmote_buf[1]='e';
        remmote_buf[2]='l';
        remmote_buf[3]='l';
        remmote_buf[4]='o';
        SI24R1_TxPacket(remmote_buf);
         //回复到rxmode
        SI24R1_RX_Mode();*/

       app_transmit_data();
        vTaskDelayUntil(&xLastWakeTime, remote_period);//延时6000ms
        
        // TODO: 实现远程通讯逻辑
    }
 }
void key_task(void *args)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    
    //task启动之后，不断执行的内容
    while (1)
    {
       app_process_key_data();
        vTaskDelayUntil(&xLastWakeTime, key_period);//延时2000ms
    }
}
void adc_task(void *args)
{
    TickType_t xLastWakeTime;
    xLastWakeTime = xTaskGetTickCount();
    joystick_init();
    //task启动之后，不断执行的内容
    while (1)
    {
       app_process_joystick_data();
        vTaskDelayUntil(&xLastWakeTime, adc_period);//延时2000ms
    }
}
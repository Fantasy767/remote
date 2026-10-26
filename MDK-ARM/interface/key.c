#include "key.h"
#include "main.h"
#include "portmacro.h"
key_type get_key(void)
{
    
    if(HAL_GPIO_ReadPin(up_GPIO_Port,up_Pin)==GPIO_PIN_RESET)
    {
        vTaskDelay(5);
        if(HAL_GPIO_ReadPin(up_GPIO_Port,up_Pin)==GPIO_PIN_RESET)
        {
            while(HAL_GPIO_ReadPin(up_GPIO_Port,up_Pin)==GPIO_PIN_RESET)
            {vTaskDelay(1);}
            return key_up;
        }
    }
    else if(HAL_GPIO_ReadPin(down_GPIO_Port,down_Pin)==GPIO_PIN_RESET)
    {
        vTaskDelay(5);
        if(HAL_GPIO_ReadPin(down_GPIO_Port,down_Pin)==GPIO_PIN_RESET)
        {
            while(HAL_GPIO_ReadPin(down_GPIO_Port,down_Pin)==GPIO_PIN_RESET)
            {vTaskDelay(1);}
            return key_down;
        }
    }
    else if(HAL_GPIO_ReadPin(left_GPIO_Port,left_Pin)==GPIO_PIN_RESET)
    {
        vTaskDelay(5);
        if(HAL_GPIO_ReadPin(left_GPIO_Port,left_Pin)==GPIO_PIN_RESET)
        {
            while(HAL_GPIO_ReadPin(left_GPIO_Port,left_Pin)==GPIO_PIN_RESET)
            {vTaskDelay(1);}
            return key_left;
        }
    }
    else if(HAL_GPIO_ReadPin(right_GPIO_Port,right_Pin)==GPIO_PIN_RESET)
    {
        vTaskDelay(5);
        if(HAL_GPIO_ReadPin(right_GPIO_Port,right_Pin)==GPIO_PIN_RESET)
        {
            while(HAL_GPIO_ReadPin(right_GPIO_Port,right_Pin)==GPIO_PIN_RESET)
            {vTaskDelay(1);}
            return key_right;
        }
    }
    /*
    else if(HAL_GPIO_ReadPin(right_x_GPIO_Port,right_x_Pin)==GPIO_PIN_RESET)
    {
        vTaskDelay(5);
        if(HAL_GPIO_ReadPin(right_x_GPIO_Port,right_x_Pin)==GPIO_PIN_RESET)
        {
            while(HAL_GPIO_ReadPin(right_x_GPIO_Port,right_x_Pin)==GPIO_PIN_RESET){}
            return key_right_x_short;
        }
    }*/
    else if(HAL_GPIO_ReadPin(left_x_GPIO_Port,left_x_Pin)==GPIO_PIN_RESET)
    {
        vTaskDelay(5);
        if(HAL_GPIO_ReadPin(left_x_GPIO_Port,left_x_Pin)==GPIO_PIN_RESET)
        {
            while(HAL_GPIO_ReadPin(left_x_GPIO_Port,left_x_Pin)==GPIO_PIN_RESET)
            {vTaskDelay(1);}
            return key_left_x;
        }
    }
    //右上比较特殊
    else if(HAL_GPIO_ReadPin(right_x_GPIO_Port,right_x_Pin)==GPIO_PIN_RESET)
    {
        TickType_t count1=xTaskGetTickCount();
        vTaskDelay(5);
        if(HAL_GPIO_ReadPin(right_x_GPIO_Port,right_x_Pin)==GPIO_PIN_RESET)
        {
            while(HAL_GPIO_ReadPin(right_x_GPIO_Port,right_x_Pin)==GPIO_PIN_RESET)
            {
                vTaskDelay(1);
            }
           TickType_t count2=xTaskGetTickCount();
           if(count2-count1>1000)
           {
              //a long key
              return  key_right_x_long;
           }
           else
           {
            //a short key
             return key_right_x_short;
           }
        }
    }
        
        return key_none;
}

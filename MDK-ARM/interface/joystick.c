#include "joystick.h"
#include "stm32f1xx_hal_adc.h"
uint16_t adc_value[4]={0};
void joystick_init(void)
{
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)adc_value, 4);

}
void joystick_get_value(joystick*js)
{
    js->thr=adc_value[0];
    js->yaw=adc_value[1];
    js->pitch=adc_value[2];
    js->roll=adc_value[3];
}

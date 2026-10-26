#ifndef __APP_TRANSMIT_H
#define __APP_TRANSMIT_H

#include "SI24R1.h"
#include "app_data.h"
#include "FreeRTOS.h"
#include "task.h"
#define FIRST_HEAD_CHECK 'w'
#define SECOND_HEAD_CHECK 'z'
#define THIRD_HEAD_CHECK 'y'
void app_transmit_data();

#endif

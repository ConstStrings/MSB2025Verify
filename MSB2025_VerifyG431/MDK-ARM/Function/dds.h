#ifndef __DDS_H
#define __DDS_H

#include "main.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include "uart.h"

#define DDS_BUFF_SIZE 1024
#define DDS_SAMPLE_RATE 1000000
#define PI 3.14159265f


void DDS_Init(void);
void DDS_Reload(uint32_t frequency, float amplitude);

extern TIM_HandleTypeDef htim6;
extern DAC_HandleTypeDef hdac1;

#endif 



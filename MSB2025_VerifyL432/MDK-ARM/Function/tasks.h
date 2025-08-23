#ifndef __TASKS_H__
#define __TASKS_H__

#include "main.h"
#include "oled.h"
#include "uart.h"

#define SAMPLE_SIZE 4096
#define SAMPLE_RATE 1333333

#define FREQ_HI_THRESHOLD 2500
#define FREQ_LO_THRESHOLD 1500

extern ADC_HandleTypeDef hadc1;

void startup(void);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
float calculate_freq(void);

#endif
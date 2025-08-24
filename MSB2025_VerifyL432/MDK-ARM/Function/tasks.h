#ifndef __TASKS_H__
#define __TASKS_H__

#include "main.h"
#include "oled.h"
#include "uart.h"
#include "math.h"
#include "arm_math.h"
#include "arm_const_structs.h"

#define PI 3.14159265358979323846

#define SAMPLE_SIZE 1024
#define SAMPLE_RATE 1333333

#define FREQ_HI_THRESHOLD 2500
#define FREQ_LO_THRESHOLD 1500

#define  FFT_LENGTH		  SAMPLE_SIZE

extern ADC_HandleTypeDef hadc1;

void startup(void);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
float calculate_freq(void);

#endif

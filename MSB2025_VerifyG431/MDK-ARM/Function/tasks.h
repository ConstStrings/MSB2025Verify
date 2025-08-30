#ifndef __TASKS_H__
#define __TASKS_H__

#include "main.h"
#include "oled.h"
#include "uart.h"
#include "math.h"
#include "arm_math.h"
#include "arm_const_structs.h"

#define SAMPLE_SIZE 1024
#define SAMPLE_RATE 708333

#define FREQ_HI_THRESHOLD 1000
#define FREQ_LO_THRESHOLD -1000

#define  FFT_LENGTH		  SAMPLE_SIZE

enum WaveformType {
    WAVE_SINE = 0,
    WAVE_SQUARE,
    WAVE_TRIANGLE,
    WAVE_NO_SIGNAL
};

extern ADC_HandleTypeDef hadc1;
extern DAC_HandleTypeDef hdac1;
extern TIM_HandleTypeDef htim6;

void startup(void);
void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc);
void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac);
float calculate_freq(void);
void calculate_fft(void);
void calculate_mixfreq(void);
void dac_update(void);
enum WaveformType Calculate_Waveform(void);

#endif

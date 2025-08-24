#include "tasks.h"

static uint16_t My_adcData [SAMPLE_SIZE+1]={0};
static uint16_t Process_Buffer[SAMPLE_SIZE]={0};

static float    fft_inputbuf [FFT_LENGTH*2];	
static float    fft_outputbuf[FFT_LENGTH];

void startup(void)
{
    // OLED_Init();
    // OLED_Clear();
    // OLED_ShowString(0,0,"MSB2025",16,0);
    // OLED_ShowString(0,2,"VerifyL432",16,0);
    myprintf("System Start\r\n");

    for(int i = 0; i < 5; i++) {
        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_SET);
        HAL_Delay(500);

        HAL_GPIO_WritePin(LED_GPIO_Port, LED_Pin, GPIO_PIN_RESET);
        HAL_Delay(500);
    }

    /*****************************ADC*************************/  
    HAL_ADCEx_Calibration_Start(&hadc1, ADC_SINGLE_ENDED);
    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)My_adcData, SAMPLE_SIZE+1);
}

float calculate_freq(void)
{
    int zero_crossings = 0;
    int in_high_flag = 0;
    int in_low_flag = 0;
    for(int i = 1; i < SAMPLE_SIZE; i++)
    {
        if (Process_Buffer[i] > FREQ_HI_THRESHOLD && Process_Buffer[i-1] < FREQ_LO_THRESHOLD)
        {
            zero_crossings++;
        }
        else if (Process_Buffer[i] < FREQ_LO_THRESHOLD && Process_Buffer[i-1] > FREQ_LO_THRESHOLD)
        {
            zero_crossings++;
        }   
        else if (in_high_flag == 0 && in_low_flag == 0 && Process_Buffer[i] < FREQ_HI_THRESHOLD && Process_Buffer[i] > FREQ_LO_THRESHOLD)
        {
            if (Process_Buffer[i-1] > FREQ_HI_THRESHOLD)
            {
                in_high_flag = 1;
            } 
            else if (Process_Buffer[i-1] < FREQ_LO_THRESHOLD)
            {
                in_low_flag = 1;
            }
        }
        else if (in_high_flag == 1 && Process_Buffer[i] < FREQ_LO_THRESHOLD)
        {
            zero_crossings++;
            in_high_flag = 0;
        }
        else if (in_low_flag == 1 && Process_Buffer[i] > FREQ_HI_THRESHOLD)
        {
            zero_crossings++;
            in_low_flag = 0;
        }
    }
    // myprintf("Zero Crossings:%d\n", zero_crossings);
    float freq = SAMPLE_RATE * (zero_crossings / 2.0f) / (SAMPLE_SIZE - 1);
    myprintf("Calculated Freq:%.2f\n", freq);
    return freq;
}

void calculate_fft() // 未加窗
{
    for (int i = 0; i < FFT_LENGTH; i++)
    {
        fft_inputbuf[2 * i] = (Process_Buffer[i] * 0.5f * (1.0f - cosf(2 * PI * i / (FFT_LENGTH - 1)))) * 3.3f / 4095.0f;
        fft_inputbuf[2 * i + 1] = 0;
    }

    arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf, 0, 1);
    arm_cmplx_mag_f32(fft_inputbuf, fft_outputbuf, FFT_LENGTH);

    for (int i = 0; i < FFT_LENGTH; i++)
    {
        myprintf("%d,%f\r\n", i, fft_outputbuf[i]); //未加窗
    }
}

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance == ADC1)
    {
        memcpy(Process_Buffer, My_adcData+(sizeof(uint16_t)), SAMPLE_SIZE * sizeof(uint16_t));
        // calculate_freq();
        calculate_fft();
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)My_adcData, SAMPLE_SIZE+1);
        // for(int i = 1; i < SAMPLE_SIZE; i++)
        // {
        //     myprintf("%d\n", My_adcData[i]);
        // }
        // myprintf("\r\n");
    }
}

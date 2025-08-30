#include "dds.h"

static uint16_t dac_buffer[DDS_BUFF_SIZE] = {0};

void DDS_Init(void)
{
    HAL_TIM_Base_Start_IT(&htim6);
    HAL_DAC_SetValue(&hdac1, DAC_CHANNEL_1, DAC_ALIGN_12B_R, 2048);
    HAL_DAC_Start(&hdac1, DAC_CHANNEL_1); 
}

void DDS_Reload(uint32_t frequency, float amplitude)
{
    uint16_t sppl = DDS_SAMPLE_RATE / frequency; // samples per period
    uint16_t period_count = DDS_BUFF_SIZE / sppl; // number of periods in buffer
    uint16_t real_buff_size = period_count * sppl; // actual buffer size used

    float phase_step = (2.0f * PI) / sppl;
    HAL_DAC_Stop_DMA(&hdac1,DAC_CHANNEL_1);
    for (int i = 0; i < real_buff_size; i++) {
        float sample = sinf(i * phase_step);
        // myprintf("Sample %d: %.2f\n", i, sample);
        dac_buffer[i] = (uint16_t)(((sample + 1.0f) * amplitude / 2.0f) * 4095.0f / 3.3f);
        // myprintf("DAC Buffer[%d]: %d\n", i, dac_buffer[i]);
    }
    HAL_DAC_Start_DMA(&hdac1,DAC_CHANNEL_1, (uint32_t *)dac_buffer, real_buff_size, DAC_ALIGN_12B_R);
    myprintf("DDS Buffer Updated: Frequency = %lu Hz, Amplitude = %.2f V\n", frequency, amplitude);
}



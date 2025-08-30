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
    float phase_step = (2.0f * PI * frequency) / DDS_SAMPLE_RATE;
    uint16_t target_index;
    float target_error = 1;
    for (int i = 1; i < DDS_BUFF_SIZE; i++) {
        float sample = sinf(i * phase_step);
        float d_sample = cosf(i * phase_step);
        if(d_sample > 0 && fabs(sample) < target_error)
        {
            target_error = fabs(sample);
            target_index = i;
        }
    }
    myprintf("Target Index: %d, Target Error: %.2f\n", target_index, target_error);
    uint16_t real_buff_size = target_index;
    uint16_t period_count = (frequency * real_buff_size) / DDS_SAMPLE_RATE;
    phase_step = (2.0f * PI * period_count) / real_buff_size;
    float real_freq = (DDS_SAMPLE_RATE * period_count) / (real_buff_size);
    myprintf("Real Frequency: %.2f Hz\n", real_freq);

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



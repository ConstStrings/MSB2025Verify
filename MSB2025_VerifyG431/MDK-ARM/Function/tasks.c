#include "tasks.h"

static uint16_t My_adcData [SAMPLE_SIZE+1]={0};
static int16_t Process_Buffer[SAMPLE_SIZE]={0};

static float    fft_inputbuf [FFT_LENGTH*2];	
static float    fft_outputbuf[FFT_LENGTH];

void startup(void)
{
    OLED_Init();
    OLED_Clear();
    OLED_ShowString(32,0,"MSB2025",16,0);
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
	
	
	//HAL_DAC_Start_DMA(&hdac1,DAC_CHANNEL_1, (uint32_t *)Process_Buffer, SAMPLE_SIZE, DAC_ALIGN_12B_R);
	//HAL_DAC_Start_DMA(&hdac1,DAC_CHANNEL_1, (uint32_t *)dac_test, 4, DAC_ALIGN_12B_R);
}

float calculate_freq(void)
{
    int zero_crossings = 0;
    int in_high_flag = 0;
    int in_low_flag = 0;
    for(int i = 1; i < SAMPLE_SIZE; i++)
    {
        if (in_high_flag == 0 && in_low_flag == 0 && Process_Buffer[i] > FREQ_HI_THRESHOLD && Process_Buffer[i-1] < FREQ_LO_THRESHOLD)
        {
            zero_crossings++;
        }
        else if (in_high_flag == 0 && in_low_flag == 0 && Process_Buffer[i] < FREQ_LO_THRESHOLD && Process_Buffer[i-1] > FREQ_LO_THRESHOLD)
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

    float cur_freq = SAMPLE_RATE * (zero_crossings / 2.0f) / (SAMPLE_SIZE - 1);

    static float freq = 0;
    freq = 0.1f * cur_freq + 0.9f * freq;

    return freq;
}

void calculate_fft() // Hanning Window
{
    for (int i = 0; i < FFT_LENGTH; i++)
    {
        fft_inputbuf[2 * i] = Process_Buffer[i] * 0.5f * (1.0f - cosf(2 * PI * i / (FFT_LENGTH - 1)));
        fft_inputbuf[2 * i + 1] = 0;
    }

    arm_cfft_f32(&arm_cfft_sR_f32_len1024, fft_inputbuf, 0, 1);
    arm_cmplx_mag_f32(fft_inputbuf, fft_outputbuf, FFT_LENGTH);

    // for (int i = 0; i < FFT_LENGTH; i++)
    // {
    //     myprintf("%d,%f\r\n", i, fft_outputbuf[i]); 
    // }
}

void calculate_mixfreq()
{
    float freq1 = 0, freq2 = 0, value1 = 0, value2 = 0;
    fft_outputbuf[0] = 0; 
    fft_outputbuf[1] = 0; 
    for (int i = 1; i < FFT_LENGTH / 2; i++)
    {
        if (fft_outputbuf[i] > fft_outputbuf[i-1] && fft_outputbuf[i] > fft_outputbuf[i+1])
        {
            if (fft_outputbuf[i] > value1)
            {
                freq2 = freq1;
                value2 = value1;
                freq1 = i;
                value1 = fft_outputbuf[i];
            }
            else if (fft_outputbuf[i] > value2)
            {
                freq2 = i;
                value2 = fft_outputbuf[i];
            }
        }
    }
    freq1 *= SAMPLE_RATE / FFT_LENGTH;
    freq2 *= SAMPLE_RATE / FFT_LENGTH;
    myprintf("Freq1:%.2f, Value1:%.2f\r\n", freq1, value1);
    myprintf("Freq2:%.2f, Value2:%.2f\r\n", freq2, value2);
    OLED_ShowString(0,2,"Mix Signal Mode",16,0);
    OLED_ShowString(0,4,"Freq1:",16,0);
    OLED_ShowString(110,4,"Hz",16,0);
    OLED_ShowNum(48,4,(uint32_t)freq1,7,16,0);
    OLED_ShowString(0,6,"Freq2:",16,0);
    OLED_ShowString(110,6,"Hz",16,0);
    OLED_ShowNum(48,6,(uint32_t)freq2,7,16,0);
}

enum WaveformType Calculate_Waveform(void)
{
    float freq = calculate_freq();
    myprintf("Waveform Frequency: %.2f Hz\r\n", freq);
    OLED_ShowString(0,6,"Freq:",16,0);
    OLED_ShowNum(40,6,(uint32_t)freq,6,16,0);
    OLED_ShowString(110,6,"Hz",16,0);

    int16_t rms;
	arm_rms_q15(Process_Buffer,SAMPLE_SIZE,&rms);
	int16_t maxVal, minVal;

    arm_max_q15(Process_Buffer, SAMPLE_SIZE, &maxVal, NULL);
    arm_min_q15(Process_Buffer, SAMPLE_SIZE, &minVal, NULL);

    float ratio = (float)rms / (maxVal - minVal);
    myprintf("RMS: %d, Max: %d, Min: %d, Ratio: %.4f\r\n", rms, maxVal, minVal, ratio);
    OLED_ShowString(0,2,"Pure Signal Mode",16,0);
    if (rms < 100)
    {
        myprintf("Waveform: No Signal\r\n");
        OLED_ShowString(0,4,"No Signal     ",16,0);
        return WAVE_NO_SIGNAL;
    }
    else if (ratio > 0.30f && ratio < 0.45f)
    {
        myprintf("Waveform: Sine Wave\r\n");
        OLED_ShowString(0,4,"Type: Sine    ",16,0);
        return WAVE_SINE;
    }
    else if (ratio < 0.30f)
    {
        myprintf("Waveform: Triangle Wave\r\n");
        OLED_ShowString(0,4,"Type: Triangle",16,0);
        return WAVE_TRIANGLE;
    }
    else if (ratio > 0.45f)
    {
        myprintf("Waveform: Square Wave\r\n");
        OLED_ShowString(0,4,"Type: Square  ",16,0);
        return WAVE_SQUARE;
    }
    
    return WAVE_NO_SIGNAL;
}


void generate_ifft(void)
{
	// arm_cfft_f32(&arm_cfft_sR_f32_len2048, fft_inputbuf, 1, 1);
}




void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)
{
    if(hadc->Instance == ADC1)
    {
        // memcpy(Process_Buffer, My_adcData+(sizeof(uint16_t)), SAMPLE_SIZE * sizeof(uint16_t));
        int32_t mean = 0;
        for(int i = 1; i <= SAMPLE_SIZE; i++)
        {
            mean += My_adcData[i];
        }
        mean /= SAMPLE_SIZE;
        for(int i = 1; i <= SAMPLE_SIZE; i++)
        {
            Process_Buffer[i-1] = (int16_t)My_adcData[i] - mean;
            // myprintf("Process_Buffer[%d]: %f\n", i-1, Process_Buffer[i-1]);
        }
        Calculate_Waveform();
        // calculate_fft();
        // calculate_mixfreq();
        // dac_update();
        HAL_ADC_Start_DMA(&hadc1, (uint32_t *)My_adcData, SAMPLE_SIZE+1);
        // for(int i = 1; i < SAMPLE_SIZE; i++)
        // {
        //     myprintf("%d\n", My_adcData[i]);
        // }
        // myprintf("\r\n");
    }
}

//void HAL_DAC_ConvCpltCallbackCh1(DAC_HandleTypeDef* hdac)
//{
//	if(hdac->Instance == DAC1)
//	{
//		HAL_DAC_Start_DMA(&hdac1,DAC_CHANNEL_1, (uint32_t *)Process_Buffer, SAMPLE_SIZE, DAC_ALIGN_12B_R);
//	}
//}


#ifndef __UI_H__
#define __UI_H__

#include "main.h"
#include "oled.h"
#include "tasks.h"

extern ADC_HandleTypeDef hadc1;
extern DAC_HandleTypeDef hdac1;
extern TIM_HandleTypeDef htim6;

void UI_Init(void);
void UI_Update(void);

enum Page
{
    PAGE_HOME,
    PAGE_PURE,
    PAGE_MIX
};

#endif

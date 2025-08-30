#include "ui.h"

enum Page current_page = PAGE_HOME;
uint8_t home_select = 0;
uint8_t mix_select = 0;

void UI_Init(void)
{
    OLED_Init();
    OLED_Clear();
    current_page = PAGE_HOME;
}

void UI_Update(void)
{
    switch (current_page)
    {
    case PAGE_HOME:
        OLED_ShowString(32,0,"MSB2025",16,0);    
        OLED_ShowString(0, 2, "Pure Signal", 16, !home_select);
        OLED_ShowString(0, 4, "Mix Signal", 16, home_select);
        break;
    case PAGE_PURE:
        ADC_Start();
        break;
    case PAGE_MIX:
        ADC_Start();
        break;
    default:
        break;
    }
}

static uint32_t last_press_time = 0;
#define DEBOUNCE_DELAY 50  // 50ms debounce delay

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
    uint32_t current_time = HAL_GetTick();
    
    // Debounce check
    if (current_time - last_press_time < DEBOUNCE_DELAY) {
        return;
    }
    last_press_time = current_time;
    
    switch (GPIO_Pin)
    {
    case GPIO_PIN_4:
            switch (current_page)
            {
            case PAGE_HOME:
                home_select = !home_select;
                break;
            case PAGE_MIX:
                mix_select = !mix_select;
                break;
            default:
                break;
            }
        break;
    case GPIO_PIN_11:
        switch (current_page)
            {
            case PAGE_HOME:
                current_page = home_select ? PAGE_MIX : PAGE_PURE;
                OLED_Clear();
                break;
            case PAGE_PURE:
                current_page = PAGE_HOME;
                OLED_Clear();
                break;
            case PAGE_MIX:
                current_page = PAGE_HOME;
                OLED_Clear();
                break;
            default:
                break;
            }
        break;
    default:
        break;
    }
}

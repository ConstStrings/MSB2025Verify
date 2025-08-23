#include "uart.h"

uint8_t RxBuffer1[1];

int myprintf(const char *format, ...)
{
    char buffer[128]; 
    va_list args;
    va_start(args, format);
    vsnprintf(buffer, sizeof(buffer), format, args);
    va_end(args);

    return HAL_UART_Transmit(&huart2, (uint8_t*)buffer, strlen(buffer), HAL_MAX_DELAY);
}

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{ 
    if (huart->Instance == USART2)	
    {

        HAL_UART_Receive_IT(&huart2,RxBuffer1,1);	
    }
}

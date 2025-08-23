#ifndef __UART_H__
#define __UART_H__

#include "main.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

typedef struct Satellite
{
    uint8_t PRN;
    uint8_t Elevation;
    uint16_t Azimuth;
    uint8_t SNR;
}Satellite;

extern UART_HandleTypeDef huart2;

int myprintf(const char *format, ...);

#endif
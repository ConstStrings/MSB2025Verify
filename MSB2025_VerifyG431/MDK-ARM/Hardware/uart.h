#ifndef __UART_H__
#define __UART_H__

#include "main.h"
#include <string.h>
#include <stdarg.h>
#include <stdio.h>

extern UART_HandleTypeDef huart3;

int myprintf(const char *format, ...);

#endif

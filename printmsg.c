/*
 * printmsg.c
 *
 *  Created on: Sep 10, 2021
 *      Author: BillyC
 */
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
#include "stm32f4xx_hal.h"

#define BL_DEBUG_MSG_EN

extern UART_HandleTypeDef huart2;
void printmsg(char * format, ...)
{
# ifdef BL_DEBUG_MSG_EN
	char str[80];

	va_list args;
	va_start (args, format);
	vsprintf(str, format, args);
	HAL_UART_Transmit(&huart2,  (uint8_t *)str, strlen(str), HAL_MAX_DELAY);
	va_end(args);
#endif
}

int __io_putchar(int ch)
{
	HAL_UART_Transmit(&huart2,  (uint8_t *) &ch, 1, 10);

	return ch;
}

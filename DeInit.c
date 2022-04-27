/*
 * DeInit.c
 *
 *  Created on: Feb 15, 2022
 *      Author: BillyC
 */

#include "main.h"


extern UART_HandleTypeDef huart2;




void deinitEverything()
{
    HAL_UART_DeInit(&huart2);
	HAL_GPIO_DeInit(LD2_GPIO_Port, LD2_Pin);
	HAL_GPIO_DeInit(B1_GPIO_Port, B1_Pin);


	 /* GPIO Ports Clock Enable */
	  __HAL_RCC_GPIOC_CLK_DISABLE();
	  __HAL_RCC_GPIOH_CLK_DISABLE();
	  __HAL_RCC_GPIOA_CLK_DISABLE();
	  __HAL_RCC_GPIOB_CLK_DISABLE();

       HAL_RCC_DeInit();
       HAL_DeInit();
/*
       SysTick->CTRL = 0;
       SysTick->LOAD = 0;
       SysTick->VAL = 0;
*/
}

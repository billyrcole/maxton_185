/*
 * task_TempADC.c
 *
 *  Created on: Mar 2, 2022
 *      Author: BillyC
 */



/**
* @brief Function implementing the TaskTempADC thread.
* @param argument: Not used
* @retval None
*/

#include "main.h"
#include "cmsis_os.h"
#include "printmsg.h"
#include  "message.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>


extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;

extern osMessageQueueId_t QuepressureSenHandle;


float  ActualPressure;

/**
  * \brief		Calculates the temperature from the raw ADC value
  *
  * \param[in]	raw_adc		Unsigned 16-bit integer from ADC
  * \return		Temperature in degrees as a floating point value
  */
static float getPressure(uint16_t raw_pressure){

 	// Convert the raw ADC value into psi

 	return (raw_pressure * 1.68);
 }




void  pressureSensor(void *argument)
{
	// printf("pressure task\n\r");
	   /* USER CODE END 2 */

  /* Infinite loop */
  for(;;)
  {
	  uint16_t pressureReading;
	 // if(HAL_ADC_Start_DMA(&hadc1,  (uint32_t*)&pressureReading, 1) != HAL_OK)
	 //	  Error_Handler();
	  HAL_ADC_Start_IT(&hadc1);
	  osMessageQueueGet(QuepressureSenHandle, &pressureReading, 0U,0U);

	//  ActualPressure = getPressure(pressureReading);
	 // printf("pressureSensor.\r\n");
	//  osMessageQueuePut(QpressureSensorHandle, &pressureReading[2], 0U,0U);
   //   printf("pressure = %d \n\r",pressureReading);
      osDelay(500);
  }
  /* USER CODE END TempADC */
}

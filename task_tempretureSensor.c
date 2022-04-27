/*
 * tempretureSensor.c
 *
 *  Created on: Mar 4, 2022
 *      Author: BillyC
 */


#include "main.h"
#include "cmsis_os.h"
#include "printmsg.h"
#include  "message.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
//#include "cmsis_os2.h"

extern osTimerId_t tempReportTimmerHandle;

extern uint32_t exec1;
uint32_t    timerDelay = 1000;
osStatus_t  status;

extern ADC_HandleTypeDef hadc2;
extern DMA_HandleTypeDef hdma_adc2;
extern int16_t  ActualTemp;
extern osMessageQueueId_t QueTasktempSensorHandle;
//extern osMessageQueueId_t QtempretureSensorHandle;
/**
  * \brief		Calculates the temperature from the raw ADC value
  *
  * \param[in]	raw_adc		Unsigned 16-bit integer from ADC
  * \return		Temperature in degrees as a floating point value
  */
static float getTemp(uint16_t raw_adc){

 	// Convert the raw ADC value into degrees Celsius
 	float tmpC = (((float)raw_adc * 2062.5/4096) - 645) / 10; //595

 	return (tmpC*1.8 + 32);
 }



/* tempReport function */
void tempReport(void *argument)
{
  /* USER CODE BEGIN tempReport */

  /* USER CODE END tempReport */
}



// if (status != osOK) {
   // Timer could not be started
 //}



 uint16_t tempretureReading;
void tempretureSensor(void *argument)
{

/*
	osTimerStart(tempReportTimmerHandle, timerDelay);
	if (status != osOK)
	  {
		 printf("tempreture task timmer not stated\n\r");
	  }
*/
	// printf("tempreture task\n\r");
  /* USER CODE BEGIN tempretureSensor */
  /* Infinite loop */
  for(;;)
  {

	  HAL_ADC_Start_IT(&hadc2);

	  //if(HAL_ADC_Start_DMA(&hadc2,(uint32_t*)&tempretureReading, 1) != HAL_OK)
	 	//  Error_Handler();

	  osMessageQueueGet(QueTasktempSensorHandle, &tempretureReading, 0U,osWaitForever);
	 // printf("tempretureSensor.\r\n");
	//  osMessageQueuePut(QpressureSensorHandle, &pressureReading[2], 0U,0U);

      ActualTemp = getTemp(tempretureReading);
    //  printf("tempreture = %d \n\r",tempretureReading);
    osDelay(1000);
  }
  /* USER CODE END tempretureSensor */
}

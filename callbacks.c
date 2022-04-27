/*
 * callbacks.c
 *
 *  Created on: Mar 1, 2022
 *      Author: BillyC
 */

#include "main.h"
#include "cmsis_os.h"

extern uint16_t tempretureReading;
extern osMessageQueueId_t QueTasktempSensorHandle;
extern osMessageQueueId_t QuepressureSenHandle;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern uint16_t pressureReading;
uint16_t adcVal[16];

float  ActualTemp;
float  ActualPressure;


extern int16_t actcel;
int16_t position;
extern int16_t indx;
int16_t oldpos;
int16_t speed;
int16_t oldspeed;
extern TIM_HandleTypeDef htim2;
extern TIM_HandleTypeDef htim7;


 /**
  * \brief		Calculates the temperature from the raw ADC value
  *
  * \param[in]	raw_adc		Unsigned 16-bit integer from ADC
  * \return		Temperature in degrees as a floating point value
  */

void HAL_ADC_ConvCpltCallback(ADC_HandleTypeDef* hadc)  //<----------------------------------------------------------------------------
  {
	  uint16_t rawPressure;
      uint16_t rawTempreure;


	//raw = HAL_ADC_GetValue(&hadc1);
 //  ActualTemp = getTemp( adcVal[0]);

 // ActualPressure = getPressure( pressureReading[1]);
    HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);


 if(hadc == &hadc1)
 {
	 rawPressure = HAL_ADC_GetValue (&hadc1);
	 osMessageQueuePut(QuepressureSenHandle, &rawPressure, 0U,0U);
 }

 if  (hadc == &hadc2)
 {


	 rawTempreure = HAL_ADC_GetValue (&hadc2);
	 	 osMessageQueuePut(QueTasktempSensorHandle, &rawTempreure, 0U,0U);
 }
	// raw_pressure = HAL_ADC_GetValue(&hadc2);
	//RealPressure = getPressure(raw_pressure);
 }
/**/



/**
  * @brief  Input Capture callback in non-blocking mode
  * @param  htim TIM IC handle
  * @retval None
  */
void HAL_TIM_IC_CaptureCallback(TIM_HandleTypeDef *htim)
{
	if(htim == &htim2)
	{


	}

}




/**
  * @brief  Period elapsed callback in non-blocking mode
  * @param  htim TIM handle
  * @retval None

void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
	if (htim->Instance == TIM7)
	  {
		indx++;
		if(indx == 500)
		{
			speed = ((position - oldpos)*2);//in clicks per sec
			actcel = ((speed - oldspeed)*2);
			oldspeed = speed;
			oldpos = position;
			indx = 0;
		}


	  }


}
 */

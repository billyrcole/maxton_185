/*
 * task_Encoder.c
 *
 *  Created on: Mar 14, 2022
 *      Author: BillyC
 */
#include "main.h"
#include "cmsis_os.h"
#include "printmsg.h"
#include  "message.h"
#include <stdarg.h>
#include <string.h>
#include <stdio.h>
extern osMessageQueueId_t hQueueMotorCmdsHandle;

#define PULSES_PER_REVOLUTION 2500.0

 int16_t actcel = 0;
 int16_t position;
 int16_t indx = 0;
 int16_t oldpos;
 double spd;

 int16_t oldspeed;
 extern TIM_HandleTypeDef htim2;
 uint32_t u;
 volatile uint32_t i=0;
 volatile uint32_t tim2_cnt1=0;
 volatile uint32_t tim2_cnt2=0;
 volatile uint32_t tick = 0;
 volatile unsigned int  diff=0;

void task_Encoder(void *argument)
{

//	osStatus_t status;

  /* USER CODE BEGIN task_Encoder */
	HAL_TIM_Encoder_Start(&htim2, TIM_CHANNEL_ALL);


  /* Infinite loop */

   tim2_cnt1 = __HAL_TIM_GET_COUNTER(&htim2);
   tick = HAL_GetTick();

  for(;;)
  {
	 // status = osMessageQueuePut(hQueueMotorCmdsHandle, &msg, 0U, osWaitForever);
	//  	if (status == osOK)
	 // 				printmsg("osMessageQueuePut...osOK \n\r");

      u =HAL_GetTick();
	 if ((u - tick) > 1000L)
	  {
		 i++;
		 tim2_cnt2 = __HAL_TIM_GET_COUNTER(&htim2);

		 if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2))
		   {
			 if (tim2_cnt2 < tim2_cnt1) /* Check for counter underflow */
				 diff = tim2_cnt1 - tim2_cnt2;
			else
				 diff = (4294967295 - tim2_cnt1) + tim2_cnt2;



		   }// END if (__HAL_TIM_IS_TIM_COUNTING_DOWN(&htim2))
		 else
		 {
			 if (tim2_cnt2 > tim2_cnt1) /* Check for counter overflow */
				 diff = tim2_cnt2 - tim2_cnt1;
			 else
				 diff = (4294967295 - tim2_cnt1) + tim2_cnt2;
		 }//end else


	//	 printf("Difference = %u \n\r",diff);
		 spd = ((diff / PULSES_PER_REVOLUTION)/60.0);

		 /* If the first three bits of SMCR register are set to 0x3
		  * then the timer is set in X4 mode (TIM_ENCODERMODE_TI12)
		  * and we need to divide the pulses counter by two, because
		  * they include the pulses for both the channels
		  */

		 if ((TIM2->SMCR & 0x3) == 0x3)
			 spd /= 2;

		// printf("Speed: %f RPM\r\n", speed);

tim2_cnt1 = __HAL_TIM_GET_COUNTER(&htim2);
		 tick =  HAL_GetTick();

	  }// END  if (HAL_GetTick() - tick > 1000L)


  /* USER CODE END task_Encoder */
   }
}






	//  printf("Encoder task\n\r");
	  /*supende wait for irq to start
	    tim2_cnt = __HAL_TIM_GET_COUNTER(&htim2);
	  	count = (int16_t)tim2_cnt;
	  	position = count/4;
	  	 printf("position = %d \n\r",position);
*/


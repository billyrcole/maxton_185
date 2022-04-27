/*
 * task_motorParamUpdate.c
 *
 *  Created on: Mar 7, 2022
 *      Author: BillyC
 */
#include "main.h"
#include "cmsis_os.h"
#include "flashWrite.h"
#include "printmsg.h"
#include  "message.h"
#include "motors.h"
#include "TMC5041.h"
#include "testheader.h"


extern osMessageQueueId_t QueueMotorParamUpdateHandle;

#define TEST_CASE 10
TMC5041TypeDef tmc5041;
extern SPI_HandleTypeDef hspi2;
#define MOTOR_DOWN_CHECK		MOTOR_NUM_1
#define MOTOR_BYPASS			MOTOR_NUM_0






void motorParamUpdate(void *argument)
{
	osStatus_t status;
	uint8_t msg;

	l_coil_msg.ID = CAN_NODE_ID_VCB;
	l_coil_msg.DataLength = 1;
  /* USER CODE BEGIN motorParamUpdate */

	   __HAL_SPI_ENABLE(&hspi2);
	    HAL_GPIO_WritePin(GPIOC, MOTORCS_Pin, GPIO_PIN_SET);
	    HAL_Delay(100);
	   motor_init(&hspi2, &tmc5041);   // MotorError_t
	    HAL_Delay(5000);

	    motor_setPosition(MOTOR_DOWN_CHECK, 0);
	    HAL_Delay(5000);
	    //motor_moveToPosition(MOTOR_DOWN_CHECK, 10000);//motor_cmd.Value
	     motor_moveAtSpeed(MOTOR_BYPASS, -40730);
	     HAL_Delay(5000);
/**/

  /* Infinite loop */
  for(;;)
  {
#ifdef TEST_CASE



	  status = osMessageQueueGet(QueueMotorParamUpdateHandle, &msg, NULL, osWaitForever);

  if (status == 0)
  {


	  switch (msg)
	  {
	       case 0x80:
	    	   new_call = EVENT_NO_COIL;
	    	  // motor_moveAtSpeed(MOTOR_DOWN_CHECK, 40730);
              break;
	       case 0x81:
	    	   new_call = EVENT_DL;
	    	   //motor_moveAtSpeed(MOTOR_DOWN_CHECK, -40730);
	    	   break;
	       case 0x82:
	    	   new_call = EVENT_DHS;
	    	  // motor_stop(MOTOR_DOWN_CHECK);
	    	   break;
	       case 0x83:
	    	   new_call = EVENT_UL;
	    	   //motor_moveAtSpeed(MOTOR_DOWN_CHECK, -40730);
	    	   break;
	       case 0x84:
	    	   new_call = EVENT_UHS;
	    	   motor_stop(MOTOR_DOWN_CHECK);

	    	   break;



	       default:
	    	   break;;


	  }

		/* Check if the call is new and send a command if it is */
		if(new_call != last_call){
			l_coil_msg.Data[0] = new_call;
			osMessageQueuePut(hQueueFSMHandle, &l_coil_msg, 0U,0U);

		//	printf("Call received: %d\r\n", new_call);

			last_call = new_call;
		}


  }


#endif
    osDelay(10000);

  }

}/* USER CODE END motorParamUpdate */

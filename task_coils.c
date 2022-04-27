/**
 * @file		task_coils.c
 * @brief		RTOS task that handles incoming coil calls
 * @addtogroup	emv185
 * @author		Taylor Street <tstreet@maxtonvalve.com>
 * @date		11/12/2020
 *
 * @{
 */
#include <stdio.h>
#include "main.h"
#include "sysconfig.h"
//#include "rtos_tasks.h"
#include "message.h"
#include "cmsis_os.h"
#include "task.h"


extern osMessageQueueId_t hQueueFSMHandle;

#define COIL_CALL_DEBOUNCE_MASK		( 0x0004 )		/*!< Debounce requires 4 consecutive samples */

/* Call trackers */
Event_t last_call = EVENT_NO_COIL;
Event_t new_call = EVENT_NO_COIL;

/* Debounce counters */
uint32_t uhs_count = 0;
uint32_t ul_count = 0;
uint32_t dhs_count = 0;
uint32_t dl_count = 0;
uint32_t no_coil_count = 0;

/* CAN message */
static CANMessage_t l_coil_msg;

void task_coils(void *params)
{
	/* Get the tick count for cyclic task operation */
//	TickType_t xPreviousWakeTime = xTaskGetTickCount();

	//printf("Coils task started.\r\n");



	l_coil_msg.ID = CAN_NODE_ID_VCB;
	l_coil_msg.DataLength = 1;

	while(1){

		/* Record the coil GPIO inputs */
		portENTER_CRITICAL();
		gU = (HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_0) == GPIO_PIN_RESET) ? 1 : 0;              //BLUE - dri by PA12
		//gU = (HAL_GPIO_ReadPin(PIN_UHS_GPIO_Port, PIN_UHS_Pin) == GPIO_PIN_RESET) ? 1 : 0;
		gUL = (HAL_GPIO_ReadPin(PIN_UL_GPIO_Port, PIN_UL_Pin) == GPIO_PIN_RESET) ? 1 : 0;  //RED - dri by PC8
		gD = (HAL_GPIO_ReadPin(PIN_DHS_GPIO_Port,PIN_DHS_Pin) == GPIO_PIN_RESET) ? 1 : 0;  //GREY - dri by PC5
		gDL = (HAL_GPIO_ReadPin(PIN_DL_GPIO_Port, PIN_DL_Pin) == GPIO_PIN_RESET) ? 1 : 0;  //VIO -  dri by PC6
		portEXIT_CRITICAL();

		/* Shift on every loop to serve as a "counter" */
		uhs_count <<= 1;
		ul_count <<= 1;
		dhs_count <<= 1;
		dl_count <<= 1;
		no_coil_count <<= 1;

		/* Decipher the call and debounce it before sending an item to CAN out task */
		if(gU && !gD && !gDL){					// UHS
			uhs_count |= 1;

			if( !(~uhs_count & COIL_CALL_DEBOUNCE_MASK) ){
				new_call = EVENT_UHS;
			}
		}
		else if(!gU && gUL && !gD && !gDL){		// UL
			ul_count |= 1;

			if( !(~ul_count & COIL_CALL_DEBOUNCE_MASK) ){
				new_call = EVENT_UL;
			}
		}
		else if(!gU && !gUL && gD){				// DHS
			dhs_count |= 1;

			if( !(~dhs_count & COIL_CALL_DEBOUNCE_MASK) ){
				new_call = EVENT_DHS;
			}
		}
		else if(!gU && !gUL && !gD && gDL){		// DL
			dl_count |= 1;

			if( !(~dl_count & COIL_CALL_DEBOUNCE_MASK) ){
				new_call = EVENT_DL;
			}
		}
		else if(!gU && !gUL && !gD && !gDL){	// No Coils
			no_coil_count |= 1;

			if( !(~no_coil_count & COIL_CALL_DEBOUNCE_MASK) ){
				new_call = EVENT_NO_COIL;
			}
		}

		/* Check if the call is new and send a command if it is */
		if(new_call != last_call){
			l_coil_msg.Data[0] = new_call;

			osMessageQueuePut(hQueueFSMHandle, &l_coil_msg, 0U,0U);
		//	printf("Call received: %d\r\n", new_call);

			last_call = new_call;
		}

		/* Debouncing delay (possibly unnecessary) */
		//vTaskDelayUntil(&xPreviousWakeTime, pdMS_TO_TICKS(20));
	}
}

/** @} */

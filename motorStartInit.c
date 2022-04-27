/*
 * motorStartInit.c
 *
 *  Created on: Apr 18, 2022
 *      Author: BillyC
 */
#include "main.h"
#include "printmsg.h"
#include  "message.h"
#include "motors.h"
#include "TMC5041.h"

TMC5041TypeDef tmc5041;
extern SPI_HandleTypeDef hspi2;
#define MOTOR_DOWN_CHECK		MOTOR_NUM_1
#define MOTOR_BYPASS			MOTOR_NUM_0


void motorStartInit()
{
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
}

/**
 * @file		task_motors.c
 * @brief		RTOS task that handles all requests for motor and solenoid activity
 * @addtogroup	emv185
 * @author		Taylor Street <tstreet@maxtonvalve.com>
 * @date		04/17/2020
 *
 * @{
 */

#include "sysconfig.h"
#include "motors.h"
#include "cmsis_os.h"
/* External objects -------------------------------------------------------*/
//extern osMessageQueueId_t hQueueMotorCmdsHandle;
extern osMessageQueueId_t hQueueMotorCmdsHandle;
//MotorCommand_t motor_cmd;
static MotorCommand_t motor_cmd;
void task_motors(void *params)
{
								/* Holds in the incoming motor command */

//	printf("MOTORS task started.\r\n");

	while(1){

		/* Wait for a new motor command to come in */
		osMessageQueueGet(hQueueMotorCmdsHandle, &motor_cmd,NULL, osWaitForever);

	//	printf("MOTOR message received.\r\n");

		switch(motor_cmd.Command){

			case MOTOR_CMD_SET_SPEED:
				motor_setVelocity(motor_cmd.MotorNumber, motor_cmd.Value);
				break;

			case MOTOR_CMD_STOP:
				motor_stop(motor_cmd.MotorNumber);
				break;

			case MOTOR_CMD_MOVE_TO_POS:
				if(motor_cmd.MotorNumber == MOTOR_BYPASS){
					motor_moveToPosition(MOTOR_BYPASS, motor_cmd.Value);
				}
				else if(motor_cmd.MotorNumber == MOTOR_DOWN_CHECK){
					motor_moveToPosition(MOTOR_DOWN_CHECK, motor_cmd.Value);
					osDelay(100);
					motor_setPosition(MOTOR_DOWN_CHECK, 0);//motor_cmd.Value
				}
				break;

			case MOTOR_CMD_MOVE_BY_AMOUNT:
				if(motor_cmd.MotorNumber == MOTOR_BYPASS){
					motor_moveByAmount(MOTOR_BYPASS, motor_cmd.Value);
				}
				else if(motor_cmd.MotorNumber == MOTOR_DOWN_CHECK){
					motor_moveByAmount(MOTOR_DOWN_CHECK, motor_cmd.Value);
				}
				break;

			case MOTOR_CMD_MOVE_AT_RATE:
				motor_moveAtSpeed(motor_cmd.MotorNumber, motor_cmd.Value);
				break;

			case MOTOR_CMD_HOME:
				/* This command depends on the BP or DC sensor interrupt being set
				 * and stopping the motor when it is hit.
				 */
				if(motor_cmd.MotorNumber == MOTOR_BYPASS){
					motor_moveAtSpeed(MOTOR_BYPASS, 50000);
				}
				else if(motor_cmd.MotorNumber == MOTOR_DOWN_CHECK){
					motor_moveAtSpeed(MOTOR_DOWN_CHECK, -50000);
				}
				break;

			case MOTOR_CMD_SOLENOID_ENGAGE:
				if(motor_cmd.Value == SOLENOID_CLOSED){
					SOLENOID_SET_CLOSE();
					printf("Solenoid closed.\r\n");
				}
				else if(motor_cmd.Value == SOLENOID_OPEN){
					SOLENOID_SET_OPEN();
					printf("Solenoid opened.\r\n");
				}
				break;

		} /* END switch */
		 osDelay(100);

	} /* END while */
}

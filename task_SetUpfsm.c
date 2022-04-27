/**
  ******************************************************************************
  * \file		task_fsm.c
  * \brief		RTOS task that operates as the state machine and executes incoming
  * 			commands from the CAN input task
  * \date		05/28/2020
  * \author		Taylor Street <tstreet@maxtonvalve.com>
  * \ingroup	emv185
  ******************************************************************************
  * @{
  */

#include "sysconfig.h"
//#include "rtos_tasks.h"
#include "message.h"
#include "cmsis_os.h"


/*** External Objects ********************************************************/
extern void valve_initialize(void);
extern CANMessage_t coil_msg;
extern osMessageQueueId_t hQueueFSMHandle;
extern osMessageQueueId_t hQueueMotorCmdsHandle;

/*** Private Objects *********************************************************/
static FSM_t fsm;
static MotorCommand_t motor_cmd;

/*** Private Functions *******************************************************/
static void run_uhs(FSM_t *sm);
static void run_ul(FSM_t *sm);
static void run_dhs(FSM_t *sm);
static void run_dl(FSM_t *sm);
static void stop_up(FSM_t *sm);
static void stop_down(FSM_t *sm);
static void run_learn_up(FSM_t *sm);
static void run_learn_down(FSM_t *sm);
static void run_learn_stop(FSM_t *sm);

static void ignore_event(FSM_t *sm);
static const State_t fsm_get_state(void);

// TODO: When a SETUP event occurs in STATE_LEARN, exit setup gracefully instead of just ignoring the event

/* State machine action map */
void (*const state_machine[MAX_STATES][MAX_EVENTS])(FSM_t *sm) = {
/*  ******************************** EVENTS *****************************************************/
/*	 UHS,			UL,				DHS,			DL,				NO_COIL,			SETUP	*/
	{ignore_event,	run_uhs,		run_ul,			run_dhs,		run_dl,				ignore_event},	/* Idle				*/
    {stop_up,		ignore_event,	run_ul,			stop_up,		stop_up,			ignore_event},	/* Running, UHS		*/
    {stop_up,		run_uhs,		ignore_event,	stop_up,		stop_up,			ignore_event},	/* Running, UL		*/
    {stop_down,		stop_down,		stop_down,		ignore_event,	run_dl,				ignore_event},	/* Running, DHS		*/
    {stop_down,		stop_down,		stop_down,		run_dhs,		ignore_event,		ignore_event},	/* Running, DL		*/
    {ignore_event,	ignore_event,	ignore_event,	ignore_event,	ignore_event,		ignore_event},	/* Procedures for STATE_SETUP */
    {run_learn_stop,run_learn_up,	run_learn_up,	run_learn_down,	run_learn_down,		ignore_event},	/* In Learn Mode	*/
};

void task_fsm(void *params)
{
	//Event_t event;

//	printf("SYSTEM task started.\r\n");

	/* Set the initial values of the FSM */
	fsm.CurrentState = STATE_IDLE;
	fsm.Inspection = false;
	fsm.Mode = MODE_NORMAL;

	/* Initialize the valve configuration values */
	valve_initialize();

	/* Start the heartbeat timer */
	//xTimerStart(hTimerHeartbeat, portMAX_DELAY);

	while(1){

		//xQueueReceive(hQueueFSM, &event, portMAX_DELAY);

		osMessageQueueGet(hQueueFSMHandle, &coil_msg, 0U,0U);
//		printf("System State Changed.\r\n");

		// TODO: If state == SETUP, suspend motors task, suspend encoder task
		// TODO: If state == IDLE, reset motors queue and resume motors task
		// TODO: If state == RUNNING, resume encoder task

	    if(	((coil_msg.Data[0] >= 0) && (coil_msg.Data[0] < MAX_EVENTS))
				&& ((fsm.CurrentState >= 0) && (fsm.CurrentState < MAX_STATES)) ) {

	    	/* Load the event into the state machine */
	    	fsm.Event = (Event_t) coil_msg.Data[0];

	    	/* Execute the action for the state/event combination. The current state
	    	 * will be updated in the action function. */
	        state_machine[fsm.CurrentState][fsm.Event](&fsm);

	    } else {
	        // TODO: Handle invalid state/event combinations
	    	printf("Invalid event.\r\n");
	    }

	}

}


static const State_t fsm_get_state(void)
{
	return fsm.CurrentState;
}


/*****************************************************************************/
/*** State Machine Actions ***************************************************/
/*****************************************************************************/

/* For run actions, the runs can either be
 * executed with run profiles or using a sequence
 * of run calls that rely on vTaskDelay()
 */

static void run_uhs(FSM_t *sm)
{
	/* Update the FSM state */
	sm->CurrentState = STATE_RUN_UHS;

	printf("UHS call.\r\n");

	switch(sm->Mode){

		case MODE_NORMAL:
			motor_cmd.MotorNumber = MOTOR_BYPASS;
			motor_cmd.Command = MOTOR_CMD_SET_SPEED;
			motor_cmd.Value = EMV185.Speeds.UA;
			//xQueueSendToBack(hQueueMotorCmds, &motor_cmd, 1000);
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

			motor_cmd.Command = MOTOR_CMD_MOVE_TO_POS;
			motor_cmd.Value = EMV185.Positions.UHS;
			//xQueueSendToBack(hQueueMotorCmds, &motor_cmd, 1000);
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);
			break;

		case MODE_OSV:

			break;

		case MODE_RELIEF:
			break;

		default:
			break;
	}

}

static void run_ul(FSM_t *sm)
{
	/* Update the FSM state */
	sm->CurrentState = STATE_RUN_UL;

	printf("UL call.\r\n");

	switch(sm->Mode){

		case MODE_NORMAL:
			motor_cmd.MotorNumber = MOTOR_BYPASS;
			motor_cmd.Command = MOTOR_CMD_SET_SPEED;
			motor_cmd.Value = EMV185.Speeds.UT;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

			motor_cmd.Command = MOTOR_CMD_MOVE_TO_POS;
			motor_cmd.MotorNumber = MOTOR_BYPASS;
			motor_cmd.Value = EMV185.Positions.UL;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

			break;

		case MODE_OSV:

			break;

		case MODE_RELIEF:
			break;

		default:
			break;
	}

}

static void run_dhs(FSM_t *sm)
{
	/* Update the FSM state */
	sm->CurrentState = STATE_RUN_DHS;

	printf("DHS call.\r\n");

	switch(sm->Mode){

		case MODE_NORMAL:

			/* Pull the solenoid up */
			motor_cmd.Command = MOTOR_CMD_SOLENOID_ENGAGE;
			motor_cmd.Value = SOLENOID_OPEN;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

			motor_cmd.MotorNumber = MOTOR_DOWN_CHECK;
			motor_cmd.Command = MOTOR_CMD_SET_SPEED;
			motor_cmd.Value = EMV185.Speeds.DA;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

			/* Move to the down accelerate position */
			motor_cmd.Command = MOTOR_CMD_MOVE_TO_POS;
			motor_cmd.MotorNumber = MOTOR_DOWN_CHECK;
			motor_cmd.Value = EMV185.Positions.DHS;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

			break;

		case MODE_OSV:

			break;

		case MODE_RELIEF:
			break;

		default:
			break;
	}

}

static void run_dl(FSM_t *sm)
{
	/* Update the FSM state */
	sm->CurrentState = STATE_RUN_DL;

	printf("DL call.\r\n");

	switch(sm->Mode){

		case MODE_NORMAL:

			/* Pull the solenoid up */
			motor_cmd.Command = MOTOR_CMD_SOLENOID_ENGAGE;
			motor_cmd.Value = SOLENOID_OPEN;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

			motor_cmd.MotorNumber = MOTOR_DOWN_CHECK;
			motor_cmd.Command = MOTOR_CMD_SET_SPEED;
			motor_cmd.Value = EMV185.Speeds.DT;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

			/* Move to the down transition position */
			motor_cmd.Command = MOTOR_CMD_MOVE_TO_POS;
			motor_cmd.MotorNumber = MOTOR_DOWN_CHECK;
			motor_cmd.Value = EMV185.Positions.DL;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

			break;

		case MODE_OSV:

			break;

		case MODE_RELIEF:
			break;

		default:
			break;
	}

}

// TODO: Change the name of this to reflect a NO COIL call is coming in
//		rather than stopping all motor movement
static void stop_up(FSM_t *sm)
{
	printf("Up Stop call.\r\n");

	switch(sm->Mode){
		case MODE_NORMAL:
			motor_cmd.MotorNumber = MOTOR_BYPASS;
			motor_cmd.Command = MOTOR_CMD_SET_SPEED;
			motor_cmd.Value = EMV185.Speeds.US;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

			motor_cmd.Command = MOTOR_CMD_MOVE_TO_POS;
			motor_cmd.MotorNumber = MOTOR_BYPASS;
			motor_cmd.Value = EMV185.Positions.ParkBP;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

			break;

		case MODE_OSV:

			break;

		case MODE_RELIEF:
			break;

		default:
			break;
	}

	/* Update the FSM state */
	sm->CurrentState = STATE_IDLE;
}

static void stop_down(FSM_t *sm)
{
	printf("Down Stop call.\r\n");

	switch(sm->Mode){
		case MODE_NORMAL:

			/* Move to down stop position */
			motor_cmd.Command = MOTOR_CMD_MOVE_TO_POS;
			motor_cmd.MotorNumber = MOTOR_DOWN_CHECK;
			motor_cmd.Value = EMV185.Positions.ParkDC;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

			motor_cmd.MotorNumber = MOTOR_DOWN_CHECK;
			motor_cmd.Command = MOTOR_CMD_SET_SPEED;
			motor_cmd.Value = EMV185.Speeds.DS;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

			/* Drop the solenoid */
			motor_cmd.Command = MOTOR_CMD_SOLENOID_ENGAGE;
			motor_cmd.Value = SOLENOID_CLOSED;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

			break;

		case MODE_OSV:

			break;

		case MODE_RELIEF:
			break;

		default:
			break;
	}

	/* Update the FSM state */
	sm->CurrentState = STATE_IDLE;
}

static void run_learn_up(FSM_t *sm)
{
	// Run learn up

	/* Should the state be LEARN, LEARN_UP, or just RUN_UP? */
	sm->CurrentState = STATE_LEARN;
}

static void run_learn_down(FSM_t *sm)
{
	// Run learn up

	sm->CurrentState = STATE_LEARN;
}

static void run_learn_stop(FSM_t *sm)
{
	// Run learn down

	sm->CurrentState = STATE_LEARN;
}

static void ignore_event(FSM_t *sm)
{
	/* Handle invalid events or just do nothing */
	printf("Invalid event and state combination.\r\n");
}

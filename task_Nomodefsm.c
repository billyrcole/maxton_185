/*
 * task_SetUpfsm.c
 *
 *  Created on: Feb 28, 2022
 *      Author: BillyC
 */


#include "sysconfig.h"
#include "message.h"
#include "cmsis_os.h"
#include "motorValues.h"

/*** External Objects ********************************************************/
extern void valve_initialize(void);
//extern CANMessage_t coil_msg;
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
static void ignore_event(FSM_t *sm);
static const State_t fsm_get_state(void);
//#define TEST
#ifdef TEST
/* State machine action map */
void (*const state_machine[MAX_STATES][MAX_EVENTS])(FSM_t *sm) = {
/*  ******************************** EVENTS *****************************************************/
/*	 UHS,			UL,				DHS,			DL,				NO_COIL,				*/
	{run_uhs,	    ignore_event,	run_dhs,		ignore_event,	ignore_event	},	/* Idle				*/
    {run_uhs,	    run_ul,	        ignore_event,	ignore_event,	stop_up			},	/* Running, UHS		*/
    {ignore_event,	run_ul,		   ignore_event,	ignore_event,	stop_up			},	/* Running, UL		*/
    {ignore_event,	ignore_event,	run_dhs,		run_dl,     	stop_down		},	/* Running, DHS		*/
    {ignore_event,  ignore_event,	ignore_event,	run_dl,		    stop_down	    },	/* Running, DL		*/

};

#else
void (*const state_machine[MAX_STATES][MAX_EVENTS])(FSM_t *sm) = {
/*  ******************************** EVENTS *****************************************************/
/*	 EVENT_NO_COIL		EVENT_UHS,				EVENT_UL,			    EVENT_DHS,	        EVENT_DL,				*/
	{ignore_event,	     run_uhs,	            run_ul,		             run_dhs,	        run_dl	        },	/* Idle				*/
    {stop_up,	     	ignore_event,	        run_ul,	                 stop_up,	        stop_up	        },	/* Running, UHS		*/
    {stop_up,			run_uhs,	    	    ignore_event,	         stop_up,	        stop_up	        },	/* Running, UL		*/
    {stop_down,			stop_down,	            stop_down,		         ignore_event,      run_dl		    },	/* Running, DHS		*/
    {stop_down,  		stop_down ,	            stop_down,	             run_dhs,		    ignore_event	},	/* Running, DL		*/

};
#endif





//static FSM_t fsm;
//static MotorCommand_t motor_cmd;

static CANMessage_t coil_msg;
void motorfsm(void *params)
{

	/* Set the initial values of the FSM */
	fsm.CurrentState = STATE_IDLE;      // STATE_IDLE - STATE_RUN_UHS - STATE_RUN_UL - STATE_RUN_DHS - STATE_RUN_DL
	fsm.Inspection = false;
	fsm.Mode = MODE_NORMAL;

	/* Initialize the valve configuration values */
//	valve_initialize();

	/* Start the heartbeat timer */
	//xTimerStart(hTimerHeartbeat, portMAX_DELAY);

	while(1){
		osMessageQueueGet(hQueueFSMHandle, &coil_msg, 0U,osWaitForever);
//		printf("System State Changed.\r\n");


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

			motor_cmd.MotorNumber = 1;//MOTOR_BYPASS;
			motor_cmd.Command = MOTOR_CMD_SET_SPEED;
			motor_cmd.Value = EMV185.Speeds.UA;
			//xQueueSendToBack(hQueueMotorCmds, &motor_cmd, 1000);
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,osWaitForever);

			motor_cmd.Command = MOTOR_CMD_MOVE_TO_POS;
			motor_cmd.Value = EMV185.Positions.UHS; //EMV185.Positions.UHS
			//xQueueSendToBack(hQueueMotorCmds, &motor_cmd, 1000);
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,osWaitForever);

}//end static void run_uhs(FSM_t *sm)






static void run_ul(FSM_t *sm)
{
	/* Update the FSM state */
	sm->CurrentState = STATE_RUN_UL;

	printf("UL call.\r\n");


			motor_cmd.MotorNumber = MOTOR_BYPASS;
			motor_cmd.Command = MOTOR_CMD_SET_SPEED;
			motor_cmd.Value = EMV185.Speeds.UT;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

			motor_cmd.Command = MOTOR_CMD_MOVE_TO_POS;
			motor_cmd.MotorNumber = MOTOR_BYPASS;
			motor_cmd.Value = EMV185.Positions.UL;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

}

static void run_dhs(FSM_t *sm)
{
	/* Update the FSM state */
	sm->CurrentState = STATE_RUN_DHS;

	printf("DHS call.\r\n");

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

}// end  run_dhs(FSM_t *sm)



static void run_dl(FSM_t *sm)
{
	/* Update the FSM state */
	sm->CurrentState = STATE_RUN_DL;

	printf("DL call.\r\n");

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


}// end run_dl(FSM_t *sm)

// TODO: Change the name of this to reflect a NO COIL call is coming in
//		rather than stopping all motor movement
static void stop_up(FSM_t *sm)
{
	/* Update the FSM state */
		sm->CurrentState = STATE_IDLE;

	printf("Up Stop call.\r\n");


			motor_cmd.MotorNumber = MOTOR_BYPASS;
			motor_cmd.Command = MOTOR_CMD_SET_SPEED;
			motor_cmd.Value = EMV185.Speeds.US;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

			motor_cmd.Command = MOTOR_CMD_MOVE_TO_POS;
			motor_cmd.MotorNumber = MOTOR_BYPASS;
			motor_cmd.Value = EMV185.Positions.ParkBP;
			osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);



}// end stop_up(FSM_t *sm)

static void stop_down(FSM_t *sm)
{


	printf("Down Stop call.\r\n");

	/* Update the FSM state */
	sm->CurrentState = STATE_IDLE;

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


}// end stop_down(FSM_t *sm)

static void ignore_event(FSM_t *sm)
{

	printf("ignore_event.\r\n");
}

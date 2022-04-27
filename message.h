/**
  ******************************************************************************
  * \file		message.h
  * \brief		Contains all information common between the VCB and RDB projects.
  * 			This file should always be identical in both projects.
  * \date		03/08/2021
  * \author		Taylor Street <tstreet@maxtonvalve.com>
  * \ingroup	emv185
  ******************************************************************************
  * @{
  */

#ifndef _MESSAGE_H_
#define _MESSAGE_H_

#include <stdbool.h>

#define MAX_EVENTS		5
#define MAX_STATES		5

/*** Enumerations ************************************************************/



#define CAN_NODE_ID_VCB			0x62
/**
 * @brief	Possible events to occur in the state machine. Order is critical.
 * 			Any changes to the events require changes to the state
 * 			machine table.
 * @{
 */
typedef enum Event {
	EVENT_NO_COIL = 0x00,
	EVENT_UHS = 0x01,
	EVENT_UL = 0x02,
	EVENT_DHS = 0x03,
	EVENT_DL = 0x04,
	EVENT_SETUP = 0x05
} Event_t;
/** @} */

/**
 * @brief	State machine structure.
 *
 * @{
 */




/**
 * @brief	General purpose messages.
 * @{
 */
typedef enum Message {
	MESSAGE_HEARTBEAT = 0xC0,
	MESSAGE_CONFIG = 0xC1,
	MESSAGE_LEARN = 0xC2
} Message_t;
/** @} */

/**
 * @brief	Motor and actuator commands
 * @{
 */
typedef enum MotorCommand_Type {
	MOTOR_CMD_SET_SPEED = 0xA4,			/*!< Set the motor speed in microsteps/sec */
	MOTOR_CMD_STOP = 0xA5,				/*!< Dead stop the specified motor */
	MOTOR_CMD_MOVE_TO_POS = 0xA6,		/*!< Absolute positional movement */
	MOTOR_CMD_MOVE_BY_AMOUNT = 0xA7,	/*!< Relative positional movement */
	MOTOR_CMD_MOVE_AT_RATE = 0xA8,		/*!< Constant speed movement */
	MOTOR_CMD_HOME = 0xA9,				/*!< Move to preset home position */
	MOTOR_CMD_SOLENOID_ENGAGE = 0xAA	/*!< Open or close the solenoid */
} MotorCommand_Type_t;
/** @} */

/**
 * @brief	Items to be configured
 * @{
 */
typedef enum ConfigureItem {
	CONFIGURE_SETUP_EXIT = 0x70,
	CONFIGURE_INSPECTION = 0x71,
	CONFIGURE_ADJ_MODE = 0x72,
	CONFIGURE_PHASE_POSITION = 0x73,
	CONFIGURE_PHASE_SPEED = 0x74,
	CONFIGURE_ACTUATOR_SPEED = 0x75,
	CONFIGURE_PISTON_COUNT = 0x76,
	CONFIGURE_PISTON_TYPE = 0x77,
	CONFIGURE_PISTON_SIZE = 0x78
} ConfigureItem_t;
/** @} */

/**
 * @brief	Status items
 * @{
 */
typedef enum StatusItem {
	STATUS_REQUEST_PRESSURE = 0x80,
	STATUS_REQUEST_TEMP = 0x81,
	STATUS_REQUEST_POS = 0x82,
	STATUS_REQUEST_SPEED = 0x83
} StatusItem_t;
/** @} */

/**
 * @brief	Possible states for the state machine. Order is critical.
 * 			Any changes to the states require changes to the state
 * 			machine table.
 * 	@{
 */
typedef enum State {
	STATE_IDLE,
	STATE_RUN_UHS,
	STATE_RUN_UL,
	STATE_RUN_DHS,
	STATE_RUN_DL,
	STATE_SETUP,
	STATE_LEARN
} State_t;
/** @} */

/**
 * @brief	Mode extended state variable for the state machine. The mode
 * 			modifies the behavior of event processing within the state machine.
 * @{
 */
typedef enum Mode {
	MODE_NORMAL,
	MODE_RELIEF,
	MODE_OSV
} Mode_t;
/** @} */

/**
 * @brief	Elevator car acceleration phases
 * @{
 */
typedef enum Phase {
	PHASE_UA,
	PHASE_UT,
	PHASE_US,
	PHASE_DA,
	PHASE_DT,
	PHASE_DS,
	PHASE_UHS,
	PHASE_UL,
	PHASE_UP_STOP,
	PHASE_DHS,
	PHASE_DL,
	PHASE_DOWN_STOP
} Phase_t;
/** @} */

/**
 * @brief	Speeds
 * @{
 */
typedef enum PhaseSpeed {
	PHASE_SPEED_UL,
	PHASE_SPEED_DL,
	PHASE_SPEED_DHS
} PhaseSpeed_t;
/** @} */

/*** Structures **************************************************************/

/**
 * @brief	Messages from or to CAN tasks
 * @{
 */
typedef struct CANMessage {
	uint32_t ID;
	uint32_t DataLength;
	uint8_t Data[8];
} CANMessage_t;
/** @} */

/**
 * @brief	System-level commands
 * @{
 */
typedef struct SystemCommand {
	uint8_t Command;
	uint8_t Value;
	uint8_t	Length;
	uint8_t Data[8];
} SystemCommand_t;
/** @} */

/**
 * @brief	Motor driver commands
 * @{
 */
typedef struct MotorCommand {
	uint8_t Command;		/*!< Motor command of type @ref MotorCommand_Type_t */
	uint8_t MotorNumber;	/*!< Motor number, if more than 1 motor */
	int32_t Value;			/*!< Speed, position, etc. depending on the command */
	uint8_t Specifier;		/*!< Additional value that varies by command */
} MotorCommand_t;
/** @} */

/**
 * @brief	The configuration structure is a packet with a single
 * 			configuration instruction. It can be either a read or
 * 			write packet. For write packets, the @ref Value field
 * 			is ignored.
 * 	@{
 */
typedef struct Configuration {
	ConfigureItem_t		Item;
	uint8_t				Specifier;
	uint32_t Value;			/**!< Speed, position, etc. depending on the command */
	bool				Write;
} Configuration_t;
/** @} */



typedef struct FSM {
	State_t		CurrentState;	/*!< Current state of the state machine. */
	Mode_t		Mode;			/*!< Current mode of the state machine. */
	bool		Inspection;		/*!< State variable indicating encoderless mode if true. */
	Event_t		Event;			/*!< Current event being processed by the state machine. */
} FSM_t;


struct BLE_fields {
     // LSB

    uint32_t motor_velcity   :5;
    uint32_t motor           :1;
    uint32_t  FIQ:1;
    uint32_t  IRQ:1;
    uint32_t  A:1;
    uint32_t  E:1;
    uint32_t  IT6:6;
    uint32_t  GE:4;
    uint32_t  DNM:4;
    uint32_t  J:1;
    uint32_t  IT2:2;
    uint32_t  Q:1;
    uint32_t  V:1;
    uint32_t  C:1;
    uint32_t  Z:1;
    uint32_t  N:1;

     // MSB
  };


typedef union BLE_reg_tag {
	uint32_t BLE_raw;
    struct BLE_fields fields;
} CPSR_reg_t;







#endif /* _MESSAGE_H_ */

/** @} */

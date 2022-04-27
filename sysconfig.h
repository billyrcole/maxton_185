/**
  ******************************************************************************
  * \file		sysconfig.h
  * \brief		This file contains the common definitions used throughout
  * 			the application, as well as some configurable entities
  * \date		03/02/2021
  * \author		Taylor Street <tstreet@maxtonvalve.com>
  * \addtogroup	emv185
  ******************************************************************************
  * @{
  */

#ifndef _SYSCONFIG_H_
#define _SYSCONFIG_H_

#ifdef __cplusplus
extern "C" {
#endif


#include <stdio.h>
#include <string.h>
#include <stm32f4xx_hal.h>

/* API */
#include "message.h"
#include <motors.h>


/* FreeRTOS includes
#include <FreeRTOS.h>
#include <task.h>
#include <queue.h>
#include <semphr.h>
#include <timers.h>
*/
/* RTOS task headers */
//#include "rtos_tasks.h"

/** \name Configurable Values */
#define NUM_OF_MOTORS						2
#define HEARTBEAT_RATE_MS					50
#define ENCODER_RATE_INCHES_500PPR			( (float)0.00300022 )
#define ENCODER_RATE_INCHES_2500PPR			( (float)0.00118752 )

/** \name Solenoid Operation */
#define SOLENOID_SET_OPEN()					( HAL_GPIO_WritePin(PORT_SOL_EN, PIN_SOL_EN, GPIO_PIN_SET) )
#define SOLENOID_SET_CLOSE()				( HAL_GPIO_WritePin(PORT_SOL_EN, PIN_SOL_EN, GPIO_PIN_RESET) )

/** \name Valve Sizing Macros */
#define VALVE_SIZE_IN_INCHES( x )			( (uint8_t)((x - 3) / 0.0625) )			/*!< Valid input range is 3" to 8" */
#define VALVE_SIZE_IN_MM( x )				( VALVE_SIZE_IN_INCHES( (x / 25.4) ) )	/*!< Valid input range is 76mm to 203mm */
#define MIL_TO_USTEP( x )					( (51200 / (1000 * 0.125)) * x )
#define VALVE_SIZE_MINIMUM					VALVE_SIZE_IN_INCHES(3)
#define VALVE_SIZE_MAXIMUM					VALVE_SIZE_IN_INCHES(8)

/** \name RTOS Macros */
#ifdef SEGGER_SYSTEMVIEW_ENABLED
	#define RTOS_TIMER_ENTER(tid)			SEGGER_SYSVIEW_RecordEnterTimer(tid)
	#define RTOS_TIMER_EXIT()				SEGGER_SYSVIEW_RecordExitTimer()
#else
	#define RTOS_TIMER_ENTER(thandle)
	#define RTOS_TIMER_EXIT()
#endif
/** @} */



/* System Port & Pin Definitions */
//#define PIN_PUMP_EN				GPIO_PIN_6
#define PORT_PUMP_EN			GPIOA
#define PIN_U					GPIO_PIN_7
#define PORT_U					GPIOC
#define PIN_UL					GPIO_PIN_8
#define PORT_UL					GPIOC
#define PIN_D					GPIO_PIN_7
#define PORT_D					GPIOC
#define PIN_DL					GPIO_PIN_6
#define PORT_DL					GPIOC






/* Global Objects */
volatile State_t gCurrentState;
volatile uint8_t gTemp;
volatile uint16_t gPressure;
volatile int16_t gCarSpeed;
volatile int32_t gCarPosition;
volatile uint8_t gMode;
volatile uint8_t gConnStatus;
volatile uint8_t gU;
volatile uint8_t gUL;
volatile uint8_t gD;
volatile uint8_t gDL;





/**
 * System Valve Type (Roped or Direct)
 * @{
 */
typedef enum ValveType {
	VALVE_TYPE_DIRECT,
	VALVE_TYPE_ROPED
} ValveType_t;
/** @} */

/**
 * Motor Positions
 *
 * This holds the configuration of the motor positions of
 * where to move to for up high speed, up leveling, etc. These
 * values are initialized with defaults and then are updated in
 * Learn Mode.
 * @{
 */
typedef struct PhasePositions {
	int32_t UHS;		/*!< Motor position for Up High Speed */
	int32_t UL;			/*!< Motor position for Up Leveling */
	int32_t ParkBP;		/*!< Motor position for bypass motor parking */
	int32_t DHS;		/*!< Motor position for Down High Speed */
	int32_t DL;			/*!< Motor position for Down Leveling */
	int32_t ParkDC;		/*!< Motor position for down check motor parking */
} PhasePositions_t;
/** @} */

/**
 * Phase Speeds
 *
 * These refer to the *motor speeds* for the specified
 * movement. For instance, the UA speed will set the
 * motor to move at the given speed (counts/second) for
 * the movement that takes the car from park to up high speed.
 * @{
 */
typedef struct PhaseSpeeds {
	uint32_t UA;		/*!< Motor rate for Up Accelerate */
	uint32_t UT;		/*!< Motor rate for Up Transition */
	uint32_t US;		/*!< Motor rate for Up Stop */
	uint32_t DA;		/*!< Motor rate for Down Accelerate */
	uint32_t DT;		/*!< Motor rate for Down Transition */
	uint32_t DS;		/*!< Motor rate for Down Stop */
} PhaseSpeeds_t;
/** @} */






/**
 * Valve Info
 *
 * This is the primary superstructure that contains all
 * configurable information about the valve. These values
 * are configured in Learn Mode or modified via a CAN
 * command.
 * @{
 */
typedef struct ValveInfo {
	ValveType_t Type;				/*!< \ref ValveType */
	uint8_t PistonCount;			/*!< Piston count: 1 or 2 pistons */
	uint8_t Size;					/*!< Piston size, used with \ref VALVE_SIZE_IN_INCHES or \ref VALVE_SIZE_IN_MM */
	uint16_t SpeedUL;				/*!< Target car speed for up leveling */
	uint16_t SpeedDL;				/*!< Target car speed for down leveling */
	uint16_t SpeedDHS;				/*!< Target car speed for down high speed */
//	uint32_t RateBP;
//	uint32_t RateDC;
	PhasePositions_t Positions;		/*!< Motor positions for phases of operation */
	PhaseSpeeds_t Speeds;			/*!< Motor speeds for moving to \ref Positions */
} ValveInfo_t;
/** @} */

//__attribute__ ((section ("mysectionBss")))const ValveInfo_t EMV185;		/*!< Global EMV185 object for use basically everywhere */

/* Global Function Prototypes */
void Error_Handler(void);

/** \name Solednoid States */
#define SOLENOID_OPEN					1
#define SOLENOID_CLOSED					0

/** \name Default Motor Rates: Bypass */
#define RATE_DEFAULT_BYPASS_A1			0x000003E8		// A1 = 1000 (disabled)
#define RATE_DEFAULT_BYPASS_V1			0x00000000		// Disable A1 and D1, use AMAX and DMAX instead
#define RATE_DEFAULT_BYPASS_D1			0x000304D0		// D1 = 1400 (disabled)
#define RATE_DEFAULT_BYPASS_AMAX		0x0000FFFF		// AMAX = 65535 (max)
#define RATE_DEFAULT_BYPASS_VMAX		200000
#define RATE_DEFAULT_BYPASS_DMAX		0x0000FFFF		// DMAX = 65535 (max)

/** \name Default Motor Rates: Down Check */
#define RATE_DEFAULT_DOWN_CHECK_A1		0x000003E8
#define RATE_DEFAULT_DOWN_CHECK_V1		0x00000000
#define RATE_DEFAULT_DOWN_CHECK_D1		0x000304D0
#define RATE_DEFAULT_DOWN_CHECK_AMAX	0x0000FFFF
#define RATE_DEFAULT_DOWN_CHECK_VMAX	200000
#define RATE_DEFAULT_DOWN_CHECK_DMAX	0x0000FFFF

/** \name Default Valve Configuration */
#define POS_DEFAULT_UA					210739
#define POS_DEFAULT_UT					185549
#define POS_DEFAULT_US					183501
#define POS_DEFAULT_DA					-116873
#define POS_DEFAULT_DT					-32768
#define POS_DEFAULT_DS					0
#define POS_DEFAULT_BYPASS_PARK			196537
#define POS_DEFAULT_DOWN_CHECK_PARK		0

/** \name Motor Aliases */
#define MOTOR_DOWN_CHECK		MOTOR_NUM_1
#define MOTOR_BYPASS			MOTOR_NUM_0

/** \name CAN Node IDs */
#define CAN_NODE_ID_MASTER		0x61
#define CAN_NODE_ID_SLAVE		0x62

/** \name System Port & Pin Definitions */
#define PIN_PRESS_ADC			GPIO_PIN_0
#define PORT_PRESS_ADC			GPIOB
#define PIN_TEMP_ADC	  		GPIO_PIN_5
#define PORT_TEMP_ADC	 	 	GPIOC
#define PIN_V_MONITOR			GPIO_PIN_13
#define PORT_V_MONITOR			GPIOC
#define PIN_SOL_EN				GPIO_PIN_9
#define PORT_SOL_EN				GPIOC
#define PIN_PUMP_EN				GPIO_PIN_2
#define PORT_PUMP_EN			GPIOA
#define PIN_DC_SENSOR			GPIO_PIN_1
#define PORT_DC_SENSOR			GPIOB
#define PIN_BP_SENSOR			GPIO_PIN_3
#define PORT_BP_SENSOR	  		GPIOA

/** \name CAN Port & Pin Definitions */
#define PIN_CAN_TX				GPIO_PIN_12
#define PIN_CAN_RX				GPIO_PIN_11
#define PORT_CAN				GPIOA
#define AF_CAN_TXRX				GPIO_AF9_CAN1

/** \name USART Port & Pin Definitions */
#define PIN_UART_TX				GPIO_PIN_12
#define PORT_UART_TX			GPIOC
#define PIN_UART_RX				GPIO_PIN_2
#define PORT_UART_RX			GPIOD
#define AF_UART_TXRX			GPIO_AF8_UART5

/** \name TMC5041 SPI Port & Pin Definitions */
#define PIN_TMC_MOSI			GPIO_PIN_7
#define PORT_TMC_MOSI			GPIOA
#define AF_TMC_MOSI				GPIO_AF5_SPI1
#define PIN_TMC_MISO			GPIO_PIN_6
#define PORT_TMC_MISO			GPIOA
#define AF_TMC_MISO				GPIO_AF5_SPI1
#define PIN_TMC_SCK				GPIO_PIN_5
#define PORT_TMC_SCK			GPIOA
#define AF_TMC_SCK				GPIO_AF5_SPI1
#define PIN_TMC_NSS				GPIO_PIN_4
#define PORT_TMC_NSS			GPIOA
#define AF_TMC_NSS				GPIO_AF5_SPI1

/** \name TMC5041 Auxiliary Port & Pin Definitions */
#define PIN_TMC_PP				GPIO_PIN_4
#define PORT_TMC_PP				GPIOC
#define PIN_TMC_INT				GPIO_PIN_5
#define PORT_TMC_INT			GPIOB
#define PIN_TMC_DRV_EN			GPIO_PIN_9
#define PORT_TMC_DRV_EN			GPIOA
#define PIN_TMC_MCO				GPIO_PIN_8
#define PORT_TMC_MCO			GPIOA

/** \name Encoder Port & Pin Definitions */
#define PIN_ENC_A				GPIO_PIN_0
#define PIN_ENC_B				GPIO_PIN_1
#define PORT_ENC_AB				GPIOA
#define AF_ENC_AB				GPIO_AF1_TIM2
#define PIN_ENC_Z				GPIO_PIN_11
#define PORT_ENC_Z				GPIOC

/** \name Debug Port & Pin Definitions */
#define PIN_TMS					GPIO_PIN_13
#define PORT_TMS				GPIOA
#define PIN_TCK					GPIO_PIN_14
#define PORT_TCK				GPIOA
#define PIN_SWO					GPIO_PIN_3
#define PORT_SWO				GPIOB

/** \name EEPROM I2C Port & Pin Definitions */
#define PIN_EEPROM_SDA			GPIO_PIN_7
#define PIN_EEPROM_SCL			GPIO_PIN_6
#define PORT_EEPROM				GPIOB

/** \name LED Port & Pin Definitions */
#define PIN_LED3				GPIO_PIN_0
#define PIN_LED4				GPIO_PIN_1
#define PIN_LED5				GPIO_PIN_2
#define PIN_LED6				GPIO_PIN_3
#define PORT_LEDS				GPIOC

#ifdef __cplusplus
}
#endif

#endif /* _SYSCONFIG_H_ */

/** @} */	// File-level tag

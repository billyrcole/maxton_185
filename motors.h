/*
 * motors.h
 *
 *  Created on: Mar 24, 2020
 *      Author: taylor
 */

#ifndef _MOTORS_H_
#define _MOTORS_H_

#include "TMC5041.h"

TMC5041TypeDef *_tmc5041;
ConfigurationTypeDef *_tmc_config;
int32_t register_state[TMC5041_REGISTER_COUNT];

typedef struct Ramps {
	uint32_t RateUT;
	uint32_t RateUA;
	uint32_t RateUS;
	uint32_t RateDT;
	uint32_t RateDA;
	uint32_t RateDS;
} Ramps_t;

/**
 * @defgroup MotorError		Motor Error
 * @{
 */
typedef enum MotorError {
	MOTOR_SUCCESS		= 0,
	MOTOR_FAILURE		= 1
} MotorError_t;
/** @} */

/**
 * @defgroup MotorNum	Motor Number
 * @{
 */
typedef enum MotorNum {
	MOTOR_NUM_0			= 0,
	MOTOR_NUM_1			= 1
} MotorNum_t;
/** @} */

/**
 * @defgroup MotorDirection	Motor Direction
 * @{
 */
typedef enum MotorDirection {
	MOTOR_DIRECTION_POS		= TMC5041_MODE_VELOCITY_POS,
	MOTOR_DIRECTION_NEG		= TMC5041_MODE_VELOCITY_NEG
} MotorDirection_t;
/** @} */

typedef enum MotorRate {
	MOTOR_RATE_A1,
	MOTOR_RATE_V1,
	MOTOR_RATE_AMAX,
	MOTOR_RATE_VMAX,
	MOTOR_RATE_DMAX,
	MOTOR_RATE_D1
} MotorRate_t;


MotorError_t motor_init(SPI_HandleTypeDef *spi, TMC5041TypeDef *tmc);
MotorError_t motor_setDefaults(void);
MotorError_t motor_moveToPosition(MotorNum_t motor, int32_t position);
MotorError_t motor_moveAtSpeed(MotorNum_t motor, int32_t speed);
MotorError_t motor_moveByAmount(MotorNum_t motor, int32_t amount);
MotorError_t motor_stop(MotorNum_t motor);
int32_t motor_readPosition(MotorNum_t motor);
MotorError_t motor_setPosition(MotorNum_t motor, int32_t position);
uint32_t motor_readVelocity(MotorNum_t motor);
MotorError_t motor_setVelocity(MotorNum_t motor, uint32_t value);
uint32_t motor_readRate(MotorNum_t motor, MotorRate_t rate);
MotorError_t motor_setRate(MotorNum_t motor, MotorRate_t rate, uint32_t value);
MotorError_t motor_sgHome(MotorNum_t motor);

#endif /* _MOTORS_H_ */

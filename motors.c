/**
 * @file		motors.c
 * @brief		API for motor driver access
 * @author		Taylor Street <tstreet@maxtonvalve.com>
 * @date		03/24/2020
 *
 * @{
 */

#include "sysconfig.h"
#include "motors.h"
#include "cmsis_os.h"
MotorError_t motor_init(SPI_HandleTypeDef *spi, TMC5041TypeDef *tmc)
{
	if(tmc == NULL){
		return MOTOR_FAILURE;
	}

	/* Attach the TMC5041 to the API */
	_tmc5041 = tmc;

	/* Initialize the TMC5041 driver */
	tmc5041_init(_tmc5041, spi, _tmc_config, register_state);

	/* Set the initial values of the motor settings */
	motor_setDefaults();

	return MOTOR_SUCCESS;
}

MotorError_t motor_setDefaults(void)
{
	/* Set motor 0 initial rates */
	_tmc5041->A1[0] = RATE_DEFAULT_BYPASS_A1;
	_tmc5041->V1[0] = RATE_DEFAULT_BYPASS_V1;
	_tmc5041->D1[0] = RATE_DEFAULT_BYPASS_D1;
	_tmc5041->AMAX[0] = RATE_DEFAULT_BYPASS_AMAX;
	_tmc5041->VMAX[0] = RATE_DEFAULT_BYPASS_VMAX;
	_tmc5041->DMAX[0] = RATE_DEFAULT_BYPASS_DMAX;

	/* Set motor 1 initial rates */
	_tmc5041->A1[1] = RATE_DEFAULT_DOWN_CHECK_A1;
	_tmc5041->V1[1] = RATE_DEFAULT_DOWN_CHECK_V1;
	_tmc5041->D1[1] = RATE_DEFAULT_DOWN_CHECK_D1;
	_tmc5041->AMAX[1] = RATE_DEFAULT_DOWN_CHECK_AMAX;
	_tmc5041->VMAX[1] = RATE_DEFAULT_DOWN_CHECK_VMAX;
	_tmc5041->DMAX[1] = RATE_DEFAULT_DOWN_CHECK_DMAX;

	/* Set generic configuration from 15.1 of the datasheet */
	tmc5041_writeInt(_tmc5041, TMC5041_GCONF, 0x00000008);				// GCONF=8: Enable PP and INT outputs

	/* Motor 0 initialization */
	tmc5041_writeInt(_tmc5041, TMC5041_CHOPCONF(0),		0x000100C5);	// CHOPCONF: TOFF=5, HSTRT=4, HEND=1, TBL=2, CHM=0 (spreadCycle)
	tmc5041_writeInt(_tmc5041, TMC5041_IHOLD_IRUN(0),	0x00010F04);	// IHOLD_IRUN: IHOLD=5, IRUN=15 (1/2 current), IHOLDDELAY=1
	tmc5041_writeInt(_tmc5041, TMC5041_TZEROWAIT(0),	0x00002710);	// TZEROWAIT=10000
	tmc5041_writeInt(_tmc5041, TMC5041_PWMCONF(0),		0x000400C8);	// PWM_CONF: AUTO=1, 2/1024 Fclk, Switch amplitude limit=200, Grad=1, stealthChop disabled
	tmc5041_writeInt(_tmc5041, TMC5041_VHIGH(0),		0x00061A80);	// VHIGH=400 000: Set VHIGH to a high value to allow stealthChop
	tmc5041_writeInt(_tmc5041, TMC5041_VCOOLTHRS(0),	0x00007530);	// VCOOLTHRS=30000: Set upper limit for stealthChop to about 30RPM

	tmc5041_writeInt(_tmc5041, TMC5041_A1(0),    _tmc5041->A1[0]);
	tmc5041_writeInt(_tmc5041, TMC5041_V1(0),    _tmc5041->V1[0]);
	tmc5041_writeInt(_tmc5041, TMC5041_D1(0),    _tmc5041->D1[0]);
	tmc5041_writeInt(_tmc5041, TMC5041_AMAX(0),  _tmc5041->AMAX[0]);
	tmc5041_writeInt(_tmc5041, TMC5041_VMAX(0),  _tmc5041->VMAX[0]);
	tmc5041_writeInt(_tmc5041, TMC5041_DMAX(0),  _tmc5041->DMAX[0]);
	tmc5041_writeInt(_tmc5041, TMC5041_VSTOP(0), 0x00000010);			// Stopping velocity close to zero
	tmc5041_writeInt(_tmc5041, TMC5041_COOLCONF(0), 0x00020000); 		//SET SGT TO 2 UNDER COOLCONF_M1

	/* Motor 1 initialization */
	tmc5041_writeInt(_tmc5041, TMC5041_CHOPCONF(1),		0x000100C5);	// CHOPCONF: TOFF=5, HSTRT=4, HEND=1, TBL=2, CHM=0 (spreadCycle)
	tmc5041_writeInt(_tmc5041, TMC5041_IHOLD_IRUN(1),	0x00010F04);	// IHOLD_IRUN: IHOLD=5, IRUN=15 (1/2 current), IHOLDDELAY=1
	tmc5041_writeInt(_tmc5041, TMC5041_TZEROWAIT(1),	0x00002710);	// TZEROWAIT=10000
	tmc5041_writeInt(_tmc5041, TMC5041_PWMCONF(1),		0x000400C8);	// PWM_CONF: AUTO=1, 2/1024 Fclk, Switch amplitude limit=200, Grad=1
	tmc5041_writeInt(_tmc5041, TMC5041_VHIGH(1),		0x00061A80);	// VHIGH=400 000: Set VHIGH to a high value to allow stealthChop
	tmc5041_writeInt(_tmc5041, TMC5041_VCOOLTHRS(1),	0x00007530);	// VCOOLTHRS=30000: Set upper limit for stealthChop to about 30RPM

	tmc5041_writeInt(_tmc5041, TMC5041_A1(1),    _tmc5041->A1[1]);
	tmc5041_writeInt(_tmc5041, TMC5041_V1(1),    _tmc5041->V1[1]);
	tmc5041_writeInt(_tmc5041, TMC5041_D1(1),    _tmc5041->D1[1]);
	tmc5041_writeInt(_tmc5041, TMC5041_AMAX(1),  _tmc5041->AMAX[1]);
	tmc5041_writeInt(_tmc5041, TMC5041_VMAX(1),  _tmc5041->VMAX[1]);
	tmc5041_writeInt(_tmc5041, TMC5041_DMAX(1),  _tmc5041->DMAX[1]);
	tmc5041_writeInt(_tmc5041, TMC5041_VSTOP(1), 0x00000010);			// Stopping velocity close to zero
	tmc5041_writeInt(_tmc5041, TMC5041_COOLCONF(1), 0x00020000); 		//SET SGT TO 2 UNDER COOLCONF_M1

	/* Enable stopping with stallGuard2 */
//	tmc5041_writeInt(_tmc5041, TMC5041_SWMODE(0), 0x00000400);
//	tmc5041_writeInt(_tmc5041, TMC5041_SWMODE(1), 0x00000400);

	/* Disable stopping with stallGuard2 */
//	tmc5041_writeInt(_tmc5041, TMC5041_SWMODE(0), 0x00000000);
//	tmc5041_writeInt(_tmc5041, TMC5041_SWMODE(1), 0x00000000);

	/* Make sure the motors are in positioning mode and
	 * hard stop the motors. The TMC5041 is not reset during
	 * a power cycle, so previous movement commands can be
	 * re-activated.
	 */
	tmc5041_writeInt(_tmc5041, MOTOR_NUM_0, TMC5041_MODE_POSITION);
	tmc5041_writeInt(_tmc5041, MOTOR_NUM_1, TMC5041_MODE_POSITION);
	motor_stop(MOTOR_BYPASS);
	motor_stop(MOTOR_DOWN_CHECK);

	return MOTOR_SUCCESS;
}

/**
  * @brief  Moves the motor to the specified position in the specified direction.
  *
  * @param  motor	target motor of type @ref Motor_Num
  * @param  position	target position of the motor (Min: 0, Max: 0xFFFFFFFF)
  * @param	direction	rotation direction of type @ref Motor_Direction
  *
  * @note   None
  *
  * @retval	@ref MotorError status
  */
MotorError_t motor_moveToPosition(MotorNum_t motor, int32_t position)
{
	/* Change to Positioning Mode */
	tmc5041_writeInt(_tmc5041, TMC5041_RAMPMODE(motor), TMC5041_MODE_POSITION);

	/* Set the velocity */
	tmc5041_writeInt(_tmc5041, TMC5041_VMAX(motor), _tmc5041->VMAX[motor]);

	/* Move to position */
	tmc5041_writeInt(_tmc5041, TMC5041_XTARGET(motor), position);

	/* Update tracking variables */
	_tmc5041->Mode = TMC5041_MODE_POSITION;
	_tmc5041->Position[motor] = position;

	return MOTOR_SUCCESS;
}

MotorError_t motor_moveAtSpeed(MotorNum_t motor, int32_t speed)
{
	if(motor > (NUM_OF_MOTORS - 1)){
		return MOTOR_FAILURE;
	}
	/* Set the velocity */
	if((speed > 8388096L) || (speed < -8388096L)){
		return MOTOR_FAILURE;
	}

	/* Change to Velocity Mode */
	if(speed > 0){
		tmc5041_writeInt(_tmc5041, TMC5041_VMAX(motor), speed);
		tmc5041_writeInt(_tmc5041, TMC5041_RAMPMODE(motor), TMC5041_MODE_VELOCITY_POS);
		_tmc5041->Mode = TMC5041_MODE_VELOCITY_POS;
	}
	else{
		tmc5041_writeInt(_tmc5041, TMC5041_VMAX(motor), (-1)*speed);
		tmc5041_writeInt(_tmc5041, TMC5041_RAMPMODE(motor), TMC5041_MODE_VELOCITY_NEG);
		_tmc5041->Mode = TMC5041_MODE_VELOCITY_NEG;
	}

	return MOTOR_SUCCESS;
}

MotorError_t motor_moveByAmount(MotorNum_t motor, int32_t amount)
{
	int32_t new_pos = motor_readPosition(motor);

	new_pos += amount;

	motor_moveToPosition(motor, new_pos);

	return MOTOR_SUCCESS;
}

MotorError_t motor_stop(MotorNum_t motor)
{
	if(_tmc5041->Mode == TMC5041_MODE_POSITION){
		tmc5041_writeInt(_tmc5041, TMC5041_VSTART(motor), 0);
		tmc5041_writeInt(_tmc5041, TMC5041_VMAX(motor), 0);
	}
	else if( (_tmc5041->Mode == TMC5041_MODE_VELOCITY_POS) || (_tmc5041->Mode == TMC5041_MODE_VELOCITY_NEG) ){
		tmc5041_writeInt(_tmc5041, TMC5041_VMAX(motor), 0);
		tmc5041_writeInt(_tmc5041, TMC5041_AMAX(motor), 0xFFFF);
	}
	else{
		return MOTOR_FAILURE;
	}

	// TODO: AMAX and VSTART need to be reset to initial/usable values

	/* Update the motor position */
	_tmc5041->Position[motor] = motor_readPosition(motor);

	return MOTOR_SUCCESS;
}

int32_t motor_readPosition(MotorNum_t motor)
{
	return (int32_t)tmc5041_readInt(_tmc5041, TMC5041_XACTUAL(motor));
}

MotorError_t motor_setPosition(MotorNum_t motor, int32_t position)
{
	tmc5041_writeInt(_tmc5041, TMC5041_XACTUAL(motor), position);

	_tmc5041->Position[motor] = position;

	return MOTOR_SUCCESS;
}

uint32_t motor_readVelocity(MotorNum_t motor)
{
	return (uint32_t)tmc5041_readInt(_tmc5041, TMC5041_VACTUAL(motor));
}

MotorError_t motor_setVelocity(MotorNum_t motor, uint32_t velocity)
{
	tmc5041_writeInt(_tmc5041, TMC5041_VMAX(motor), velocity);

	_tmc5041->VMAX[motor] = velocity;

	return MOTOR_SUCCESS;
}

uint32_t motor_readRate(MotorNum_t motor, MotorRate_t rate)
{
	switch(rate){
		case MOTOR_RATE_A1:
			return (uint32_t)tmc5041_readInt(_tmc5041, TMC5041_A1(motor));
			break;
		case MOTOR_RATE_V1:
			return (uint32_t)tmc5041_readInt(_tmc5041, TMC5041_V1(motor));
			break;
		case MOTOR_RATE_AMAX:
			return (uint32_t)tmc5041_readInt(_tmc5041, TMC5041_AMAX(motor));
			break;
		case MOTOR_RATE_VMAX:
			return (uint32_t)tmc5041_readInt(_tmc5041, TMC5041_VMAX(motor));
			break;
		case MOTOR_RATE_DMAX:
			return (uint32_t)tmc5041_readInt(_tmc5041, TMC5041_DMAX(motor));
			break;
		case MOTOR_RATE_D1:
			return (uint32_t)tmc5041_readInt(_tmc5041, TMC5041_D1(motor));
			break;
	}

	return 0;
}

MotorError_t motor_setRate(MotorNum_t motor, MotorRate_t rate, uint32_t value)
{
	switch(rate){
		case MOTOR_RATE_A1:
			_tmc5041->A1[motor] = value;
			tmc5041_writeInt(_tmc5041, TMC5041_A1(motor), value);
			break;
		case MOTOR_RATE_V1:
			_tmc5041->V1[motor] = value;
			tmc5041_writeInt(_tmc5041, TMC5041_V1(motor), value);
			break;
		case MOTOR_RATE_AMAX:
			_tmc5041->AMAX[motor] = value;
			tmc5041_writeInt(_tmc5041, TMC5041_AMAX(motor), value);
			break;
		case MOTOR_RATE_VMAX:
			_tmc5041->VMAX[motor] = value;
			tmc5041_writeInt(_tmc5041, TMC5041_VMAX(motor), value);
			break;
		case MOTOR_RATE_DMAX:
			_tmc5041->DMAX[motor] = value;
			tmc5041_writeInt(_tmc5041, TMC5041_DMAX(motor), value);
			break;
		case MOTOR_RATE_D1:
			_tmc5041->D1[motor] = value;
			tmc5041_writeInt(_tmc5041, TMC5041_D1(motor), value);	break;
		default:
			return MOTOR_FAILURE;
	}

	return MOTOR_SUCCESS;
}

MotorError_t motor_sgHome(MotorNum_t motor)
{
	uint8_t motor_stopFlag = 0;
	uint8_t errorFlag = 0;

	/* Disable the sensor interrupts to prevent interfering with homing */
	if(motor == MOTOR_DOWN_CHECK){
		HAL_NVIC_DisableIRQ(EXTI1_IRQn);
	}
	else if(motor == MOTOR_BYPASS){
		HAL_NVIC_DisableIRQ(EXTI3_IRQn);
	}

	// Clear the RAMPSTAT register for the specified motor
	tmc5041_readInt(_tmc5041, TMC5041_RAMPSTAT(motor));

	// Run motor toward base and check for it to stall
	motor_moveAtSpeed(motor, -100000);

	osDelay(pdMS_TO_TICKS(100));
	tmc5041_writeInt(_tmc5041, TMC5041_SWMODE(motor), 0x00000400);

	while(!motor_stopFlag){
		int32_t drv_status = tmc5041_readInt(_tmc5041, TMC5041_DRVSTATUS(motor));

		if((drv_status & TMC5041_STALLGUARD_MASK)){
			motor_stopFlag = 1;
		}
		else{
			vTaskDelay(pdMS_TO_TICKS(25));
		}

		errorFlag++;

		if(errorFlag == 250){
			motor_stop(motor);
			return MOTOR_FAILURE;
		}
		//if both are high set motor_stopFlag to 1
	}

	motor_stop(motor);
	tmc5041_writeInt(_tmc5041, TMC5041_SWMODE(motor), 0x00000000);	//turn off stall guard

	tmc5041_readInt(_tmc5041, TMC5041_RAMPSTAT(motor));	//read and clear ramp_stat

	/* Set the the stall point (i.e. motor base) as the 0 location */
	motor_setPosition(motor, 0);

	/* Re-enable the interrupts */
	if(motor == MOTOR_DOWN_CHECK){
		HAL_NVIC_EnableIRQ(EXTI1_IRQn);
		HAL_NVIC_ClearPendingIRQ(EXTI1_IRQn);
	}
	else if(motor == MOTOR_BYPASS){
		HAL_NVIC_EnableIRQ(EXTI3_IRQn);
		HAL_NVIC_ClearPendingIRQ(EXTI3_IRQn);
	}

	/* Move the motor away from base and let the interrupt stop it */
	motor_moveAtSpeed(motor, 100000);

	return MOTOR_SUCCESS;
}

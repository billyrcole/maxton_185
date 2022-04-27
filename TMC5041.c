/*
 * TMC5041.c
 *
 *  Created on: 07.07.2017
 *      Author: LK
 *    Based on: TMC562-MKL.h (26.01.2012 OK)
 */

#include "TMC5041.h"
#include "main.h"

void tmc5041_readWriteArray(TMC5041TypeDef *tmc5041, uint8_t *data)
{
	SPI_HandleTypeDef *p_spi;
	uint8_t buffer[5];

	p_spi = tmc5041->spi;

	while(HAL_SPI_GetState(p_spi) != HAL_SPI_STATE_READY);

	__HAL_SPI_ENABLE(p_spi);


	 HAL_GPIO_WritePin(GPIOC, MOTORCS_Pin, GPIO_PIN_RESET);

	if( HAL_SPI_TransmitReceive(p_spi,  data, buffer, 5, HAL_MAX_DELAY) != HAL_OK )

	    { }

	 HAL_GPIO_WritePin(GPIOC, MOTORCS_Pin, GPIO_PIN_SET);

	__HAL_SPI_DISABLE(p_spi);

 	for(int i = 0; i < 5; i++){
		*(data + i) = buffer[i];
	}
}

void tmc5041_writeDatagram(TMC5041TypeDef *tmc5041, uint8_t address, uint8_t x1, uint8_t x2, uint8_t x3, uint8_t x4)
{
	uint8_t data[5] = {address | TMC5041_WRITE_BIT, x1, x2, x3, x4 };
	tmc5041_readWriteArray(tmc5041, &data[0]);

	int32_t value = (x1 << 24) | (x2 << 16) | (x3 << 8) | x4;

	// Write to the shadow register and mark the register dirty
	address = TMC_ADDRESS(address);
	tmc5041->config->shadowRegister[address] = value;
	tmc5041->registerAccess[address] |= TMC_ACCESS_DIRTY;
}

void tmc5041_writeInt(TMC5041TypeDef *tmc5041, uint8_t address, int32_t value)
{
	tmc5041_writeDatagram(tmc5041, address, BYTE(value, 3), BYTE(value, 2), BYTE(value, 1), BYTE(value, 0));
}

int32_t tmc5041_readInt(TMC5041TypeDef *tmc5041, uint8_t address)
{
	address = TMC_ADDRESS(address);

	// register not readable -> shadow register copy
	if(!TMC_IS_READABLE(tmc5041->registerAccess[address]))
		return tmc5041->config->shadowRegister[address];

	uint8_t data[5] = { 0, 0, 0, 0, 0 };

	// Write the addresses
	data[0] = address;
	tmc5041_readWriteArray(tmc5041, &data[0]);

	// Read the data that is shifted through
	data[0] = address;
	tmc5041_readWriteArray(tmc5041, &data[0]);

	return (data[1] << 24) | (data[2] << 16) | (data[3] << 8) | data[4];
}

void tmc5041_init(TMC5041TypeDef *tmc5041, SPI_HandleTypeDef *spi, ConfigurationTypeDef *config, const int32_t *registerResetState)
{
	tmc5041->spi = spi;

	tmc5041->VMAX[0]			= 0;
	tmc5041->VMAX[1]			= 0;
	tmc5041->Position[0]		= 0;
	tmc5041->Position[1]		= 0;

	tmc5041->config               = config;
	tmc5041->config->channel      = 0;
	tmc5041->config->state        = CONFIG_READY;

	for(int i = 0; i < TMC5041_REGISTER_COUNT; i++)
	{
		tmc5041->registerAccess[i]      = tmc5041_defaultRegisterAccess[i];
		tmc5041->registerResetState[i]  = registerResetState[i];
	}
}

/*
 * flashWrite.h
 *
 *  Created on: Mar 30, 2022
 *      Author: BillyC
 */

#ifndef INC_FLASHWRITE_H_
#define INC_FLASHWRITE_H_

#define FLASH_SECTOR1_BASE_ADDRESS	0x08004000
#define FLASH_SECTOR2_BASE_ADDRESS	0x08008000
#define FLASH_SECTOR3_BASE_ADDRESS	0x0800C000
#define FLASH_SECTOR4_BASE_ADDRESS	0x08010000
#define FLASH_SECTOR5_BASE_ADDRESS	0x08020000
#define FLASH_SECTOR6_BASE_ADDRESS	0x08040000
#define FLASH_SECTOR7_BASE_ADDRESS	0x08060000

uint32_t InteralFlash_Write_Data (uint32_t StartSectorAddress, uint32_t * Data, uint16_t numberofwords);
void Flash_Read_Data (uint32_t StartPageAddress, uint32_t *RxBuf, uint16_t numberofwords);

#endif /* INC_FLASHWRITE_H_ */

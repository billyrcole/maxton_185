/*
 * flashWrite.c
 *
 *  Created on: Oct 14, 2021
 *      Author: BillyC
 */

#include "main.h"
#include "flashWrite.h"



static uint32_t GetSector(uint32_t Address);

uint32_t InteralFlash_Write_Data (uint32_t StartSectorAddress, uint32_t  *Data, uint16_t numberofbytes)
{

	static FLASH_EraseInitTypeDef EraseInitStruct;

	int sofar=0;

	uint32_t StartSector;
	uint32_t EndSector;

	uint32_t EndSectorAdress;

	uint32_t SectorsTobeErase ,  SectorError;

/* Unlock the Flash to enable the flash control register access
 * After Reset, the Flash memory Program/Erase Controller is locked *************/
	HAL_FLASH_Unlock();

	/* Before programming the desired addresses, an erase operation is performed using
       the flash erase sector feature
       Erase the user Flash area*/
	 StartSector = GetSector(StartSectorAddress);
	 EndSectorAdress = StartSectorAddress + numberofbytes;
	 EndSector = GetSector(EndSectorAdress);

	 /* Fill EraseInit structure
	    The erase procedure is done by filling the EraseInit structure.
         Fill in starting erase sector and the number of sectors to be erased.
        At this stage, all these sectors will be erased one by one separately.
     */
	 EraseInitStruct.TypeErase = FLASH_TYPEERASE_SECTORS;
	 EraseInitStruct.Sector = StartSector;


	 SectorsTobeErase = (EndSector - StartSector) + 1;
	 EraseInitStruct.NbSectors = SectorsTobeErase;
	 EraseInitStruct.VoltageRange = FLASH_VOLTAGE_RANGE_3;

	 /*if problem occurs on a sector, erase will be stopped and faulty sector will
       be returned to user (through variable 'SectorError').*/

	 if(HAL_FLASHEx_Erase(&EraseInitStruct, &SectorError) != HAL_OK)
	 {
	     //Erase error!
		 return HAL_FLASH_GetError ();
	 }

/*Once the erase operation is finished, sector word programming operation will be performed
in the Flash memory. The written data is then read back and checked.*/

	   while (sofar<numberofbytes)
	   {
	     if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, StartSectorAddress, Data[sofar]) == HAL_OK)
	     {
	    	 StartSectorAddress += 4;  // use StartPageAddress += 2 for half word and 8 for double word
	    	 sofar++;
	     }
	     else
	     {
	       /* Error occurred while writing data in Flash memory*/
	    	 return HAL_FLASH_GetError ();
	     }
	   }

	   /* Lock the Flash to disable the flash control register access (recommended
	      to protect the FLASH memory against possible unwanted operation) *********/
	   HAL_FLASH_Lock();
 /* Program the user Flash area word by word*/


	return 0;
}// end Flash_Write_Data




static uint32_t GetSector(uint32_t Address)
{
  uint32_t sector = 0;

  /* BANK 1 */
  if((Address >= 0x08000000) && (Address < 0x8004000))     // 16K BYTES
  {
    sector = FLASH_SECTOR_0;
  }


  else if((Address >= 0x8004000) && (Address < 0x08007FFF))  // 16K BYTES
    {
      sector = FLASH_SECTOR_1;
    }

  else if((Address >= 0x08008000) && (Address < 0x0800BFFF))   // 16K BYTES
    {
      sector = FLASH_SECTOR_2;
    }

  else if((Address >= 0x0800C000) && (Address < 0x0800FFFF))   // 16K BYTES
    {
      sector = FLASH_SECTOR_3;
    }

  else if((Address >= 0x08010000) && (Address < 0x0801FFFF))    // 64K BYTES
    {
      sector = FLASH_SECTOR_4;
    }
  else if((Address >= 0x08020000) && (Address < 0x0803FFFF))     // 128K BYTES
    {
      sector = FLASH_SECTOR_5;
    }

  else if((Address >= 0x08040000) && (Address < 0x0805FFFF))     // 128K BYTES
    {
      sector = FLASH_SECTOR_6;
    }
  else if((Address >= 0x08060000) && (Address < 0x0807FFFF))      // 128K BYTES
    {
      sector = FLASH_SECTOR_7;
    }
  return sector;
}


void Flash_Read_Data (uint32_t StartPageAddress, uint32_t *RxBuf, uint16_t numberofwords)
{

	while (1)
	{

		*RxBuf = *(__IO uint32_t *)StartPageAddress;
		StartPageAddress += 4;
		RxBuf++;
		if (!(numberofwords--)) break;
	}
}


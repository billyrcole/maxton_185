/*
 * ble_task.c
 *
 *  Created on: Feb 18, 2022
 *      Author: BillyC
 */
#include <stdio.h>
#include "app_ble_hello.h"
#include "cmsis_os.h"

void bleTaskentry(void *argument)
{
  /* USER CODE BEGIN bleTaskentry */
	  //Enable BLE module
	  blue_init();// <---------------------------------------------------------Enable BLE module---------------------------------------

  /* Infinite loop */
  for(;;)
  {
	  // Process BLE module
	  bluenrg_process();  // <---------------------------------------------Process BLE module---------------------------------------
  	  osDelay(1);
  }

 }

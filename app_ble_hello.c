/*
 * app_ble_hello.c
 *
 *  Created on: Jan 7, 2022
 *      Author: BillyC
 */
#include "main.h"
#include "cmsis_os.h"
#include "bluenrg1_types.h"
#include "hci.h"
#include "bluenrg_conf.h"
#include "bluenrg1_hci_le.h"
#include "bluenrg1_aci.h"
#include "bluenrg1_gap.h"
#include <string.h>
#include "app_ble_hello.h"
#include "service.h"
#include "printmsg.h"
#include "ble_service_tempretureSensor.h"
#include "ble_service_motorcmd.h"
#include <stdio.h>
// device  address
#define BDADDR_SIZE 6
uint8_t SERVER_BDADDR[] = {0x01,0x02,0x03,0x04,0x08,0x06};

void blue_init(void)
{
	const char *name = "hello_man";
	tBleStatus ret;

    uint8_t  bdaddr[BDADDR_SIZE];


    uint16_t service_handle;

    // device name CHARACTERISTICS
    uint16_t dev_name_char_handle;
    uint16_t appearance_char_handle;

    BLUENRG_memcpy(bdaddr,SERVER_BDADDR,sizeof(SERVER_BDADDR));

    /* Initialize HCI */
      hci_init(APP_UserEvtRx,NULL);

    	/* RESET HCI */
       ret =  hci_reset();
      if(ret != BLE_STATUS_SUCCESS)
       {
    	  printmsg("hci_reset fail... \n\r");
       }
      HAL_Delay(100);


    	/*CONFIGURE DEVICE ADDRESS */
      ret = aci_hal_write_config_data(CONFIG_DATA_PUBADDR_OFFSET, CONFIG_DATA_PUBADDR_LEN,bdaddr);
        if(ret != BLE_STATUS_SUCCESS)
        {
        	printmsg("aci_hal_write_config_data fail... \n\r");
        }
   	/* INIT GATT SEVER */
        ret =  aci_gatt_init();
       if(ret != BLE_STATUS_SUCCESS)
       {
    	   printmsg("aci_gatt_init fail... \n\r");
       }


    	/* INIT GAP SERVICE */
       ret=aci_gap_init(GAP_PERIPHERAL_ROLE,0,0x07,&service_handle,&dev_name_char_handle,&appearance_char_handle);
       if(ret != BLE_STATUS_SUCCESS)
             {
    	          printmsg("aci_gap_init fail... \n\r");
             }

    	/* UPDATE device name CHARACTERISTICS */
       ret = aci_gatt_update_char_value(service_handle,dev_name_char_handle,0,strlen(name),(uint8_t *)name );
        if(ret != BLE_STATUS_SUCCESS)
                     {
            	          printmsg("aci_gatt_update_char_value fail... \n\r");
                     }

    	/* ADD CUSTOM SERVICE
        ret= add_temp_services();

        if(ret != BLE_STATUS_SUCCESS)
              {
                 printmsg("add_simple_service add_temp_services fail... \n\r");
              }

*/

        ret= add_motor_services();

        if(ret != BLE_STATUS_SUCCESS)
              {
                 printmsg("add_simple_service add_motor_services fail... \n\r");
              }


}

uint8_t btn_state;

void bluenrg_process(void)
{
	tBleStatus ret;
	uint8_t local_name[] = {AD_TYPE_COMPLETE_LOCAL_NAME,'S','E','T','U','P','-','M','D'};

	/* SET DEVICE IN GENERAL DISCOVERABLE MODE */
	ret= aci_gap_set_discoverable(ADV_IND, 0, 0, PUBLIC_ADDR, NO_WHITE_LIST_USE, sizeof(local_name), local_name, 0, NULL, 0, 0);
	/* if(ret != BLE_STATUS_SUCCESS)
	             {
	           	printmsg("elavator in setup mode \n\r");
		            HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
		            //  HAL_Delay(500);
		              osDelay(1000);

	             }
*/
      /* PROCESS USER EVENT */
	  hci_user_evt_proc();


}







	/*
	 * 	  if(HAL_GPIO_ReadPin(GPIOC, GPIO_PIN_13) == GPIO_PIN_RESET)

	   {
	 	  printmsg("BL_DEBUG_MSG:Button is pressed .. going to BL mode\n\r");
	 	  bootloader_uart_read_data();

	   }
	   else
	   {
	 	  printmsg("BL_DEBUG_MSG:Button is not pressed .. will execute user app\n\r");
	 	  	  bootloader_jump_to_user_app();
	   }
	 *
	 *
	 *
	 */



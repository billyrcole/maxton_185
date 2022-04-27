/*
 * ble_service_pressureSensor.c
 *
 *  Created on: Mar 9, 2022
 *      Author: BillyC
 */

#include "main.h"
#include "bluenrg1_gap.h"
#include "bluenrg1_gatt_aci.h"
#include <stdlib.h>

//#include "bt_command.h"
#include "printmsg.h"

void receive_data(uint8_t * data_buffer, uint8_t num_bytes);


////////////////////////////////////////////////////////////////////////////  PRESSURE_SERVICE   ////////////////////////////////////////////////////////////////////////////

uint8_t PRESSURE_SERVICE_UUID[16] = {0x76,0xaa,0x1c,0x30,
		                           0x01,0x09,0x97,0x9f,
		                           0xe2,0x11,0x9e,0xb1,
						           0xe0,0xf2,0x83,0xe9};


uint8_t PRESSURE_TX_CHAR_UUID[16]  = {0x66,0x9a,0x0c,0x20,
		                          0x00,0x08,0x96,0x9e,
		                          0xe2,0x11,0x9e,0xb1,
						          0xe1,0xf2,0xa3,0xc9};



////////////////////////////////////////////////////////////////////   handles   ////////////////////////////////////////////////////////////////////////////////////

uint16_t pressure_service_handle;
uint16_t tx_pressure_char_handle;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/*
static uint8_t pressure_connected = 0;
static uint8_t pressure_set_connectable = 1;
static uint8_t pressure_notification_enabled = 0;
static uint16_t pressure_connection_handle = 0;
*/
/**/

tBleStatus add_pressure_services (void)
{
	tBleStatus ret;

    Service_UUID_t pressure_service_uuid;
    Char_UUID_t tx_pressure_char_uuid;


    BLUENRG_memcpy(pressure_service_uuid.Service_UUID_128,PRESSURE_SERVICE_UUID,sizeof(PRESSURE_SERVICE_UUID));
    BLUENRG_memcpy(tx_pressure_char_uuid.Char_UUID_128,PRESSURE_TX_CHAR_UUID,sizeof(PRESSURE_TX_CHAR_UUID));

    /////////////////////////////////////////////////////////////////ADD CHAT SERVICE//////////////////////////////////////////////////////////
               /* ADD CHAT SERVICE */

               ret = aci_gatt_add_service(UUID_TYPE_128,
            		                      &pressure_service_uuid,
    									  PRIMARY_SERVICE,
    									  7,
    									  &pressure_service_handle);


                if(ret != BLE_STATUS_SUCCESS)
                  {
                  	printmsg("aci_gatt_add_service fail... \n\r");
                  }


                /* ADD TX CHARACTERISTICS */

                ret = aci_gatt_add_char(pressure_service_handle,
                		                UUID_TYPE_128,
    									&tx_pressure_char_uuid,
    									20,
    									CHAR_PROP_NOTIFY,
    									ATTR_PERMISSION_NONE,
              		                    0,
    								    0,
    									1,
    									&tx_pressure_char_handle);

                  if(ret != BLE_STATUS_SUCCESS)
                    {
                    	printmsg("aci_gatt_add_char fail... \n\r");
                    }

          return (ret);

}// end add_services





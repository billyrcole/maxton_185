/*
 * ble_service_tempretureSensor.c
 *
 *  Created on: Mar 9, 2022
 *      Author: BillyC
 */

#include "main.h"
#include "bluenrg1_gap.h"
#include "bluenrg1_gatt_aci.h"
#include <stdlib.h>
#include "cmsis_os.h"

//#include "bt_command.h"
#include "printmsg.h"



extern osMessageQueueId_t QueTasktempSensorHandle;
extern osMessageQueueId_t QuepressureSenHandle;
extern ADC_HandleTypeDef hadc1;
extern ADC_HandleTypeDef hadc2;
extern uint16_t pressureReading;

void receive_data(uint8_t * data_buffer, uint8_t num_bytes);


////////////////////////////////////////////////////////////////////////////  TEMPREURE_SERVICE   ////////////////////////////////////////////////////////////////////////////

uint8_t TEMPREURE_SERVICE_UUID[16] = {0x76,0xaa,0x1c,0x30,
		                           0x01,0x09,0x97,0x9f,
		                           0xe2,0x11,0x9e,0xb1,
						           0xe0,0xf2,0x73,0xd9};


uint8_t TEMP_TX_CHAR_UUID[16]      = {0x66,0x9a,0x0c,0x20,
		                          0x00,0x08,0x96,0x9e,
		                          0xe2,0x11,0x9e,0xb1,
						          0xe1,0xf2,0x73,0xd9};



////////////////////////////////////////////////////////////////////   handles   ////////////////////////////////////////////////////////////////////////////////////

uint16_t tempreure_service_handle;
uint16_t tempreure_char_handle;


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


uint8_t temp_connected = 0;
uint8_t temp_set_connectable = 1;
uint8_t temp_notification_enabled = 0;
uint16_t temp_connection_handle = 0;



tBleStatus add_temp_services (void)
{
	tBleStatus ret;

    Service_UUID_t tempreure_service_uuid;
    Char_UUID_t tx_char_uuid;


    BLUENRG_memcpy(tempreure_service_uuid.Service_UUID_128,TEMPREURE_SERVICE_UUID,sizeof(TEMPREURE_SERVICE_UUID));
    BLUENRG_memcpy(tx_char_uuid.Char_UUID_128,TEMP_TX_CHAR_UUID,sizeof(TEMP_TX_CHAR_UUID));

    /////////////////////////////////////////////////////////////////ADD CHAT SERVICE//////////////////////////////////////////////////////////
               /* ADD CHAT SERVICE */

               ret = aci_gatt_add_service(UUID_TYPE_128,
            		                      &tempreure_service_uuid,
    									  PRIMARY_SERVICE,
    									  7,
    									  &tempreure_service_handle);


                if(ret != BLE_STATUS_SUCCESS)
                  {
                  	printmsg("aci_gatt_add_service fail... \n\r");
                  }


                /* ADD TX CHARACTERISTICS */

                ret = aci_gatt_add_char(tempreure_service_handle,
                		                UUID_TYPE_128,
    									&tx_char_uuid,
    									2,
    									CHAR_PROP_READ,
    									ATTR_PERMISSION_NONE,
              		                    GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,
    								    0,
    									1,
    									&tempreure_char_handle);

                  if(ret != BLE_STATUS_SUCCESS)
                    {
                    	printmsg("aci_gatt_add_char fail... \n\r");
                    }

          return (ret);

}// end add_services








void update_tempertureSenor_data(int16_t new_data)
	{
	     tBleStatus ret;

	     ret = aci_gatt_update_char_value(tempreure_service_handle,tempreure_char_handle,0,2,(uint8_t *)&new_data);

	     if(ret != BLE_STATUS_SUCCESS)
	          {
	          	printmsg("aci_gatt_update_char_value tempreure_char fail... \n\r");
	          }

	}



/*
void GAP_tempSenConnectionComplete_CB(uint8_t addr[6],uint16_t handle)
{
	temp_connected = 1;
	temp_connection_handle = handle;
	 printmsg("tempre Connection Complete... \n\r");
}



void GAP_tempSenDisconnectComplete_CB(uint8_t addr[6],uint16_t handle)
{

	 printmsg(" tempre DisonnectionComplete_CB Complete... \n\r");
}
*/

void Read_tempSenRequest_CB(uint16_t handle)
{
	 int16_t  Temp;
	osMessageQueueGet(QueTasktempSensorHandle, &Temp, 0U,0U);
	update_tempertureSenor_data(Temp);

	if(temp_connection_handle != 0)
		aci_gatt_allow_read(temp_connection_handle);


}


void aci_gatt_read_permit_req_event(uint16_t Connection_Handle,
		                            uint16_t Attribute_Handle,
									uint16_t Offset)
{
	Read_tempSenRequest_CB(Attribute_Handle);
}


void hci_le_connection_complete_event( uint8_t Status,
		                               uint16_t Connection_Handle,
									   uint8_t Role,
									   uint8_t Peer_Address_Type,
									   uint8_t Peer_Address[6],
									   uint16_t Conn_Interval,
									   uint16_t Conn_Latency,
									   uint16_t Supervision_Timeout,
									   uint8_t Master_Clock_Accuracy)
{

	temp_connected = 1;
	temp_connection_handle = Connection_Handle;
	  printmsg("Disonnected... \n\r");

}

void hci_disconnection_complte_event(uint8_t Status, uint16_t Connection_Handle,  uint8_t Reason )
{
	temp_connected = 1;
	temp_set_connectable = 1;
  Connection_Handle = 0;
  printmsg("Disonnected... \n\r");
}


void APP_UserEvtRx(void * pData)

{
	uint32_t i;
	hci_spi_pckt *hci_pckt = (hci_spi_pckt *)pData;


  /* process event packet */
	if(hci_pckt->type == HCI_EVENT_PKT)
	{
       /* GET DATA FROM PACKET */
		hci_event_pckt *event_pckt = (hci_event_pckt *) hci_pckt->data;

		/* process meta data event */
		if(event_pckt->evt == EVT_LE_META_EVENT)
		{
			/* GET META DATA  */
			 evt_le_meta_event *evt = (void *)event_pckt->data;

			 /*process each meta data event */
			 for(i=0; i<(sizeof(hci_le_meta_events_table)/sizeof(hci_le_meta_events_table_type));  i++)
			 {

				 if (evt->subevent == hci_le_meta_events_table[i].evt_code)
				 {
                    hci_le_meta_events_table[i].process((void *)evt->data);
				 }
			 }


		}

	     /* process vendor event */
		else if(event_pckt->evt == EVT_VENDOR)
		{
			evt_blue_aci *blue_evt = (void *)event_pckt->data;


			/*process each vendor event */
			 for(i=0; i<(sizeof(hci_vendor_specific_events_table)/sizeof(hci_vendor_specific_events_table_type));  i++)
			 {
				 if (blue_evt->ecode == hci_vendor_specific_events_table[i].evt_code)
				 {
                     hci_vendor_specific_events_table[i].process((void *)blue_evt->data);

				 }
			 }

		}

		/* process other events */
		else
		{
			for(i=0; i<(sizeof(hci_events_table)/sizeof(hci_events_table_type));  i++)
		        {
				    if (event_pckt->evt == hci_events_table[i].evt_code)
						{
				          hci_events_table[i].process((void *)event_pckt->data);

						}
		        }

		}


	}  /* end  process event packet */

} /* end void APP_UserEvtRx(void * pData) */



/**/


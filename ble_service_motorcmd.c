/*
 * ble_service_motorcmd.c
 *
 *  Created on: Mar 28, 2022
 *      Author: BillyC
 */
#include "main.h"
#include "bluenrg1_gap.h"
#include "bluenrg1_gatt_aci.h"
#include <stdlib.h>
#include "cmsis_os.h"

//#include "bt_command.h"
#include "printmsg.h"

static void receive_data(uint8_t * data_buffer, uint8_t num_bytes);
extern osMessageQueueId_t QueueMotorParamUpdateHandle;

static uint8_t connected = 0;
static uint8_t set_connectable = 1;
//static uint8_t notification_enabled = 0;
static uint16_t connection_handle = 0;

uint8_t MOTOR_SERVICE_UUID[16] = {0x76,0xaa,0x1c,0x30,
		                           0x01,0x09,0x97,0x9f,
		                           0xe2,0x11,0x9e,0xb1,
						           0xe0,0xf2,0x73,0xab};

uint8_t MOTORCMD_CHAR_UUID[16]       = {0x92,0x25,0xc1,0xfa,
		                           0x86,0x69,0x46,0xa1,
		                           0x85,0xb8,0x76,0x60,
						           0x71,0x85,0xa7,0xba};


uint16_t motor_service_handle;
uint16_t motorcmd_char_handle;



tBleStatus add_motor_services (void)
{

	tBleStatus ret;

    Service_UUID_t motor_service_uuid;
    Char_UUID_t motorcmd_char_uuid;

	BLUENRG_memcpy(motor_service_uuid.Service_UUID_128,MOTOR_SERVICE_UUID,sizeof(MOTOR_SERVICE_UUID));
	BLUENRG_memcpy(motorcmd_char_uuid.Char_UUID_128,MOTORCMD_CHAR_UUID,sizeof(MOTORCMD_CHAR_UUID));



	/* ADD MOTOR  SERVICE */

	 ret = aci_gatt_add_service(UUID_TYPE_128, &motor_service_uuid, PRIMARY_SERVICE, 7, &motor_service_handle);
	  if(ret != BLE_STATUS_SUCCESS)
	    {
	    	printmsg("aci_gatt_add_service motor_service fail... \n\r");
	    }


	  /* ADD MOTOR COMMAND CHARACTERISTICS */

        ret = aci_gatt_add_char(motor_service_handle,
        		                   UUID_TYPE_128,
								   &motorcmd_char_uuid,
								   2,
								   CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RESP,
								   ATTR_PERMISSION_NONE,
        		                   GATT_NOTIFY_ATTRIBUTE_WRITE,/* The application will be notified when a client writes to this attribute.*/
								   0,
								   1,
								   &motorcmd_char_handle);

            if(ret != BLE_STATUS_SUCCESS)
              {
              	printmsg("aci_gatt_add_char fail... \n\r");
              }

 return (ret);
}//end add_motor_services (void)




static uint8_t msg;

void receive_data(uint8_t * data_buffer, uint8_t num_bytes)
{
  //HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);
	//msg.Idx= 0x82;
	//msg.Buf= 0x50;
osStatus_t status;
//	for(int i = 0; i<num_bytes; i++)
      msg  =  * data_buffer;
		status = osMessageQueuePut(QueueMotorParamUpdateHandle, &msg, 0U, 0U);
		if (status == osOK)
			printmsg("aci_gatt_add_char fail... \n\r");
}





void Attriburte_Modified_CB(uint16_t handle,
		                    uint8_t data_length,
						    uint8_t * att_data)
{
  if(handle == motorcmd_char_handle + 1)
  {
	  receive_data(att_data, data_length);
  }


}



/*This event is generated to the application by the GATT server when a client modifies any
    attribute on the server, as consequence of one of the following GATT procedures:
- write without response
- signed write without response
- write characteristic value
- write long characteristic value
- reliable write.
*/
void aci_gatt_attribute_modified_event(uint16_t Connection_Handle,
                                       uint16_t Attribute_Handle,
		                               uint16_t Offset,
                                       uint16_t Attr_Data_Length,
									   uint8_t Attr_Data[])
{

	Attriburte_Modified_CB(Attribute_Handle, Attr_Data_Length,Attr_Data);

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

	 connected = 1;
	  connection_handle = Connection_Handle;
	  printmsg("Disonnected... \n\r");

}




void hci_disconnection_complte_event(uint8_t Status, uint16_t Connection_Handle,  uint8_t Reason )
{
  connected = 1;
  set_connectable = 1;
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






/*
 * service.c
 *
 *  Created on: Jan 12, 2022
 *      Author: BillyC
 *
 *      NUMBER OF SERVICES :2
 *
 *      HEALTH SERVICE AND WEATHER SERVICE
 *      HEALTH SERVICE HAS TWO CHARACRTERISTIC   -1.BEATS PER MINUTE (BPM)
 *                                               -2.WEIGHT
 *
 *      WEATHER SERVICE HAS TWO CHARACTRERISTICS  -1. TEMPERATURE
 *                                               -2. HUMIDITY
 */



#include "main.h"
#include "bluenrg1_gap.h"
#include "bluenrg1_gatt_aci.h"
#include <stdlib.h>

//#include "bt_command.h"
#include "printmsg.h"

void receive_data(uint8_t * data_buffer, uint8_t num_bytes);

////////////////////////////////////////////////////////////////////////////   WEATHER_SERVICE   ////////////////////////////////////////////////////////////////////////////



uint8_t WEATHER_SERVICE_UUID[16] = {0x92,0x25,0xc1,0xfa,
		                           0x86,0x69,0x46,0xa1,
		                           0x85,0xb8,0x76,0x60,
						           0x70,0x85,0xa7,0x83};

uint8_t TEMP_CHAR_UUID[16]       = {0x92,0x25,0xc1,0xfa,
		                           0x86,0x69,0x46,0xa1,
		                           0x85,0xb8,0x76,0x60,
						           0x71,0x85,0xa7,0x83};

uint8_t HUMIDITY_CHAR_UUID[16]   = {0x92,0x25,0xc1,0xfa,
		                           0x86,0x69,0x46,0xa1,
		                           0x85,0xb8,0x76,0x60,
						           0x72,0x85,0xa7,0x83};



////////////////////////////////////////////////////////////////////////////   HEALTH_SERVICE   ////////////////////////////////////////////////////////////////////////////

uint8_t HEALTH_SERVICE_UUID[16] = {0x66,0x9a,0x0c,0x20,
		                           0x00,0x08,0x96,0x9e,
		                           0xe2,0x11,0x9e,0xb1,
						           0xe0,0xf2,0x73,0xd9};


uint8_t BPM_CHAR_UUID[16]      = {0x66,0x9a,0x0c,0x20,
		                          0x00,0x08,0x96,0x9e,
		                          0xe2,0x11,0x9e,0xb1,
						          0xe1,0xf2,0x73,0xd9};

uint8_t WEIGHT_CHAR_UUID[16]   = {0x66,0x9a,0x0c,0x20,
		                          0x00,0x08,0x96,0x9e,
		                          0xe2,0x11,0x9e,0xb1,
						          0xe2,0xf2,0x73,0xd9};




////////////////////////////////////////////////////////////////////////////  CHAT_SERVICE   ////////////////////////////////////////////////////////////////////////////

uint8_t CHATTING_SERVICE_UUID[16] = {0x76,0xaa,0x1c,0x30,
		                           0x01,0x09,0x97,0x9f,
		                           0xe2,0x11,0x9e,0xb1,
						           0xe0,0xf2,0x73,0xd9};


uint8_t TX_CHAR_UUID[16]      = {0x66,0x9a,0x0c,0x20,
		                          0x00,0x08,0x96,0x9e,
		                          0xe2,0x11,0x9e,0xb1,
						          0xe1,0xf2,0x73,0xd9};

uint8_t RX_CHAR_UUID[16]   = {0x66,0x9a,0x0c,0x20,
		                          0x00,0x08,0x96,0x9e,
		                          0xe2,0x11,0x9e,0xb1,
						          0xe2,0xf2,0x73,0xd9};


////////////////////////////////////////////////////////////////////   handles   ////////////////////////////////////////////////////////////////////////////////////




uint16_t heath_service_handle;
uint16_t bpm_char_handle;
uint16_t weight_char_handle;

uint16_t weather_service_handle;
uint16_t temp_char_handle;
uint16_t hum_char_handle;


uint16_t chatting_service_handle;
uint16_t tx_char_handle;
uint16_t rx_char_handle;


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t connected = 0;
uint8_t set_connectable = 1;
uint8_t notification_enabled = 0;
uint16_t connection_handle = 0;





tBleStatus add_services (void)
{
	tBleStatus ret;

    Service_UUID_t weather_service_uuid;
    Char_UUID_t hum_char_uuid;
    Char_UUID_t temp_char_uuid;


    Service_UUID_t heath_service_uuid;
    Char_UUID_t bpm_char_uuid;
    Char_UUID_t weight_char_uuid;


    Service_UUID_t chatting_service_uuid;
    Char_UUID_t tx_char_uuid;
    Char_UUID_t rx_char_uuid;



    BLUENRG_memcpy(weather_service_uuid.Service_UUID_128,WEATHER_SERVICE_UUID,sizeof(WEATHER_SERVICE_UUID));
    BLUENRG_memcpy(hum_char_uuid.Char_UUID_128,HUMIDITY_CHAR_UUID,sizeof(HUMIDITY_CHAR_UUID));
    BLUENRG_memcpy(temp_char_uuid.Char_UUID_128,TEMP_CHAR_UUID,sizeof(TEMP_CHAR_UUID));

    BLUENRG_memcpy(heath_service_uuid.Service_UUID_128,HEALTH_SERVICE_UUID,sizeof(HEALTH_SERVICE_UUID));
    BLUENRG_memcpy(bpm_char_uuid.Char_UUID_128,BPM_CHAR_UUID,sizeof(BPM_CHAR_UUID));
    BLUENRG_memcpy(weight_char_uuid.Char_UUID_128,WEIGHT_CHAR_UUID,sizeof(WEIGHT_CHAR_UUID));


    BLUENRG_memcpy(chatting_service_uuid.Service_UUID_128,CHATTING_SERVICE_UUID,sizeof(CHATTING_SERVICE_UUID));
    BLUENRG_memcpy(tx_char_uuid.Char_UUID_128,TX_CHAR_UUID,sizeof(TX_CHAR_UUID));
    BLUENRG_memcpy(rx_char_uuid.Char_UUID_128,RX_CHAR_UUID,sizeof(RX_CHAR_UUID));






 /* ADD HEALTH SERVICE */

 ret = aci_gatt_add_service(UUID_TYPE_128, &heath_service_uuid, PRIMARY_SERVICE, 7, &heath_service_handle);
  if(ret != BLE_STATUS_SUCCESS)
    {
    	printmsg("aci_gatt_add_service fail... \n\r");
    }


  /* ADD HEALTH SERVICE CHARACTERISTICS */

  ret = aci_gatt_add_char(heath_service_handle, UUID_TYPE_128, &bpm_char_uuid,2, CHAR_PROP_READ, ATTR_PERMISSION_NONE,
		                  GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,0,0,&bpm_char_handle);

    if(ret != BLE_STATUS_SUCCESS)
      {
      	printmsg("aci_gatt_add_char fail... \n\r");
      }



    ret = aci_gatt_add_char(heath_service_handle, UUID_TYPE_128, &weight_char_uuid,2, CHAR_PROP_READ, ATTR_PERMISSION_NONE,
  		                  GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,0,0,&weight_char_handle);

      if(ret != BLE_STATUS_SUCCESS)
        {
        	printmsg("aci_gatt_add_char fail... \n\r");
        }


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

      /* ADD WEATHER SERVICE */

      ret = aci_gatt_add_service(UUID_TYPE_128, &weather_service_uuid, PRIMARY_SERVICE, 7, &weather_service_handle);
       if(ret != BLE_STATUS_SUCCESS)
         {
         	printmsg("aci_gatt_add_service fail... \n\r");
         }


       /* ADD TEMPERATURE CHARACTERISTICS */

       ret = aci_gatt_add_char(weather_service_handle, UUID_TYPE_128, &temp_char_uuid,2, CHAR_PROP_READ, ATTR_PERMISSION_NONE,
     		                  GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,0,0,&temp_char_handle);

         if(ret != BLE_STATUS_SUCCESS)
           {
           	printmsg("aci_gatt_add_char fail... \n\r");
           }


         /* ADD HUMIDITY CHARACTERISTICS */

         ret = aci_gatt_add_char(weather_service_handle, UUID_TYPE_128, &hum_char_uuid,2, CHAR_PROP_READ, ATTR_PERMISSION_NONE,
       		                  GATT_NOTIFY_READ_REQ_AND_WAIT_FOR_APPL_RESP,0,0,&hum_char_handle);

           if(ret != BLE_STATUS_SUCCESS)
             {
             	printmsg("aci_gatt_add_char fail... \n\r");
             }

/////////////////////////////////////////////////////////////////ADD CHAT SERVICE//////////////////////////////////////////////////////////
           /* ADD CHAT SERVICE */

           ret = aci_gatt_add_service(UUID_TYPE_128,
        		                      &chatting_service_uuid,
									  PRIMARY_SERVICE,
									  7,
									  &chatting_service_handle);


            if(ret != BLE_STATUS_SUCCESS)
              {
              	printmsg("aci_gatt_add_service fail... \n\r");
              }


            /* ADD TX CHARACTERISTICS */

            ret = aci_gatt_add_char(chatting_service_handle,
            		                UUID_TYPE_128,
									&tx_char_uuid,
									20,
									CHAR_PROP_NOTIFY,
									ATTR_PERMISSION_NONE,
          		                    0,
								    0,
									1,
									&tx_char_handle);

              if(ret != BLE_STATUS_SUCCESS)
                {
                	printmsg("aci_gatt_add_char fail... \n\r");
                }


              /* ADD RX CHARACTERISTICS */

              ret = aci_gatt_add_char(chatting_service_handle,
            		                   UUID_TYPE_128,
									   &rx_char_uuid,
									   20,
									   CHAR_PROP_WRITE | CHAR_PROP_WRITE_WITHOUT_RESP,
									   ATTR_PERMISSION_NONE,
            		                   GATT_NOTIFY_ATTRIBUTE_WRITE,/* The application will be notified when a client writes to this attribute.*/
									   0,
									   1,
									   &rx_char_handle);

                if(ret != BLE_STATUS_SUCCESS)
                  {
                  	printmsg("aci_gatt_add_char fail... \n\r");
                  }


  return (ret);

}



void update_bpm_data(int16_t new_data)
	{
	     tBleStatus ret;

	     ret = aci_gatt_update_char_value(heath_service_handle,bpm_char_handle,0,2,(uint8_t *)&new_data);

	     if(ret != BLE_STATUS_SUCCESS)
	          {
	          	printmsg("aci_gatt_update_char_value fail... \n\r");
	          }

	}

void update_weight_data(int16_t new_data)
	{
	     tBleStatus ret;

	     ret = aci_gatt_update_char_value(heath_service_handle,weight_char_handle,0,2,(uint8_t *)&new_data);

	     if(ret != BLE_STATUS_SUCCESS)
	          {
	          	printmsg("aci_gatt_update_char_value fail... \n\r");
	          }

	}



void update_temperture_data(int16_t new_data)
	{
	     tBleStatus ret;

	     ret = aci_gatt_update_char_value(weather_service_handle,temp_char_handle,0,2,(uint8_t *)&new_data);

	     if(ret != BLE_STATUS_SUCCESS)
	          {
	          	printmsg("aci_gatt_update_char_value fail... \n\r");
	          }

	}


void update_humidity_data(int16_t new_data)
	{
	     tBleStatus ret;

	     ret = aci_gatt_update_char_value(weather_service_handle,hum_char_handle,0,2,(uint8_t *)&new_data);

	     if(ret != BLE_STATUS_SUCCESS)
	          {
	          	printmsg("aci_gatt_update_char_value fail... \n\r");
	          }

	}






/*
void GAP_ConnectionComplete_CB(uint8_t addr[6],uint16_t handle)
{
	 connected = 1;
	 connection_handle = handle;
	 printmsg("Connection Complete... \n\r");
}

void GAP_DisonnectionComplete_CB(uint8_t addr[6],uint16_t handle)
{

	 printmsg("DisonnectionComplete_CB Complete... \n\r");
}

*/

int bpm=85;
int weight=90;
int temp= 20;
int hum= 80;



void Attriburte_Modified_CB(uint16_t handle,
		                    uint8_t data_length,
						    uint8_t * att_data)
{
  if(handle == rx_char_handle + 1)
  {
	  receive_data(att_data, data_length);
  }

  else if(handle == tx_char_handle + 2)
  {
	  if(att_data[0] == 0x01)
		  notification_enabled = 1;
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


/**
  * @brief This event is generated when a notification is received from the server.
  * @param Connection_Handle Connection handle related to the response
  * @param Attribute_Handle The handle of the attribute
  * @param Attribute_Value_Length Length of Attribute_Value in octets
  * @param Attribute_Value The current value of the attribute
  * @retval None
*/
void aci_gatt_notification_event(uint16_t Connection_Handle,
                                 uint16_t Attribute_Handle,
                                 uint8_t Attribute_Value_Length,
                                 uint8_t Attribute_Value[])
	{

	       if(Attribute_Handle == (tx_char_handle + 2))
	          {
		        receive_data(Attribute_Value,Attribute_Value_Length);
	          }
	}



void Read_Request_CB(uint16_t handle)
{

  if(handle == (bpm_char_handle + 1))
  {
	  bpm=bpm+2;
	  update_bpm_data(bpm);
  }

  if(handle == (weight_char_handle + 1))
   {
	  weight=weight+2;
	  update_weight_data(weight);
   }


  if(handle == (temp_char_handle + 1))
   {
	  temp=temp+2;
	  update_temperture_data(temp);
   }

  if(handle == (hum_char_handle + 1))
   {
	  hum=hum+2;
	  update_humidity_data(hum);
   }




	if(connection_handle != 0)
	{
		aci_gatt_allow_read(connection_handle);


	}

}

void aci_gatt_read_permit_req_event(uint16_t Connection_Handle,
		                            uint16_t Attribute_Handle,
									uint16_t Offset)
{
	Read_Request_CB(Attribute_Handle);
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




uint8_t rcv_data[20];

void receive_data(uint8_t * data_buffer, uint8_t num_bytes)
{
  HAL_GPIO_TogglePin(GPIOA, GPIO_PIN_5);

	for(int i = 0; i<num_bytes; i++)
	{
		rcv_data[i] = data_buffer[i];
	}

	switch (rcv_data[0])
	     {
	          case 0x80:
	        	//  HAL_DeInit();
	        	  printmsg("got %h \n\r",rcv_data[0]);
	             break;
	          case 0x81:
	        	//  HAL_RCC_DeInit();
	        	 //     HAL_DeInit();
	        	  printmsg("got %h \n\r",rcv_data[0]);
	             break;

	          default:
	        	  printmsg("default \n\r");
	     }


	}

	//bootloader_jump_to_user_app(FLASH_SECTOR4_BASE_ADDRESS);
	//bootloader_jump_to_user_app();
	//bootloader_jump_to_user_app2(FLASH_SECTOR4_BASE_ADDRESS);




void send_data(uint8_t * data_buffer, uint8_t num_bytes)

{
	tBleStatus  ret;

	/* Udate characteristic value */

	   ret = aci_gatt_update_char_value(chatting_service_handle,tx_char_handle,0,num_bytes,data_buffer);

	     if(ret != BLE_STATUS_SUCCESS)
	          {
	          	printmsg("aci_gatt_update_char_value fail... \n\r");
	          }
}










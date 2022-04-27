/*
 * service.h
 *
 *  Created on: Jan 12, 2022
 *      Author: BillyC
 */

#ifndef INC_SERVICE_H_
#define INC_SERVICE_H_

tBleStatus add_services(void);
void APP_UserEvtRx(void * pData);

void send_data(uint8_t * data_buffer, uint8_t num_bytes);
#endif /* INC_SERVICE_H_ */

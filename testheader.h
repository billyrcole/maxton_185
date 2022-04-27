/*
 * testheader.h
 *
 *  Created on: Mar 31, 2022
 *      Author: BillyC
 */

#ifndef INC_TESTHEADER_H_
#define INC_TESTHEADER_H_
#include "message.h"
/* Call trackers */
extern Event_t last_call;
extern Event_t new_call;
static CANMessage_t l_coil_msg;
extern osMessageQueueId_t hQueueFSMHandle;




#endif /* INC_TESTHEADER_H_ */

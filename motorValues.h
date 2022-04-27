/*
 * motorValues.h
 *
 *  Created on: Apr 13, 2022
 *      Author: BillyC
 */

#ifndef INC_MOTORVALUES_H_
#define INC_MOTORVALUES_H_

#include "sysconfig.h"


__attribute__ ((section ("mysectionBss")))const ValveInfo_t EMV185 = {VALVE_TYPE_DIRECT,100,10,10000,10000,10000,
		                                                  {10000,10000,10000,10000,10000,10000},{10000,10000,10000,10000,10000,10000}};		/*!< Global EMV185 object for use basically everywhere */


#endif /* INC_MOTORVALUES_H_ */

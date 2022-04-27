/*
 * config.c
 *
 *  Created on: Feb 21, 2022
 *      Author: BillyC
 */

static void setup_valve(FSM_t *sm);





static void setup_valve(FSM_t *sm)
{
	CANMessage_t can_msg;		/* For outgoing update messages to RDB */
	Configuration_t config;		/* Incoming configuration packets */
	State_t next_state = STATE_SETUP;
	uint32_t buf;

	/* Set the default destination for all outgoing CAN messages */
	can_msg.ID = CAN_NODE_ID_MASTER;

	/* Update the state machine's state	 */
	sm->CurrentState = STATE_SETUP;

	printf("Entering Setup.\r\n");

	/* Loop until a request to exit setup is received */
	while(next_state == STATE_SETUP){

		osMessageQueuePut(hQueueMotorCmdsHandle, &motor_cmd,0U,1000U);

}

}
/** @} */

		/* The first byte of the CAN packet identifies the value being returned */
		can_msg.Data[0] = config.Item;

		switch(config.Item){

			case CONFIGURE_SETUP_EXIT:

				/* Clear the FSM and motor queues to make sure commands that
				 * came in during setup don't get executed immediately upon
				 * returning to idle state.
				 */
			//	xQueueReset(hQueueFSM);
				//xQueueReset(hQueueMotorCmds);

				next_state = STATE_IDLE;
				break;

			case CONFIGURE_INSPECTION:

				if(config.Write){
					fsm.Inspection = (bool)config.Specifier;
				}
				else{	/* Read request */
					can_msg.Data[1] = fsm.Inspection;
					can_msg.DataLength = 2;
				//	xQueueSendToBack(hQueueCanOut, &can_msg, portMAX_DELAY/2);
				}
				break;

			case CONFIGURE_ADJ_MODE:

				if(config.Write){
					fsm.Mode = (bool)config.Specifier;
				}
				else{	/* Read request */
					can_msg.Data[1] = fsm.Mode;
					can_msg.DataLength = 2;
					xQueueSendToBack(hQueueCanOut, &can_msg, portMAX_DELAY/2);
				}
				break;

			case CONFIGURE_PHASE_POSITION:
				// TODO: Write values to EEPROM
				buf = config.Value;

				if(config.Write){
				switch(config.Specifier){
						case VALUE_POS_UHS:
							EMV185.Positions.UHS = config.Value;
							eeprom_write32(EEPROM_ADDR_VAL_POS_UHS_32B, buf);
						break;
						case VALUE_POS_UL:
							EMV185.Positions.UL = buf;
							eeprom_write32(EEPROM_ADDR_VAL_POS_UL_32B, buf);
						break;
						case VALUE_POS_UP_STOP:
							EMV185.Positions.ParkBP = buf;
							eeprom_write32(EEPROM_ADDR_VAL_POS_PARK_BP_32B, buf);
						break;
						case VALUE_POS_DHS:
							EMV185.Positions.DHS = buf;
							eeprom_write32(EEPROM_ADDR_VAL_POS_DHS_32B, buf);
						break;
						case VALUE_POS_DL:
							EMV185.Positions.DL = buf;
							eeprom_write32(EEPROM_ADDR_VAL_POS_DL_32B, buf);
						break;
						case VALUE_POS_DOWN_STOP:
							EMV185.Positions.ParkDC = buf;
							eeprom_write32(EEPROM_ADDR_VAL_POS_PARK_DC_32B, buf);
						break;
				}
				}
				else{
					switch(config.Specifier){
						case VALUE_POS_UHS:
							buf = EMV185.Positions.UHS; break;
						case VALUE_POS_UL:
							buf = EMV185.Positions.UL; break;
						case VALUE_POS_UP_STOP:
							buf = EMV185.Positions.ParkBP; break;
						case VALUE_POS_DHS:
							buf = EMV185.Positions.DHS; break;
						case VALUE_POS_DL:
							buf = EMV185.Positions.DL; break;
						case VALUE_POS_DOWN_STOP:
							buf = EMV185.Positions.ParkDC; break;
					}
					can_msg.DataLength = 6;
					can_msg.Data[1] = config.Specifier;
					can_msg.Data[2] = buf;
					can_msg.Data[3] = (buf >> 8) & 0xFF;
					can_msg.Data[4] = (buf >> 16) & 0xFF;
					can_msg.Data[5] = (buf >> 24) & 0xFF;
					xQueueSendToBack(hQueueCanOut, &can_msg, portMAX_DELAY/2);
				}

				break;

			case CONFIGURE_PHASE_SPEED:

				buf = config.Value;

				if(config.Write){
					switch(config.Specifier){
						case PHASE_SPEED_UL:
							EMV185.SpeedUL = buf;
							eeprom_write16(EEPROM_ADDR_VAL_SPEED_UL_16B, buf);
							break;
						case PHASE_SPEED_DL:
							EMV185.SpeedDL = buf;
							eeprom_write16(EEPROM_ADDR_VAL_SPEED_DL_16B, buf);
							break;
						case PHASE_SPEED_DHS:
							EMV185.SpeedDHS = buf;
							eeprom_write16(EEPROM_ADDR_VAL_SPEED_DHS_16B, buf);
							break;
				}
				}
				else{
					switch(config.Specifier){
						case PHASE_SPEED_UL:
							buf = EMV185.SpeedUL; break;
						case PHASE_SPEED_DL:
							buf = EMV185.SpeedDL; break;
						case PHASE_SPEED_DHS:
							buf = EMV185.SpeedDHS; break;
					}
					can_msg.DataLength = 4;
					can_msg.Data[1] = config.Specifier;
					can_msg.Data[2] = buf;
					can_msg.Data[3] = (buf >> 8) & 0xFF;
					xQueueSendToBack(hQueueCanOut, &can_msg, portMAX_DELAY/2);
				}

				break;

			case CONFIGURE_ACTUATOR_SPEED:

				buf = config.Value;

				if(config.Write){
					switch(config.Specifier){
						case PHASE_UA:
							EMV185.Speeds.UA = buf;
							eeprom_write32(EEPROM_ADDR_VAL_RATE_UA_32B, buf);
							break;
						case PHASE_UT:
							EMV185.Speeds.UT = buf;
							eeprom_write32(EEPROM_ADDR_VAL_RATE_UT_32B, buf);
							break;
						case PHASE_US:
							EMV185.Speeds.US = buf;
							eeprom_write32(EEPROM_ADDR_VAL_RATE_US_32B, buf);
							break;
						case PHASE_DA:
							EMV185.Speeds.DA = buf;
							eeprom_write32(EEPROM_ADDR_VAL_RATE_DA_32B, buf);
							break;
						case PHASE_DT:
							EMV185.Speeds.DT = buf;
							eeprom_write32(EEPROM_ADDR_VAL_RATE_DT_32B, buf);
							break;
						case PHASE_DS:
							EMV185.Speeds.DS = buf;
							eeprom_write32(EEPROM_ADDR_VAL_RATE_DS_32B, buf);
							break;
					}

				}
				else{
					switch(config.Specifier){
						case PHASE_UA:
							buf = EMV185.Speeds.UA;	break;
						case PHASE_UT:
							buf = EMV185.Speeds.UT;	break;
						case PHASE_US:
							buf = EMV185.Speeds.US;	break;
						case PHASE_DA:
							buf = EMV185.Speeds.DA;	break;
						case PHASE_DT:
							buf = EMV185.Speeds.DT;	break;
						case PHASE_DS:
							buf = EMV185.Speeds.DS;	break;
					}
					can_msg.DataLength = 6;
					can_msg.Data[1] = config.Specifier;
					can_msg.Data[2] = buf;
					can_msg.Data[3] = (buf >> 8) & 0xFF;
					can_msg.Data[4] = (buf >> 16) & 0xFF;
					can_msg.Data[5] = (buf >> 24) & 0xFF;
					xQueueSendToBack(hQueueCanOut, &can_msg, portMAX_DELAY/2);
				}
				break;

			case CONFIGURE_PISTON_COUNT:
				if(config.Write){
					if((config.Specifier > 0) && (config.Specifier < 3)){
						EMV185.PistonCount = config.Specifier;
						eeprom_write8(EEPROM_ADDR_CFG_PISTON_COUNT_8B, EMV185.PistonCount);
					}
				}
				else{
					can_msg.DataLength = 2;
					can_msg.Data[1] = EMV185.PistonCount;
					xQueueSendToBack(hQueueCanOut, &can_msg, portMAX_DELAY/2);
				}
				break;

			case CONFIGURE_PISTON_TYPE:
				if(config.Write){
					if(config.Specifier <= VALVE_TYPE_ROPED){
						EMV185.Type = config.Specifier;
						eeprom_write8(EEPROM_ADDR_CFG_VALVE_TYPE_8B, EMV185.Type);
					}
				}
				else{
					can_msg.DataLength = 2;
					can_msg.Data[1] = EMV185.Type;
					xQueueSendToBack(hQueueCanOut, &can_msg, portMAX_DELAY/2);
				}
				break;

			case CONFIGURE_PISTON_SIZE:
				if(config.Write){
					if((config.Specifier >= VALVE_SIZE_MINIMUM) && (config.Specifier <= VALVE_SIZE_MAXIMUM)){
						EMV185.Size = config.Specifier;
						eeprom_write8(EEPROM_ADDR_CFG_VALVE_SIZE_8B, EMV185.Size);
					}
				}
				else{
					can_msg.DataLength = 2;
					can_msg.Data[1] = EMV185.Size;
					xQueueSendToBack(hQueueCanOut, &can_msg, portMAX_DELAY/2);
				}
				break;
		}
	}

	/* Transition to next state before exiting */
	sm->CurrentState = next_state;

	printf("Exiting Setup.\r\n");
}


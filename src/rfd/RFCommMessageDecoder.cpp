/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFCommMessageDecoder.h"
#include "RFCommMessage.h"
#include "BidcosFrameDecoder.h"
#include <stdio.h>

RFCommMessageDecoder::RFCommMessageDecoder(void)
{
}

RFCommMessageDecoder::~RFCommMessageDecoder(void)
{
}

std::string RFCommMessageDecoder::ToString(RFCommMessage* msg)
{
	static const char* INFO_MESSAGES[]={"DUTY_CYCLE", "TX_COMPLETE", "RX_AUTH_REQUEST", "KEYS"};
	static const char* ERROR_MESSAGES[]={"HARDWARE", "TIMEOUT", "DUTY_CYCLE_FULL", "AUTH_FAILED",
		"ADDRESS_NOT_SET", "EEPROM_CRC", "SET_KEY_IGNORED", "UNKNOWN_AES_KEY"};
	static char buffer[256];
	std::string result;
	if(msg->GetCommand()==RFCommMessage::CMD_ERROR){
		snprintf(buffer, sizeof(buffer), "Error #%d", msg->GetID()&0xff);
		result=buffer;
		for(unsigned int i=0;i<msg->GetSize();i++){
			if(i==0 && msg->GetByteData(i) < sizeof(ERROR_MESSAGES)/sizeof(ERROR_MESSAGES[0])){
				snprintf(buffer, sizeof(buffer), " %s", ERROR_MESSAGES[msg->GetByteData(i)]);
			}else{
				snprintf(buffer, sizeof(buffer), " %02X",  (int)msg->GetByteData(i));
			}
			result+=buffer;
		}
		return result;
	}
	if(msg->GetCommand()==RFCommMessage::CMD_INFO){
		snprintf(buffer, sizeof(buffer), "Info #%d", msg->GetID()&0xff);
		result=buffer;
		for(unsigned int i=0;i<msg->GetSize();i++){
			if(i==0 && msg->GetByteData(i) < sizeof(INFO_MESSAGES)/sizeof(INFO_MESSAGES[0])){
				snprintf(buffer, sizeof(buffer), " %s", INFO_MESSAGES[msg->GetByteData(i)]);
			}else{
				snprintf(buffer, sizeof(buffer), " %02X",  (int)msg->GetByteData(i));
			}
			result+=buffer;
		}
		return result;
	}
	if(msg->GetCommand()==RFCommMessage::CMD_ADDRESS){
		result="Address ";
		for(unsigned int i=0;i<msg->GetSize();i++){
			snprintf(buffer, sizeof(buffer), "%02X", (int)msg->GetByteData(i));
			result+=buffer;
		}
		return result;
	}
	if(msg->GetCommand()==RFCommMessage::CMD_SET_KEY){
		result="SetKey ";
		for(unsigned int i=0;i<msg->GetSize();i++){
			snprintf(buffer, sizeof(buffer), "%02X", (int)msg->GetByteData(i));
			result+=buffer;
		}
		return result;
	}
	if(msg->GetCommand()==RFCommMessage::CMD_GET_KEYS){
		result="GetKeys ";
		for(unsigned int i=0;i<msg->GetSize();i++){
			snprintf(buffer, sizeof(buffer), "%02X", (int)msg->GetByteData(i));
			result+=buffer;
		}
		return result;
	}
	snprintf(buffer, sizeof(buffer), "#%d", (int)msg->GetID());

    BidcosFrame payload=msg->ExtractPayload();
	return std::string(buffer)+BidcosFrameDecoder::ToString(&payload);
}

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFCommMessageAESKey.h"
#include "RFManager.h"
#include "RFCommMessageDecoder.h"

#include <Logger.h>

RFCommMessageAESKey::RFCommMessageAESKey(void)
{
}

RFCommMessageAESKey::~RFCommMessageAESKey(void)
{
}

bool RFCommMessageAESKey::ProcessResponse(CommMessage *m, t_state* new_state)
{
	RFCommMessage* rf_msg=(RFCommMessage*)m;
//	LOG(Logger::LOG_DEBUG, "RFCommMessageAESKey::ProcessResponse() %s", RFCommMessageDecoder::ToString(rf_msg).c_str());
	if(rf_msg->GetCommand()==RFCommMessage::CMD_INFO && rf_msg->GetByteData(RFCommMessage::INFO_FIELD_MESSAGE)==RFCommMessage::INFO_TYPE_KEYS){
		pthread_mutex_lock(&mutex);
		if(state==WAIT_ACK){
			m->SetParent(this);
			vect_responses.push_back(m);
			*new_state=ACKED;
			pthread_mutex_unlock(&mutex);
		}
	}else{
		return false;
	}
	LOG(Logger::LOG_DEBUG, "RX: %s", RFCommMessageDecoder::ToString(rf_msg).c_str());
    return true;
}

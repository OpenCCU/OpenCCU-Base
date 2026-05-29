/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFCommMessage.h"
#include "RFCommMessageDecoder.h"
#include "RFController.h"
#include <Logger.h>
#include <utils.h>
#include <stdio.h>

RFCommMessage::RFCommMessage(void)
{
	SetCommand(CMD_SEND);
	SetResponseTimeout(2000);
	flags = 0;
	auth_key=BidcosFrame::INVALID_AUTH_KEY;
	rx_timestamp = time_millis();
	rssi=BidcosFrame::INVALID_RSSI_VALUE;
}

RFCommMessage::~RFCommMessage(void)
{
}

void RFCommMessage::SetTelegramCounter(int counter)
{
	if(GetPayloadOffset()>=0)SetIntValue(PayloadField(BidcosFrame::FIELD_COUNTER), counter);
};

int RFCommMessage::GetTelegramCounter()
{
	if(GetPayloadOffset()>=0)return GetIntValue(PayloadField(BidcosFrame::FIELD_COUNTER));
	else return -1;
};

int RFCommMessage::GetCtrl()
{
	if(GetPayloadOffset()>=0)return GetIntValue(PayloadField(BidcosFrame::FIELD_CTRL));
	else return -1;
};

int RFCommMessage::GetSenderAddress()
{
	if(GetPayloadOffset()>=0)return GetIntValue(PayloadField(BidcosFrame::FIELD_SENDER));
	else return -1;
}

int RFCommMessage::GetReceiverAddress()
{
	if(GetPayloadOffset()>=0)return GetIntValue(PayloadField(BidcosFrame::FIELD_RECEIVER));
	else return -1;
}

bool RFCommMessage::ProcessResponse(CommMessage *m, t_state* new_state)
{
	//	LOG(Logger::LOG_DEBUG, "RFCommMessage::ProcessResponse()");
	RFCommMessage* rf_msg=(RFCommMessage*)m;

	if(rf_msg->GetCommand()==RFCommMessage::CMD_INFO){
		if(rf_msg->GetID() != GetID())return false;
		switch(rf_msg->GetByteData(0)){
			case RFCommMessage::INFO_TYPE_TX_COMPLETE:
				pthread_mutex_lock(&mutex);
				if(state==WAIT_ACK){
					*new_state=ACKED;
				}
				pthread_mutex_unlock(&mutex);
				return true;
			case RFCommMessage::INFO_TYPE_RX_AUTH_REQUEST:
				SetAuthKey(rf_msg->GetByteData(1));
				return true;
			default:
				return false;
		}
	}else if(rf_msg->GetCommand()==RFCommMessage::CMD_ERROR){
		//		if(id!=rf_msg->GetID()){
		//			printf("id mismatch\n");
		//			return false;
		//		}
		if((rf_msg->GetByteData(0x00)==RFCommMessage::ERROR_UNKNOWN_AES_KEY)){
			SetAuthKey(BidcosFrame::UNKNOWN_AUTH_KEY);
		}
		pthread_mutex_lock(&mutex);
		if(state==WAIT_ACK){
			*new_state=DROPPED;
			//			LOG(Logger::LOG_DEBUG, "new_state=DROPPED");
		}
		pthread_mutex_unlock(&mutex);
		if(GetCollectResponses()){
			rf_msg->parent=this;
			vect_responses.push_back(rf_msg);
		}
		return true;
	}else if(
		(rf_msg->GetCommand()==RFCommMessage::CMD_RESPONSE || 
		rf_msg->GetCommand()==RFCommMessage::CMD_EVENT ||
		rf_msg->GetCommand()==RFCommMessage::CMD_AES_EVENT) && 
		(rf_msg->GetSenderAddress()==this->GetReceiverAddress())){
			if(rf_msg->GetFlags() & FLAG_PRELIMINARY){
				LOG(Logger::LOG_DEBUG, "Ignoring preliminary response: %s", RFCommMessageDecoder::ToString(rf_msg).c_str());
				return false;
			}
			if(id!=rf_msg->GetID()){
				LOG(Logger::LOG_DEBUG, "id mismatch");
				return false;
			}
			if(rf_msg->GetCommand()==RFCommMessage::CMD_AES_EVENT){
				//rf_msg is a valid authenticated response. Set the key used in authentication.
				SetAuthKey(rf_msg->GetAuthKey());
			}else{
				//rf_msg is not authenticated so the original message isn't authenticated also
				SetAuthKey(BidcosFrame::INVALID_AUTH_KEY);
			}

			pthread_mutex_lock(&mutex);
			if(state==WAIT_ACK){
				*new_state=ACKED;
				//			LOG(Logger::LOG_DEBUG, "new_state=ACKED");
			}
			pthread_mutex_unlock(&mutex);
			if(GetCollectResponses()){
				LOG(Logger::LOG_DEBUG, "RX: %s", RFCommMessageDecoder::ToString(rf_msg).c_str());
				rf_msg->parent=this;
				vect_responses.push_back(rf_msg);
			}
			return true;
	}else{
		return false;
	}
}

int RFCommMessage::GetPayloadOffset()
{
	switch(GetCommand()){
		case CMD_SEND_AT:
			//time before payload
			return 4;

		case CMD_RESPONSE:
		case CMD_EVENT:
			//time and rssi before payload
			return 5;

		case CMD_SEND:
		case CMD_AES_EVENT:
			return 0;

		case CMD_ERROR:
		case CMD_ADDRESS:
		case CMD_OVERRIDE:
		case CMD_SET_KEY:
		case CMD_GET_KEYS:
		case CMD_INFO:
		default:
			//no payload at all
			return -1;
	}
}

BidcosFrame RFCommMessage::ExtractPayload()
{
	BidcosFrame payload;
	int offset=GetPayloadOffset();
	if(offset>=0){
		for(unsigned int i=offset;i<GetSize();i++)payload.SetByteData(i-offset, GetByteData(i));
		payload.SetTimestamp(rx_timestamp);
		payload.SetDeviceWokenup((GetFlags()&FLAG_WOKENUP)!=0);
		payload.SetPreliminary((GetFlags()&FLAG_PRELIMINARY)!=0);
		payload.SetRSSI(-1*GetRSSI());
		if(GetCommand()==RFCommMessage::CMD_AES_EVENT)
		{
			payload.SetAuthKey(GetAuthKey());
		}

	}
	return payload;
}

bool RFCommMessage::SetPayload(const BidcosFrame& payload)
{
	int offset=GetPayloadOffset();
	if(offset<0)return false;

	for(unsigned int i=0;i<payload.GetSize();i++)SetByteData(i+offset, payload.GetByteData(i));
	return true;
}

void RFCommMessage::SetRSSI(int rssi)
{
	this->rssi=rssi;
}

int RFCommMessage::GetRSSI()
{
	if(GetPayloadOffset()==5){
		return GetIntValue(FIELD_RSSI);
	}else{
		return rssi;
	}
}

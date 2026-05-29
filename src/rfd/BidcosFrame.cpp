/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BidcosFrame.h"
#include "BidcosFrameDecoder.h"
#include <Logger.h>
#include "RFManager.h"
#include <utils.h>

BidcosFrame::BidcosFrame(void):unreach_reason(UNREACH_FALSE)
{
	auth_key=INVALID_AUTH_KEY;
	device_wokenup=false;
	timestamp=time_millis();
	type_index=2;
	rssi=INVALID_RSSI_VALUE;
    preliminary=false;
}

BidcosFrame::~BidcosFrame(void)
{
	ClearResponses();
}

void BidcosFrame::SetTelegramCounter(int counter)
{
	SetIntValue(FIELD_COUNTER, counter);
};

int BidcosFrame::GetTelegramCounter() const
{
	return GetIntValue(FIELD_COUNTER);
};

void BidcosFrame::SetCtrl(int flags)
{
	SetIntValue(FIELD_CTRL, flags);
};

int BidcosFrame::GetCtrl() const
{
	return GetIntValue(FIELD_CTRL);
};

void BidcosFrame::SetSenderAddress(int address)
{
	SetIntValue(FIELD_SENDER, address);
};

void BidcosFrame::SetReceiverAddress(int address)
{
	SetIntValue(FIELD_RECEIVER, address);
};

int BidcosFrame::GetSenderAddress() const
{
	return GetIntValue(FIELD_SENDER);
}

int BidcosFrame::GetReceiverAddress() const
{
	return GetIntValue(FIELD_RECEIVER);
}

BidcosFrame::ReceiverType BidcosFrame::GetReceiverType() const
{
	unsigned int address=GetReceiverAddress();
	if(address==RFManager::GetSingleton()->GetBidcosAddress())return RCV_CENTRAL;
	if(GetCtrl()&CTRL_BCAST)return RCV_BROADCAST;
	return RCV_OTHER;
}

int BidcosFrame::GetType() const
{
	return GetIntValue(FIELD_TYPE);
};

bool BidcosFrame::TransformToSimulationMessage()
{
	BidcosFrame orig_msg=*this;
	SetType(FT_SIMULATION);
	SetIntValue(FIELD_SIM_TYPE, orig_msg.GetType());
	SetIntValue(FIELD_SIM_SENDER, orig_msg.GetSenderAddress());
	for(unsigned int i=9;i<orig_msg.GetSize();i++)SetByteData(i+4, orig_msg.GetByteData(i));
	return true;
}

bool BidcosFrame::TransformFromSimulationMessage()
{
	BidcosFrame orig_msg=*this;
	SetType(orig_msg.GetIntValue(FIELD_SIM_TYPE));
	SetSenderAddress(orig_msg.GetIntValue(FIELD_SIM_SENDER));
	Resize(9);
	for(unsigned int i=13;i<orig_msg.GetSize();i++)SetByteData(i-4, orig_msg.GetByteData(i));
	return true;
}

bool BidcosFrame::CheckAndAddResponse(const BidcosFrame& response)
{
	if(response.GetSenderAddress() == GetReceiverAddress()){
        if( response.MatchType( FT_ACK_W_AES_CHALLENGE ) )
        {
            // Fake having accepted this by returning true to avoid further processing
            LOG(Logger::LOG_DEBUG, "Response eaten: %s", BidcosFrameDecoder::ToString(&response).c_str());
            return true;
        }
        if(AddResponse(response)){
            LOG(Logger::LOG_DEBUG, "Response accepted: %s", BidcosFrameDecoder::ToString(&response).c_str());
        }
		return true;
	}
	return false;
}

 BidcosFrame::unreach_reason_t BidcosFrame::GetUnreachReason() {
	return this->unreach_reason;
}
void BidcosFrame::SetUnreachReason(BidcosFrame::unreach_reason_t reason)
{
	this->unreach_reason = reason;
}
bool BidcosFrame::AddResponse(const BidcosFrame& response)
{
    //if(vec_responses.size() && (*vec_responses.back()) == response)return false;
	vec_responses.push_back(new BidcosFrame(response));
	return true;
}

void BidcosFrame::ClearResponses()
{
	for(t_vec_responses::iterator it=vec_responses.begin();it!=vec_responses.end();it++)delete *it;
	vec_responses.clear();
}

unsigned int BidcosFrame::GetResponseCount() const
{
	return vec_responses.size();
}

BidcosFrame* BidcosFrame::GetResponse(unsigned int index/*=0*/)
{
	if(index >= vec_responses.size())return NULL;
	return vec_responses[index];
}

bool BidcosFrame::CheckReceiveComplete(uint64_t* wait_time_ms)
{
    if(! (GetCtrl() & CTRL_BIDI))return true;
    if(GetResponse(0))return true;

    if(wait_time_ms)
    {
    	if( (*wait_time_ms) < 1000) {
    		*wait_time_ms = 1000;
    	}
        uint64_t next_expected_receive_time=GetTimestamp()+(*wait_time_ms);//was 1000
        uint64_t now=time_millis();
        if(next_expected_receive_time>now)*wait_time_ms=next_expected_receive_time-now;
        else *wait_time_ms=0;
    }
    return false;
}

bool BidcosFrame::WasNacked() const {
	if(vec_responses.size() > 0) {
		BidcosFrame* pResponseFrame = vec_responses.at((vec_responses.size()-1));
		return pResponseFrame->IsNack();
	}
	return false;
}

bool BidcosFrame::HasTimestamp() const
{
	return timestamp!=0;
}

uint64_t BidcosFrame::GetTimestamp() const
{
	return timestamp;
}

void BidcosFrame::SetTimestamp(uint64_t timestamp)
{
	if(!timestamp)timestamp++;
	this->timestamp=timestamp;
}

bool BidcosFrame::DeviceWokenup() const
{
	return device_wokenup;
}

void BidcosFrame::SetDeviceWokenup(bool dwu)
{
	device_wokenup=dwu;
}

void BidcosFrame::ResetAuthKey()
{
	auth_key=INVALID_AUTH_KEY;
}

bool BidcosFrame::IsValid() const
{
	return GetSize() >= 9;
}

void BidcosFrame::SetInterfaceId(const std::string& s)
{
	interface_id=s;
}

const std::string& BidcosFrame::GetInterfaceId() const
{
	return interface_id;
}

void BidcosFrame::SetRSSI(int rssi)
{
	this->rssi=rssi;
}

int BidcosFrame::GetRSSI() const
{
	return rssi;
}

bool BidcosFrame::IsPreliminary() const
{
    return preliminary;
}

void BidcosFrame::SetPreliminary(bool prelim)
{
    preliminary=prelim;
}

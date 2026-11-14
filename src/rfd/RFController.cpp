/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// RFController.cpp: Implementierung der Klasse RFController.
//
//////////////////////////////////////////////////////////////////////

#include "RFController.h"
#include "RFManager.h"
#include "CommController.h"
#include "CommMessage.h"
#include "CommMessageQueue.h"
#include <Logger.h>
#include <utils.h>
#include "RFCommMessageDecoder.h"
#include "RFCommMessageAESKey.h"
#include "RFCentral.h"
#ifndef WIN32
#include <UnixSerialPortWrapper.h>
#endif
#include <SocketPortWrapper.h>
#include <OSCompat.h>

/*static*/ const char* RFController::CCU_RFD_DEVICE="/dev/hss_rf";

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

RFController::RFController():CommController()
{
	pthread_cond_init(&cond_send_inhibit, NULL);
	pthread_mutex_init(&mutex_send_inhibit, NULL);
	pthread_mutex_init(&mutex_wakeup_devices, NULL);
	bidcos_address=0;
	send_inhibit_flags=0;
	send_inhibit_cnt_rx=0;
	send_inhibit_cnt_tx=0;
	send_inhibit_timestamp=0;
}

RFController::~RFController()
{
	pthread_cond_destroy(&cond_send_inhibit);
	pthread_mutex_destroy(&mutex_send_inhibit);
	pthread_mutex_destroy(&mutex_wakeup_devices);
}

bool RFController::SendFrame(BidcosFrame* frame)
{
	RFCommMessage msg;
	msg.SetDontDelete(true);
	msg.SetCommand(RFCommMessage::CMD_SEND);
	msg.SetPayload(*frame);

	TxQueueAddMessage(&msg);

	bool success=msg.WaitUntilSent();

	// Check for the auth key even if the message was not sent successfully.
	frame->SetAuthKey(msg.GetAuthKey());

	if(!success)return false;

	unsigned int i=0;
	RFCommMessage* response;
	do{
		response=msg.GetResponse(i);
		if(response){
			BidcosFrame response_payload=response->ExtractPayload();
			if(response_payload.IsValid()) 
			{
				frame->CheckAndAddResponse(response_payload);
				RFManager::GetSingleton()->ProcessIncomingFrame(response_payload);
			}
		}
		i++;
	}while(response);
	return true;
}

bool RFController::CheckBeforeSend(CommMessage* msg)
{
	RFCommMessage* rf_msg=(RFCommMessage*)msg;
//	LOG(Logger::LOG_DEBUG, "CheckBeforeSend() cmd=0x%02x", msg->GetCommand());
	if(msg->GetCommand()==RFCommMessage::CMD_SEND){

		//wait for the sender to become ready without violating the RF timing
		pthread_mutex_lock(&mutex_send_inhibit);
		uint64_t timeout=(send_inhibit_timestamp+SEND_INHIBIT_TIMEOUT)-time_millis();
		while(send_inhibit_flags && (timeout>0)){
			struct timespec abs_timeout=millis2abstime(timeout);
			pthread_cond_timedwait(&cond_send_inhibit, &mutex_send_inhibit, &abs_timeout);
			timeout=(send_inhibit_timestamp+SEND_INHIBIT_TIMEOUT)-time_millis();
		}
		if(send_inhibit_flags){
			LOG(Logger::LOG_DEBUG, "send_inhibit timeout 0x%02X", send_inhibit_flags);
		}
		//implicitely reset flags if timeout occured
		send_inhibit_flags=INHIBIT_TX;
		send_inhibit_timestamp=time_millis();
		send_inhibit_cnt_tx=rf_msg->GetID();
		//LOG(Logger::LOG_DEBUG, "send_inhibit_flags=0x%X", send_inhibit_flags);
		pthread_mutex_unlock(&mutex_send_inhibit);
	}
	LOG(Logger::LOG_DEBUG, "TX: %s", RFCommMessageDecoder::ToString(rf_msg).c_str());
	return true;
}

bool RFController::NeedsAES(RFCommMessage* msg, int* key_index)
{
	if((msg->GetReceiverAddress()!=GetAddress()) || !(msg->GetCtrl() & BidcosFrame::CTRL_BIDI))return false;
	uint64 aes_channels;
	if(!GetDeviceAesPolicy(msg->GetSenderAddress(), key_index, &aes_channels))return false;
	if(!aes_channels)return false;

	switch(msg->GetType()){
		case BidcosFrame::FT_SWITCH:
		case BidcosFrame::FT_CONDITIONAL_SWITCH:
		case BidcosFrame::FT_LEVEL:
			{
				int channel=msg->ExtractPayload().GetIntValue(BidcosFrame::FIELD_SWITCH_CHANNEL);
				return (aes_channels & (uint64(1)<<channel))!=0;
			}
		case BidcosFrame::FT_INFO:
			{
				BidcosFrame f=msg->ExtractPayload();
				if(f.MatchType(BidcosFrame::FT_INFO_STATUS)){
					int channel=msg->ExtractPayload().GetIntValue(BidcosFrame::FIELD_INFO_STATUS_CH);
					return ((channel==0) && (aes_channels!=0)) || ( (aes_channels & (uint64(1)<<channel)) != 0 );
				}else{
					return true;
				}
			}
			break;
	}
	return false;
}

bool RFController::CheckAfterReceive(CommMessage* msg)
{
	int override_flags=0;
	int override_key_index=0;
//    uint64_t rx_timestamp=time_millis();

    RFCommMessage* rf_msg=(RFCommMessage*)msg;
	switch(rf_msg->GetCommand()){
		case RFCommMessage::CMD_EVENT:
		case RFCommMessage::CMD_RESPONSE:
			if(NeedsAES(rf_msg, &override_key_index)){
				pthread_mutex_lock(&mutex_send_inhibit);
				send_inhibit_flags|=INHIBIT_AES;
				send_inhibit_timestamp=time_millis();
				pthread_mutex_unlock(&mutex_send_inhibit);
				override_flags |= RFCommMessage::OVERRIDE_AES;
                rf_msg->AddFlags(RFCommMessage::FLAG_PRELIMINARY);
			}
		//no break here
		case RFCommMessage::CMD_AES_EVENT:
			{
				pthread_mutex_lock(&mutex_wakeup_devices);
				t_map_wakeup_devices::iterator it=map_wakeup_devices.find(rf_msg->GetSenderAddress());
				int wakeup_counter=-1;
                int ack_means_wakeup_telegram_counter=-1;
				bool wakeup=false;
				if(it != map_wakeup_devices.end()){
					ack_means_wakeup_telegram_counter=it->second.telegram_counter;
					wakeup_counter=it->second.counter;
					wakeup=true;
				}
				pthread_mutex_unlock(&mutex_wakeup_devices);
				if((rf_msg->GetCtrl() & BidcosFrame::CTRL_WAKEMEUP)){
					if(wakeup){
						int receiver=rf_msg->GetReceiverAddress();
						if(receiver==GetAddress() && (rf_msg->GetCtrl()&BidcosFrame::CTRL_BIDI)){
							// Bidirectional unicast message to us. Send ACK with wakeup
							override_flags |= RFCommMessage::OVERRIDE_WAKEUP;
                            rf_msg->AddFlags(RFCommMessage::FLAG_WOKENUP);
						}else if(!(rf_msg->GetCtrl()&BidcosFrame::CTRL_BIDI)){
							// Unidirectional message. Send a wakeup request with a probability of 0.5
							if((wakeup_counter<2)||rand()&0x01){
								BidcosFrame payload;
								int address=rf_msg->GetSenderAddress();
								payload.SetReceiverAddress(address);
								payload.SetSenderAddress(GetAddress());
								payload.SetType(BidcosFrame::FT_WAKEUP);
								payload.SetCtrl(BidcosFrame::CTRL_BIDI|BidcosFrame::CTRL_RPT_ENABLE|BidcosFrame::CTRL_WAKEUP);
								payload.SetTelegramCounter(rf_msg->GetTelegramCounter());

								RFCommMessage instant_response;
								instant_response.SetCommand(RFCommMessage::CMD_SEND_AT);
								instant_response.SetPayload(payload);
								instant_response.SetIntValue(RFCommMessage::FIELD_TIMESTAMP, msg->GetIntValue(RFCommMessage::FIELD_TIMESTAMP) + 128);

								std::string raw_data=instant_response.PrepareRawData();
								Send(raw_data);
								pthread_mutex_lock(&mutex_wakeup_devices);
								WakeupDeviceData& wdd=map_wakeup_devices[address];
								wdd.counter++;
								wdd.telegram_counter=rf_msg->GetTelegramCounter();
								pthread_mutex_unlock(&mutex_wakeup_devices);
								LOG(Logger::LOG_DEBUG, "Wakeup %d requested: %s", wakeup_counter+1, RFCommMessageDecoder::ToString(&instant_response).c_str());
							}else{
								LOG(Logger::LOG_DEBUG, "Not requesting wakeup for 0x%06X this time", rf_msg->GetSenderAddress());
							}
						}
					}
				} 
				if(override_flags){
					CommMessage instant_response;
					instant_response.SetCommand(RFCommMessage::CMD_OVERRIDE);
					instant_response.SetByteData(RFCommMessage::OVERRIDE_FIELD_TYPE, override_flags);
					instant_response.SetByteData(RFCommMessage::OVERRIDE_FIELD_ID, rf_msg->GetID());
					instant_response.SetByteData(RFCommMessage::OVERRIDE_FIELD_KEY, override_key_index);
					std::string raw_data=instant_response.PrepareRawData();
					Send(raw_data);
                    map_aes_rssi[rf_msg->GetSenderAddress()]=rf_msg->GetIntValue(RFCommMessage::FIELD_RSSI);
					const char* OVERRIDE_TEXT[]={ "", "AES", "Wakeup", "AES and Wakeup" };
					LOG(Logger::LOG_DEBUG, "%s requested for 0x%06X with id %02X and key %d", OVERRIDE_TEXT[override_flags&0x03], rf_msg->GetSenderAddress(), rf_msg->GetID(), override_key_index);
				}
				if(rf_msg->GetReceiverAddress()==GetAddress() && (rf_msg->GetCtrl() & BidcosFrame::CTRL_BIDI)){
					pthread_mutex_lock(&mutex_send_inhibit);
					send_inhibit_flags|=INHIBIT_RX;
					send_inhibit_cnt_rx=rf_msg->GetID();
					send_inhibit_timestamp=time_millis();
					//LOG(Logger::LOG_DEBUG, "send_inhibit_flags=0x%X(RX)", send_inhibit_flags);
					pthread_mutex_unlock(&mutex_send_inhibit);
				};

				pthread_mutex_lock(&mutex_send_inhibit);
				if(rf_msg->GetCommand()==RFCommMessage::CMD_AES_EVENT){
                	uint64 aes_channels;
                    int key_index;
                    if(GetDeviceAesPolicy(rf_msg->GetSenderAddress(), &key_index, &aes_channels)){
                        rf_msg->SetAuthKey(key_index);
                    }
                    int rssi=map_aes_rssi[rf_msg->GetSenderAddress()];
                    if(rssi)rf_msg->SetRSSI(rssi);
                    map_aes_rssi.erase(rf_msg->GetSenderAddress());
					send_inhibit_flags &= ~INHIBIT_AES;
					pthread_cond_signal(&cond_send_inhibit);
				}
				pthread_mutex_unlock(&mutex_send_inhibit);

				//Check if the sending device has been woken up
				if( (rf_msg->GetTelegramCounter() == ack_means_wakeup_telegram_counter) && (rf_msg->GetType()==BidcosFrame::FT_ACK_OR_NACK) ){
                    rf_msg->AddFlags(RFCommMessage::FLAG_WOKENUP);
    				pthread_mutex_lock(&mutex_wakeup_devices);
	       			t_map_wakeup_devices::iterator it=map_wakeup_devices.find(rf_msg->GetSenderAddress());
       				if(it != map_wakeup_devices.end()){
	        			it->second.telegram_counter=-1;
    				}
	    			pthread_mutex_unlock(&mutex_wakeup_devices);
				}
				return true;
			}
		case RFCommMessage::CMD_INFO:
			switch(rf_msg->GetByteData(0x00)){
				case RFCommMessage::INFO_TYPE_DUTY_CYCLE:
					if(rf_msg->GetByteData(1)){
						LOG(Logger::LOG_DEBUG, "RX: %s", RFCommMessageDecoder::ToString(rf_msg).c_str());
					}
					return true;
				break;
				case RFCommMessage::INFO_TYPE_TX_COMPLETE:
					//fall through to CMD_ERROR branch
				break;
				case RFCommMessage::INFO_TYPE_RX_AUTH_REQUEST:
				break;
			}
			// no break
		case RFCommMessage::CMD_ERROR:
			LOG(Logger::LOG_DEBUG, "RX: %s", RFCommMessageDecoder::ToString(rf_msg).c_str());
			int counter=rf_msg->GetID();
			pthread_mutex_lock(&mutex_send_inhibit);
			if(counter==send_inhibit_cnt_rx)send_inhibit_flags &= ~INHIBIT_RX;
			if(counter==send_inhibit_cnt_tx)send_inhibit_flags &= ~INHIBIT_TX;
			pthread_cond_signal(&cond_send_inhibit);
//			LOG(Logger::LOG_DEBUG, "send_inhibit_flags=0x%X(INFO/ERROR) [%d]", send_inhibit_flags, counter);
			pthread_mutex_unlock(&mutex_send_inhibit);

			pthread_mutex_lock(&mutex_tx_state);
			int cur_receiver_address=-1;
			if(cur_tx_message)cur_receiver_address=((RFCommMessage*)cur_tx_message)->GetReceiverAddress();
			pthread_mutex_unlock(&mutex_tx_state);

			if((rf_msg->GetByteData(0x00)==RFCommMessage::ERROR_UNKNOWN_AES_KEY) && (cur_receiver_address>=0)){
				LOG(Logger::LOG_ERROR, "0x%06X requested us to authenticate with an unknown key index", cur_receiver_address);
				//ToDo: Maybe the application needs this information for peering with new devices
			}
			return true;
		break;
	}
	LOG(Logger::LOG_ERROR, "Check AfterReceive() failed");
	return false;
}

bool RFController::SendOwnAddress()
{
	RFCommMessage msg;
	msg.SetCommand(RFCommMessage::CMD_ADDRESS);
	msg.SetResponseTimeout(0);
	msg.SetReceiveMasks(0, 0, 0);
	msg.SetIntValue(0, 0, 3, 0, GetAddress());
	msg.SetDontDelete(true);
	TxQueueAddMessage(&msg);
	msg.WaitUntilSent();
	return true;

}

bool RFController::AddDeviceWakeupRequest(int address,bool lazyConfig)
{
	if(!BidcosInterface::AddDeviceWakeupRequest(address, lazyConfig))return false;
	pthread_mutex_lock(&mutex_wakeup_devices);
	map_wakeup_devices[address];
	pthread_mutex_unlock(&mutex_wakeup_devices);
	return true;
}

bool RFController::RemoveDeviceWakeupRequest(int address)
{
	if(!BidcosInterface::RemoveDeviceWakeupRequest(address))return false;
	pthread_mutex_lock(&mutex_wakeup_devices);
	map_wakeup_devices.erase(address);
	pthread_mutex_unlock(&mutex_wakeup_devices);
	return true;
}

bool RFController::ChangeAESKey(AesKeyType type, int index, const std::string &key)
{
	RFCommMessageAESKey msg;
	msg.SetCommand(RFCommMessage::CMD_SET_KEY);
	msg.SetIntValue(0, 0, 1, 0, (int)type);
	msg.SetIntValue(1, 0, 1, 0, index);
	msg.SetStringValue(2, 16, key);
	msg.SetDontDelete(true);
	TxQueueAddMessage(&msg);
	bool success=msg.WaitUntilSent();
	if(!success)return false;

	RFCommMessage* response=msg.GetResponse();
	if(!response)return false;
	return true;
}

bool RFController::SetAesKeyTemp(int index, const std::string& data)
{
    if(index==0)index=-1;
	return ChangeAESKey(AES_KEY_TYPE_TEMP, index, data);
}

bool RFController::SetAesKeyUser(int index, const std::string& data, int last_index, const std::string& last_data)
{
    int default_index;
    int current_index;
    int previous_index;
    int temp_index;
    if(!GetAesKeyIndexes(&default_index, &current_index, &previous_index, &temp_index))return false;
    if( current_index == index && previous_index == last_index )
    {
        return true;
    }
	if(!ChangeAESKey(AES_KEY_TYPE_CURRENT_USER, index, data))return false;
    if(!GetAesKeyIndexes(&default_index, &current_index, &previous_index, &temp_index))return false;
    if( current_index != index || previous_index != last_index )
    {
        LOG(Logger::LOG_ERROR, "RFController::SetAesKeyUser(): Key index mismatch, got %d/%d, expected %d/%d", current_index, previous_index, index, last_index);
        return false;
    }
    return true;
}

bool RFController::GetAesKeyIndexes(int* default_key, int*current_key, int* previous_key, int* temp_key)
{
	RFCommMessageAESKey msg;
	msg.SetCommand(RFCommMessage::CMD_GET_KEYS);
	msg.SetDontDelete(true);
	TxQueueAddMessage(&msg);
	bool success=msg.WaitUntilSent();
	if(!success)return false;
	RFCommMessage* response=msg.GetResponse();
	if(!response)return false;
	*default_key=response->GetByteData(RFCommMessage::INFO_FIELD_DATA);
	*current_key=response->GetByteData(RFCommMessage::INFO_FIELD_DATA+1);
	*previous_key=response->GetByteData(RFCommMessage::INFO_FIELD_DATA+2);
	*temp_key=response->GetByteData(RFCommMessage::INFO_FIELD_DATA+3);
	return true;
}

void RFController::ProcessReceivedMessage(CommMessage* p)
{
	RFCommMessage* rf_msg=dynamic_cast<RFCommMessage*>(p);
	if(!rf_msg){
		delete p;
		return;
	}
	BidcosFrame payload=rf_msg->ExtractPayload();
	payload.SetInterfaceId(GetSerialNumber());

	if(payload.IsValid() && GetConcentrator())GetConcentrator()->ProcessReceivedFrame(payload);
	delete p;
}

int RFController::GetAddress()
{
	return bidcos_address;
}

bool RFController::StartInterface(int bidcos_address)
{
	this->bidcos_address=bidcos_address;
	if(!IsCommunicationStarted())Start();
	if(!IsCommunicationStarted())return false;
    return SendOwnAddress();

}

bool RFController::StopInterface()
{
	Stop();
	return !IsCommunicationStarted();
}

bool RFController::Init(std::map<std::string, std::string>& params)
{
    PropertyMap global_config = RFManager::GetSingleton()->GetConfigPropertyMap();
    std::string saved_section=global_config.GetCurrentSection();
    global_config.SetCurrentSection("");
    PropertyMap ids;
    ids.ReadFromFile(OSCompat::FixPath(global_config.GetStringValue("Address File")));
	params["Serial Number"]=ids.GetStringValue("SerialNumber", "CCU");
    global_config.SetCurrentSection(saved_section);

    if(!BidcosInterface::Init(params))return false;
    std::string& port_type=params["Port Type"];
    if(port_type=="TCP") {
        std::string& host=params["Host"];
        if(host.empty())
        {
            LOG(Logger::LOG_ERROR, "Port type \"TCP\" needs parameter \"Host\"");
            return false;
        }
        int port=strtol(params["Port"].c_str(), NULL, 0);
        if(port<=0)
        {
            LOG(Logger::LOG_ERROR, "Port type \"TCP\" needs positive integer parameter \"Port\"");
            return false;
        }
        SocketPortWrapper* pw=new SocketPortWrapper();
		if(pw->Open(host.c_str(), port)<0){
            LOG(Logger::LOG_ERROR, "Unable to open %s:%d", host.c_str(), port);
			delete pw;
			return false;
		}
        SetPortWrapper(pw);
#ifndef WIN32
    } else if(port_type=="CCU" || port_type.empty() ) {
    	UnixSerialPortWrapper* pw=new UnixSerialPortWrapper();
		if(pw->Open(CCU_RFD_DEVICE)<0){
            LOG(Logger::LOG_ERROR, "Unable to open line port %s", CCU_RFD_DEVICE);
			delete pw;
			return false;
		}
        SetPortWrapper(pw);
#endif
    } else {
        LOG(Logger::LOG_ERROR, "RFController::Init(): Port type %s not supported", port_type.c_str());
    }
   	if(!IsCommunicationStarted()){
        LOG(Logger::LOG_ERROR, "RFController::Init(): communication failure");
	    return false;
	}
    return true;
}

bool RFController::IsConnected()
{
    return true;
}

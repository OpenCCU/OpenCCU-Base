// HS485Controller.cpp: Implementierung der Klasse HS485Controller.
//
//////////////////////////////////////////////////////////////////////

#include "HS485ControllerCCU1.h"
#include "HS485Manager.h"
//#include "CommController.h"
#include "HS485CommMessageCCU1.h"
#include "HS485CommMessageDecoder.h"
#include "HS485Central.h"
#include <Logger.h>
#include <stdio.h>

const int HS485_RESPONSE_TIMEOUT = 200;

//HS485ControllerCCU1* HS485ControllerCCU1::singleton=NULL;
//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HS485ControllerCCU1::HS485ControllerCCU1()
:HS485Controller()
{
}

HS485ControllerCCU1::~HS485ControllerCCU1()
{
	pthread_mutex_destroy(&mutex_address_info);
//    if(singleton==this)singleton=NULL;
}


HS485ControllerCCU1* HS485ControllerCCU1::CreateSingletonInstance()
{
	if(singleton == NULL) {
		singleton = new HS485ControllerCCU1();
	}
	return (HS485ControllerCCU1*)singleton;
}

CommMessage* HS485ControllerCCU1::NewMessage()
{
	return (CommMessage*)new HS485CommMessageCCU1();
}

HS485CommMessage* HS485ControllerCCU1::CreateNewMessage()
{
	return (HS485CommMessage*)HS485ControllerCCU1::NewMessage();
}

int HS485ControllerCCU1::Discovery(std::vector<uint32_t>* devices)
{
	int retval=-1;
	BroadcastSleepMode(true);

	HS485CommMessage* pMsg = HS485ControllerCCU1::CreateNewMessage();
	pMsg->SetReceiveMasks(0x18, 0x10, 0x00);
    pMsg->SetCommand(0x01);
    pMsg->SetDontDelete(true);
    pMsg->SetResponseTimeout(10000);
    //Maximum number of devices to return
    pMsg->SetUShortData(0,256);
	TxQueueAddMessage(pMsg);
    if(!pMsg->WaitUntilSent(NULL)){
      LOG(Logger::LOG_ERROR, "discovery msg send failure");
	}else{
        int i=0;
	    CommMessage* response=pMsg->GetResponse(i);
        while(response){
            if(response->GetCommand()==0x83){
                devices->push_back(response->GetULongData(0));
				//LOG(Logger::LOG_DEBUG, "Device found: %08lX", devices->back());
            }else if(response->GetCommand()==0x84){
                //int error=response->GetByteData(0);
                int count=response->GetUShortData(1);
                retval=count;
				break;
            }
            i++;
    	    response=pMsg->GetResponse(i);
        }
		retval=0;
	}
	BroadcastSleepMode(false);
	delete pMsg;
	return retval;
}

bool HS485ControllerCCU1::SendMessage(HS485CommMessage* msg)
{
	uint32_t address=msg->GetReceiverAddress();
	msg->SetSenderAddress(GetAddress());
	if(address!=0xffffffff){
		msg->SetTimeout(HS485_RESPONSE_TIMEOUT);//timeout 10ms
	}else{
		msg->SetTimeout(0);//no response expected
		msg->SetResponseTimeout(0);
	}
	bool wait=msg->GetDontDelete();
	TxQueueAddMessage(msg);
	if(wait){
		if(!msg->WaitUntilSent(NULL)){
            LOG(Logger::LOG_DEBUG, "SendMessage(%08lX, %s) failed", address, HS485CommMessageDecoder::ToString(msg).c_str());
			return false;
	    }
//		LOG(Logger::LOG_DEBUG, "SendMessage(%08lX, %s) OK", address, msg->DumpToString().c_str());
	}
	return true;
}

bool HS485ControllerCCU1::SendMessage(uint32_t address, const std::string &msg, std::string *response)
{
	bool retval=true;
    HS485Frame frame;
    frame.SetSenderAddress(HS485Central::GetSingleton()->GetAddress());
    frame.SetReceiverAddress(address);
    frame.SetCtrl(HS485Frame::CTRL_IFRAME);
    frame.SetPayload(msg);
	HS485CommMessage* cmsg= HS485ControllerCCU1::CreateNewMessage();
	cmsg->SetDontDelete(true);
    cmsg->SetFrame(frame);

	if(!SendMessage(cmsg)){
		retval=false;
	}else{
		HS485CommMessage* responseMsg=(HS485CommMessage*)cmsg->GetResponse();
		if(responseMsg && response){
			*response=responseMsg->ExtractFrame().GetPayload();
		}
	}
	delete cmsg;
	return retval;
}

bool HS485ControllerCCU1::SendBootloaderMessage(uint32_t address, const std::string &msg, std::string *response)
{
	bool retval=true;
    HS485Frame frame;
    frame.SetReceiverAddress(address);
    frame.SetCtrl(HS485Frame::CTRL_BOOT_IFRAME);
    frame.SetPayload(msg);

	HS485CommMessage* cmsg= CreateNewMessage();
	cmsg->SetDontDelete(true);
	cmsg->SetCommand(HS485CommMessage::CMD_BOOT_SEND);
	if(address!=0xffffffff){
		cmsg->SetReceiveMasks(5, 5, 2);
		cmsg->SetDontDelete(true);
        cmsg->SetTimeout(HS485_RESPONSE_TIMEOUT);//timeout 10ms
	}else{
        cmsg->SetTimeout(0);//no response expected
		cmsg->SetResponseTimeout(0);
	}
	cmsg->SetFrame(frame);

	if(!SendMessage(cmsg)){
		retval=false;
	}else{
		HS485CommMessage* responseMsg=cmsg->GetResponse();
		if(responseMsg && response){
			*response=responseMsg->ExtractFrame().GetPayload();
		}
	}
	delete cmsg;
	return retval;
}

void HS485ControllerCCU1::UpdateControlChar(uint32_t receiver, unsigned char* cc)
{
//	LOG(Logger::LOG_DEBUG, "UpdateControlChar %08lX", receiver);
	HS485Controller::UpdateControlChar(receiver, cc);
}

bool HS485ControllerCCU1::CheckRxCounter(uint32_t sender, unsigned char cc)
{
	return HS485Controller::CheckRxCounter(sender, cc);
}

std::string HS485ControllerCCU1::GetDeviceDescription(uint32_t address)
{
	return HS485Controller::GetDeviceDescription(address);
}
/*
HS485ControllerCCU1* HS485ControllerCCU1::GetSingleton()
{
    return singleton;
}
*/
bool HS485ControllerCCU1::CheckBeforeSend(CommMessage* msg)
{
	HS485CommMessage* cmsg=(HS485CommMessage*)msg;
//	LOG(Logger::LOG_DEBUG, "CheckBeforeSend() cmd=0x%02x", msg->GetCommand());
	switch(cmsg->GetCommand()){
		case HS485CommMessage::CMD_SEND:
		case HS485CommMessage::CMD_BOOT_SEND:
			uint32_t address=cmsg->GetReceiverAddress();
			unsigned char cc=cmsg->GetCtrl();
			UpdateControlChar(address, &cc);
			cmsg->SetCtrl(cc);
		break;
	}
	LOG(Logger::LOG_DEBUG, "TX: %s", HS485CommMessageDecoder::ToString(cmsg).c_str());
	return true;
}

bool HS485ControllerCCU1::CheckAfterReceive(CommMessage* msg)
{
	HS485CommMessage* cmsg=(HS485CommMessage*)msg;
	LOG(Logger::LOG_DEBUG, "RX: %s", HS485CommMessageDecoder::ToString(cmsg).c_str());
	switch(cmsg->GetCommand()){
		case HS485CommMessage::CMD_DISC_RESULT:
		case HS485CommMessage::CMD_DISC_END:
		case HS485CommMessage::CMD_ERROR:
			return true;
		case HS485CommMessage::CMD_RESPONSE:
		{
			pthread_mutex_lock(&mutex_tx_state);
			if(!cur_tx_message){
				pthread_mutex_unlock(&mutex_tx_state);
				break;
			}
			int tx_cmd=cur_tx_message->GetCommand();
			if(	( tx_cmd==HS485CommMessage::CMD_SEND ||	tx_cmd==HS485CommMessage::CMD_BOOT_SEND ) && cur_tx_message->GetID() == msg->GetID()){
				uint32_t address=((HS485CommMessage*)cur_tx_message)->GetReceiverAddress();
				pthread_mutex_unlock(&mutex_tx_state);
				return CheckRxCounter(address, cmsg->GetCtrl());
			}else{
				pthread_mutex_unlock(&mutex_tx_state);
                return false;
			}
		}
		case HS485CommMessage::CMD_EVENT:
		{
			uint32_t receiver=cmsg->GetReceiverAddress();
			uint32_t sender=cmsg->GetSenderAddress();
			unsigned char cc=cmsg->GetCtrl();
			//don't process discovery frames any further
			if((cc&0x07)==0x03)return false;
			return receiver==0xffffffff || CheckRxCounter(sender, cc);
		}
		break;
	}
	return false;
}

void HS485ControllerCCU1::ClearAddressInfo(uint32_t address/*=0xffffffff*/)
{
	pthread_mutex_lock(&mutex_address_info);
	if(address==0xffffffff){
		map_address_info.clear();
	}else{
		map_address_info.erase(address);
	}
	pthread_mutex_unlock(&mutex_address_info);
}


void HS485ControllerCCU1::ProcessReceivedMessage(CommMessage* msg)
{
    /*rxq.AddMessage(msg);
	//tell the XmlRpc main loop that we now have some input
	#ifdef WIN32
		send(pipe_fds[1], "", 1, 0);
	#else
		write(pipe_fds[1], "", 1);
	#endif*/
	HS485Controller::ProcessReceivedMessage(msg);
}

unsigned HS485ControllerCCU1::handleEvent(unsigned eventType)
{
	return HS485Controller::handleEvent(eventType);
}



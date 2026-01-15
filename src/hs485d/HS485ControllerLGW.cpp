/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <HS485ControllerLGW.h>
#include <HS485Controller.h>
#include <HS485CommMessageLGW.h>
#include <Logger.h>
#include <HS485Central.h>
#include <HMWLGWTransportFrame.h>
#include <HMWLGWCommand.h>
#include <HMWLGWUtils.h>
#include <LGWPortWrapper.h>
//#include <HS485CommMessageDecoder.h>

#define HS485_RESPONSE_TIMEOUT 700
#define HS485_LGW_TIMEOUT 200

//-------------------------------------------------------------------------------------------------------------

HS485ControllerLGW::IncomingHMWLGWData::IncomingHMWLGWData()
: pHMWLGWTransportFrame(NULL)
{
}

//-------------------------------------------------------------------------------------------------------------

HS485ControllerLGW::IncomingHMWLGWData::~IncomingHMWLGWData()
{
	if(pHMWLGWTransportFrame != NULL)
	{
		delete pHMWLGWTransportFrame;
		pHMWLGWTransportFrame = NULL;
	}
}

//-------------------------------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------------------------------

HS485ControllerLGW::HS485ControllerLGW()
: HS485Controller()
, keepaliveMsgThread(0)
, keepaliveMsgThreadEnabled(false)
{
	pthread_attr_t threadAttr;
	pthread_attr_init(&threadAttr);
	pthread_attr_setstacksize(&threadAttr, 512*1024);
	keepaliveMsgThreadEnabled = true;
	pthread_create(&keepaliveMsgThread, &threadAttr, HS485ControllerLGW::keepAliveMsgThreadFunction, (void*)this);
	pthread_attr_destroy(&threadAttr);
}

//-------------------------------------------------------------------------------------------------------------

HS485ControllerLGW::~HS485ControllerLGW()
{
	if(keepaliveMsgThread != 0) {
		pthread_cancel( keepaliveMsgThread );
		void* foo;
		pthread_join(keepaliveMsgThread, &foo);
	}
}

//-------------------------------------------------------------------------------------------------------------

HS485ControllerLGW* HS485ControllerLGW::CreateSingletonInstance()
{
	if(singleton == NULL) {
		singleton = new HS485ControllerLGW();
	}
	return (HS485ControllerLGW*)singleton;
}

//-------------------------------------------------------------------------------------------------------------

CommMessage* HS485ControllerLGW::NewMessage()
{
	return new HS485CommMessageLGW();
}

//-------------------------------------------------------------------------------------------------------------

HS485CommMessage* HS485ControllerLGW::CreateNewMessage()
{
	return (HS485CommMessage*)HS485ControllerLGW::NewMessage();
}

//-------------------------------------------------------------------------------------------------------------

/*
int HS485ControllerLGW::Start()
{
	LOG(Logger::LOG_DEBUG, "HS485ControllerLGW::Start()");
	int retVal = CommController::Start();
	if(retVal == 1) {
		pthread_attr_t threadAttr;
		pthread_attr_init(&threadAttr);
		pthread_attr_setstacksize(&threadAttr, 512*1024);
		keepaliveMsgThreadEnabled = true;
		pthread_create(&keepaliveMsgThread, &threadAttr, HS485ControllerLGW::keepAliveMsgThreadFunction, (void*)this);
		pthread_attr_destroy(&threadAttr);
	}
	return retVal;
}

int HS485ControllerLGW::Stop()
{
	LOG(Logger::LOG_DEBUG, "HS485ControllerLGW::Stop()");
	int retVal = CommController::Stop();
	keepaliveMsgThreadEnabled = false;
	if(retVal == 1) {
		if(keepaliveMsgThread != NULL) {
			pthread_cancel( keepaliveMsgThread );
			void* foo;
			pthread_join(keepaliveMsgThread, &foo);
		}
	}
	return retVal;
}
*/

//-------------------------------------------------------------------------------------------------------------

void* HS485ControllerLGW::keepAliveMsgThreadFunction(void * pController)
{
	HS485ControllerLGW* pThis = (HS485ControllerLGW*)pController;
	const unsigned int usSleepTime = 20 * 1000 * 1000; //20 seconds
	HS485CommMessageLGW* pMsg = NULL;
	usleep(usSleepTime);
	while(pThis->keepaliveMsgThreadEnabled) {
		//LOG(Logger::LOG_DEBUG, "HS485ControllerLGW::keepAliveMsgThreadFunction(): Sending ");
		pMsg = (HS485CommMessageLGW*)pThis->CreateNewMessage();
		pMsg->SetDontDelete(true);
		pMsg->SetHMWLGWCommand(LGW_CMD_KEEPALIVE);
		pMsg->SetResponseTimeout(HS485_RESPONSE_TIMEOUT);//when nothing set default is 500ms
		pThis->TxQueueAddMessage(pMsg);
		if(!pMsg->WaitUntilSent(NULL)){
			LOG(Logger::LOG_ERROR, "HS485ControllerLGW::keepAliveMsgThreadFunction(): Did not get an answer");
			((LGWPortWrapper*)pThis->port_wrapper)->reconnect();
		}
		if(pMsg != NULL) {
			delete pMsg;
			pMsg = NULL;
		}
		usleep(usSleepTime);
	}
	return NULL;
}

//-------------------------------------------------------------------------------------------------------------

void HS485ControllerLGW::BroadcastSleepMode(bool on)
{
	//LOG(Logger::LOG_ALL, "HS485ControllerLGW::BroadcastSleepMode()");
	HS485Controller::BroadcastSleepMode(on);
}

//-------------------------------------------------------------------------------------------------------------

bool HS485ControllerLGW::CheckBeforeSend(CommMessage* msg)
{//Dummy

	//LOG(Logger::LOG_ALL, "HS485ControllerLGW::CheckBeforeSend() ");
	if(msg != NULL) {
		HS485CommMessageLGW* cmsg=(HS485CommMessageLGW*)msg;
		//LOG(Logger::LOG_DEBUG, "CheckBeforeSend() cmd=0x%02x", msg->GetCommand());
		if(LGW_CMD_SEND == cmsg->GetHMWLGWCommand()){
			uint32_t address=cmsg->GetReceiverAddress();
			unsigned char cc=cmsg->GetCtrl();
			UpdateControlChar(address, &cc);
			cmsg->SetCtrl(cc);
			return true;
		}
		else if(LGW_CMD_KEEPALIVE == cmsg->GetHMWLGWCommand()) {
			//nothing to do
			return true;
		}
		else {
			std::string xStr;
			xStr.append(1, (char)cmsg->GetHMWLGWCommand());
			LOG(Logger::LOG_DEBUG, "HS485ControllerLGW::CheckBeforeSend(): Command type not handled: %c (%s hex)",cmsg->GetHMWLGWCommand(), toDebugHexStr(xStr).c_str() );
			return false;
		}
	}
	else {
		LOG(Logger::LOG_FATAL_ERROR, "HS485ControllerLGW::CheckBeforeSend(): Message pointer is NULL");
		return false;
	}

}


bool HS485ControllerLGW::CheckAfterReceive(CommMessage* msg)
{
	//LOG(Logger::LOG_ALL, "HS485ControllerLGW::CheckAfterReceive()");
	HS485Controller::CheckAfterReceive(msg);
	return true;
}

void HS485ControllerLGW::ProcessReceivedMessage(CommMessage* msg)
{
//	LOG(Logger::LOG_ALL, "HS485ControllerLGW::ProcessReceivedMessage().");
	HS485Controller::ProcessReceivedMessage(msg);
}

std::string HS485ControllerLGW::GetDeviceDescription(uint32_t address)
{
//	LOG(Logger::LOG_ALL, "HS485ControllerLGW::GetDeviceDescription()");
	return HS485Controller::GetDeviceDescription(address);
}

bool HS485ControllerLGW::SendMessage(HS485CommMessage* msg)
{
	const unsigned int maxRetries = 2;//1 normal send + 2 retries 
	HS485CommMessageLGW* pMsg = dynamic_cast<HS485CommMessageLGW*>(msg);
//	LOG(Logger::LOG_ALL, "HS485ControllerLGW::SendMessage(msg)");
	if(pMsg != NULL) {
		for(unsigned int i = 0; i < maxRetries; i++) {
			pMsg->SetHMWLGWCommand(LGW_CMD_SEND);
			uint32_t address=pMsg->GetReceiverAddress();
			pMsg->SetSenderAddress(GetAddress());
			if(address!=0xffffffff){
				//LOG(Logger::LOG_ALL, "HS485ControllerLGW::SendMessage(msg): SetTimeouts %X", address);
				pMsg->SetTimeout(HS485_LGW_TIMEOUT);//timeout
				pMsg->SetResponseTimeout(HS485_RESPONSE_TIMEOUT);//when nothing set default is 500ms
			}else{
				//LOG(Logger::LOG_ALL, "HS485ControllerLGW::SendMessage(msg): SetTimeouts(0) %X", address);
				pMsg->SetTimeout(0);//no response expected
				pMsg->SetResponseTimeout(0);
			}
			bool wait=pMsg->GetDontDelete();
			TxQueueAddMessage(pMsg);
			if(wait){
				if(!pMsg->WaitUntilSent(NULL)){
					//LOG(Logger::LOG_DEBUG, "SendMessage(%08lX, %s) failed", address, HS485CommMessageDecoder::ToString(msg).c_str());
					LOG(Logger::LOG_DEBUG, "HS485ControllerLGW::SendMessage(msg): Failed");
					if(i < maxRetries) { 
						LOG(Logger::LOG_DEBUG, "HS485ControllerLGW::SendMessage(msg): Send failed...retrying (%d/%d)", i, maxRetries);
						continue; 
					}
					LOG(Logger::LOG_ERROR, "HS485ControllerLGW::SendMessage(msg): Send finally failed.");
					return false;
				}
				//LOG(Logger::LOG_DEBUG, "SendMessage(%08lX, %s) OK", address, msg->ToString().c_str());
			}
			return true;
		}//retries
	}
	else {
		LOG(Logger::LOG_ERROR, "HS485ControllerLGW::SendMessage(msg): Message pointer is NULL");
		return false;
	}
	return false;
}

bool HS485ControllerLGW::SendMessage(uint32_t receiver, const std::string& msg, std::string* response)
{
//	LOG(Logger::LOG_ALL, "HS485ControllerLGW::SendMessage(receiver,msg,response)");

	bool retval=true;
    HS485Frame frame;
    frame.SetSenderAddress(HS485Central::GetSingleton()->GetAddress());
    frame.SetReceiverAddress(receiver);
    frame.SetCtrl(HS485Frame::CTRL_IFRAME);
    frame.SetPayload(msg);
	HS485CommMessageLGW* pMsg = (HS485CommMessageLGW*)CreateNewMessage();
	//pMsg->SetHMWLGWCommand(LGW_CMD_SEND); //Will be done in SendMessage(msg)
	pMsg->SetReceiverAddress(receiver);
	pMsg->SetDontDelete(true);
	pMsg->SetFrame(frame);

	//Send and collect response
	if(!SendMessage(pMsg)){
		LOG(Logger::LOG_ERROR, "HS485ControllerLGW::SendMessage(): Error sending message.");
		retval=false;
	}else{
		HS485CommMessage* responseMsg=(HS485CommMessage*)pMsg->GetResponse();
		if((responseMsg != NULL) && (response != NULL)){
			*response=responseMsg->ExtractFrame().GetPayload();
			//LOG(Logger::LOG_ALL, "HS485ControllerLGW::SendMessage(): Response frame payload: %s", toDebugHexStr(*response).c_str());
		}
	}

	//cleanup
	delete pMsg;
	return retval;
}

bool HS485ControllerLGW::SendBootloaderMessage(uint32_t receiver, const std::string& msg, std::string* response)
{
//	LOG(Logger::LOG_ALL, "HS485ControllerLGW::SendBootloaderMessage() Test implementation using normal send.");
	bool retval=true;
    HS485Frame frame;
    frame.SetSenderAddress(HS485Central::GetSingleton()->GetAddress());//wird bei ccu1 weggelassen,
    frame.SetReceiverAddress(receiver);
    frame.SetCtrl(HS485Frame::CTRL_BOOT_IFRAME);
    frame.SetPayload(msg);
	HS485CommMessageLGW* pMsg = (HS485CommMessageLGW*)CreateNewMessage();
	//pMsg->SetHMWLGWCommand(LGW_CMD_SEND); //Will be done in SendMessage(msg)
	pMsg->SetReceiverAddress(receiver);
	pMsg->SetDontDelete(true);
	pMsg->SetFrame(frame);

	//Send and collect response
	if(!SendMessage(pMsg)){
		LOG(Logger::LOG_ERROR, "HS485ControllerLGW::SendMessage(): Error sending message.");
		retval=false;
	}else{
		HS485CommMessage* responseMsg=(HS485CommMessage*)pMsg->GetResponse();
		if((responseMsg != NULL) && (response != NULL)){
			*response=responseMsg->ExtractFrame().GetPayload();
//			LOG(Logger::LOG_ALL, "HS485ControllerLGW::SendMessage(): Response frame payload: %s", toDebugHexStr(*response).c_str());
		}
	}

	//cleanup
	delete pMsg;
	return retval;
}

int HS485ControllerLGW::Discovery(std::vector<uint32_t>* devices)
{
//	LOG(Logger::LOG_ALL, "HS485ControllerLGW::Discovery() Test implementation.");
	BroadcastSleepMode(true);

	bool done = true;
	HS485CommMessageLGW* pMsg = (HS485CommMessageLGW*)CreateNewMessage();
	pMsg->SetHMWLGWCommand(LGW_CMD_DISCOVERY);
	pMsg->SetDontDelete(true);
	pMsg->SetResponseTimeout(HS485_RESPONSE_TIMEOUT*3);
	//set cmd data (max no devices)
	std::string data;
	data.append(1, (char)0x00);
	data.append(1, (char)0xFF);
	pMsg->SetHMWLGWCommandData(data);

	//Send stuff
	TxQueueAddMessage(pMsg);
	if(!pMsg->WaitUntilSent(NULL)) {
		LOG(Logger::LOG_ERROR, "HS485ControllerLGW::Discovery(): Error sending discovery (timeout).");
		done = false;
	}
	else {//handle responses
		int r = 0;
		HS485CommMessageLGW* pResponse = (HS485CommMessageLGW*)pMsg->GetResponse(r);
		while(pResponse != NULL) {
			//LOG(Logger::LOG_ALL, "command type: %c", (char)pResponse->hmwlgwCommand.getCommandType());
			//LOG(Logger::LOG_ALL, "command data: %s", toDebugHexStr(pResponse->hmwlgwCommand.getCommandData()).c_str());

			if(pResponse->hmwlgwCommand.getCommandType() == LGW_RPL_DISCOVERY_RESULT)
			{
				std::string s = pResponse->hmwlgwCommand.getCommandData();
				if(s.size() == 4) {
					devices->push_back(convertBigEndianStringToUnsignedInt(s));
				}
				else {
					LOG(Logger::LOG_ERROR, "HS485ControllerLGW::Discovery(): Wrong amount of data in discovery result.");
				}
			}
			else if(pResponse->hmwlgwCommand.getCommandType() == LGW_EVT_DISCOVERY_END) {
				std::string s = pResponse->getCommandData();
				if(s.size() == 3) {
					unsigned char errorCode = s.at(0);
					unsigned int deviceCount = convertBigEndianStringToUnsignedInt( s.substr(1, 2) );
					LOG(Logger::LOG_INFO, "Discovery finished with code %d. Found %d devices.", (unsigned int)errorCode, deviceCount);
				}
				else {
					LOG(Logger::LOG_ERROR, "HS485ControllerLGW::Discovery(): Wrong amount of data in discovery end.");
				}
			}
			else {
				LOG(Logger::LOG_ERROR, "HS485ControllerLGW::Discovery(): Response is not a discovery result or -end.");
			}
			//Next
			r++;
			pResponse = (HS485CommMessageLGW*)pMsg->GetResponse(r);
		}
	}
	BroadcastSleepMode(false);
	//cleanup
	delete pMsg;

	return done;
}

void HS485ControllerLGW::ClearAddressInfo(uint32_t address/*=0xffffffff*/)
{
	pthread_mutex_lock(&mutex_address_info);
	if(address==0xffffffff){
		map_address_info.clear();
	}else{
		map_address_info.erase(address);
	}
	pthread_mutex_unlock(&mutex_address_info);
}

unsigned int HS485ControllerLGW::handleEvent(unsigned int eventType)
{
//	LOG(Logger::LOG_ALL, "HS485ControllerLGW::handleEvent()");
	return HS485Controller::handleEvent(eventType);
}

bool HS485ControllerLGW::CheckRxCounter(uint32_t receiver, unsigned char cc)
{
//	LOG(Logger::LOG_ALL, "HS485ControllerLGW::CheckRxCounter():");
	return HS485Controller::CheckRxCounter(receiver, cc);
}

void HS485ControllerLGW::UpdateControlChar(uint32_t receiver, unsigned char* cc)
{
	HS485Controller::UpdateControlChar(receiver, cc);
//	LOG(Logger::LOG_ALL, "HS485ControllerLGW::UpdateControlChar(): Using parent impl.");
}

bool HS485ControllerLGW::ProcessReceivedData(std::string* s)
{
	if(s != NULL) {
		//LOG(Logger::LOG_ALL, "HS485ControllerLGW::ProcessReceivedData");
		//LOG(Logger::LOG_ALL, "Incoming data: %s", toDebugHexStr(*s).c_str());
		if(incomingHMWLGWData.pHMWLGWTransportFrame == NULL) {
			incomingHMWLGWData.pHMWLGWTransportFrame = new HMWLGWTransportFrame();
		}
		if(incomingHMWLGWData.leftover.size() > 0) {
			s->insert(0, incomingHMWLGWData.leftover);
			incomingHMWLGWData.leftover.clear();
		}
		bool complete = incomingHMWLGWData.pHMWLGWTransportFrame->addIncomingFrameData(*s, incomingHMWLGWData.leftover);
		if(complete) {
			//LOG(Logger::LOG_ALL, "Message complete, payload is:\n%s", toDebugHexStr(incomingHMWLGWData.pHMWLGWTransportFrame->getPayload()).c_str());
			handleReceivedFrame(incomingHMWLGWData.pHMWLGWTransportFrame);
			delete incomingHMWLGWData.pHMWLGWTransportFrame;//New frame next time
			incomingHMWLGWData.pHMWLGWTransportFrame = NULL;
			s->clear();
			return true;//This method is called as long as we return true.
		}
		else {
			s->clear();
			return false;//frame incomplete, read more data
		}
		return true;
	}
	else {
		LOG(Logger::LOG_ERROR, "HS485ControllerLGW::ProcessReceivedData(): Pointer is NULL. Nothing to process.");
		return false;
	}
}

bool HS485ControllerLGW::getLGWStatus(std::string* serial, bool* connected)
{
	if(port_wrapper != NULL) {
		LGWPortWrapper* pLGWPortWrapper = dynamic_cast<LGWPortWrapper*>(port_wrapper);
		if(pLGWPortWrapper != NULL) {
			if(connected != NULL) {
				(*connected) = pLGWPortWrapper->isConnected();
			}
			else {
				return false;
			}
			if(serial != NULL) {
				serial->assign(pLGWPortWrapper->getSerial());
			}
			else {
				return false;
			}
			return true;
		}
	}
	return false;
}

bool HS485ControllerLGW::handleReceivedFrame(HMWLGWTransportFrame* pIncomingTransportFrame)
{
	if(pIncomingTransportFrame != NULL) {
		//Create CommMessage from transport frame.
		//LOG(Logger::LOG_ALL, "HS485ControllerLGW::handleReceivedFrame()");
		HS485CommMessageLGW*  pCommMessage = new HS485CommMessageLGW(*pIncomingTransportFrame);
		pthread_mutex_lock(&mutex_tx_state);
		bool handled=false;
		if(cur_tx_message){
			handled=((HS485CommMessageLGW*)cur_tx_message)->ProcessResponse(pCommMessage, &tx_state);
			//LOG(Logger::LOG_ALL, "HS485ControllerLGW::handleReceivedFrame(): Response processed. handled=%s.", (handled ? "true" : "false"));
		}
		if(handled){
			pthread_cond_signal(&cond_tx_state);
			if(pCommMessage->GetParent()){
				CheckRxCounter(pCommMessage->GetSenderAddress(), pCommMessage->GetCtrl());
				//cur_tx_message is responsible for further handling m
				pthread_mutex_unlock(&mutex_tx_state);
				return true;
			}
		}
		else {//Error or event
			if(pCommMessage->hmwlgwCommand.getCommandType() == LGW_EVT_EVENT)
			{
//				LOG(Logger::LOG_ALL, "HS485ControllerLGW::handleReceivedFrame(): Event");
				if((cur_tx_message != NULL)&& (!cur_tx_message->GetCollectResponses())) {
					pthread_cond_signal(&cond_tx_state);
				}
//				LOG(Logger::LOG_DEBUG, "HS485ControllerLGW::handleReceivedFrame(): Handling event.");
				pCommMessage->SetCommand(HS485CommMessageLGW::CMD_EVENT);
			}
		}
		pthread_mutex_unlock(&mutex_tx_state);
		ProcessReceivedMessage(pCommMessage);
		return true;
	}
	else {
		LOG(Logger::LOG_ERROR, "HS485ControllerLGW::handleReceivedFrame(): Transportframe pointer is NULL.");
		return false;
	}
}

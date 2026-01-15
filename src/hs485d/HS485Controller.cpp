/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <HS485Controller.h>
#include <HS485ControllerCCU1.h>
#include <Logger.h>
#include <HS485Manager.h>
#include <stdio.h>

HS485Controller* HS485Controller::singleton=NULL;


HS485Controller::HS485Controller()
: CommController()
{
	pthread_mutex_init(&mutex_address_info, NULL);
    //if(!singleton)singleton=this;
	if(pipe(pipe_fds)){
		perror("pipe()");
	}
	setfd(pipe_fds[0]);
}

HS485Controller::~HS485Controller()
{
	if(singleton==this)singleton=NULL;
}

HS485Controller* HS485Controller::GetSingleton()
{
	return singleton;
}

CommMessage* HS485Controller::NewMessage()
{
	return new HS485CommMessage();
}


HS485CommMessage* HS485Controller::CreateNewMessage()
{
	return (HS485CommMessage*)HS485Controller::NewMessage();
}

bool HS485Controller::getLGWStatus(std::string* /*serial*/, bool* /*connected*/) {
	return false;
}


bool HS485Controller::CheckBeforeSend(CommMessage*)
{//Dummy
	return false;
}


bool HS485Controller::CheckAfterReceive(CommMessage*)
{//Dummy
	return false;
}

void HS485Controller::ProcessReceivedMessage(CommMessage* msg)
{
	rxq.AddMessage(msg);
	//tell the XmlRpc main loop that we now have some input
	#ifdef WIN32
		send(pipe_fds[1], "", 1, 0);
	#else
		if(write(pipe_fds[1], "", 1) != 1)
			LOG(Logger::LOG_ERROR, "HS485Controller::ProcessReceivedMessage(): Could not write().");
	#endif
}

std::string HS485Controller::GetDeviceDescription(uint32_t address)
{
	std::string msg="h";
    std::string result;
    std::string response;
    if(SendMessage(address, msg, &response)){
        result+=response;
    }else{
        return "";
    }
    msg="v";
    if(SendMessage(address, msg, &response)){
        if(response.size()==1)response.append(1, (char)0);
        result+=response;
    }else{
        return "";
    }
    if(result.size()!=4)return "";
    return result;
}

bool HS485Controller::SendMessage(HS485CommMessage*)
{//Dummy
	return false;
}

bool HS485Controller::SendMessage(uint32_t, const std::string &, std::string *)
{//DUmmy
	return false;
}

bool HS485Controller::SendBootloaderMessage(uint32_t, const std::string &, std::string *)
{//Dummy
	return false;
}

int HS485Controller::Discovery(std::vector<uint32_t>* )
{//Dummy
	return 0;
}

void HS485Controller::ClearAddressInfo(uint32_t address/*=0xffffffff*/)
{
	pthread_mutex_lock(&mutex_address_info);
	if(address==0xffffffff){
		map_address_info.clear();
	}else{
		map_address_info.erase(address);
	}
	pthread_mutex_unlock(&mutex_address_info);
}

void HS485Controller::BroadcastSleepMode(bool on)
{
	LOG(Logger::LOG_ALL, "HS485Controller::BroadcastSleepMode(): %s ", (on ? "on" : "off") );
	for(int i=0;i<2;i++){
		SendMessage(0xffffffff, on?"z":"Z", NULL);
		usleep(100000);
	}
}

unsigned int HS485Controller::handleEvent(unsigned int)
{
//	LOG(Logger::LOG_ALL, "HS485Controller::handleEvent()");
	char buffer[32];
#ifdef WIN32
    recv(getfd(), buffer, 32, 0);
#else
	if(read(getfd(), buffer, 32) == -1)
	  LOG(Logger::LOG_ERROR, "HS485Controller::handleEvent(): could not read().");
#endif
    CommMessage * msg = rxq.GetNextMessage();
    while(msg){
    	HS485CommMessage* pMsg = dynamic_cast<HS485CommMessage*>(msg);
    	if(pMsg != NULL) {
    		HS485Frame frame=pMsg->ExtractFrame();
    		HS485Manager::GetSingleton()->ProcessIncomingFrame(frame);
    	}
    	else {
    		LOG(Logger::LOG_ERROR, "HS485Controller::handleEvent(): Message is of incorrect type.");
    	}
        delete msg;
        msg = NULL;
        msg = rxq.GetNextMessage();
    }
    return 1;
}

bool HS485Controller::CheckRxCounter(uint32_t sender, unsigned char cc)
{
	if(cc&0x01){
		//this is no I-Frame
		return true;
	}
	pthread_mutex_lock(&mutex_address_info);
	AddressInfo& ai=map_address_info[sender];
	if(ai.rx_counter==((cc>>1)&0x03) && !(cc&HS485Frame::CTRL_SYN)){
		LOG(Logger::LOG_DEBUG, "%08lX: tx_counter=%d, SYN=%d, stored rx_counter=%d, CheckRxCounter=false", sender, (int)((cc>>1)&0x03), (cc&0x80)?1:0, ai.rx_counter);
		pthread_mutex_unlock(&mutex_address_info);
		return false;
	}
	if(((ai.rx_counter+2)&0x03)==((cc>>1)&0x03)){
		LOG(Logger::LOG_DEBUG, "%08lX: tx_counter=%d, SYN=%d, stored rx_counter=%d, CheckRxCounter=true", sender, (int)((cc>>1)&0x03), (int)(cc&0x80)?1:0, ai.rx_counter);
	}
	LOG(Logger::LOG_DEBUG, "%08lX: tx_counter=%d, SYN=%d, stored rx_counter=%d, CheckRxCounter=true", sender, (int)((cc>>1)&0x03), (int)(cc&0x80)?1:0, ai.rx_counter);
	ai.rx_counter=(cc>>1)&0x03;
	pthread_mutex_unlock(&mutex_address_info);
	return true;
}

void HS485Controller::UpdateControlChar(uint32_t receiver, unsigned char* cc)
{
	if(receiver==0xffffffff){
		*cc |= HS485Frame::CTRL_SYN;
	}
	pthread_mutex_lock(&mutex_address_info);
	AddressInfo ai=map_address_info[receiver];
	bool syn=false;
	if(ai.tx_counter==0xff){
		ai.tx_counter=0;
		syn=true;
	}
	if(ai.rx_counter==0xff){
		ai.rx_counter=0x04;
		syn=true;
	}
	ai.tx_counter=(ai.tx_counter+1)&0x03;
	map_address_info[receiver]=ai;
	pthread_mutex_unlock(&mutex_address_info);


	(*cc)|=(syn?HS485Frame::CTRL_SYN:(ai.tx_counter&0x03)<<1)|((ai.rx_counter&0x03)<<5);
}


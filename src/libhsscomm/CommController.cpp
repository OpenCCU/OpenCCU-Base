/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <sys/types.h>
#include <sys/stat.h>
#ifndef WIN32
#include <sys/ioctl.h>
#include <termios.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/timeb.h>
#include <sys/resource.h>
#include <errno.h>
#else
#include <io.h>
#pragma warning(disable:4786)
#include "win_pipe.h"
#endif
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include "CommController.h"
#include "CommMessage.h"
#include <utils.h>
#include <signal.h>
#include <Logger.h>
#include <OSCompat.h>

//uncomment this line for debugging
//#define DUMP_PACKETS

void* buscontroller_rx_thread_function(void* arg)
{
	return reinterpret_cast<CommController*>(arg)->RxThreadFunction();
}
void* buscontroller_tx_thread_function(void* arg)
{
	return reinterpret_cast<CommController*>(arg)->TxThreadFunction();
}

CommController::CommController()
{
	pthread_mutex_init(&mutex_send, NULL);
	pthread_mutex_init(&mutex_exit, NULL);
	pthread_cond_init(&cond_tx_state, NULL);
	pthread_cond_init(&cond_tx_data_available, NULL);
	pthread_mutex_init(&mutex_tx_state, NULL);
	pthread_mutex_init(&mutex_tx_data_available, NULL);
	exit=false;
	thread_rx=thread_tx=0;
	port_wrapper=0;
	cur_tx_message=0;
    tx_state=CommMessage::INITIAL;
}

CommController::~CommController()
{
	Stop();
	if(cur_tx_message)delete cur_tx_message;
	pthread_mutex_destroy(&mutex_send);
	pthread_mutex_destroy(&mutex_exit);
	pthread_mutex_destroy(&mutex_tx_state);
	pthread_mutex_destroy(&mutex_tx_data_available);
	pthread_cond_destroy(&cond_tx_data_available);
	pthread_cond_destroy(&cond_tx_state);
	pthread_mutex_destroy(&mutex_tx_state);
	delete port_wrapper;
}

int CommController::TxQueueAddMessage( CommMessage* msg )
{
	msg->SetState(CommMessage::QUEUED);
    txq.AddMessage(msg);
    
	pthread_mutex_lock(&mutex_tx_data_available);
	pthread_cond_signal(&cond_tx_data_available);
	pthread_mutex_unlock(&mutex_tx_data_available);
    
	return 1;
}

int CommController::Send(const std::string& data)
{
	int retval;
	pthread_mutex_lock(&mutex_send);
	retval=port_wrapper->SendData(data);
	pthread_mutex_unlock(&mutex_send);
	return retval;
}

void* CommController::TxThreadFunction()
{
    OSCompat::RaiseThreadPriority();
	CommMessage* msg;
	pthread_mutex_lock(&mutex_tx_state);
	tx_state=CommMessage::INITIAL;
	pthread_mutex_unlock(&mutex_tx_state);
//    int local_response_counter;
   
	while(true){
		while(true){
            uint64_t timeout=1000;
			msg=txq.WaitNextMessage(&timeout);
			pthread_mutex_lock(&mutex_exit);
			if(exit){
				pthread_mutex_unlock(&mutex_tx_state);
				goto exit_thread;
			}
			pthread_mutex_unlock(&mutex_exit);
            if(!msg)continue;
			msg->AssignID();
			//OK, we have a packet, send it
			CheckBeforeSend(msg);
			/*
			if(!CheckBeforeSend(msg)){
				//we are not gonna send this, so get rid of it...
				msg->SetState(CommMessage::DROPPED);
				if(!msg->GetDontDelete()){
					delete msg;
				}else{
					msg->SetState(CommMessage::DROPPED);
				}
				msg=NULL;
				continue;
			}
			*/
			const std::string raw_data=msg->PrepareRawData();
#ifdef DUMP_PACKETS
			LOG(Logger::LOG_DEBUG, "%lu: TX %s", time_millis(), msg->DumpToString().c_str());
#endif
			if(msg->GetResponseTimeout()){
				msg->SetState(CommMessage::WAIT_ACK);
				pthread_mutex_lock(&mutex_tx_state);
				cur_tx_message=msg;
				tx_state=CommMessage::WAIT_ACK;
//				response_counter=0;
//				local_response_counter=0;
				pthread_mutex_unlock(&mutex_tx_state);
			}
			Send(raw_data);
//			port_wrapper->SendData(raw_data);
			//now wait for responses if required
			if(msg->GetResponseTimeout()){
        uint64_t end_time=time_millis()+msg->GetResponseTimeout();
				int64_t response_timeout=end_time-time_millis();
				while(response_timeout>0){
//					printf("response_timeout=%d\n", response_timeout);
					pthread_mutex_lock(&mutex_tx_state);
					if(tx_state==CommMessage::WAIT_ACK){
						//printf("Waiting for response\n");
						struct timespec timeout=millis2abstime(response_timeout);
						if(pthread_cond_timedwait(&cond_tx_state, &mutex_tx_state, &timeout)==0){
		        			end_time=time_millis()+msg->GetResponseTimeout();
							response_timeout=end_time-time_millis();
						}
					}
					pthread_mutex_lock(&mutex_exit);
					if(exit){
						pthread_mutex_unlock(&mutex_tx_state);
						goto exit_thread;
					}
					pthread_mutex_unlock(&mutex_exit);
					if(tx_state!=CommMessage::WAIT_ACK){//we are done with this message
//						printf("TxThreadFunction calling SetState()\n");
						cur_tx_message=0;
						pthread_mutex_unlock(&mutex_tx_state);
						if(!msg->GetDontDelete()){
							delete msg;
						}else{
							msg->SetState(tx_state);
						}
						msg=NULL;
						break;
					}
//					local_response_counter=response_counter;
					pthread_mutex_unlock(&mutex_tx_state);
					response_timeout=end_time-time_millis();
				}
				if(response_timeout<=0){//timeout occured
					LOG(Logger::LOG_ERROR, "response timeout");
					pthread_mutex_lock(&mutex_tx_state);
					tx_state=CommMessage::INITIAL;//prevent rx thread from accessing cur_tx_message
    				cur_tx_message=0;
					pthread_mutex_unlock(&mutex_tx_state);
					if(!msg->GetDontDelete()){
						delete msg;
					}else{
						msg->SetState(CommMessage::DROPPED);
					}
					msg=NULL;
				}
			}else{
				if(!msg->GetDontDelete()){
					delete msg;
				}else{
					msg->SetState(CommMessage::ACKED);
				}
				msg=NULL;
			}
        }
		struct timespec timeout=millis2abstime(1000);
		pthread_mutex_lock(&mutex_tx_data_available);
		pthread_cond_timedwait(&cond_tx_data_available, &mutex_tx_data_available, &timeout);
		pthread_mutex_unlock(&mutex_tx_data_available);
		
		pthread_mutex_lock(&mutex_exit);
		if(exit)goto exit_thread;
		pthread_mutex_unlock(&mutex_exit);
		
	}
	
exit_thread:
	pthread_mutex_unlock(&mutex_exit);
    if(msg){
		if(!msg->GetDontDelete())delete msg;
        else msg->SetState(CommMessage::DROPPED);
    }
	return 0;
}

CommMessage* CommController::MessageFromReceivedData(std::string *s)
{
	if(s->length()<3)return NULL;
    unsigned int frame_length=(*s)[0];
    if(s->length()<=frame_length)return NULL;
	CommMessage* m=NewMessage();
	m->command=((int)(*s)[1])&0xff;
	m->id=((int)(*s)[2])&0xff;
    m->data=s->substr(3, frame_length-2);
    *s=s->substr(frame_length+1);
	return m;
}


bool CommController::ProcessReceivedData(std::string* s)
{
    CommMessage* m=MessageFromReceivedData(s);
    if(!m)return false;
#ifdef DUMP_PACKETS
		LOG(Logger::LOG_DEBUG, "%lu: RX %s", time_millis(), m->DumpToString().c_str());
#endif
	if(!CheckAfterReceive(m)){
		delete m;
		return true;
	}
//	printf("pthread_mutex_lock %p\n", cur_tx_message);
	pthread_mutex_lock(&mutex_tx_state);
    bool handled=false;
    if(cur_tx_message){
		//printf("Calling ProcessResponse()\n");
        handled=cur_tx_message->ProcessResponse(m, &tx_state);
//        if(handled)response_counter++;
    }
    if(handled){
		//printf("message was handled\n");
		pthread_cond_signal(&cond_tx_state);
		if(m->GetParent()){
			//cur_tx_message is responsible for further handling m
			pthread_mutex_unlock(&mutex_tx_state);
			return true;
		}
    }
//	printf("pthread_mutex_unlock\n");
	pthread_mutex_unlock(&mutex_tx_state);
    ProcessReceivedMessage(m);
    return true;
}

void* CommController::RxThreadFunction()
{
    OSCompat::RaiseThreadPriority();
	while(true){
		if(port_wrapper->WaitForData(1000))port_wrapper->ReadData(&rx_buffer);
		pthread_mutex_lock(&mutex_exit);
		if(exit)break;
		pthread_mutex_unlock(&mutex_exit);
        while(ProcessReceivedData(&rx_buffer));
	}
	pthread_mutex_unlock(&mutex_exit);
	return 0;
}

int CommController::Start()
{
	Stop();
	pthread_mutex_lock(&mutex_exit);
	exit=false;
	pthread_mutex_unlock(&mutex_exit);
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 512*1024);
	pthread_create(&thread_tx, &attr, buscontroller_tx_thread_function, reinterpret_cast<void*>(this));
	pthread_create(&thread_rx, &attr, buscontroller_rx_thread_function, reinterpret_cast<void*>(this));
	pthread_attr_destroy(&attr);
	return 1;
}

int CommController::Stop()
{
	void* status;
	pthread_mutex_lock(&mutex_exit);
	exit=true;
	pthread_mutex_unlock(&mutex_exit);

	pthread_mutex_lock(&mutex_tx_data_available);
	pthread_cond_signal(&cond_tx_data_available);
	pthread_mutex_unlock(&mutex_tx_data_available);

	pthread_mutex_lock(&mutex_tx_state);
	pthread_cond_signal(&cond_tx_state);
	pthread_mutex_unlock(&mutex_tx_state);

	if(thread_tx)pthread_join(thread_tx, &status);
	if(thread_rx)pthread_join(thread_rx, &status);
	thread_rx=thread_tx=0;
	return 1;
}

void CommController::SetPortWrapper(PortWrapper *pw, bool check_controller /*=true*/)
{
	Stop();
	port_wrapper=pw;
	Start();
}

bool CommController::IsCommunicationStarted()
{
	return 	thread_tx && thread_rx;
}

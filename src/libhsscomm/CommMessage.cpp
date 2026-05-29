/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// CommMessage.cpp: Implementierung der Klasse CommMessage.
//
//////////////////////////////////////////////////////////////////////
#include "CommMessage.h"
#include <utils.h>
#include "Logger.h"

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

/*static*/ int CommMessage::s_id=0xff;

const char* STATES[]={
	"INITIAL", "QUEUED", "WAIT_ACK", "RECEIVED", "ACKED", "NACKED", "DROPPED"
};

CommMessage::CommMessage()
{
	parent=0;
	cmd_mask_ack=0;
	cmd_mask_nack=0;
	cmd_mask_response=0;
	command=-1;
	state=INITIAL;
    dont_delete=false;
	collect_responses=true;
    priority=0;
	id=-1;
    response_timeout=500;
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond_state_changed, NULL);
}

CommMessage::~CommMessage()
{
//	printf("~CommMessage() this=%p\n", this);
	ClearResponses();
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond_state_changed);
}

void CommMessage::ClearResponses()
{
	t_messageVect::iterator it;
	for(it=vect_responses.begin();it!=vect_responses.end();it++){
		delete *it;
	}
	vect_responses.clear();
}

bool CommMessage::ProcessResponse(CommMessage *m, t_state* new_state)
{
	if(id!=m->id){
//		printf("id mismatch\n");
		return false;
	}
	pthread_mutex_lock(&mutex);
//    printf("CommMessage::ProcessResponse() state=%d\n", state);
    if(state==WAIT_ACK){
	    if(cmd_mask_ack & (1<<(m->command&0x7f))){
//		    printf("cmd_mask_ack match\n");
		    *new_state=ACKED;
	    }else if(cmd_mask_nack & (1<<(m->command&0x7f))){
//		    printf("cmd_mask_nack match\n");
		    *new_state=NACKED;
	    }else if(cmd_mask_response & (1<<(m->command&0x7f))){
//		    printf("cmd_mask_response match\n");
		    *new_state=WAIT_ACK;
	    }else{
		    pthread_mutex_unlock(&mutex);
//		    printf("cmd_mask mismatch\n");
		    return false;
	    }
		if(collect_responses){
			m->parent=this;
			vect_responses.push_back(m);
		}
		pthread_mutex_unlock(&mutex);
		return true;
    }
	pthread_mutex_unlock(&mutex);
    return false;
}

void CommMessage::SetState(CommMessage::t_state s)
{
//	printf("CommMessage::SetState(%s)\n", STATES[s]);
	pthread_mutex_lock(&mutex);
	this->state=s;
	pthread_cond_signal(&cond_state_changed);
	pthread_mutex_unlock(&mutex);
}

CommMessage::t_state CommMessage::GetState() const
{
	CommMessage* This=const_cast<CommMessage*>(this);
	pthread_mutex_lock(&This->mutex);
	t_state s=state;
	pthread_mutex_unlock(&This->mutex);
	return s;
}

bool CommMessage::WaitUntilSent(uint64_t *timeout)
{
	uint64_t end_time=0;
	if(timeout){
		end_time=time_millis()+*timeout;
	}
	pthread_mutex_lock(&mutex);
//    printf("WaitUntilSent(): initialstate=%s\n", STATES[state]);
	while(state!=ACKED && state!=NACKED && state!=DROPPED){
//        printf("WaitUntilSent(): before wait state=%s\n", STATES[state]);
		if(timeout){
			struct timespec abs_timeout=millis2abstime(*timeout);
			pthread_cond_timedwait(&cond_state_changed, &mutex, &abs_timeout);
		}else{
			pthread_cond_wait(&cond_state_changed, &mutex);
		}
//        printf("WaitUntilSent(): after wait state=%s\n", STATES[state]);
		pthread_mutex_unlock(&mutex);
		if(timeout && *timeout){
			uint64_t now=time_millis();
			int64_t diff=end_time-now;
			if(diff>0)*timeout=diff;
			else *timeout=0;
		}
    	pthread_mutex_lock(&mutex);
		if(timeout && !*timeout)break;
	}
	pthread_mutex_unlock(&mutex);
	return GetState()==ACKED;
}

CommMessage::t_state CommMessage::WaitStateChanged(uint64_t *timeout)
{
	uint64_t end_time=0;
	if(timeout){
		end_time=time_millis()+*timeout;
	}
	pthread_mutex_lock(&mutex);
	if(timeout){
		struct timespec abs_timeout=millis2abstime(*timeout);
		pthread_cond_timedwait(&cond_state_changed, &mutex, &abs_timeout);
	}else{
		pthread_cond_wait(&cond_state_changed, &mutex);
	}
	pthread_mutex_unlock(&mutex);
	if(timeout && *timeout){
		uint64_t now=time_millis();
		int64_t diff=end_time-now;
		if(diff>0)*timeout=diff;
		else *timeout=0;
	}
	return GetState();
}

std::string CommMessage::PrepareRawData()
{
	if(id<0)id=GetNextID();
    std::string s;
    s.reserve(data.size()+3);
    s.resize(3);
    s[0]=data.length()+2;
    s[1]=command;
    s[2]=id;
    s+=data;
    return s;
}

/*
std::string CommMessage::DumpToString()
{
	char buffer[256];
	snprintf(buffer, sizeof(buffer), "CMD=0x%02x, ID=0x%02x, DATA=", (int)command, (int)id);
	std::string s=buffer;
	for(unsigned int i=0;i<data.size();i++){
		char c=data[i];
		snprintf(buffer, sizeof(buffer), "%02X", (int)c);
		s+=buffer;
	}
	return s;
}
*/

CommMessage* CommMessage::GetResponse(unsigned int index/*=0*/)
{
	CommMessage* response=NULL;
	pthread_mutex_lock(&mutex);
	if(vect_responses.size()>index)response=vect_responses[index];
	pthread_mutex_unlock(&mutex);
	return response;
}

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// CommMessageQueue.cpp: Implementierung der Klasse CommMessageQueue.
//
//////////////////////////////////////////////////////////////////////

#include "CommMessageQueue.h"
#include "CommMessage.h"
#include <utils.h>
#include <algorithm>
#include <Logger.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

CommMessageQueue::CommMessageQueue()
{
	pthread_mutex_init(&mutex, NULL);
	pthread_cond_init(&cond, NULL);
}

CommMessageQueue::~CommMessageQueue()
{
	pthread_mutex_lock(&mutex);
	t_message_list::iterator it;
    for(it=message_list.begin();it!=message_list.end();it++)delete *it;
	pthread_mutex_unlock(&mutex);
	pthread_mutex_destroy(&mutex);
	pthread_cond_destroy(&cond);
    

}

void CommMessageQueue::AddMessage(CommMessage *msg)
{
	pthread_mutex_lock(&mutex);

    if(message_list.empty() || message_list.back()->GetPriority()>=msg->GetPriority()){//handle the average case fast
		message_list.push_back(msg);
	}else{
		t_message_list::iterator it;
		for(it=message_list.begin();it!=message_list.end();it++){
			if((*it)->GetPriority()>=msg->GetPriority())continue;
			message_list.insert(it, msg);
			break;
		}
	}
	pthread_cond_signal(&cond);
	pthread_mutex_unlock(&mutex);
}

bool CommMessageQueue::DeleteMessage(CommMessage *p)
{
	bool retval=false;
	pthread_mutex_lock(&mutex);
	t_message_list::iterator it=std::find(message_list.begin(), message_list.end(), p);
    if(it!=message_list.end()){
        message_list.erase(it);
		delete p;
		retval=true;
	}
	pthread_mutex_unlock(&mutex);
	return retval;
}

CommMessage* CommMessageQueue::GetNextMessage()
{
    uint64_t timeout=0;
    return WaitNextMessage(&timeout);
}

CommMessage* CommMessageQueue::WaitNextMessage(uint64_t* timeout)
{
	CommMessage* retval=NULL;
	uint64_t end_time=0;
	if(timeout){
		end_time=time_millis()+*timeout;
	}
    pthread_mutex_lock(&mutex);
    do{
	    if(message_list.empty()){
	        if(timeout){
		        struct timespec abs_timeout=millis2abstime(*timeout);
		        pthread_cond_timedwait(&cond, &mutex, &abs_timeout);
	        }else{
		        pthread_cond_wait(&cond, &mutex);
	        }
        }
	    if(!message_list.empty()){
            retval=message_list.front();
            message_list.pop_front();
            break;
	    }else{
        	if(timeout && *timeout){
            uint64_t now=time_millis();
		        int64_t diff=end_time-now;
		        if(diff>0)*timeout=diff;
		        else *timeout=0;
            }
	    }
    }while(timeout==NULL || *timeout!=0);
	pthread_mutex_unlock(&mutex);
   	if(timeout && *timeout){
        uint64_t now=time_millis();
        int64_t diff=end_time-now;
        if(diff>0)*timeout=diff;
        else *timeout=0;
    }
	return retval;
}

size_t CommMessageQueue::GetSize()
{
	size_t size=0;
	pthread_mutex_lock(&mutex);
	size=message_list.size();
	pthread_mutex_unlock(&mutex);
	return size;
}

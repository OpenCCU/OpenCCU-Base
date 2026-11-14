/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// TimerQueue.cpp: Implementierung der Klasse TimerQueue.
#include "TimerQueue.h"
#include "TimerTarget.h"
//TimerQueue timerQueue;
// Konstruktion/Destruktion
#include <typeinfo>
#include <limits.h>
#include <time.h>

#define NSEC_PER_SEC      1000000000L
#define NSEC_PER_MILLISEC 1000000L
#define MSEC_PER_SEC      1000L

TimerQueue::TimerQueue()
{
	clock_gettime(CLOCK_MONOTONIC, &last_time);
}
TimerQueue::~TimerQueue()
{
	TimerTarget::timerQueueExists=false;
}
TimerQueue::Entry::Entry(TimerTarget* target, uint32_t timeout, uint32_t cookie)
{
	this->target=target;
	this->cookie=cookie;
	clock_gettime(CLOCK_MONOTONIC, &dueTime);

	// add timeout (ms) to dueTime
	dueTime.tv_sec += (timeout / MSEC_PER_SEC);
	dueTime.tv_nsec += (timeout % MSEC_PER_SEC) * NSEC_PER_MILLISEC;

	if (dueTime.tv_nsec >= NSEC_PER_SEC) {
		dueTime.tv_sec++;
		dueTime.tv_nsec -= NSEC_PER_SEC;
	} else if (dueTime.tv_nsec < 0) {
		dueTime.tv_sec--;
		dueTime.tv_nsec += NSEC_PER_SEC;
	}
}
void TimerQueue::RequestTimer(TimerTarget* target, uint32_t timeout, uint32_t cookie)
{
	Entry entry(Entry(target, timeout, cookie));
	queue_t::iterator it=queue.begin();
	while(it!=queue.end()){
		if(! ((*it) <= entry))break;
		it++;
	}
	queue.insert(it, entry);
}
void TimerQueue::KillTimer(TimerTarget* target, uint32_t cookie)
{
	Entry entry(Entry(target, 0, cookie));
	queue.remove(entry);
}
void TimerQueue::KillAllTimers(TimerTarget* target)
{
	queue_t::iterator it=queue.begin();
	while(it!=queue.end()){
		if(it->target==target)it=queue.erase(it);
		else it++;
	}
}
int64_t TimerQueue::TimeBeforeNextDue()
{
	if(queue.empty())return -1;
	Entry& e=queue.front();
	struct timespec now;
	clock_gettime(CLOCK_MONOTONIC, &now);
	int64_t secs_since_last_call = (now.tv_sec - last_time.tv_sec);
	if(secs_since_last_call<0){
		//time warp to the past detected
		BackwarpAdjust(-1 * secs_since_last_call);
	}
	last_time=now;
	int64_t diffSeconds = static_cast<int64_t>(e.dueTime.tv_sec) - static_cast<int64_t>(now.tv_sec);
	int64_t diffMilliseconds = (e.dueTime.tv_nsec - now.tv_nsec) / NSEC_PER_MILLISEC; // to ms
	if( diffMilliseconds < 0 )
	{
	    diffSeconds--;
	    diffMilliseconds += MSEC_PER_SEC;
	}
	//negative due time -> expire immediately
	if( diffSeconds < 0 )
	  return 0;
	//due time too long to be represented by return value, return a very long time
	if( (diffSeconds + 1) >= (INT64_MAX / MSEC_PER_SEC) )
	  return (INT64_MAX - MSEC_PER_SEC);

	return diffSeconds * MSEC_PER_SEC + diffMilliseconds;
}
void TimerQueue::BackwarpAdjust(unsigned int offset)
{
	for(queue_t::iterator it=queue.begin();it!=queue.end();it++){
		it->dueTime.tv_sec -= offset;
	}
}
void TimerQueue::Execute()
{
	while(!queue.empty()){
		Entry& e=queue.front();
		Entry now(0, 0, 0);
		if(e <= now){
			TimerTarget* target=e.target;
			int cookie=e.cookie;
			queue.pop_front();
			target->OnTimer(cookie);
		}else{
			break;
		}
	}
}

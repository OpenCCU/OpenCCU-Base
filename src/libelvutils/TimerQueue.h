/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// TimerQueue.h: Schnittstelle für die Klasse TimerQueue.
#ifndef _TIMERQUEUE_H_
#define _TIMERQUEUE_H_

#include "dllexport.h"
#include <list>
#include <stdint.h>
#include <time.h>

class TimerTarget;
class ELVUTILS_DLLEXPORT TimerQueue  
{
public:
	void Execute();
	int64_t TimeBeforeNextDue();
	TimerQueue();
	virtual ~TimerQueue();
protected:
	void BackwarpAdjust(unsigned int offset);
	void RequestTimer(TimerTarget* target, uint32_t timeout, uint32_t cookie);
	void KillTimer(TimerTarget* target, uint32_t cookie);
	void KillAllTimers(TimerTarget* target);
	class Entry{
	public:
		Entry(TimerTarget* target, uint32_t timeout, uint32_t cookie);
		bool operator<(const Entry& e)
		{
			return 
				(dueTime.tv_sec < e.dueTime.tv_sec) ||
				((dueTime.tv_sec == e.dueTime.tv_sec) && (dueTime.tv_nsec < e.dueTime.tv_nsec));
		}
		bool operator<=(const Entry& e)
		{
			return 
				(dueTime.tv_sec < e.dueTime.tv_sec) ||
				((dueTime.tv_sec == e.dueTime.tv_sec) && (dueTime.tv_nsec <= e.dueTime.tv_nsec));
		}
		bool operator==(const Entry& e)
		{
			return (target==e.target) && (cookie==e.cookie);
		}
		TimerTarget* target;
		struct timespec dueTime;
		uint32_t cookie;
	};
	typedef std::list<Entry> queue_t;
	queue_t queue;
	friend class TimerTarget;
	struct timespec last_time;
};

#endif

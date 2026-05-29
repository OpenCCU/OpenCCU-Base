/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// TimerTarget.h: Schnittstelle für die Klasse TimerTarget.
#ifndef _TIMERTARGET_H_
#define _TIMERTARGET_H_
#include "dllexport.h"
#include "TimerQueue.h"
#include <vector>
#include <stdint.h>

class ELVUTILS_DLLEXPORT TimerTarget  
{
public:
	TimerTarget();
	virtual ~TimerTarget();
	static TimerQueue s_timerQueue;
protected:
	void KillTimer(uint32_t cookie);
	void KillAllTimers();
	void KillTimersIntervall(uint32_t min, uint32_t max);
	std::vector<uint32_t> GetAllTimerCookies();
	void ElapseAllTimersNOW();
	virtual void OnTimer(uint32_t cookie)=0;
	void RequestTimer(int32_t timeout, uint32_t cookie);
	friend class TimerQueue;
	static bool timerQueueExists;
};
#endif

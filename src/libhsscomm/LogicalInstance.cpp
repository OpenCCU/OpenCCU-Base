/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "LogicalInstance.h"
#include "CommMessage.h"

#include <Logger.h>
#include <set>
#include <cinttypes>

using namespace XmlRpc;

LogicalInstance::LogicalInstance(void)
{
}

LogicalInstance::~LogicalInstance(void)
{
}

bool LogicalInstance::RegisterInternalValueEvent(const std::string& id, LogicalInstance::EventReceiver* rec)
{
	event_receivers.insert(std::make_pair(id, rec));
	return true;
}

void LogicalInstance::SendInternalValueEvent(const std::string& id)
{
	XmlRpcValue val;
	if(GetInternalValue(id, &val)){
		SendInternalValueEvent(id, val);
	}
}

void LogicalInstance::SendInternalValueEvent(const std::string& id, XmlRpc::XmlRpcValue& val)
{
	typedef event_receivers_t::iterator it_t;
	std::pair<it_t, it_t> range=event_receivers.equal_range(id);
	for(it_t it=range.first;it!=range.second;it++){
		it->second->OnEvent(this, id, val);
	}
}

bool LogicalInstance::ScheduleAutotimerEvent(const std::string& value_id, XmlRpc::XmlRpcValue& value, uint32_t delay)
{
	LOG(Logger::LOG_DEBUG, "Scheduling event in %" PRIu32 "ms %s.%s=%s", delay, GetSerial().c_str(), value_id.c_str(), value.toText().c_str());
	t_scheduled_event& evt=map_scheduled_events[value_id];
	evt.value=value;
	evt.time=time_millis()+delay;
	KillTimer(TIMER_AUTOTIMER_EVENTS);
	//correct timer will be calculated by first call to ProcessAutotimerEvents()
	RequestTimer(0, TIMER_AUTOTIMER_EVENTS);
	return true;
}

bool LogicalInstance::CancelAutotimerEvent(const std::string& value_id)
{
	map_scheduled_events.erase(value_id);
	//calculate timer
	KillTimer(TIMER_AUTOTIMER_EVENTS);
	ProcessAutotimerEvents();
	return true;
}

void LogicalInstance::OnTimer(uint32_t cookie)
{
	switch(cookie){
		case TIMER_AUTOTIMER_EVENTS:
			ProcessAutotimerEvents();
		break;
	}
}

void LogicalInstance::ProcessAutotimerEvents()
{
	uint64_t now=time_millis();
	int32_t new_delay=0;

	std::set<std::string> erase_set;
	t_map_scheduled_events::iterator it;
	for(it=map_scheduled_events.begin();it!=map_scheduled_events.end();it++){
		t_scheduled_event& evt=it->second;
		int64_t time_diff=evt.time-now;
		if(time_diff<=0){
			SetValue(it->first, evt.value);
			ReportEvent(it->first, evt.value);
			erase_set.insert(it->first);
		}else{
			if(!new_delay || time_diff<new_delay)new_delay=time_diff;
		}
	}
	if(new_delay)RequestTimer(new_delay, TIMER_AUTOTIMER_EVENTS);
	for(std::set<std::string>::iterator erase_it=erase_set.begin();erase_it!=erase_set.end();erase_it++)map_scheduled_events.erase(*erase_it);
}

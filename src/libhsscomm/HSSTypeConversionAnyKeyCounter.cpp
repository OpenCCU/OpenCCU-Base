/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HSSTypeConversionAnyKeyCounter.h"

#include "Logger.h"
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSTypeConversionAnyKeyCounter> HSSTypeConversionAnyKeyCounterFactory;

using namespace XmlRpc;

static const char* COUNTER_ID="_COUNTER";

HSSTypeConversionAnyKeyCounter::HSSTypeConversionAnyKeyCounter(void)
{
	counter_mask=0xff;
	sim_counter_offset=0x80;
}

HSSTypeConversionAnyKeyCounter::~HSSTypeConversionAnyKeyCounter(void)
{
}

bool HSSTypeConversionAnyKeyCounter::CheckCreationTag(const char *tag)
{
	if(strstr(tag, "type_conversion_")!=tag)return false;
	return strstr(tag, "_key_counter")!=0;
}

bool HSSTypeConversionAnyKeyCounter::InitFromXml(XMLNode &node, XMLNode &root_node)
{
	const char* temp=node.getAttribute("sim_counter");
	if(temp)sim_counter_id=temp;

	temp=node.getAttribute("counter_size");
	if(temp){
		int counter_size=strtoul(temp, NULL, 0);
		sim_counter_offset=1<<(counter_size-1);
		counter_mask=(1<<counter_size)-1;
	}
    return true;
}

bool HSSTypeConversionAnyKeyCounter::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
    try{
		if(sim_counter_id.size()){
			XmlRpcValue temp;
			int sim_counter=sim_counter_offset;
			if(inst->GetStoredValue(sim_counter_id, &temp)){
				sim_counter=(((int&)temp)+1)&counter_mask;
			}
			temp=sim_counter;
			inst->SetStoredValue(sim_counter_id, temp, ValueStore::FLAG_VOLATILE);
		}
        *out=0;
    }catch(XmlRpcException){
        return false;
    }
	return true;
}

bool HSSTypeConversionAnyKeyCounter::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
    try{
        out->clear();
		if(!event)return true;
		if(sim_counter_id.size()){
			int sim_counter=(((int&)in)+sim_counter_offset)&counter_mask;
			XmlRpcValue temp=sim_counter;
			inst->SetStoredValue(sim_counter_id, temp, ValueStore::FLAG_VOLATILE);
		}
		int last_counter=-1;
		XmlRpcValue temp;
		if(inst->GetStoredValue(COUNTER_ID, &temp)){
			last_counter=(int&)temp;
		}
		if((int&)in == last_counter)return false;
		inst->SetStoredValue(COUNTER_ID, in, ValueStore::FLAG_VOLATILE);
    }catch(XmlRpcException){
        return false;
    }
	return true;
}

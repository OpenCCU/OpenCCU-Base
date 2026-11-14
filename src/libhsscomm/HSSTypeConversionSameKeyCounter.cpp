/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HSSTypeConversionSameKeyCounter.h"

#include "Logger.h"
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSTypeConversionSameKeyCounter> HSSTypeConversionAnyKeyCounterFactory;

using namespace XmlRpc;

HSSTypeConversionSameKeyCounter::HSSTypeConversionSameKeyCounter(void)
{
}

HSSTypeConversionSameKeyCounter::~HSSTypeConversionSameKeyCounter(void)
{
}

bool HSSTypeConversionSameKeyCounter::CheckCreationTag(const char *tag)
{
	if(strstr(tag, "type_conversion_")!=tag)return false;
	return strstr(tag, "_key_same_counter")!=0;
}

bool HSSTypeConversionSameKeyCounter::InitFromXml(XMLNode &node, XMLNode &root_node)
{
	const char* temp=node.getAttribute("sim_counter");
	if(temp)sim_counter_id=temp;

    return true;
}

bool HSSTypeConversionSameKeyCounter::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
	return false;
}

bool HSSTypeConversionSameKeyCounter::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
    try{
        out->clear();
		if(!event)return true;

		int last_counter=-1;
		XmlRpcValue temp;
		if(inst->GetStoredValue(sim_counter_id, &temp)){
			last_counter=(int&)temp;
		}

		if((int&)in != last_counter)
		{
			inst->SetStoredValue(sim_counter_id, in, ValueStore::FLAG_VOLATILE);
			return false;
		}
    }catch(XmlRpcException){
        return false;
    }
	return true;
}

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionBooleanInteger.cpp: Implementierung der Klasse HSSTypeConversionBooleanInteger.
//
//////////////////////////////////////////////////////////////////////

#include "HSSTypeConversionBooleanInteger.h"
#include <Logger.h>
#include "type_registry.h"
#include <limits.h>

static hsscomm::type_registry::factory<HSSTypeConversionBooleanInteger> HSSTypeConversionBooleanIntegerFactory;


using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSTypeConversionBooleanInteger::HSSTypeConversionBooleanInteger()
{
	lower_threshold=1;
	upper_threshold=INT_MAX;
	false_val=0;
	true_val=1;
	invert=false;
}

HSSTypeConversionBooleanInteger::~HSSTypeConversionBooleanInteger()
{

}

bool HSSTypeConversionBooleanInteger::CheckCreationTag(const char *tag)
{
    if(strcmp("type_conversion_boolean_integer", tag)==0)return true;
    return false;
}

bool HSSTypeConversionBooleanInteger::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "HSSTypeConversionBooleanInteger::InitFromXml()");
    const char* temp=node.getAttribute("range");
	if(!temp)temp=node.getAttribute("threshold");
	if(temp){
		const char* stop_char;
		if(temp[0]=='-'){
			lower_threshold=INT_MIN;
			stop_char=temp;
		}else{
			lower_threshold=strtol(temp, (char**)&stop_char, 0);
		}
		if(*stop_char=='-'){
			upper_threshold=strtol(stop_char+1, NULL, 0);
		}
	}
	if(upper_threshold<lower_threshold){
		LOG(Logger::LOG_ERROR, "Invalid range in boolean to integer conversion");
		return false;
	}

    temp=node.getAttribute("true");
	if(temp)true_val=strtol(temp, NULL, 0);

    temp=node.getAttribute("false");
	if(temp)false_val=strtol(temp, NULL, 0);

    temp=node.getAttribute("invert");
	if(temp)invert=temp[0]=='t';

    return true;
}

bool HSSTypeConversionBooleanInteger::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
    try{
		(int&)*out=(((bool&)in)!=invert)?true_val:false_val;
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

bool HSSTypeConversionBooleanInteger::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
    try{
        (bool&)*out=(((int&)in>=lower_threshold) && ((int&)in<=upper_threshold)) != invert;
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

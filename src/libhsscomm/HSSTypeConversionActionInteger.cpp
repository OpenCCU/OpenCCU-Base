/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionActionInteger.cpp: Implementierung der Klasse HSSTypeConversionActionInteger.
//
//////////////////////////////////////////////////////////////////////

#include "HSSTypeConversionActionInteger.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSTypeConversionActionInteger> HSSTypeConversionActionIntegerFactory;


using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSTypeConversionActionInteger::HSSTypeConversionActionInteger()
{
}

HSSTypeConversionActionInteger::~HSSTypeConversionActionInteger()
{

}

bool HSSTypeConversionActionInteger::CheckCreationTag(const char *tag)
{
    if(strcmp("type_conversion_action_integer", tag)==0)return true;
    return false;
}

bool HSSTypeConversionActionInteger::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "HSSTypeConversionActionInteger::InitFromXml()");
	return true;
}

bool HSSTypeConversionActionInteger::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
    try{
		(int&)*out=(bool&)in;
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

bool HSSTypeConversionActionInteger::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
    try{
        (bool&)*out=false;
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

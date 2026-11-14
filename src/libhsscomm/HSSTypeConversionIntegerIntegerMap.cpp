/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionIntegerIntegerMap.cpp: Implementierung der Klasse HSSTypeConversionIntegerIntegerMap.
//
//////////////////////////////////////////////////////////////////////

#include "HSSTypeConversionIntegerIntegerMap.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSTypeConversionIntegerIntegerMap> HSSTypeConversionIntegerIntegerMapFactory;


using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSTypeConversionIntegerIntegerMap::HSSTypeConversionIntegerIntegerMap()
{
//    LOG(Logger::LOG_DEBUG, "HSSTypeConversionIntegerIntegerMap::HSSTypeConversionIntegerIntegerMap() this=%p", this);
}

HSSTypeConversionIntegerIntegerMap::~HSSTypeConversionIntegerIntegerMap()
{
//    LOG(Logger::LOG_DEBUG, "HSSTypeConversionIntegerIntegerMap::~HSSTypeConversionIntegerIntegerMap() this=%p", this);
}

bool HSSTypeConversionIntegerIntegerMap::CheckCreationTag(const char *tag)
{
    if(strcmp("type_conversion_integer_integer_map", tag)==0)return true;
    if(strcmp("type_conversion_option_integer", tag)==0)return true;
    return false;
}

bool HSSTypeConversionIntegerIntegerMap::InitFromXml(XMLNode &node, XMLNode &root_node)
{
	return value_map.InitFromXml(node, root_node);
}

bool HSSTypeConversionIntegerIntegerMap::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
    try{
        (int&)*out=value_map.MapToDevice((int&)in);
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

bool HSSTypeConversionIntegerIntegerMap::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
    try{
        (int&)*out=value_map.MapFromDevice((int&)in);
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

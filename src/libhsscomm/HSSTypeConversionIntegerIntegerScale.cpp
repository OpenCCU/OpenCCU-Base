/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionIntegerIntegerScale.cpp: Implementierung der Klasse HSSTypeConversionIntegerIntegerScale.
//
//////////////////////////////////////////////////////////////////////

#include "HSSTypeConversionIntegerIntegerScale.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSTypeConversionIntegerIntegerScale> HSSTypeConversionIntegerIntegerScaleFactory;


using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSTypeConversionIntegerIntegerScale::HSSTypeConversionIntegerIntegerScale()
{
	mul=div=1;
	offset=0;
}

HSSTypeConversionIntegerIntegerScale::~HSSTypeConversionIntegerIntegerScale()
{

}

bool HSSTypeConversionIntegerIntegerScale::CheckCreationTag(const char *tag)
{
    if(strcmp("type_conversion_integer_integer_scale", tag)==0)return true;
    return false;
}

bool HSSTypeConversionIntegerIntegerScale::InitFromXml(XMLNode &node, XMLNode &root_node)
{
    const char* temp=node.getAttribute("mul");
	if(temp)mul=strtol(temp, NULL, 0);

    temp=node.getAttribute("div");
	if(temp)div=strtol(temp, NULL, 0);

    temp=node.getAttribute("offset");
	if(temp)offset=strtol(temp, NULL, 0);

    return true;
}

bool HSSTypeConversionIntegerIntegerScale::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
    try{
		(int&)*out=((int&)in*mul)/div+offset;
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

bool HSSTypeConversionIntegerIntegerScale::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
    try{
		(int&)*out=(((int&)in-offset)*div)/mul;
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionFloatInteger.cpp: Implementierung der Klasse HSSTypeConversionFloatInteger.
//
//////////////////////////////////////////////////////////////////////

#include "HSSTypeConversionFloatInteger.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSTypeConversionFloatInteger> HSSTypeConversionFloatIntegerFactory;


using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSTypeConversionFloatInteger::HSSTypeConversionFloatInteger()
{
    factor=1.0;
	offset=0.0;
}

HSSTypeConversionFloatInteger::~HSSTypeConversionFloatInteger()
{

}

bool HSSTypeConversionFloatInteger::CheckCreationTag(const char *tag)
{
    if(strcmp("type_conversion_float_integer", tag)==0)return true;
    if(strcmp("type_conversion_float_integer_scale", tag)==0)return true;
    return false;
}

bool HSSTypeConversionFloatInteger::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "HSSTypeConversionFloatInteger::InitFromXml()");
    const char* temp=node.getAttribute("factor");
    if(temp)factor=strtod(temp, NULL);
    
    temp=node.getAttribute("offset");
    if(temp)offset=strtod(temp, NULL);

    return true;
}

bool HSSTypeConversionFloatInteger::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
    try{
		double dval=((double&)in+offset)*factor;
		if(dval>=0)dval+=0.5;
		else dval-=0.5;
        ((int&)*out)=(int)dval;
    }catch(XmlRpcException){
        return false;
    }
    //LOG(Logger::LOG_DEBUG, "HSSTypeConversionFloatInteger::LogicalToPhysical() %s -> %s", in.toText().c_str(), out->toText().c_str());
    return true;
}

bool HSSTypeConversionFloatInteger::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
    try{
        ((double&)*out)=(((double)((int&)in))/factor)-offset;
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

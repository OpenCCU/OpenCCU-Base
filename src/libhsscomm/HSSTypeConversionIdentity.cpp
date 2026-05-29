/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionIdentity.cpp: Implementierung der Klasse HSSTypeConversionIdentity.
//
//////////////////////////////////////////////////////////////////////

#include "HSSTypeConversionIdentity.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSTypeConversionIdentity> HSSTypeConversionIdentityFactory;


using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSTypeConversionIdentity::HSSTypeConversionIdentity()
{
}

HSSTypeConversionIdentity::~HSSTypeConversionIdentity()
{

}

bool HSSTypeConversionIdentity::CheckCreationTag(const char *tag)
{
	if(!strstr(tag, "type_conversion_"))return false;
	const char* type1=tag+16;
	const char* type2=type1+strlen(type1)/2+1;
	const char* p=strstr(type1, type2);
//	LOG(Logger::LOG_DEBUG, "HSSTypeConversionIdentity::CheckCreationTag(%s)=%s", tag, p!=NULL && p<type2?"true":"false");
	return p!=NULL && p<type2;
}

bool HSSTypeConversionIdentity::InitFromXml(XMLNode &node, XMLNode &root_node)
{
    return true;
}

bool HSSTypeConversionIdentity::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
    try{
        *out=in;
    }catch(XmlRpcException){
        return false;
    }
//    LOG(Logger::LOG_DEBUG, "HSSTypeConversionIdentity::LogicalToPhysical() %s -> %s", in.toText().c_str(), out->toText().c_str());
    return true;
}

bool HSSTypeConversionIdentity::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
    try{
        *out=in;
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

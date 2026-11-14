/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSLogicalTypeString.cpp: Implementierung der Klasse HSSLogicalTypeString.
//
//////////////////////////////////////////////////////////////////////

#include "HSSLogicalTypeString.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSLogicalTypeString> HSSLogicalTypeStringFactory;

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSLogicalTypeString::HSSLogicalTypeString()
{
	default_value="";
}

HSSLogicalTypeString::~HSSLogicalTypeString()
{

}

bool HSSLogicalTypeString::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "HSSLogicalTypeString::InitFromXml()");
    if(!HSSLogicalType::InitFromXml(node, root_node))return false;

    const char* temp=node.getAttribute("default");
    if(temp)default_value=temp;
    return true;
}

bool HSSLogicalTypeString::CheckCreationTag(const char* tag)
{
    return strcmp("logical_type_string", tag)==0;
}

bool HSSLogicalTypeString::EnforceConstraints(LogicalInstance* inst, XmlRpc::XmlRpcValue *val, operation_t op)
{
    return val->getType()==XmlRpcValue::TypeString;
}

bool HSSLogicalTypeString::GetDescription(XmlRpc::XmlRpcValue* val)
{
	if(!HSSLogicalType::GetDescription(val))return false;
    try{
		(*val)["TYPE"]="STRING";
		(std::string&)(*val)["MIN"]="";
		(std::string&)(*val)["MAX"]="";
		(std::string&)(*val)["DEFAULT"]=default_value;
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

XmlRpc::XmlRpcValue HSSLogicalTypeString::GetDefault()
{
	return default_value;
}


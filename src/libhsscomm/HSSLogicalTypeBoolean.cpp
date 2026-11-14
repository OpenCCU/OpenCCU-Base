/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSLogicalTypeBoolean.cpp: Implementierung der Klasse HSSLogicalTypeBoolean.
//
//////////////////////////////////////////////////////////////////////

#include "HSSLogicalTypeBoolean.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSLogicalTypeBoolean> HSSLogicalTypeBooleanFactory;

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSLogicalTypeBoolean::HSSLogicalTypeBoolean()
{
	default_value=false;
}

HSSLogicalTypeBoolean::~HSSLogicalTypeBoolean()
{

}

bool HSSLogicalTypeBoolean::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "HSSLogicalTypeBoolean::InitFromXml()");
    if(!HSSLogicalType::InitFromXml(node, root_node))return false;

    const char* temp=node.getAttribute("default");
    if(temp)default_value=temp[0]=='t' || temp[0]=='1';
    return true;
}

bool HSSLogicalTypeBoolean::CheckCreationTag(const char* tag)
{
    return strcmp("logical_type_boolean", tag)==0;
}

bool HSSLogicalTypeBoolean::EnforceConstraints(LogicalInstance* inst, XmlRpc::XmlRpcValue *val, operation_t op)
{
    if(val->getType()==XmlRpcValue::TypeBoolean)return true;
    try{
		std::string s_val=*val;
		if(s_val.empty())return false;
		val->clear();
		if(s_val[0]=='t')(bool&)*val=true;
		else if(s_val[0]=='f')(bool&)*val=false;
		else (bool&)*val=atoi(s_val.c_str())!=0;
	    return true;
    }catch(XmlRpcException){}
    return false;
}

bool HSSLogicalTypeBoolean::GetDescription(XmlRpc::XmlRpcValue* val)
{
	if(!HSSLogicalType::GetDescription(val))return false;
    try{
		(*val)["TYPE"]="BOOL";
		(bool&)(*val)["MIN"]=false;
		(bool&)(*val)["MAX"]=true;
		(bool&)(*val)["DEFAULT"]=default_value;
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

XmlRpc::XmlRpcValue HSSLogicalTypeBoolean::GetDefault()
{
	return default_value;
}


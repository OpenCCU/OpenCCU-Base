/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485LogicalTypeAddress.cpp: Implementierung der Klasse HS485LogicalTypeAddress.
//
//////////////////////////////////////////////////////////////////////

#include "HS485LogicalTypeAddress.h"
#include <limits.h>
#include <Logger.h>
#include <dynamic.h>

static dynamic::factory<HS485LogicalTypeAddress> HS485LogicalTypeAddressFactory;

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HS485LogicalTypeAddress::HS485LogicalTypeAddress()
{
}

HS485LogicalTypeAddress::~HS485LogicalTypeAddress()
{

}

bool HS485LogicalTypeAddress::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "HS485LogicalTypeAddress::InitFromXml()");
    if(!HSSLogicalType::InitFromXml(node, root_node))return false;

    return true;
}

bool HS485LogicalTypeAddress::CheckCreationTag(const char *tag)
{
    return strcmp("logical_type_address", tag)==0;
}

bool HS485LogicalTypeAddress::EnforceConstraints(LogicalInstance* inst, XmlRpc::XmlRpcValue *val, operation_t op)
{
	return val->getType()==XmlRpcValue::TypeString;
}

bool HS485LogicalTypeAddress::GetDescription(XmlRpc::XmlRpcValue* val)
{
	if(!HSSLogicalType::GetDescription(val))return false;
    try{
		(*val)["TYPE"]="ADDRESS";
		(*val)["MIN"]="";
		(*val)["MAX"]="";
		(*val)["DEFAULT"]="";
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

XmlRpc::XmlRpcValue HS485LogicalTypeAddress::GetDefault()
{
	return "";
}


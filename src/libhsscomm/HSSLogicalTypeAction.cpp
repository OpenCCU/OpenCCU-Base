/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSLogicalTypeAction.cpp: Implementierung der Klasse HSSLogicalTypeAction.
//
//////////////////////////////////////////////////////////////////////

#include "HSSLogicalTypeAction.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSLogicalTypeAction> HSSLogicalTypeActionFactory;

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSLogicalTypeAction::HSSLogicalTypeAction()
{
}

HSSLogicalTypeAction::~HSSLogicalTypeAction()
{

}

bool HSSLogicalTypeAction::InitFromXml(XMLNode &node, XMLNode &root_node)
{
    if(!HSSLogicalType::InitFromXml(node, root_node))return false;

    return true;
}

bool HSSLogicalTypeAction::CheckCreationTag(const char* tag)
{
    return strcmp("logical_type_action", tag)==0;
}

bool HSSLogicalTypeAction::EnforceConstraints(LogicalInstance* inst, XmlRpc::XmlRpcValue *val, operation_t op)
{
	try{
		switch(op){
			case OP_EVENT:
				(bool&)(*val)=true;
				break;
			case OP_READ:
				(bool&)(*val)=false;
				break;
			case OP_WRITE:
				if(val->getType()==XmlRpcValue::TypeInvalid)(bool&)(*val)=false;
				else (bool&)(*val)=true;
				break;
		}
		return true;
	}catch(XmlRpcException&){
		return false;
	}
}

bool HSSLogicalTypeAction::GetDescription(XmlRpc::XmlRpcValue* val)
{
	if(!HSSLogicalType::GetDescription(val))return false;
    try{
		(*val)["TYPE"]="ACTION";
		(bool&)(*val)["MIN"]=false;
		(bool&)(*val)["MAX"]=true;
		(bool&)(*val)["DEFAULT"]=false;
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

XmlRpc::XmlRpcValue HSSLogicalTypeAction::GetDefault()
{
	//return empty value that will be turned into "false" by EnforceConstraints()
	return XmlRpcValue();
}


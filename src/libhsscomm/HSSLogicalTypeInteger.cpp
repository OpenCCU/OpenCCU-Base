/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSLogicalTypeInteger.cpp: Implementierung der Klasse HSSLogicalTypeInteger.
//
//////////////////////////////////////////////////////////////////////

#include "HSSLogicalTypeInteger.h"
#include <limits.h>
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSLogicalTypeInteger> HSSLogicalTypeIntegerFactory;

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSLogicalTypeInteger::HSSLogicalTypeInteger()
{
    min=INT_MIN;
    max=INT_MAX;
    effective_min=INT_MIN;
    effective_max=INT_MAX;
	special_values.assertArray(0);
    default_value=0;
}

HSSLogicalTypeInteger::~HSSLogicalTypeInteger()
{

}

bool HSSLogicalTypeInteger::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "HSSLogicalTypeInteger::InitFromXml()");
    if(!HSSLogicalType::InitFromXml(node, root_node))return false;

    const char* temp=node.getAttribute("min");
    if(temp)min=strtol(temp, NULL, 0);

    temp=node.getAttribute("max");
    if(temp)max=strtol(temp, NULL, 0);

	effective_min=min;
	effective_max=max;

    temp=node.getAttribute("default");
    if(temp)default_value=strtol(temp, NULL, 0);

    int i=0;
    XMLNode option_node=node.getChildNode("special_value", &i);
    while(!option_node.isEmpty()){
		XmlRpcValue& sv=special_values[i-1];

        const char* temp=option_node.getAttribute("value");
        if(!temp)return false;
		int value=strtol(temp, NULL, 0);
		(int&)sv["VALUE"]=value;

        temp=option_node.getAttribute("id");
        if(!temp)return false;
		(std::string&)sv["ID"]=temp;

		if(value<effective_min)effective_min=value;
		if(value>effective_max)effective_max=value;
        option_node=node.getChildNode("special_value", &i);
    }

    return true;
}

bool HSSLogicalTypeInteger::CheckCreationTag(const char *tag)
{
    return strcmp("logical_type_integer", tag)==0;
}

bool HSSLogicalTypeInteger::EnforceConstraints(LogicalInstance* inst, XmlRpc::XmlRpcValue *val, operation_t op)
{
    if(val->getType()==XmlRpcValue::TypeString){
		std::string s_val=*val;
		if(s_val.empty())return false;
		val->clear();
		*val=atoi(s_val.c_str());
	}
    try{
        int& v=(int&)*val;
        if(v<effective_min)v=effective_min;
        if(v>effective_max)v=effective_max;
    }catch(XmlRpcException){
        return false;
    }
    return true;
}

bool HSSLogicalTypeInteger::GetDescription(XmlRpc::XmlRpcValue* val)
{
	if(!HSSLogicalType::GetDescription(val))return false;
    try{
		(*val)["TYPE"]="INTEGER";
		(*val)["MIN"]=min;
		(*val)["MAX"]=max;
		(*val)["DEFAULT"]=default_value;
		if(special_values.size()){
			(*val)["SPECIAL"]=special_values;
		}
    }catch(XmlRpcException){
        return false;
    }
    return true;
}


XmlRpc::XmlRpcValue HSSLogicalTypeInteger::GetDefault()
{
	return default_value;
}


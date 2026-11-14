/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSLogicalTypeFloat.cpp: Implementierung der Klasse HSSLogicalTypeFloat.
//
//////////////////////////////////////////////////////////////////////

#include "HSSLogicalTypeFloat.h"
#include <float.h>
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSLogicalTypeFloat> HSSLogicalTypeFloatFactory;

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSLogicalTypeFloat::HSSLogicalTypeFloat()
{
    min=DBL_MIN;
    max=DBL_MAX;
    effective_min=DBL_MIN;
    effective_max=DBL_MAX;
	special_values.assertArray(0);
    default_value=0.0;
}

HSSLogicalTypeFloat::~HSSLogicalTypeFloat()
{

}

bool HSSLogicalTypeFloat::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "HSSLogicalTypeFloat::InitFromXml()");
    if(!HSSLogicalType::InitFromXml(node, root_node))return false;

    const char* temp=node.getAttribute("min");
    if(temp)min=strtod(temp, NULL);

    temp=node.getAttribute("max");
    if(temp)max=strtod(temp, NULL);

	effective_min=min;
	effective_max=max;

    temp=node.getAttribute("default");
    if(temp)default_value=strtod(temp, NULL);

    int i=0;
    XMLNode option_node=node.getChildNode("special_value", &i);
    while(!option_node.isEmpty()){
		XmlRpcValue& sv=special_values[i-1];

        const char* temp=option_node.getAttribute("value");
        if(!temp)return false;
		double value=strtod(temp, NULL);
		(double&)sv["VALUE"]=value;

        temp=option_node.getAttribute("id");
        if(!temp)return false;
		(std::string&)sv["ID"]=temp;

		if(value<effective_min)effective_min=value;
		if(value>effective_max)effective_max=value;
        option_node=node.getChildNode("special_value", &i);
    }

    return true;
}

bool HSSLogicalTypeFloat::CheckCreationTag(const char *tag)
{
    return strcmp("logical_type_float", tag)==0;
}

bool HSSLogicalTypeFloat::EnforceConstraints(LogicalInstance* inst, XmlRpc::XmlRpcValue *val, operation_t op)
{
    //LOG(Logger::LOG_DEBUG, "HSSLogicalTypeFloat::EnforceConstraints(%s)", val->toText().c_str());
    if(val->getType()!=XmlRpcValue::TypeDouble){
		std::string s_val=*val;
		if(s_val.empty())return false;
		val->clear();
		*val=atof(s_val.c_str());
	}
    try{
        double& v=(double&)*val;
        if(v<effective_min)v=effective_min;
        if(v>effective_max)v=effective_max;
    }catch(XmlRpcException){
        return false;
    }
    //LOG(Logger::LOG_DEBUG, "retval=%s", val->toText().c_str());
    return true;
}

bool HSSLogicalTypeFloat::GetDescription(XmlRpc::XmlRpcValue* val)
{
	if(!HSSLogicalType::GetDescription(val))return false;
    try{
		(*val)["TYPE"]="FLOAT";
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

XmlRpc::XmlRpcValue HSSLogicalTypeFloat::GetDefault()
{
	return default_value;
}


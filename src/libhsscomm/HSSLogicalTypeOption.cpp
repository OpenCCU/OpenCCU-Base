/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSLogicalTypeOption.cpp: Implementierung der Klasse HSSLogicalTypeOption.
//
//////////////////////////////////////////////////////////////////////

#include "HSSLogicalTypeOption.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSLogicalTypeOption> HSSLogicalTypeOptionFactory;


using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSLogicalTypeOption::HSSLogicalTypeOption()
{
//    LOG(Logger::LOG_DEBUG, "HSSLogicalTypeOption::HSSLogicalTypeOption() this=%p", this);
	default_option=0;
}

HSSLogicalTypeOption::~HSSLogicalTypeOption()
{
//    LOG(Logger::LOG_DEBUG, "HSSLogicalTypeOption::~HSSLogicalTypeOption() this=%p", this);
}

bool HSSLogicalTypeOption::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "HSSLogicalTypeOption::InitFromXml()");
    if(!HSSLogicalType::InitFromXml(node, root_node))return false;

    int i=0;
    XMLNode option_node=node.getChildNode("option", &i);
    while(!option_node.isEmpty()){
        const char* temp=option_node.getAttribute("id");
        if(!temp)return false;
//        LOG(Logger::LOG_DEBUG, "HSSLogicalTypeOption::InitFromXml() option id=%s", temp);
		std::string id=temp;
		temp=option_node.getAttribute("index");
		unsigned int index;
		if(temp){
			index=strtoul(temp, NULL, 0);
			if(options.size()<=index)options.resize(index+1);
			options[index]=id;
		}else{
			index=options.size();
	        options.push_back(id);
		}

        temp=option_node.getAttribute("default");
        if(temp && temp[0]=='t')default_option=index;
        option_node=node.getChildNode("option", &i);
    }
    return true;
}

bool HSSLogicalTypeOption::CheckCreationTag(const char* tag)
{
    return strcmp("logical_type_option", tag)==0;
}

bool HSSLogicalTypeOption::EnforceConstraints(LogicalInstance* inst, XmlRpc::XmlRpcValue *val, operation_t op)
{
    if(val->getType() == XmlRpcValue::TypeInt){
        int& v=(int&)*val;
        if(v<0)v=default_option;
        if(v>=(int)options.size())v=default_option;
    }else{
		try{
			std::string s_val=*val;
			if(s_val.empty())return false;
			val->clear();
			for(unsigned int i=0;i<options.size();i++)if(options[i]==s_val){
				*val=(int)i;
				return true;
			}
			*val=atoi(s_val.c_str());
			return true;
		}catch(XmlRpcException){
			return false;
		}
    }
    return true;
}

bool HSSLogicalTypeOption::GetDescription(XmlRpc::XmlRpcValue* val)
{
	if(!HSSLogicalType::GetDescription(val))return false;
    try{
		(*val)["TYPE"]="ENUM";
		(*val)["MIN"]=int(0);
		(*val)["MAX"]=int(options.size()-1);
		(*val)["DEFAULT"]=int(default_option);
		XmlRpcValue& values=(*val)["VALUE_LIST"];
		for(unsigned int i=0;i<options.size();i++){
			values[i]=options[i];
		}
    }catch(XmlRpcException){
        return false;
    }
    return true;
}


XmlRpc::XmlRpcValue HSSLogicalTypeOption::GetDefault()
{
	return (int&)default_option;
}


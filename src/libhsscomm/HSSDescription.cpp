/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HSSDescription.h"
#include <string.h>

HSSDescription::HSSDescription(void)
{
}

HSSDescription::~HSSDescription(void)
{
	description_fields.clear();
}

bool HSSDescription::Describe(XmlRpc::XmlRpcValue* descr)
{
	for(description_fields_t::iterator it=description_fields.begin();it!=description_fields.end();it++)
	{
		(*descr)[it->first]=it->second;
	}
	return true;
}

bool HSSDescription::InitFromXml(XMLNode &node, XMLNode &root_node)
{
	int i=0;
    XMLNode field_node=node.getChildNode("field", &i);
	while(!field_node.isEmpty()){
	    const char* temp=field_node.getAttribute("id");
		if(!temp)return false;
		std::string id=temp;
	    temp=field_node.getAttribute("value");
		if(!temp)return false;
		std::string value=temp;

	    temp=field_node.getAttribute("type");
		if(!temp || strstr(temp, "str")==temp){
			description_fields[id]=value;
		}else if(strstr(temp, "int")==temp){
			description_fields[id]=(int)strtol(value.c_str(), NULL, 0);
		}else if(strstr(temp, "float")==temp){
			description_fields[id]=(double)strtod(value.c_str(), NULL);
		}else if(strstr(temp, "bool")==temp){
			(bool&)description_fields[id]=(value[0]=='t');
		}else{
			return false;
		}
	    field_node=node.getChildNode("field", &i);
	}
    return true;
}

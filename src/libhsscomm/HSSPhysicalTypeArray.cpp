/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSPhysicalTypeArray.cpp: Implementierung der Klasse HSSPhysicalTypeArray.
//
//////////////////////////////////////////////////////////////////////

#include "HSSPhysicalTypeArray.h"
#include <Logger.h>
#include "type_registry.h"

using namespace XmlRpc;

static hsscomm::type_registry::factory<HSSPhysicalTypeArray> HSSPhysicalTypeArrayFactory;


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSPhysicalTypeArray::HSSPhysicalTypeArray()
{
}

HSSPhysicalTypeArray::~HSSPhysicalTypeArray()
{
	elements_t::iterator it;
	for(it=elements.begin();it!=elements.end();it++)delete (*it);
}

bool HSSPhysicalTypeArray::InitFromXml(XMLNode &node, XMLNode &root_node)
{
    const char* temp=node.getAttribute("type");
    if(!temp)return false;
    type=temp;

	int i=0;
    XMLNode element_node=node.getChildNode("physical", &i);
    while(!element_node.isEmpty()){
	    const char* temp=element_node.getAttribute("type");
		if(!temp)return false;
		std::string creation_tag="physical_type_";
		creation_tag+=temp;
		void* obj=hsscomm::type_registry::create(creation_tag.c_str());
		if(!obj){
			LOG(Logger::LOG_ERROR, "Physical type %s not supported", temp);
			return false;
		}
		HSSPhysicalType* phys_type=dynamic_cast<HSSPhysicalType*>((HSSPhysicalType*)obj);
		if(!phys_type){
			/* TODO: get rid of the allocated memory */
			//delete obj;
			return false;
		}
		if(!phys_type->InitFromXml(element_node, root_node))return false;
		elements.push_back(phys_type);
	    element_node=node.getChildNode("physical", &i);
	}
    return true;

}

bool HSSPhysicalTypeArray::CheckCreationTag(const char *tag)
{
    if(strcmp("physical_type_array", tag)==0)return true;
    return false;
}

bool HSSPhysicalTypeArray::Get(LogicalInstance* inst, XmlRpc::XmlRpcValue* val)
{
	try{
		for(unsigned int i=0;i<elements.size();i++){
			if(!elements[i]->Get(inst, &((*val)[i])))return false;
		}
	}catch(XmlRpcException){
		return false;
	}
	return true;
}

bool HSSPhysicalTypeArray::Put(LogicalInstance* inst, XmlRpc::XmlRpcValue& val)
{
	try{
		for(unsigned int i=0;i<elements.size();i++){
			if(!elements[i]->Put(inst, val[i]))return false;
		}
	}catch(XmlRpcException){
		return false;
	}
	return true;
}

bool HSSPhysicalTypeArray::GetFromIncomingFrame(LogicalInstance* inst, StructuredFrame& frame, FrameDescription* fd, XmlRpc::XmlRpcValue* val)
{
	for(unsigned int i=0;i<elements.size();i++){
		if(!elements[i]->GetFromIncomingFrame(inst, frame, fd, val))return false;
	}
	return true;
}

bool HSSPhysicalTypeArray::SetupInstance(LogicalInstance* inst)
{
	for(unsigned int i=0;i<elements.size();i++){
		if(!elements[i]->SetupInstance(inst))return false;
	}
	return true;
}

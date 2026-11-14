/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSPhysicalType.cpp: Implementierung der Klasse HSSPhysicalType.
//
//////////////////////////////////////////////////////////////////////

#include "HSSPhysicalType.h"
#include <Logger.h>
#include "type_registry.h"
#include "HSSParameter.h"
#include <typeinfo>

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSPhysicalType::HSSPhysicalType()
{
//    LOG(Logger::LOG_DEBUG, "HSSPhysicalType::HSSPhysicalType() this=%p", this);
	data_interface=NULL;
	parent_param=NULL;
}

HSSPhysicalType::~HSSPhysicalType()
{
//    LOG(Logger::LOG_DEBUG, "HSSPhysicalType::~HSSPhysicalType() this=%p", this);
	if(data_interface)delete data_interface;
}

bool HSSPhysicalType::InitFromXml(XMLNode &node, XMLNode &root_node)
{
//    LOG(Logger::LOG_DEBUG, "HSSPhysicalType::InitFromXml()");
    const char* temp=node.getAttribute("type");
    if(!temp)return false;
    type=temp;

    temp=node.getAttribute("interface");
	if(!temp)return false;
	std::string creation_tag="data_interface_";
	creation_tag+=temp;
	void* obj=hsscomm::type_registry::create(creation_tag.c_str());
	if(!obj){
		LOG(Logger::LOG_ERROR, "Data interface %s not supported", temp);
		return false;
	}
	HSSPhysicalDataInterface* data_if=dynamic_cast<HSSPhysicalDataInterface*>((HSSPhysicalDataInterface*)obj);
	if(!data_if){
		/* TODO: get rid of the allocated memory */
		//delete obj;
		return false;
	}
	data_if->SetParent(this);
	if(!data_if->InitFromXml(node, root_node)){
		delete data_if;
		return false;
	}
	this->data_interface=data_if;

    return true;
}

const std::string& HSSPhysicalType::GetType()
{
    return type;
}
bool HSSPhysicalType::SetDefaultConfig(LogicalInstance *inst,XmlRpcValue val)
{
	return data_interface->SetDefaultConfig(inst,val);
}
bool HSSPhysicalType::Get(LogicalInstance* inst, XmlRpc::XmlRpcValue* val)
{
	if(data_interface)return data_interface->GetData(inst, val);
	else return false;
}

bool HSSPhysicalType::Determine(LogicalInstance* inst)
{
	if(data_interface)return data_interface->DetermineValue(inst);
	else return false;
}

bool HSSPhysicalType::Put(LogicalInstance* inst, XmlRpc::XmlRpcValue& val)
{
//    LOG(Logger::LOG_DEBUG, "HSSPhysicalType::Put() data_interface=%s", typeid(*data_interface).name());
	if(data_interface)return data_interface->PutData(inst, val);
	else return false;
}

bool HSSPhysicalType::GetFromIncomingFrame(LogicalInstance* inst, StructuredFrame& frame, FrameDescription* fd, XmlRpc::XmlRpcValue* val)
{
	if(data_interface)return data_interface->GetFromIncomingFrame(inst, frame, fd, val);
	else return false;
}


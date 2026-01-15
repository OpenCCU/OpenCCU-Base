/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HS485PhysicalDataInterfaceStore.h"
#include "HS485DeviceDescription.h"
#include "HS485Device.h"

#include <Logger.h>
#include "dynamic.h"
using namespace XmlRpc;

static dynamic::factory<HS485PhysicalDataInterfaceStore> HS485PhysicalDataInterfaceStoreFactory;

HS485PhysicalDataInterfaceStore::HS485PhysicalDataInterfaceStore(void)
{
	save_on_change=false;
	no_init=false;
	persistent=true;
}

HS485PhysicalDataInterfaceStore::~HS485PhysicalDataInterfaceStore(void)
{
}

bool HS485PhysicalDataInterfaceStore::InitFromXml(XMLNode &node, XMLNode &root_node)
{
	const char* temp=node.getAttribute("id");
	if(!temp){
		LOG(Logger::LOG_WARNING, "<physical interface=\"store\"> attribute \"id\" not found");
		return false;
	}
	id=temp;
	temp=node.getAttribute("save_on_change");
	if(temp && temp[0]=='t')save_on_change=true;

	temp=node.getAttribute("volatile");
	if(temp && temp[0]=='t')persistent=false;

	temp=node.getAttribute("no_init");
	if(temp && temp[0]=='t')no_init=true;

    return true;
}

bool HS485PhysicalDataInterfaceStore::GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param)
{
//	LOG(Logger::LOG_DEBUG, "HS485PhysicalDataInterfaceStore::GetData()");
	HS485LogicalInstance* hs485_inst=(HS485LogicalInstance*)inst;
	const std::string& peer=hs485_inst->GetCurParamsetPeer();
	if(peer.size()){
		return hs485_inst->GetStoredValue(id, peer, param);
	}else{
		return hs485_inst->ValueStore::GetStoredValue(id, param);
	}
}

bool HS485PhysicalDataInterfaceStore::PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param)
{
	LOG(Logger::LOG_DEBUG, "HS485PhysicalDataInterfaceStore::PutData()");
	HS485LogicalInstance* hs485_inst=(HS485LogicalInstance*)inst;
	const std::string& peer=hs485_inst->GetCurParamsetPeer();
	if(peer.size()){
		if(!hs485_inst->SetStoredValue(id, peer, param, persistent?0:ValueStore::FLAG_VOLATILE))return false;
	}else{
		if(!hs485_inst->ValueStore::SetStoredValue(id, param, persistent?0:ValueStore::FLAG_VOLATILE))return false;
	}
	if(save_on_change && !hs485_inst->GetDevice()->Save())return false;
	return true;
}

bool HS485PhysicalDataInterfaceStore::CheckCreationTag(const char *tag)
{
    if(strcmp("data_interface_store", tag)==0)return true;
    return false;
}

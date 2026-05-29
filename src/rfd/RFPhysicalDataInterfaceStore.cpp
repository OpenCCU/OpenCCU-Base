/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFPhysicalDataInterfaceStore.h"
#include "RFDeviceDescription.h"
#include "RFDevice.h"
#include "HSSPhysicalType.h"
#include "HSSParameter.h"
#include "HSSLogicalType.h"

#include <Logger.h>
#include "type_registry.h"
using namespace XmlRpc;

static hsscomm::type_registry::factory<RFPhysicalDataInterfaceStore> RFPhysicalDataInterfaceStoreFactory;

RFPhysicalDataInterfaceStore::RFPhysicalDataInterfaceStore(void)
{
	save_on_change=false;
	persistent=true;
	no_init=false;
}

RFPhysicalDataInterfaceStore::~RFPhysicalDataInterfaceStore(void)
{
}

bool RFPhysicalDataInterfaceStore::InitFromXml(XMLNode &node, XMLNode &root_node)
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

bool RFPhysicalDataInterfaceStore::GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param)
{
//	LOG(Logger::LOG_DEBUG, "RFPhysicalDataInterfaceStore::GetData()");
	RFLogicalInstance* rf_inst=(RFLogicalInstance*)inst;
	uint32_t peer=rf_inst->GetCurParamsetPeer();
	if(peer){
		return rf_inst->GetStoredValue(id, peer, param);
	}else{
		return rf_inst->ValueStore::GetStoredValue(id, param);
	}
}
bool RFPhysicalDataInterfaceStore::SetDefaultConfig(LogicalInstance* inst,XmlRpc::XmlRpcValue val)
{
	return true;
}
bool RFPhysicalDataInterfaceStore::PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param)
{
//	LOG(Logger::LOG_DEBUG, "RFPhysicalDataInterfaceStore::PutData()");
	RFLogicalInstance* rf_inst=(RFLogicalInstance*)inst;
	uint32_t peer=rf_inst->GetCurParamsetPeer();
	if(peer){
		if(!rf_inst->SetStoredValue(id, peer, param, persistent?0:ValueStore::FLAG_VOLATILE))return false;
	}else{
		if(!rf_inst->ValueStore::SetStoredValue(id, param, persistent?0:ValueStore::FLAG_VOLATILE))return false;
	}
	if(save_on_change && !rf_inst->GetDevice()->Save())return false;
	return true;
}

bool RFPhysicalDataInterfaceStore::CheckCreationTag(const char *tag)
{
    if(strcmp("data_interface_store", tag)==0)return true;
    return false;
}

bool RFPhysicalDataInterfaceStore::SetupInstance(LogicalInstance* inst)
{
	if(no_init)return true;
	XmlRpcValue dummy;
	if(GetData(inst, &dummy))return true;
	XmlRpcValue val=GetParent()->GetParent()->GetLogicalType()->GetDefault();
	if(!GetParent()->GetParent()->LogicalToPhysical(inst, val, &val))return false;
	inst->SetStoredValue(id, val, persistent?0:ValueStore::FLAG_VOLATILE);
	return true;
}

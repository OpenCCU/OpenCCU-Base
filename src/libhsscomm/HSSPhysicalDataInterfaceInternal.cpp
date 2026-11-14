/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HSSPhysicalDataInterfaceInternal.h"
#include "HSSPhysicalType.h"
#include "HSSParameter.h"
#include "type_registry.h"
#include <Logger.h>

using namespace XmlRpc;

static hsscomm::type_registry::factory<HSSPhysicalDataInterfaceInternal> HSSPhysicalDataInterfaceInternalFactory;

HSSPhysicalDataInterfaceInternal::HSSPhysicalDataInterfaceInternal(void)
{
	parent_type=NULL;
}

HSSPhysicalDataInterfaceInternal::~HSSPhysicalDataInterfaceInternal(void)
{
}

bool HSSPhysicalDataInterfaceInternal::InitFromXml(XMLNode &node, XMLNode &root_node)
{
	const char* temp=node.getAttribute("value_id");
	if(!temp)return false;
	value_id=temp;

	return true;
}

bool HSSPhysicalDataInterfaceInternal::GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param)
{
	return inst->GetInternalValue(value_id, param);
}
bool HSSPhysicalDataInterfaceInternal::SetDefaultConfig(LogicalInstance* inst,XmlRpc::XmlRpcValue val)
{
	return true;
}
bool HSSPhysicalDataInterfaceInternal::PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param)
{
	return inst->SetInternalValue(value_id, param);
}

bool HSSPhysicalDataInterfaceInternal::SetupInstance(LogicalInstance* inst){
	return inst->RegisterInternalValueEvent(value_id, this);
}

void HSSPhysicalDataInterfaceInternal::OnEvent(LogicalInstance* inst, const std::string& id, XmlRpc::XmlRpcValue& val)
{
//	LOG(Logger::LOG_DEBUG, "HSSPhysicalDataInterfaceInternal::OnEvent(%s, %s)", id.c_str(), val.toText().c_str());
	GetParent()->GetParent()->ReportEvent(inst, val);
}

bool HSSPhysicalDataInterfaceInternal::CheckCreationTag(const char *tag)
{
    if(strcmp("data_interface_internal", tag)==0)return true;
    return false;
}

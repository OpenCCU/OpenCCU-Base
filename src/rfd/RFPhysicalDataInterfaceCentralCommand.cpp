/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFPhysicalDataInterfaceCentralCommand.h"
#include "RFDeviceDescription.h"

#include <Logger.h>
#include "type_registry.h"
#include "RFLogicalInstance.h"
#include "RFDevice.h"
using namespace XmlRpc;

static hsscomm::type_registry::factory<RFPhysicalDataInterfaceCentralCommand> RFPhysicalDataInterfaceCentralCommandFactory;

RFPhysicalDataInterfaceCentralCommand::RFPhysicalDataInterfaceCentralCommand(void)
{
}

RFPhysicalDataInterfaceCentralCommand::~RFPhysicalDataInterfaceCentralCommand(void)
{
}

bool RFPhysicalDataInterfaceCentralCommand::GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param)
{
    return true;
}

bool RFPhysicalDataInterfaceCentralCommand::InitFromXml(XMLNode &node, XMLNode &root_node)
{
	if(!RFPhysicalDataInterfaceCommand::InitFromXml(node, root_node))return false;
	const char* temp=node.getAttribute("counter");
	if(temp)counter_id=temp;
    return true;
}
bool RFPhysicalDataInterfaceCentralCommand::SetDefaultConfig(LogicalInstance* inst,XmlRpc::XmlRpcValue val)
{
	return true;
}
bool RFPhysicalDataInterfaceCentralCommand::PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param)
{
	if(!value_id.empty()){
		if(!inst->SetStoredValue(value_id, param, ValueStore::FLAG_VOLATILE))return false;
	}
	if(!counter_id.empty()){
		XmlRpcValue val;
		inst->GetStoredValue(counter_id, &val);
		try{
			((int&)val)++;
			inst->SetStoredValue(counter_id, val, ValueStore::FLAG_VOLATILE);
		}catch(XmlRpcException& e){
		}
	}
	for(unsigned int i=0;i!=set_request_frames.size();i++){
		FrameDescription* fd=((RFLogicalInstance*)inst)->GetDevice()->GetDeviceDescription()->GetFrameDescription(set_request_frames[i].id);
		if(!fd)return false;
		BidcosFrame frame;
		if(!fd->InitFrame(&frame, inst, inst->GetIndex())){
			return false;
		}
		if(!((RFLogicalInstance*)inst)->SendFrame(&frame)){
			return false;
		}
	}
	return true;
}

bool RFPhysicalDataInterfaceCentralCommand::CheckCreationTag(const char *tag)
{
    if(strcmp("data_interface_central_command", tag)==0)return true;
    return false;
}

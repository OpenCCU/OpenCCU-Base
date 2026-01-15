/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HS485PhysicalDataInterfaceCommand.h"

#include <Logger.h>
#include <dynamic.h>
#include <FrameDescription.h>
#include "HS485LogicalInstance.h"
#include "HS485Device.h"
#include "HS485DeviceDescription.h"
#include "HSSPhysicalType.h"
#include "HSSParameter.h"
#include "HSSLogicalType.h"
#include <HS485Controller.h>

using namespace XmlRpc;

static dynamic::factory<HS485PhysicalDataInterfaceCommand> HS485PhysicalDataInterfaceCommandFactory;

HS485PhysicalDataInterfaceCommand::HS485PhysicalDataInterfaceCommand(void)
{
	no_init=false;
}

HS485PhysicalDataInterfaceCommand::~HS485PhysicalDataInterfaceCommand(void)
{
}

bool HS485PhysicalDataInterfaceCommand::InitFromXml(XMLNode &node, XMLNode &root_node)
{
	const char* temp=node.getAttribute("value_id");
	if(temp)value_id=temp;

	int i=0;
    XMLNode set_node=node.getChildNode("set", &i);
	while(!set_node.isEmpty()){
		const char* temp=set_node.getAttribute("request");
		if(!temp)return false;
		frame_t f;
		f.id=temp;
		set_request_frames.push_back(f);
	    set_node=node.getChildNode("set", &i);
	}

    XMLNode get_node=node.getChildNode("get");
	if(!get_node.isEmpty()){
		const char* temp=get_node.getAttribute("request");
		if(!temp)return false;
		get_request_frame.id=temp;
		temp=get_node.getAttribute("response");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<get> attribute \"response\" not found");
			return false;
		}
		get_response_frame.id=temp;
	}

	i=0;
    XMLNode event_node=node.getChildNode("event", &i);
	while(!event_node.isEmpty()){
		const char* temp=event_node.getAttribute("frame");
		if(!temp)return false;
		frame_t f;
		f.id=temp;
		event_frames.push_back(f);
		HSSParameter* param=GetParent()->GetParent();
		param->GetParamset()->SubscribeToEventFrame(param, f.id);
	    event_node=node.getChildNode("event", &i);
	}

	temp=node.getAttribute("no_init");
	if(temp && temp[0]=='t')no_init=true;

    return true;
}

bool HS485PhysicalDataInterfaceCommand::GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param)
{
	HS485Channel* channel=dynamic_cast<HS485Channel*>(inst);
	if(!channel)return false;
	if(value_id.empty())return false;
	if(get_request_frame.id.empty()){
		return inst->GetStoredValue(value_id, param);
	}
	if(inst->GetAge(value_id)<VALUE_CACHE_TIME){
		return inst->GetStoredValue(value_id, param);
	}
	FrameDescription* fd=((HS485LogicalInstance*)inst)->GetDevice()->GetDeviceDescription()->GetFrameDescription(get_request_frame.id);
	if(!fd){
//		LOG(Logger::LOG_ERROR, "HS485PhysicalDataInterfaceCommand::GetData frame description %s not found", get_request_frame.id.c_str());
		return true;
	}

    HS485Frame frame;
	if(!fd->InitFrame(&frame, inst, channel->GetPhysicalIndex())){
		LOG(Logger::LOG_ERROR, "HS485PhysicalDataInterfaceCommand::GetData could not create frame for %s", get_request_frame.id.c_str());
		return false;
    }
	frame.SetCtrl(HS485Frame::CTRL_IFRAME);

	HS485CommMessage* m = HS485Controller::GetSingleton()->CreateNewMessage();
    m->SetDontDelete(true);
	m->SetCommand(HS485CommMessage::CMD_SEND);
    m->SetFrame(frame);

	if(!channel->GetDevice()->SendMessage(m)){
		LOG(Logger::LOG_ERROR, "HS485PhysicalDataInterfaceCommand::GetData SendMessage failed for %s", get_request_frame.id.c_str());
		return false;
	}
	HS485CommMessage* response=m->GetResponse();
	if(!response){
		delete m;
		return false;
	}

	FrameDescription* resp_fd=((HS485LogicalInstance*)inst)->GetDevice()->GetDeviceDescription()->GetFrameDescription(get_response_frame.id);
	if(!resp_fd){
		return false;
	}

	int channel_index=-1;
    HS485Frame response_frame=response->ExtractFrame();
    delete m;
	if(!resp_fd->MatchFrame(response_frame, &channel_index)){
		return false;
	}

	if(channel_index!=channel->GetPhysicalIndex()){
		return false;
	}
	if(get_response_frame.process_as_event){
		channel->ProcessIncomingFrame(response_frame, resp_fd);
	}
	if(!resp_fd->GetMatchedValues().GetStoredValue(value_id, param)){
		return false;
	}
	inst->SetStoredValue(value_id, *param, ValueStore::FLAG_VOLATILE);
	return true;
}

bool HS485PhysicalDataInterfaceCommand::PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param)
{
	HS485LogicalInstance* hs485_inst=dynamic_cast<HS485LogicalInstance*>(inst);
	if(!hs485_inst)return false;
	if(!value_id.empty()){
		if(!inst->SetStoredValue(value_id, param, ValueStore::FLAG_VOLATILE))return false;
	}
	for(unsigned int i=0;i!=set_request_frames.size();i++){
		FrameDescription* fd=hs485_inst->GetDevice()->GetDeviceDescription()->GetFrameDescription(set_request_frames[i].id);
		if(!fd){
			LOG(Logger::LOG_ERROR, "HS485PhysicalDataInterfaceCommand::PutData could not find frame description \"%s\"", set_request_frames[i].id.c_str());
			return false;
		}

        HS485Frame frame;
		if(!fd->InitFrame(&frame, hs485_inst, hs485_inst->GetPhysicalIndex())){
			LOG(Logger::LOG_ERROR, "HS485PhysicalDataInterfaceCommand::PutData could not init frame \"%s\"", set_request_frames[i].id.c_str());
			return false;
		}
		frame.SetCtrl(HS485Frame::CTRL_IFRAME);

		HS485CommMessage* m = HS485Controller::GetSingleton()->CreateNewMessage();
        m->SetDontDelete(true);
		m->SetCommand(HS485CommMessage::CMD_SEND);
		m->SetCollectResponses(false);

        m->SetFrame(frame);
		if(fd->GetDirection()==FrameDescription::DIR_TO_DEVICE){
			if(!((HS485LogicalInstance*)inst)->GetDevice()->SendMessage(m)){
				LOG(Logger::LOG_ERROR, "HS485PhysicalDataInterfaceCommand::PutData SendMessage() failed");
				delete m;
				return false;
			}
		}else{
			//we will simulate a message from the device
			HS485Channel* ch=dynamic_cast<HS485Channel*>(inst);
			if(ch){
				ch->SendToPeers(m, fd->GetReceiverChannelField());
			}
		}
		delete m;
	}

	return true;
}

bool HS485PhysicalDataInterfaceCommand::CheckCreationTag(const char *tag)
{
    if(strcmp("data_interface_command", tag)==0)return true;
    return false;
}

bool HS485PhysicalDataInterfaceCommand::GetFromIncomingFrame(LogicalInstance* inst, StructuredFrame& frame, FrameDescription* fd, XmlRpc::XmlRpcValue* val)
{
	if(value_id.empty())return false;
	for(unsigned int i=0;i!=event_frames.size();i++){
		if(event_frames[i].id == fd->GetId()){
			XmlRpcValue matched_val;
			if(!fd->GetMatchedValues().GetStoredValue(value_id, &matched_val))return false;
			inst->SetStoredValue(value_id, matched_val, ValueStore::FLAG_VOLATILE);
			*val=matched_val;
			return true;
		}
	}
	return false;
}

bool HS485PhysicalDataInterfaceCommand::SetupInstance(LogicalInstance* inst)
{
	if(value_id.empty() || no_init)return true;
	HSSPhysicalType* pt=GetParent();
	if(!pt){
		LOG(Logger::LOG_ERROR, "HS485PhysicalDataInterfaceCommand::SetupInstance() physical_type is NULL");
		return false;
	}
	HSSParameter* param=pt->GetParent();
	if(!param){
		LOG(Logger::LOG_ERROR, "HS485PhysicalDataInterfaceCommand::SetupInstance() parameter is NULL");
		return false;
	}
	HSSLogicalType* lt=param->GetLogicalType();
	if(!lt){
		LOG(Logger::LOG_ERROR, "HS485PhysicalDataInterfaceCommand::SetupInstance() logical_typer is NULL");
		return false;
	}
	XmlRpcValue val=lt->GetDefault();
	lt->EnforceConstraints(inst, &val, HSSLogicalType::OP_WRITE);
	if(!GetParent()->GetParent()->LogicalToPhysical(inst, val, &val))return false;
	inst->SetStoredValue(value_id, val, ValueStore::FLAG_VOLATILE);
	return true;
}

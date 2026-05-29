/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFPhysicalDataInterfaceCommand.h"
#include "RFDeviceDescription.h"

#include <Logger.h>
#include "type_registry.h"
#include "RFLogicalInstance.h"
#include "RFDevice.h"
#include "HSSPhysicalType.h"
#include "HSSParameter.h"
#include "HSSLogicalType.h"
using namespace XmlRpc;

static hsscomm::type_registry::factory<RFPhysicalDataInterfaceCommand> RFPhysicalDataInterfaceCommandFactory;


RFPhysicalDataInterfaceCommand::RFPhysicalDataInterfaceCommand(void)
{
	no_init=false;
}

RFPhysicalDataInterfaceCommand::~RFPhysicalDataInterfaceCommand(void)
{
}

bool RFPhysicalDataInterfaceCommand::InitFromXml(XMLNode &node, XMLNode &root_node)
{
	const char* temp=node.getAttribute("value_id");
	if(temp)value_id=temp;

	int i=0;
    XMLNode set_node=node.getChildNode("set", &i);
	while(!set_node.isEmpty()){
		const char* temp=set_node.getAttribute("request");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<set> attribute \"request\" not found");
			return false;
		}
		frame_t f;
		f.id=temp;
		f.auth_violate_policy=AUTH_VIOLATE_IGNORE;
		set_request_frames.push_back(f);
	    set_node=node.getChildNode("set", &i);
	}

    XMLNode get_node=node.getChildNode("get");
	if(!get_node.isEmpty()){
		const char* temp=get_node.getAttribute("request");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<get> attribute \"request\" not found");
			return false;
		}
		get_request_frame.id=temp;
		get_request_frame.auth_violate_policy=AUTH_VIOLATE_IGNORE;
		temp=get_node.getAttribute("response");
		if(temp)get_response_frame.id=temp;

		temp=get_node.getAttribute("auth_violate_policy");
		if(temp){
			if(strstr(temp, "ignore"))get_response_frame.auth_violate_policy=AUTH_VIOLATE_IGNORE;
			else if(strstr(temp, "reject"))get_response_frame.auth_violate_policy=AUTH_VIOLATE_REJECT;
			else if(strstr(temp, "get"))get_response_frame.auth_violate_policy=AUTH_VIOLATE_GET;
			else{
				LOG(Logger::LOG_WARNING, "<get> attribute \"auth_violate_policy\" invalid value");
				return false;
			}
		}else{
			get_response_frame.auth_violate_policy=AUTH_VIOLATE_IGNORE;
		}
		temp=get_node.getAttribute("process_as_event");
		get_response_frame.process_as_event = temp && (temp[0]=='t');
	}

	i=0;
    XMLNode event_node=node.getChildNode("event", &i);
	while(!event_node.isEmpty()){
		const char* temp=event_node.getAttribute("frame");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<event> attribute \"frame\" not found");
			return false;
		}
		event_frame_t f;
		f.id=temp;
		temp=event_node.getAttribute("auth_violate_policy");
		if(temp){
			if(strstr(temp, "ignore"))f.auth_violate_policy=AUTH_VIOLATE_IGNORE;
			else if(strstr(temp, "reject"))f.auth_violate_policy=AUTH_VIOLATE_REJECT;
			else if(strstr(temp, "get"))f.auth_violate_policy=AUTH_VIOLATE_GET;
			else{
				LOG(Logger::LOG_WARNING, "<event> attribute \"auth_violate_policy\" invalid value");
				return false;
			}
		}else{
			f.auth_violate_policy=AUTH_VIOLATE_IGNORE;
		}
		XMLNode domino_node=event_node.getChildNode("domino_event");
		if(!domino_node.isEmpty()){
			temp=domino_node.getAttribute("delay_id");
			if(!temp){
				LOG(Logger::LOG_WARNING, "<domino_event> attribute \"delay_id\" not found");
				return false;
			}
			f.domino_delay_id=temp;

			temp=domino_node.getAttribute("value");
			if(!temp){
				LOG(Logger::LOG_WARNING, "<domino_event> attribute \"value\" not found");
				return false;
			}
			if(!f.domino_value.fromText(temp)){
				LOG(Logger::LOG_WARNING, "<domino_event> could not parse attribute \"value\"");
				return false;
			}
		}
		event_frames.push_back(f);

		HSSParameter* param=GetParent()->GetParent();
		param->GetParamset()->SubscribeToEventFrame(param, f.id);
	    event_node=node.getChildNode("event", &i);
	}

	i=0;
    XMLNode reset_node=node.getChildNode("reset_after_send", &i);
	while(!reset_node.isEmpty()){
		const char* temp=reset_node.getAttribute("param");
		if(!temp){
			LOG(Logger::LOG_WARNING, "<reset_after_send> attribute \"param\" not found");
			return false;
		}
		reset_after_send_params.push_back(temp);
	    reset_node=node.getChildNode("reset_after_send", &i);
	}

	temp=node.getAttribute("no_init");
	if(temp && temp[0]=='t')no_init=true;

    return true;
}

bool RFPhysicalDataInterfaceCommand::GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param)
{
	if(value_id.empty())return false;
	RFChannel* channel=dynamic_cast<RFChannel*>(inst);
	if(!channel)return false;
	if(get_request_frame.id.empty()){
		return inst->GetStoredValue(value_id, param);
	}
	if(inst->GetAge(value_id)<VALUE_CACHE_TIME){
		return inst->GetStoredValue(value_id, param);
	}
	FrameDescription* fd=channel->GetDevice()->GetDeviceDescription()->GetFrameDescription(get_request_frame.id);
	if(!fd){
//		LOG(Logger::LOG_ERROR, "RFPhysicalDataInterfaceCommand::GetData frame description %s not found", get_request_frames[i].c_str());
		return false;
	}
	BidcosFrame frame;
	if(!fd->InitFrame(&frame, channel, channel->GetIndex())){
		LOG(Logger::LOG_ERROR, "RFPhysicalDataInterfaceCommand::GetData could not create frame for %s", get_request_frame.id.c_str());
		return false;
	}
	if(channel->GetDevice()->RxNeedsWakeup()){
		if(inst->GetStoredValue(value_id, param))return true;
		channel->GetDevice()->QueueAfterWakeupFrame(frame);
		return false;
	}
	if(!channel->GetDevice()->SendFrame(&frame)){
		LOG(Logger::LOG_ERROR, "RFPhysicalDataInterfaceCommand::GetData SendFrame failed for %s", get_request_frame.id.c_str());
		return false;
	}
	BidcosFrame* response=frame.GetResponse();
	if(!response){
		return false;
	}
	bool auth_violate=channel->GetAES() && (!response->WasAuthenticated());
	if(auth_violate){
		switch(get_response_frame.auth_violate_policy)
		{
		case AUTH_VIOLATE_IGNORE:
			break;
		case AUTH_VIOLATE_REJECT:
			return false;
		case AUTH_VIOLATE_GET:
			//makes no sense since we are already processing the get response
			return false;
		}
	}
	FrameDescription* resp_fd=channel->GetDevice()->GetDeviceDescription()->GetFrameDescription(get_response_frame.id);
	int channel_index=-1;
	if( (!resp_fd) || (!resp_fd->MatchFrame(*response, &channel_index)) || (channel_index!=channel->GetIndex()) ){
		return false;
	}
	//check auth_violate here to avoid infinite loops
	if(get_response_frame.process_as_event && !auth_violate){
		channel->ProcessIncomingFrame(*response, resp_fd);
	}
	if(!resp_fd->GetMatchedValues().GetStoredValue(value_id, param)){
		return false;
	}
	inst->SetStoredValue(value_id, *param, ValueStore::FLAG_VOLATILE);
	return true;
}
bool RFPhysicalDataInterfaceCommand::SetDefaultConfig(LogicalInstance* inst,XmlRpc::XmlRpcValue val)
{
	return true;
}
bool RFPhysicalDataInterfaceCommand::PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param)
{
	RFChannel* channel=dynamic_cast<RFChannel*>(inst);
	if(!channel)return false;
	if(!value_id.empty()){
		if(!inst->SetStoredValue(value_id, param, ValueStore::FLAG_VOLATILE))return false;
	}
	for(unsigned int i=0;i!=set_request_frames.size();i++){
		FrameDescription* fd=((RFLogicalInstance*)inst)->GetDevice()->GetDeviceDescription()->GetFrameDescription(set_request_frames[i].id);
		if(!fd)return false;
		BidcosFrame frame;
		if(!fd->InitFrame(&frame, inst, inst->GetIndex())){
			return false;
		}
		if(fd->GetDirection()==FrameDescription::DIR_FROM_DEVICE){
			//we will simulate a message from the device
			RFChannel* ch=dynamic_cast<RFChannel*>(inst);
			if(ch){
				ch->SendToPeers(&frame);
			}
			return true;
		}
		if(channel->GetDevice()->RxNeedsWakeup()){
			channel->GetDevice()->QueueAfterWakeupFrame(frame);
			continue;
		}
		if(!channel->GetDevice()->SendFrame(&frame)){
			return false;
		}
		bool auth_violate=channel->GetAES() && (frame.GetAuthKey()!=channel->GetDevice()->GetAESKey());
		if(auth_violate){
			switch(set_request_frames[i].auth_violate_policy)
			{
			case AUTH_VIOLATE_IGNORE:
				break;
			case AUTH_VIOLATE_REJECT:
				return false;
			case AUTH_VIOLATE_GET:
				//makes no sense here
				return false;
			}
		}
		BidcosFrame* response=frame.GetResponse();
		if(response)channel->GetDevice()->ProcessIncomingFrame(*response);
	}
	HSSParamset* paramset=GetParent()->GetParent()->GetParamset();
	if(paramset){
		for(unsigned int i=0;i<reset_after_send_params.size();i++){
			if(!paramset->GetParameter(reset_after_send_params[i])->SetToDefault(inst)){
				LOG(Logger::LOG_ERROR, "Could not reset %s", reset_after_send_params[i].c_str());
			}
		}
	}

	return true;
}

bool RFPhysicalDataInterfaceCommand::CheckCreationTag(const char *tag)
{
    if(strcmp("data_interface_command", tag)==0)return true;
    return false;
}

bool RFPhysicalDataInterfaceCommand::GetFromIncomingFrame(LogicalInstance* inst, StructuredFrame& frame, FrameDescription* fd, XmlRpc::XmlRpcValue* val)
{
//	LOG(Logger::LOG_DEBUG, "RFPhysicalDataInterfaceCommand::GetFromIncomingFrame() id=%s", value_id.c_str());
	if(value_id.empty())return false;
	BidcosFrame* bc_frame=dynamic_cast<BidcosFrame*>(&frame);
	if(!bc_frame)return false;
	RFChannel* channel=dynamic_cast<RFChannel*>(inst);
	if(!channel)return false;
	bool auth_violation=channel->GetAES() && !bc_frame->WasAuthenticated();
	for(unsigned int i=0;i!=event_frames.size();i++){
		if(event_frames[i].id == fd->GetId()){
			if(auth_violation){
				switch(event_frames[i].auth_violate_policy)
				{
				case AUTH_VIOLATE_IGNORE:
					//LOG(Logger::LOG_DEBUG, "Ignoring AES violation value=%s frame=%s", value_id.c_str(), event_frames[i].id.c_str());
					break;
				case AUTH_VIOLATE_REJECT:
					//LOG(Logger::LOG_DEBUG, "Rejecting AES violation value=%s frame=%s", value_id.c_str(), event_frames[i].id.c_str());
					return false;
				case AUTH_VIOLATE_GET:
					if( GetParent() && GetParent()->GetParent() ){
						std::string parameter_id = GetParent()->GetParent()->GetId();
						channel->ScheduleValueGet(parameter_id);
						LOG(Logger::LOG_DEBUG, "Scheduling get for AES violation %s.%s", inst->GetSerial().c_str(), parameter_id.c_str());
					}
					return false;
				}
			}
			XmlRpcValue matched_val;
			if(!fd->GetMatchedValues().GetStoredValue(value_id, &matched_val))return false;
//			LOG(Logger::LOG_DEBUG, "RFPhysicalDataInterfaceCommand::GetFromIncomingFrame() id=%s val=%s", value_id.c_str(), matched_val.toText().c_str());
			inst->SetStoredValue(value_id, matched_val, ValueStore::FLAG_VOLATILE);
			*val=matched_val;
			if(event_frames[i].domino_delay_id.size()){
				//init a domino event
				XmlRpcValue logical_value;
				if(!GetParent()->GetParent()->PhysicalToLogical(inst, event_frames[i].domino_value, &logical_value, true)){
					LOG(Logger::LOG_WARNING, "Could not convert domino-event value");
					return true;
				}
				const std::string& param_id=GetParent()->GetParent()->GetId();
				XmlRpcValue temp;
				if(!inst->GetValue(event_frames[i].domino_delay_id, &temp)){
					LOG(Logger::LOG_WARNING, "Could not get delay value \"%s\"", event_frames[i].domino_delay_id.c_str());
					return true;
				}
				if(!(temp.getType()==XmlRpcValue::TypeInt)){
					LOG(Logger::LOG_WARNING, "Domino event delay value \"%s\" has wrong type", event_frames[i].domino_delay_id.c_str());
					return true;
				}
				inst->CancelAutotimerEvent(param_id);
				inst->ScheduleAutotimerEvent(param_id, logical_value, ((int&)temp)*1000);
			}
			return true;
		}
	}
	return false;
}

bool RFPhysicalDataInterfaceCommand::SetupInstance(LogicalInstance* inst)
{
	if(value_id.empty())return true;
	if(no_init)return true;
	HSSLogicalType* lt=GetParent()->GetParent()->GetLogicalType();
	XmlRpcValue val=lt->GetDefault();
	lt->EnforceConstraints(inst, &val, HSSLogicalType::OP_WRITE);
	if(!GetParent()->GetParent()->LogicalToPhysical(inst, val, &val))return false;
	inst->SetStoredValue(value_id, val, ValueStore::FLAG_VOLATILE);
	return true;
}

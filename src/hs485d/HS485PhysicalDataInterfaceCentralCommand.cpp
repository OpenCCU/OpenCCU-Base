/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HS485PhysicalDataInterfaceCentralCommand.h"
#include "HS485DeviceDescription.h"

#include <Logger.h>
#include "dynamic.h"
#include "HS485Central.h"
#include "HS485Manager.h"
#include <HS485Controller.h>

using namespace XmlRpc;

static dynamic::factory<HS485PhysicalDataInterfaceCentralCommand> HS485PhysicalDataInterfaceCentralCommandFactory;

HS485PhysicalDataInterfaceCentralCommand::HS485PhysicalDataInterfaceCentralCommand(void)
{
}

HS485PhysicalDataInterfaceCentralCommand::~HS485PhysicalDataInterfaceCentralCommand(void)
{
}

bool HS485PhysicalDataInterfaceCentralCommand::GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param)
{
    return true;
}

bool HS485PhysicalDataInterfaceCentralCommand::InitFromXml(XMLNode &node, XMLNode &root_node)
{
	if(!HS485PhysicalDataInterfaceCommand::InitFromXml(node, root_node))return false;
	const char* temp=node.getAttribute("counter");
	if(temp)counter_id=temp;
    return true;
}


bool HS485PhysicalDataInterfaceCentralCommand::PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param)
{
	bool retval=true;
	if(!value_id.empty()){
		if(!inst->SetStoredValue(value_id, param, ValueStore::FLAG_VOLATILE))return false;
	}
	if(!counter_id.empty()){
		XmlRpcValue val;
		inst->GetStoredValue(counter_id, &val);
		try{
			((int&)val)++;
			inst->SetStoredValue(counter_id, val, ValueStore::FLAG_VOLATILE);
		}catch(XmlRpcException e){
		}
	}
	for(unsigned int i=0;i!=set_request_frames.size();i++){
		HS485Central::HS485CentralChannel * ch=dynamic_cast<HS485Central::HS485CentralChannel*>(inst);
		if(!ch){
			LOG(Logger::LOG_DEBUG, "could not cast instance to HS485CentralChannel");
			return false;
		}
		FrameDescription* fd=ch->GetDevice()->GetDeviceDescription()->GetFrameDescription(set_request_frames[i].id);
		if(!fd)return false;
		if(ch){
			std::vector<std::string> peers;
			if(!ch->GetLinkPeers(&peers))return false;
			for(unsigned int i=0;i<peers.size();i++){
//				LOG(Logger::LOG_DEBUG, "peer=%s", peers[i].c_str());
				uint32_t peer_address;
				int peer_channel;
				if(!HS485Manager::GetSingleton()->ParseAddress(peers[i], &peer_address, &peer_channel)){
					retval=false;
					continue;
				}
                HS485Frame frame;
				if(!fd->InitFrame(&frame, inst, inst->GetIndex(), peer_channel)){
					retval=false;
					continue;
				}
				frame.SetReceiverAddress(peer_address);
				frame.SetCtrl(HS485Frame::CTRL_IFRAME);

				HS485CommMessage* m = HS485Controller::GetSingleton()->CreateNewMessage();
				m->SetDontDelete(true);
				m->SetCommand(HS485CommMessage::CMD_SEND);
                m->SetFrame(frame);

				if(!ch->SendMessage(m)){
					retval=false;
					delete m;
					continue;
				}
				else {
					delete m;
				}
			}
		}
	}
	return retval;
}

bool HS485PhysicalDataInterfaceCentralCommand::CheckCreationTag(const char *tag)
{
    if(strcmp("data_interface_central_command", tag)==0)return true;
    return false;
}

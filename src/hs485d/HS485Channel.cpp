/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HS485Channel.h"
#include "HS485ChannelDescription.h"
#include "HS485Device.h"
#include "HS485Manager.h"
#include "HS485Controller.h"
#include "HS485Central.h"
#include <typeinfo>
#include <Logger.h>
#include <stdio.h>

using namespace XmlRpc;

HS485Channel::HS485Channel(void)
{
	behaviour=0;
}

HS485Channel::~HS485Channel(void)
{
}

bool HS485Channel::GetParamsetValues(const std::string& key, XmlRpc::XmlRpcValue* set)
{
//	LOG(Logger::LOG_ALL, "HS485Channel::GetParamsetValues(): Getting %s for %s", key.c_str(), this->GetSerial().c_str());
	HS485Paramset* ps=GetDescription()->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	if(ps->IsLinkset()){
//		LOG(Logger::LOG_ALL, "HS485Channel::GetParamsetValues(): Setting CurParamsetPeer to %s", key.c_str());
		SetCurParamsetPeer(key);
	}else{
//		LOG(Logger::LOG_ALL, "HS485Channel::GetParamsetValues(): Setting CurParamsetPeer to empty string");
		SetCurParamsetPeer("");
	}
	return ps->Get(this, key, set);
}

bool HS485Channel::PutParamsetValues(const std::string& key, XmlRpc::XmlRpcValue& set)
{
	HS485Paramset* ps=GetDescription()->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	if(ps->IsLinkset()){
//		LOG(Logger::LOG_ALL, "HS485Channel::PutParamsetValues(): Link is set");
		SetCurParamsetPeer(key);
	}else{
//		LOG(Logger::LOG_ALL, "HS485Channel::PutParamsetValues(): Link is NOT set, setting with empty key");
		SetCurParamsetPeer("");
	}
//	LOG(Logger::LOG_ALL, "HS485Channel::PutParamsetValues(): Trying to put %s with %s", key.c_str(), set.toText().c_str());
	if(!ps->Put(this, key, set))return false;
//	LOG(Logger::LOG_ALL, "HS485Channel::PutParamsetValues(): %s with %s successfully put, now trying to flush", key.c_str(), set.toText().c_str());
	if(!parent_dev->eep_cache.Flush())return false;
//	LOG(Logger::LOG_ALL, "HS485Channel::PutParamsetValues(): Flush successful, now sending C", key.c_str(), set.toText().c_str());
	std::string msg="C";
	std::string response;
	parent_dev->SendMessage("C", &response);
	return true;
}

bool HS485Channel::GetParamsetDescription(const std::string& key, XmlRpc::XmlRpcValue* set)
{
	HS485Paramset* ps=GetDescription()->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	return ps->GetDefinition(set);
}

bool HS485Channel::GetValue(const std::string& name, XmlRpc::XmlRpcValue* val)
{
	HS485Paramset* ps=GetDescription()->GetParamset("VALUES");
	if(!ps)return false;
	SetCurParamsetPeer("");
	HSSParameter* param=ps->GetParameter(name);
	if(!param)return false;
	return param->GetValue(this, val);
}

bool HS485Channel::SetValue(const std::string& name, XmlRpc::XmlRpcValue& val)
{
	HS485Paramset* ps=GetDescription()->GetParamset("VALUES");
	if(!ps)return false;
	SetCurParamsetPeer("");
	HSSParameter* param=ps->GetParameter(name);
	if(!param)return false;
	return param->SetValue(this, val);
}


bool HS485Channel::GetParamsetId(const std::string& key, std::string* id)
{
	HS485Paramset* ps=GetDescription()->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	*id=ps->GetId();
	return true;
}

void HS485Channel::ReportServiceMessage(const std::string& id, XmlRpc::XmlRpcValue& val){
	HS485Manager::GetSingleton()->ReportServiceMessage(GetSerial(), id, val);
}

void HS485Channel::ReportEvent(const std::string& id, XmlRpc::XmlRpcValue& val, uint32_t burst_suppression/*=0*/)
{
	std::string addr=HS485Manager::BuildStringAddress(parent_dev->GetSerial(), index);
	HS485Manager::GetSingleton()->ReportEvent(addr, id, val);
}

void HS485Channel::SetParent(HS485Device* parent, int index, HS485ChannelDescription* desc)
{
	this->parent_dev=parent;
	this->index=index;
	this->description=desc;
	serial=HS485Manager::GetSingleton()->BuildStringAddress(parent->GetSerial(), index);
	GetDescription()->SetupInstance(this);
}

void HS485Channel::ProcessIncomingFrame(HS485Frame& frame, FrameDescription* fd)
{
	GetDescription()->ProcessIncomingFrame(this, frame, fd);
}

bool HS485Channel::SetEnforcedParameters()
{
//	LOG(Logger::LOG_DEBUG, "HS485Channel::SetEnforcedParameters() typeid(*this)=%s", typeid(*this).name());
	if(!description->SetEnforcedParameters(this))return false;
	return true;
}

bool HS485Channel::SendMessage(const std::string& msg, std::string* response)
{
	return parent_dev->SendMessage(msg, response);
}

bool HS485Channel::GetLinkPeers(std::vector<std::string>* peers)
{
	if(!GetDescription()->HasLinkPeers())return true;
	return GetDescription()->GetLinkPeers(this, peers);
}

bool HS485Channel::GetLinks(int flags, link_map_t* result)
{
	if(!GetDescription()->HasLinkPeers())return true;
	HS485LogicalInstance* sender=NULL;
	HS485LogicalInstance* receiver=NULL;
	std::string sender_serial;
	std::string receiver_serial;
	bool is_sender=GetDescription()->GetDirection()==HS485ChannelDescription::DIRECTION_SENDER;
	if(is_sender){
		sender=this;
		sender_serial=GetSerial();
	}else{
		receiver=this;
		receiver_serial=GetSerial();
	}

	std::vector<std::string> peers;
	if(!GetLinkPeers(&peers))return false;
	for(unsigned int i=0;i<peers.size();i++){
		HS485Channel* peer=dynamic_cast<HS485Channel*>(HS485Manager::GetSingleton()->GetInstance(peers[i]));
		if(!peer){
			LOG(Logger::LOG_WARNING, "Unknown peer %s", peers[i].c_str());
		}
		if(peer && peer->GetDescription()->IsHidden())continue;
		if(is_sender){
			receiver=peer;
			receiver_serial=peers[i];
		}else{
			sender=peer;
			sender_serial=peers[i];
		}

		//avoid doubles
		std::string link_key;
		link_key=sender_serial+"->"+receiver_serial;

		link_map_t::iterator result_it=result->find(link_key);
		XmlRpcValue& link=(*result)[link_key];

		t_link_info& li=link_info_map[peers[i]];
		if(((std::string&)link["NAME"]).empty())link["NAME"]=li.name;
		if(((std::string&)link["DESCRIPTION"]).empty())link["DESCRIPTION"]=li.description;

		if(result_it!=result->end()){
			//OK, we already have this link on our list. Just mark our side of the link as valid.
			((int&)result_it->second["FLAGS"])&= ~(is_sender?LINK_FLAG_SENDER_INVALID:LINK_FLAG_RECEIVER_INVALID);
			continue;
		}

		link["SENDER"]=sender_serial;
		link["RECEIVER"]=receiver_serial;

		link["NAME"]=li.name;
		link["DESCRIPTION"]=li.description;
		if( peer && ((flags&LogicalInstance::GL_FLAG_CHECK_PEER) || peer->GetDescription()->LinksAreVirtual()) && peer->IsLinkedTo(GetSerial())){
			(int&)link["FLAGS"]=0;
		}else{
			(int&)link["FLAGS"]=is_sender?LINK_FLAG_RECEIVER_INVALID:LINK_FLAG_SENDER_INVALID;
		}
		if(!sender)(int&)link["FLAGS"]|=LINK_FLAG_SENDER_UNKNOWN;
		if(!receiver)(int&)link["FLAGS"]|=LINK_FLAG_RECEIVER_UNKNOWN;
		if(sender && receiver){
			if(flags & GL_FLAG_SENDER_PARAMSET){
				XmlRpcValue& paramset=link["SENDER_PARAMSET"];
				paramset.assertStruct();
				try{
					sender->GetParamsetValues(receiver->GetSerial(), &paramset);
				}catch(XmlRpcException){
					(int&)link["FLAGS"] |= LINK_FLAG_SENDER_INVALID;
				}
			}
			if(flags & GL_FLAG_RECEIVER_PARAMSET){
				XmlRpcValue& paramset=link["RECEIVER_PARAMSET"];
				paramset.assertStruct();
				try{
					receiver->GetParamsetValues(sender->GetSerial(), &paramset);
				}catch(XmlRpcException){
					(int&)link["FLAGS"] |= LINK_FLAG_RECEIVER_INVALID;
				}
			}
		}
		if(sender && (flags & GL_FLAG_SENDER_DESCRIPTION)){
			XmlRpcValue& description=link["SENDER_DESCRIPTION"];
			description.assertStruct();
			sender->Describe(&description);
		}
		if(receiver && (flags & GL_FLAG_RECEIVER_DESCRIPTION)){
			XmlRpcValue& description=link["RECEIVER_DESCRIPTION"];
			description.assertStruct();
			receiver->Describe(&description);
		}
	}
	return true;
}

bool HS485Channel::RemoveLinkPeer(const std::string& peer)
{
	if(!GetDescription()->RemoveLinkPeer(this, peer))return false;
	link_info_map.erase(peer);
	bool retval=parent_dev->eep_cache.Flush();
	DeleteStoredValues(peer);
	RequestSave();
	return retval;
}

bool HS485Channel::AddLinkPeer(const std::string& peer)
{
	if(!GetDescription()->AddLinkPeer(this, peer))return false;
	link_info_map[peer];
	bool retval=parent_dev->eep_cache.Flush();
	RequestSave();
	return retval;
}

bool HS485Channel::IsLinkedTo(const std::string& peer)
{
	std::vector<std::string> peers;
	if(!GetLinkPeers(&peers))return false;
	for(unsigned int i=0;i<peers.size();i++){
		if(peers[i]==peer)return true;
	}
	return false;
}

bool HS485Channel::SetLinkInfo(const std::string& peer, const std::string& name, const std::string& description)
{
	link_info_map[peer].name=name;
	link_info_map[peer].description=description;
	RequestSave();
	return true;
}

bool HS485Channel::GetLinkInfo(const std::string& peer, std::string* name, std::string* description)
{
	t_link_info_map::iterator it=link_info_map.find(peer);
	if(it==link_info_map.end())return false;
	*name=it->second.name;
	*description=it->second.description;
	return true;
}

bool HS485Channel::Describe(XmlRpc::XmlRpcValue* val)
{
	bool is_array=false;
	if(val->getType()==XmlRpcValue::TypeArray){
		is_array=true;
		val->setSize(1);
	}

	XmlRpcValue& dev_desc=is_array?(*val)[0]:*val;

	dev_desc["ADDRESS"]=GetSerial();
	dev_desc["TYPE"]=GetDescription()->GetType();
	dev_desc["INDEX"]=GetIndex();
	dev_desc["PARENT_TYPE"]=GetDevice()->GetType();
	dev_desc["PARENT"]=GetDevice()->GetSerial();
	dev_desc["PARAMSETS"].assertArray(0);
	dev_desc["LINK_SOURCE_ROLES"]=GetDescription()->GetLinkSourceRoles();
	dev_desc["LINK_TARGET_ROLES"]=GetDescription()->GetLinkTargetRoles();
	dev_desc["DIRECTION"]=GetDescription()->GetDirection();
	dev_desc["AES_ACTIVE"]=false;
	dev_desc["VERSION"]=GetDevice()->GetDeviceDescription()->GetVersion();
	dev_desc["FLAGS"]=GetDescription()->GetFlags();
	GetDescription()->ListParamsets(&(dev_desc["PARAMSETS"]));
	return GetDescription()->GetAdditionalDescription()->Describe(&dev_desc);
}

bool HS485Channel::UnpeerCentral()
{
	for(t_link_info_map::iterator it=link_info_map.begin();it!=link_info_map.end();it++){
		HS485Central::HS485CentralChannel * peer_channel=dynamic_cast<HS485Central::HS485CentralChannel*>(HS485Manager::GetSingleton()->GetInstance(it->first));
		if(!peer_channel)continue;
		peer_channel->RemoveLinkPeer(GetSerial());
	}
	return true;
}

bool HS485Channel::SaveToXml(XMLNode* node)
{
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "%d", GetIndex());
	node->addAttributeConst("index", buffer);
	snprintf(buffer, sizeof(buffer), "%d", behaviour);
	node->addAttributeConst("behaviour", buffer);
	node->addAttributeConst("type", GetDescription()->GetType().c_str());
	XMLNode values_node=node->addChildConst("values");
	if(!ValueStore::SaveToXml(&values_node))return false;

	for(t_value_usage_map::iterator it=value_usage_map.begin();it!=value_usage_map.end();it++){
		XMLNode usage_node=node->addChildConst("value_usage");
		usage_node.addAttributeConst("id", it->first.c_str());
		snprintf(buffer, sizeof(buffer), "%d", it->second);
		usage_node.addAttributeConst("refcount", buffer);
	}

	XMLNode links_node=node->addChildConst("links");
	for(t_link_info_map::iterator it=link_info_map.begin();it!=link_info_map.end();it++){
		XMLNode link_node=links_node.addChildConst("link");
		link_node.addAttributeConst("peer", it->first.c_str());
		link_node.addAttributeConst("name", it->second.name.c_str());
		XMLNode description_node=link_node.addChildConst("description");
		description_node.addTextConst(it->second.description.c_str());
	}

	return true;
}

bool HS485Channel::LoadFromXml(XMLNode& node)
{
	const char* temp;
	int i=0;
	temp=node.getAttribute("behaviour");
	if(temp)behaviour=strtol(temp, NULL, 0);
	XMLNode usage_node=node.getChildNode("value_usage", &i);
	while(!usage_node.isEmpty()){
		temp=usage_node.getAttribute("refcount");
		if(!temp)return false;
		int count=strtol(temp, NULL, 0);
		temp=usage_node.getAttribute("id");
		if(!temp)return false;
		value_usage_map[temp]=count;
		usage_node=node.getChildNode("value_usage", &i);
	}
	XMLNode values_node=node.getChildNode("values");
	if(!values_node.isEmpty()){
		if(!ValueStore::LoadFromXml(values_node))return false;
	}
	XMLNode links_node=node.getChildNode("links");
	i=0;
	XMLNode link_node=links_node.getChildNode("link", &i);
	while(!link_node.isEmpty()){
		std::string peer;
		temp=link_node.getAttribute("peer");
		if(!temp)return false;
		peer=temp;
		temp=link_node.getAttribute("name");
		if(!temp)return false;
		link_info_map[peer].name=temp;
		XMLNode description_node=link_node.getChildNode("description");
		temp=description_node.getText();
		if(temp)link_info_map[peer].description=temp;
		link_node=links_node.getChildNode("link", &i);
	}
	return true;
}

void HS485Channel::RequestSave()
{
	GetDevice()->RequestSave();
}

bool HS485Channel::SendMessage(HS485CommMessage* msg)
{
	return GetDevice()->SendMessage(msg);
}

bool HS485Channel::ReportValueUsage(const std::string& value, int count)
{
	if(!GetDescription()->GetAutoregisterCentral())return true;
	value_usage_map[value]=count;
	int total_count=0;
	for(t_value_usage_map::iterator it=value_usage_map.begin();it!=value_usage_map.end();it++){
		if(it->second > 0)total_count+=it->second;
	}
	bool retval=true;
	HS485Central* central_device=HS485Central::GetSingleton();
	if(!central_device){
		LOG(Logger::LOG_ERROR, "No central device defined");
	}
	std::string listener_channel_id=central_device->GetListenerChannel()->GetSerial();
	if(total_count && !IsLinkedTo(listener_channel_id)){
		retval=AddLinkPeer(listener_channel_id);
		central_device->GetListenerChannel()->AddLinkPeer(GetSerial());
	}else if(!total_count && IsLinkedTo(listener_channel_id)){
		retval=RemoveLinkPeer(listener_channel_id);
		central_device->GetListenerChannel()->RemoveLinkPeer(GetSerial());
	}
	return retval;
}

bool HS485Channel::SendToPeers(HS485CommMessage* msg, unsigned int receiver_channel_field)
{
	bool retval=true;
	std::vector<std::string> peers;
	if(!GetLinkPeers(&peers))return false;
	HS485Device* central_device=dynamic_cast<HS485Device*>(HS485Central::GetSingleton());
	for(unsigned int i=0;i<peers.size();i++){
		HS485Channel* peer=dynamic_cast<HS485Channel*>(HS485Manager::GetSingleton()->GetInstance(peers[i]));
		if(!peer)continue;
		if(peer->GetDevice()==central_device)continue;
/*		HS485CommMessage m=*msg;
		m.SetDontDelete(true);

		HS485Frame hs485Frame = m.ExtractFrame();
		hs485Frame.SetIntValue(receiver_channel_field, peer->GetPhysicalIndex());
		hs485Frame.SetReceiverAddress(peer->GetDevice()->GetAddress());
		hs485Frame.SetSenderAddress(GetDevice()->GetAddress());
		m.SetFrame( hs485Frame );

		m.TransformToSimulationMessage();
		retval&=HS485Controller::GetSingleton()->SendMessage(&m);*/
		//Why copying it? destroys child class type.

		//msg->SetDontDelete(true); //maybe caller of this method had set that. we should not override that here

		HS485Frame hs485Frame = msg->ExtractFrame();
		hs485Frame.SetIntValue(receiver_channel_field, peer->GetPhysicalIndex());
		hs485Frame.SetReceiverAddress(peer->GetDevice()->GetAddress());
		hs485Frame.SetSenderAddress(GetDevice()->GetAddress());
		msg->SetFrame( hs485Frame );

		msg->TransformToSimulationMessage();
		retval&=HS485Controller::GetSingleton()->SendMessage(msg);
	}
	if(!msg->GetDontDelete())delete msg;
	return retval;
}

bool HS485Channel::ActivateLinkParamset(const std::string& peer, bool longpress)
{
	HS485Channel* peer_channel=dynamic_cast<HS485Channel*>(HS485Manager::GetSingleton()->GetInstance(peer));
	if(!peer_channel)return false;

	FrameDescription* fd=GetDevice()->GetDeviceDescription()->GetFrameDescription(GetDescription()->GetTestParamsetFrame(longpress));
	if(!fd){
		return false;
	}
    HS485Frame frame;
	if(!fd->InitFrame(&frame, this, GetPhysicalIndex(), peer_channel->GetPhysicalIndex())){
		return false;
	}
    frame.SetCtrl(HS485Frame::CTRL_IFRAME);
    frame.SetReceiverAddress(peer_channel->GetDevice()->GetAddress());
	frame.SetSenderAddress(GetDevice()->GetAddress());

	HS485CommMessage* m = HS485Controller::GetSingleton()->CreateNewMessage();//FIXME check for memory leaks... (was locally instantiated)
	m->SetDontDelete(true);
	m->SetCommand(HS485CommMessage::CMD_SEND);
	m->SetCollectResponses(false);
    m->SetFrame(frame);
	m->TransformToSimulationMessage();
	bool done = HS485Controller::GetSingleton()->SendMessage(m);
	delete m;
	return done;
}

bool HS485Channel::SetBehaviour(int b)
{
	if(behaviour==b)return true;
	HSSParameter* p=description->GetBehaviourParam();
	if(!p){
		LOG(Logger::LOG_WARNING, "Don't know how to set channel behaviour");
		return false;
	}
	SetCurParamsetIndex(GetEEPromIndex());
	XmlRpcValue v=b;
	if(!p->SetValue(this, v)){
		LOG(Logger::LOG_WARNING, "Could not set channel behaviour");
		return false;
	}
	behaviour=b;
	HS485Manager::GetSingleton()->ReportNewDevice(this);
	RequestSave();
	return true;
}

int HS485Channel::GetBehaviour()
{
	HSSParameter* p=description->GetBehaviourParam();
	if(!p){
		LOG(Logger::LOG_WARNING, "Don't know how to get channel behaviour");
		return behaviour;
	}
	try{
		SetCurParamsetIndex(GetEEPromIndex());
		XmlRpcValue v;
		if(!p->GetValue(this, &v)){
			LOG(Logger::LOG_WARNING, "Could not get channel %d behaviour", GetIndex());
			return behaviour;
		}
		behaviour=(int&)v;
	}catch(XmlRpcException e){
	}
	return behaviour;
}

bool HS485Channel::SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event/*=false*/)
{
	try{
		if(name=="BEHAVIOUR"){
			SetBehaviour((int&)val);
		}else{
			LOG(Logger::LOG_WARNING, "Tried to set unknown internal value %s=%s", name.c_str(), val.toText().c_str());
		}
	}catch(XmlRpcException& e){
		return false;
	}
	if(fire_event)this->SendInternalValueEvent(name, val);
	return true;
};

bool HS485Channel::GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val)
{
	try{
		if(name=="BEHAVIOUR"){
			(int&)(*val)=GetBehaviour();
		}else{
			LOG(Logger::LOG_WARNING, "Tried to get unknown internal value %s", name.c_str());
		}
	}catch(XmlRpcException& e){
		LOG(Logger::LOG_WARNING, "GetInternalValue() exception %s", e.getMessage().c_str());
		return false;
	}
	return true;
};

bool HS485Channel::GetConfig(XmlRpc::XmlRpcValue* c)
{
	XmlRpcValue paramsets;
	if(!GetDescription()->ListParamsets(&paramsets))return false;
	(*c)["PARAMSETS"];
	for(int i=0;i<paramsets.size();i++){
		if(paramsets[i]!="LINK" && paramsets[i]!="VALUES"){
			(*c)["PARAMSETS"][(std::string&)paramsets[i]].assertStruct();
			if(!GetParamsetValues(paramsets[i], &((*c)["PARAMSETS"][(std::string&)paramsets[i]])))return false;
		}
	}
	(*c)["LINKS"].assertArray(0);
	std::vector<std::string> peers;
	if(!GetLinkPeers(&peers))return false;
	for(unsigned int i=0;i<peers.size();i++){
		std::string name;
		std::string description;
		if(!GetLinkInfo(peers[i], &name, &description))return false;
		(*c)["LINKS"][i]["PEER"]=peers[i];
		(*c)["LINKS"][i]["NAME"]=name;
		(*c)["LINKS"][i]["DESCRIPTION"]=description;
		if(!GetParamsetValues(peers[i], &((*c)["PEERS"][i]["PARAMSET"])))return false;
	}
	return true;
}

bool HS485Channel::RestoreConfig(XmlRpc::XmlRpcValue& c)
{
	XmlRpcValue paramsets;
	if(!GetDescription()->ListParamsets(&paramsets))return false;
	for(int i=0;i<paramsets.size();i++){
		if(paramsets[i]!="LINK"){
			LOG(Logger::LOG_ALL, "HS485Channel::RestoreConfig(): Trying to set paramset %s",paramsets[i].toText().c_str());
			if(!PutParamsetValues(paramsets[i], c["PARAMSETS"][(std::string&)paramsets[i]]))return false;
		}
	}
	LOG(Logger::LOG_ALL, "HS485Channel::RestoreConfig(): Restoring links.");
	for(int i=0;i<c["LINKS"].size();i++){
		std::string peer=c["LINKS"][i]["PEER"];
		std::string name=c["LINKS"][i]["NAME"];
		std::string description=c["LINKS"][i]["DESCRIPTION"];
		LOG(Logger::LOG_ALL, "HS485Channel::RestoreConfig(): Trying to add peer %s",peer.c_str());
		if(!AddLinkPeer(peer))return false;
		LOG(Logger::LOG_ALL, "HS485Channel::RestoreConfig(): Peer added %s, trying to set link info",peer.c_str());
		if(!SetLinkInfo(peer, name, description))return false;
		LOG(Logger::LOG_ALL, "HS485Channel::RestoreConfig(): Link info set for %s, trying to put paramset values", peer.c_str());
		if(!PutParamsetValues(peer, c["PEERS"][i]["PARAMSET"]))return false;
		LOG(Logger::LOG_ALL, "HS485Channel::RestoreConfig(): Paramset values put successfully for %s",peer.c_str());
	}
	return true;
}

bool HS485Channel::replacePeer(const std::string& oldPeerSerial, const std::string& newPeerSerial)
{
//getlink
	return false;
}

void HS485Channel::SetSerial(const std::string& serial)
{
	this->serial = serial;
}

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFChannel.h"
#include "RFChannelDescription.h"
#include "RFDevice.h"
#include "RFManager.h"
#include <typeinfo>
#include <Logger.h>
#include "RFController.h"
#include "RFCentral.h"
#include "BidcosFrameLinkPeerReq.h"
#include "BidcosFrameDecoder.h"
#include "RFTeam.h"
#include <stdio.h>
#include <HM2Utils.h>
#include <JSONObject.h>
#include <HSSLogicalType.h>
#include <openssl/evp.h>

#define AES

#define MAX_SCHEDULED_GET_CNT 10
static const uint32_t SCHEDULED_GET_WAIT_MIN=2000;
static const uint32_t SCHEDULED_GET_WAIT_RANGE=2000;

using namespace XmlRpc;

RFChannel::RFChannel(void)
:parent_dev(0)
,description(0)
,index(0)
,event_store(true)
{
	link_peers_valid=false;
	link_peers_dirty=false;
	config_data_dirty=false;
	aes=false;
	ValueStore::hold_timestamps=true;
	team_channel=NULL;
	link_peers.clear();
	aes_cbc_counter = 1;
	behaviour = 0;
	behaviourChangePending = false;
}

RFChannel::~RFChannel(void)
{
	if(team_channel)team_channel->RemoveTeamChannel(this);
}

bool RFChannel::GetParamsetValues(const std::string& key, XmlRpc::XmlRpcValue* set)
{
	RFParamset* ps=GetDescription()->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	return ps->Get(this, key, set);
}
bool RFChannel::GetParamsetValues(const std::string& key,int mode, XmlRpc::XmlRpcValue* set)
{
	RFParamset* ps=GetDescription()->GetParamset(key);
		if(!ps){
			throw XmlRpcException("Unknown paramset", -3);
		}
		return ps->Get(this, key, set,mode);
}
bool RFChannel::PutParamsetValues(const std::string& key, XmlRpc::XmlRpcValue& set)
{
	RFParamset* ps=GetDescription()->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	if(!ps->Put(this, key, set))return false;
	RFConfigData& cd=config_data[cur_paramset_peer];
	if(!cd.CommitToDevice(this)){
		config_data_dirty=true;
		GetDevice()->CheckConfigPendingEvent();
	}
	if(key != "VALUES" || ValueStore::IsDirty()) {
		RequestSave();	
	} 
	return true;
}

bool RFChannel::GetParamsetDescription(const std::string& key, XmlRpc::XmlRpcValue* set)
{

	RFParamset* ps=GetDescription()->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	return ps->GetDefinition(set);
}

bool RFChannel::GetParamsetId(const std::string& key, std::string* id)
{
	RFParamset* ps=GetDescription()->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	*id=ps->GetId();
	return true;
}

bool RFChannel::DetermineParameter(const std::string& key, const std::string& parameter)
{
	RFParamset* ps=GetDescription()->GetParamset(key);
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	return ps->Determine(this, key, parameter);
}

bool RFChannel::GetValue(const std::string& name, XmlRpc::XmlRpcValue* val)
{
	RFParamset* ps=GetDescription()->GetParamset("VALUES");
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	HSSParameter* param=ps->GetParameter(name);
	if(!param){
		throw XmlRpcException("Unknown parameter", -5);
	}
	return param->GetValue(this, val);
}
bool RFChannel::ReadValue(const std::string& name,int mode, XmlRpc::XmlRpcValue* val)
{
	RFParamset* ps = GetDescription()->GetParamset("VALUES");
	if (!ps) {
		throw XmlRpcException("Unknown paramset", -3);
	}
	HSSParameter* param = ps->GetParameter(name);
	if (!param) {
		throw XmlRpcException("Unknown parameter", -5);
	}
	if(!param->IsReadable())
	{
		throw XmlRpcException("Parameter is not readable", -1);
	}
	if(mode == 1)
	{
		return param->GetValue(this,mode,val);
	}
	else
	{
		return param->GetValue(this, val);
    }
}
bool RFChannel::SetValue(const std::string& name, XmlRpc::XmlRpcValue& val)
{
	RFParamset* ps=GetDescription()->GetParamset("VALUES");
	if(!ps){
		throw XmlRpcException("Unknown paramset", -3);
	}
	HSSParameter* param=ps->GetParameter(name);
	if(!param){
		throw XmlRpcException("Unknown parameter", -5);
	}
	SetCurParamsetPeer(0);
	if(param->HasWriteDependencies()) {
		return setValueWithWriteDependencies(param, val);
	}
	return param->SetValue(this, val);
}

bool RFChannel::setValueWithWriteDependencies(HSSParameter* param, XmlRpc::XmlRpcValue& value)
{
		//Parse value
	JSONObject jsonObj(value.toText());
	if(!jsonObj.isValid()) {
		//TODO Throw a nice XmlRpcException here
	}
	HSSParamset* pParamset = param->GetParamset();
	//Assemble key/value set
	//XmlRpc::XmlRpcValue set;
	//set.assertStruct();
	const std::vector<HSSParameter::WriteDependency>& writeDepends = param->getWriteDependencies();
	std::string commandParamId;
	for(unsigned int i = 0; i < writeDepends.size(); i++) {
		HSSParameter::WriteDependency depend = writeDepends.at(i);
		HSSParameter* pParam = pParamset->GetParameter(depend.name);
		if(pParam != NULL) {
			if(jsonObj.contains(depend.name)) {
				if(depend.interfaceCommandParam) {
					commandParamId = depend.name;
					continue;
				}
				else {
					HSSLogicalType* pLType = pParam->GetLogicalType();
					if(!callSetValue(jsonObj, depend.name, pLType)) {
						return false;
					}
				}
			}
			else {
				std::string msg("Missing parameter ");
				msg.append(depend.name);
				throw XmlRpc::XmlRpcException(msg);
			}	
		}
		else {
			throw XmlRpc::XmlRpcException("Dependency parameter not found.");
		}
	}
	if(!commandParamId.empty()) {
		HSSParameter* pParam = pParamset->GetParameter(commandParamId);
		if(pParam != NULL) {
			HSSLogicalType* pLType = pParam->GetLogicalType();
			callSetValue(jsonObj, commandParamId, pLType);
		}
		else {
			throw XmlRpc::XmlRpcException("Command parameter not found");
		}
	}
	else {
		throw XmlRpc::XmlRpcException("Command parameter not found.");
	}

//	set[param->GetId()] = value[param->GetId()];
	//Call PutParamsetValues
//	return this->PutParamsetValues("VALUES", set);
	return true;
}

bool RFChannel::callSetValue(const JSONObject& jsonObj, const std::string& paramName, HSSLogicalType* pLType) 
{
	XmlRpc::XmlRpcValue val;
	if(pLType != NULL) {
		if(strcmp("string", pLType->GetType().c_str()) == 0) {
			val = jsonObj.getString(paramName);
		}
		else if(strcmp("integer", pLType->GetType().c_str()) == 0) {
			val = jsonObj.getInt(paramName);
		}
		if(strcmp("float", pLType->GetType().c_str()) == 0) {
			val = jsonObj.getDouble(paramName);
		}
		if(strcmp("boolean", pLType->GetType().c_str()) == 0) {
			val = jsonObj.getBool(paramName);
		}
		else {
			//TODO Throw exception
		}
		if(!this->SetValue(paramName, val)) {
			return false;
		}
	}
	return true;
}

void RFChannel::ReportServiceMessage(const std::string& id, XmlRpc::XmlRpcValue& val){
	RFManager::GetSingleton()->ReportServiceMessage(GetSerial(), id, val);
}

void RFChannel::ReportEvent(const std::string& id, XmlRpc::XmlRpcValue& val, uint32_t burst_suppression/*=0*/)
{
	if(burst_suppression){
		XmlRpcValue stored_val;
		uint32_t age;
		if(event_store.GetStoredValue(id, &stored_val, &age)){
			if(stored_val==val && age<burst_suppression){
//				LOG(Logger::LOG_DEBUG, "Event suppressed %s.%s=%s", serial.c_str(), id.c_str(), val.toText().c_str());
				return;
			}
		}
		event_store.SetStoredValue(id, val);
	}
	RFManager::GetSingleton()->ReportEvent(GetSerial(), id, val);
}

void RFChannel::SetParent(RFDevice* parent, int index, RFChannelDescription* desc)
{
	this->parent_dev=parent;
	this->index=index;
	this->description=desc;
	this->aes=GetDescription()->GetAESDefault();
	SetSerial(parent->GetSerial());
	GetDescription()->SetupInstance(this);
}

bool RFChannel::ProcessIncomingFrame(BidcosFrame& msg, FrameDescription* fd)
{
	if(GetAES() && GetDescription()->IsAesCbcSupported()) {
		//CBC Authentication, intially for weather telegrams
		//If authentication succeeds, process the msg, otherwise return.
		if( (!msg.WasAuthenticated()) && (!performCBCAuthentification(msg)) ) { //TWIST-1106: Accept already authenticated (by challenge response) too
			return false;
		}
		GetDescription()->ProcessIncomingFrame(this, msg, fd);
		return true;
	}
	else {
		GetDescription()->ProcessIncomingFrame(this, msg, fd);
		return ( (!GetAES()) || msg.WasAuthenticated() );
	}
}

void RFChannel::ProcessForwardedFrame(BidcosFrame& msg, FrameDescription* fd)
{
	GetDescription()->ProcessForwardedFrame(this, msg, fd);
}

bool RFChannel::performCBCAuthentification(BidcosFrame& bidcosFrame)
{
	if(GetDevice()->GetAESKey() == 0) {//Ensure backwards compatibility if using default key.
		return true;
	}
	const int frameSize = bidcosFrame.GetSize();
	if(frameSize >= 15) {  //15 bytes <-> 9 Byte telegram header + 2 byte counter (high and middle byte) + 4 byte MIC + sensordata ? bytes
		//- Check wether the counter is greater than the last one
		
		uint32_t tempI;
		//extract 3 byte counter
		unsigned int devCounter = 0;
		bidcosFrame.GetIntValue(frameSize-6, 0, 1, 0, &tempI);//high byte
		devCounter = (((unsigned int)tempI) & 0xFF);;
		devCounter = devCounter << 8; 
		bidcosFrame.GetIntValue(frameSize-5, 0, 1, 0, &tempI);//mid byte
		devCounter += (((unsigned int)tempI) & 0xFF);
		devCounter = devCounter << 8; 
		bidcosFrame.GetIntValue(0, 0, 1, 0, &tempI);//low byte
		devCounter += (((unsigned int)tempI) & 0xFF);
		//compare counter
		if(devCounter > getAesCbcCounter()) {
			const int devKeyIndex = GetDevice()->GetAESKey();
			if(RFManager::GetSingleton()->map_aes_keys.find(devKeyIndex) == RFManager::GetSingleton()->map_aes_keys.end()) {
				return false;
			}
			const std::string devKey = RFManager::GetSingleton()->map_aes_keys[devKeyIndex];
				//- Check integrity of message:
			//->  Calculate CBC MAC of received message
			//->  Compare calculated CBC MAC and CBC MAC of the message
			//-->    Assemble b0 and encrypt it
			std::string b0(1, 0x49);
			std::string tempStr = HM2::convertBidcosAddressToBigEndianString( bidcosFrame.GetSenderAddress() );
			b0.append(tempStr.c_str(), tempStr.size());
			tempStr = HM2::convertBidcosAddressToBigEndianString( bidcosFrame.GetReceiverAddress() );
			b0.append(tempStr.c_str(), tempStr.size());
			tempStr = HM2::convertBidcosAddressToBigEndianString( devCounter );//only 3 LSB are returned
			b0.append(tempStr.c_str(), tempStr.size());//3 byte counter
			b0.append(5, (char)0x00);//4 byte + high byte of length
			b0.append(1, (char)0x05);//low byte of length
			//LOG(Logger::LOG_ALL, "b0 (length: %d): %s", b0.size(), HM2::toDebugHexStr(b0).c_str());
			EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
			EVP_EncryptInit_ex( ctx, EVP_aes_128_ecb(), NULL, (unsigned char*)devKey.c_str(), NULL);
			// raw single-block ECB, no PKCS7 padding (CBC-MAC)
			EVP_CIPHER_CTX_set_padding( ctx, 0 );
			unsigned char* encryptBuffer = new unsigned char[16];
			int encSize = 0;
			EVP_EncryptUpdate(ctx, encryptBuffer, &encSize, (unsigned char*)b0.c_str(), 16);
			
			//Assemble b1 
			int userDataSize = bidcosFrame.GetSize() - 15; //6 byte counter and mic + 9 byte hm header
			if(userDataSize < 0) {
				return false;
			}
			else if(userDataSize > 14) {
				userDataSize = 14;//truncate if too long
			}
			std::string b1;
			b1.append(1, bidcosFrame.GetByteData(0));//telegram counter
			b1.append(1, bidcosFrame.GetByteData(1)); //flags
			for(int i = 0; i < userDataSize; i++) {
				b1.append(1, bidcosFrame.GetByteData(i+9));//9 is first index of user data.
			}
			//fill up with 0 if userdate too short
			if(userDataSize < (int)14) {
				b1.append(14-userDataSize, (char)0x00);//16 byte b1 length - 2 byte for a part of hm header data.
			}
			//LOG(Logger::LOG_ALL, "b1 (length: %d): %s", b0.size(), HM2::toDebugHexStr(b1).c_str());
			//Xor encryptBuffer with b1 and encrypt the result
			for(unsigned int i = 0; i < 16; i++) {
				encryptBuffer[i] = encryptBuffer[i] ^ b1.at(i);
			}
			
			unsigned char* encryptedStr = new unsigned char[16];
			EVP_EncryptUpdate(ctx, encryptedStr, &encSize, encryptBuffer, 16);
			EVP_CIPHER_CTX_free( ctx );
			
			bool authenticated = true;
			for(unsigned int i = 0; i < 4; i++) {
				if( encryptedStr[12+i] != bidcosFrame.GetByteData(frameSize-4+i) ) {
					//LOG(Logger::LOG_ALL, "a: %2X b: %2X", encryptedStr[12+i], bidcosFrame.GetByteData(frameSize-4+i));
					authenticated = false;
				}
			}
			delete[] encryptBuffer;
			delete[] encryptedStr;
			if(authenticated) {
				LOG(Logger::LOG_ALL, "RF-Channel. Data authentification succeeded.");
				setAesCbcCounter(devCounter);//counter incrementation is allowed on successful auth only to prevent DoS attacks. 
				bidcosFrame.SetAuthKey(devKeyIndex);
			}
			else {
				LOG(Logger::LOG_ALL, "RF-Channel. Data authentification failed.");
			}
			return authenticated;
		}
		else {
			return false;
		}
	}
	return false;
}

bool RFChannel::SetEnforcedParameters()
{
//	LOG(Logger::LOG_DEBUG, "RFChannel::SetEnforcedParameters() typeid(*this)=%s", typeid(*this).name());
	if(!GetDescription()->SetEnforcedParameters(this))return false;
	return true;
}

bool RFChannel::SendFrame(BidcosFrame* frame)
{
	bool success = parent_dev->SendFrame(frame);
	if(!success) {
		checkAndFireNAKErrorEvent(frame);
	}
	return success;
}

void RFChannel::checkAndFireNAKErrorEvent(BidcosFrame* requestFrame)
{
	if(RFManager::GetSingleton()->FireNACKErrorEventEnabled()) {
		const unsigned int responseCount = requestFrame->GetResponseCount();
		if(responseCount > 0) {
			BidcosFrame* response = requestFrame->GetResponse( (responseCount-1) );
			if(response->IsNack()) {
				int responseFrameNACKType = (response->GetByteData(9));
				XmlRpcValue errorVal;
				switch(responseFrameNACKType) {
					case BidcosFrame::FT_NACK >> 16:
						LOG(Logger::LOG_ALL, "RFChannel::fireNAKErrorEvent(): FT_NACK");
						//1, Unknown Error
						errorVal[0] = 1;
						errorVal[1] = "Unknown Error";
						RFManager::GetSingleton()->ReportEvent(serial, XmlRpcValue("ERROR"), errorVal);
						break;
					case BidcosFrame::FT_NACK_BUSY >> 16:
						LOG(Logger::LOG_ALL, "RFChannel::fireNAKErrorEvent(): FT_NACK_BUSY.");
						//2, Busy
						errorVal[0] = 2;
						errorVal[1] = "Busy";
						RFManager::GetSingleton()->ReportEvent(serial, XmlRpcValue("ERROR"), errorVal);
						break;
					case BidcosFrame::FT_NACK_MEMFULL >> 16:
						LOG(Logger::LOG_ALL, "RFChannel::fireNAKErrorEvent(): FT_NACK_MEMFULL.");
						//3, MemFull
						errorVal[0] = 3;
						errorVal[1] = "MemFull";
						RFManager::GetSingleton()->ReportEvent(serial, XmlRpcValue("ERROR"), errorVal);
						break;
					case BidcosFrame::FT_NACK_MEMFULL_PART >> 16:
						LOG(Logger::LOG_ALL, "RFChannel::fireNAKErrorEvent(): FT_NACK_MEMFULL_PART.");
						//4, MemFull
						errorVal[0] = 3;
						errorVal[1] = "MemFull";
						RFManager::GetSingleton()->ReportEvent(serial, XmlRpcValue("ERROR"), errorVal);
						break;
					case BidcosFrame::FT_NACK_TARGET_INVALID >> 16:
						LOG(Logger::LOG_ALL, "RFChannel::fireNAKErrorEvent(): FT_NACK_TARGET_INVALID.");
						//4, Target Invalid
						errorVal[0] = 4;
						errorVal[1] = "Target Invalid";
						RFManager::GetSingleton()->ReportEvent(serial, XmlRpcValue("ERROR"), errorVal);
						break;
					case BidcosFrame::FT_NACK_CHANNEL_INVALID >> 16:
						LOG(Logger::LOG_ALL, "RFChannel::fireNAKErrorEvent(): FT_NACK_CHANNEL_INVALID.");
						//5, Channel Invalid
						errorVal[0] = 5;
						errorVal[1] = "Channel Invalid";
						RFManager::GetSingleton()->ReportEvent(serial, XmlRpcValue("ERROR"), errorVal);
						break;
					default:
						LOG(Logger::LOG_ALL, "RFChannel::fireNAKErrorEvent(): Unknown NACK type: 0x%.2X.",responseFrameNACKType );
				}
			}
		}
	}
}

bool RFChannel::GetLinkPeersFromDevice(link_peer_set_t* peers)
{
	if(!GetDescription()->HasLinkPeers() && !GetDescription()->HasTeam())return true;
	BidcosFrameLinkPeerReq frame;
	frame.SetType(BidcosFrame::FT_CONFIG_PEER_LIST_REQ);
	frame.SetCtrl(BidcosFrame::CTRL_BIDI |  BidcosFrame::CTRL_RPT_ENABLE);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_CHANNEL, index);

	if(!parent_dev->SendFrame(&frame))return false;
	int i=0;
	BidcosFrame* response=frame.GetResponse(i);
	while(response){
		int pos=10;
		if(response->MatchType(BidcosFrame::FT_INFO_PEER_LIST))
		{
            while(pos+3<=25)
            {
                uint32_t peer_address;
                uint32_t peer_channel;
                if(!response->GetIntValue(pos, 0, 3, 0, &peer_address))return false;
                pos+=3;
                if(!response->GetIntValue(pos, 0, 1, 0, &peer_channel))return false;
                pos++;
                if(!peer_address){
                    return true;
                }
                LOG(Logger::LOG_DEBUG,"RFChannel::GetLinkPeersFromDevice, Link von %d mit Addresse %d",this->GetDevice()->GetAddress(),peer_address);
                peers->insert((peer_address<<8)|peer_channel);
            }
		}
		i++;
		response=frame.GetResponse(i);
	}
	return false;
}

bool RFChannel::GetLinkPeers(std::vector<std::string>* peers)
{
	if(!GetDescription()->HasLinkPeers())return true;
	if(!link_peers_valid){
		link_peers.clear();
		link_peer_set_t dev_peers;
		if(GetLinkPeersFromDevice(&dev_peers)){
			for(link_peer_set_t::iterator it=dev_peers.begin();it!=dev_peers.end();it++){
				link_peers[*it];
			}
			link_peers_valid=true;
		}else{
			GetDevice()->CheckConfigPendingEvent();
			return false;
		}
		CreateTeam();
	}
	for(link_peer_map_t::iterator it=link_peers.begin();it!=link_peers.end();it++){
		uint32_t peer_address=it->first>>8;
		uint32_t peer_channel=it->first&0xff;
		std::string s_address=RFManager::GetSingleton()->BuildStringAddress(peer_address, peer_channel);
		peers->push_back(s_address);
	}
	GetDevice()->CheckConfigPendingEvent();
	return true;
}

bool RFChannel::GetLinks(int flags, link_map_t* result)
{
	//LOG(Logger::LOG_ALL, "RFChannel::GetLinks(): ");
	//LOG(Logger::LOG_ALL, "RFChannel::GetLinks(): Called for channel=%d with flags=%.4X", GetDescription()->GetIndex(), flags);
	if(!GetDescription()->HasLinkPeers())return true;
	//LOG(Logger::LOG_ALL, "RFChannel::GetLinks(): Channel can have links.");
	RFLogicalInstance* sender=NULL;
	RFLogicalInstance* receiver=NULL;
	std::string sender_serial;
	std::string receiver_serial;

	if(!link_peers_valid){
		//LOG(Logger::LOG_ALL, "RFChannel::GetLinks(): Link peers not retrieved from device. Retrieving...");
		link_peers.clear();
		link_peer_set_t dev_peers;
		if(GetLinkPeersFromDevice(&dev_peers)){
			for(link_peer_set_t::iterator it=dev_peers.begin();it!=dev_peers.end();it++){
				link_peers[*it];
			}
			link_peers_valid=true;
		}else{
			GetDevice()->CheckConfigPendingEvent();
			return false;
		}
		CreateTeam();
	}

	for(link_peer_map_t::iterator it=link_peers.begin();it!=link_peers.end();it++){

		uint32_t peer_address=it->first>>8;
		uint32_t peer_channel=it->first&0xff;
		//LOG(Logger::LOG_ALL, "RFChannel::GetLinks(): Processing peer %.6X channel %d", peer_address, peer_channel);
		RFChannel* peer=dynamic_cast<RFChannel*>(RFManager::GetSingleton()->GetInstance(peer_address, peer_channel));
		if(peer && peer->GetDescription()->IsHidden())continue;

		RFChannelDescription::link_role_t my_role;
		RFChannelDescription::link_role_t peer_role;
		my_role.flags=0;
		peer_role.flags=0;
		bool is_sender;
		if(peer && peer->GetDescription()->HasTeam()){
			peer=peer->GetTeamChannel();
			if(!peer)continue;
		}
		if(peer && GetDescription()->GetLinkRoles(peer->GetDescription(), &my_role, &peer_role)){
			if(my_role.flags & RFChannelDescription::LINK_ROLE_FLAG_TEAM)continue;
			is_sender=my_role.flags & RFChannelDescription::LINK_ROLE_FLAG_SENDER;
		}else{
			is_sender=GetDescription()->GetDirection()==RFChannelDescription::DIRECTION_SENDER;
		}

		if(is_sender){
			sender=this;
			sender_serial=GetSerial();
			//LOG(Logger::LOG_ALL, "RFChannel::GetLinks(): This channel (%s) is sender.", sender_serial.c_str());
		}else{
			receiver=this;
			receiver_serial=GetSerial();
			//LOG(Logger::LOG_ALL, "RFChannel::GetLinks(): This channel (%s) is receiver", receiver_serial.c_str());
		}

		if(is_sender){
			receiver=peer;
			if(receiver){
				receiver_serial=receiver->GetSerial();
			}else{
				receiver_serial=RFManager::GetSingleton()->BuildStringAddress(peer_address, peer_channel);
			}
		}else{
			sender=peer;
			if(sender){
				sender_serial=sender->GetSerial();
			}else{
				sender_serial=RFManager::GetSingleton()->BuildStringAddress(peer_address, peer_channel);
			}
		}
		//LOG(Logger::LOG_ALL, "RFChannel::GetLinks(): Sender is %s", sender_serial.c_str());
		//LOG(Logger::LOG_ALL, "RFChannel::GetLinks(): Receiver is %s", receiver_serial.c_str());

		bool senderInvalid = false;
		bool receiverInvalid = false;

		//avoid doubles
		std::string link_key;
		link_key=sender_serial+"->"+receiver_serial;

		link_map_t::iterator result_it=result->find(link_key);
		XmlRpcValue& link=(*result)[link_key];

		if(((std::string&)link["NAME"]).empty())link["NAME"]=it->second.name;
		if(((std::string&)link["DESCRIPTION"]).empty())link["DESCRIPTION"]=it->second.description;

		if(result_it!=result->end()){
			//OK, we already have this link on our list. Just mark our side of the link as valid.
			((int&)result_it->second["FLAGS"])&= ~(is_sender?LINK_FLAG_SENDER_INVALID:LINK_FLAG_RECEIVER_INVALID);
			continue;
		}

		link["SENDER"]=sender_serial;
		link["RECEIVER"]=receiver_serial;

		if(peer && (peer_role.flags&RFChannelDescription::LINK_ROLE_FLAG_VIRTUAL) ){
			(int&)link["FLAGS"]=0;
		}else if(peer && (flags&LogicalInstance::GL_FLAG_CHECK_PEER) && peer->IsLinkedTo(GetDevice()->GetAddress(), GetIndex()) ){
			(int&)link["FLAGS"]=0;
		}else{
			//(int&)link["FLAGS"]=is_sender?LINK_FLAG_RECEIVER_INVALID:LINK_FLAG_SENDER_INVALID;
			if(is_sender) {
				(int&)link["FLAGS"]=LINK_FLAG_RECEIVER_INVALID;
				receiverInvalid = true;
			}
			else {
				(int&)link["FLAGS"]=LINK_FLAG_SENDER_INVALID;
				senderInvalid = true;
			}
		}
		if(!sender)(int&)link["FLAGS"]|=LINK_FLAG_SENDER_UNKNOWN;
		if(!receiver)(int&)link["FLAGS"]|=LINK_FLAG_RECEIVER_UNKNOWN;
		if(sender && receiver){
			if((flags & GL_FLAG_SENDER_PARAMSET) && !senderInvalid){
				XmlRpcValue& paramset=link["SENDER_PARAMSET"];
				paramset.assertStruct();
				try{
					sender->GetParamsetValues(receiver->GetSerial(), &paramset);
				}catch(XmlRpcException){
					(int&)link["FLAGS"] |= LINK_FLAG_SENDER_INVALID;
				}
			}
			if((flags & GL_FLAG_RECEIVER_PARAMSET) && !receiverInvalid){

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
	//LOG(Logger::LOG_ALL, "RFChannel::GetLinks(): Finished processing peers");
	if(flags & GL_FLAG_GROUP){
		//LOG(Logger::LOG_ALL, "RFChannel::GetLinks(): Processing groups (Key-Pairs etc.)");
		int other_index=GetOtherPairIndex();
		if(other_index>0){
			RFLogicalInstance* other_inst=GetDevice()->GetInstance(other_index);
			if(!other_inst || !other_inst->GetLinks(flags&~GL_FLAG_GROUP, result))return false;
		}
	}
	GetDevice()->CheckConfigPendingEvent();
	//LOG(Logger::LOG_ALL, "RFChannel::GetLinks(): Finished. Return true.");
	return true;
}

bool RFChannel::SetLinkInfo(const std::string& peer, const std::string& name, const std::string& description)
{
	int peer_address;
	int peer_channel;
	if(!RFManager::GetSingleton()->ParseAddress(peer, &peer_address, &peer_channel))return false;
	link_peer_map_t::iterator it=link_peers.find((peer_address<<8)|peer_channel);
	if(it==link_peers.end())return false;
	it->second.name=name;
	it->second.description=description;
	RequestSave();
	return true;
}

bool RFChannel::GetLinkInfo(const std::string& peer, std::string* name, std::string* description)
{
	int peer_address;
	int peer_channel;
	if(!RFManager::GetSingleton()->ParseAddress(peer, &peer_address, &peer_channel))return false;
	link_peer_map_t::iterator it=link_peers.find((peer_address<<8)|peer_channel);
	if(it==link_peers.end())return false;
	*name=it->second.name;
	*description=it->second.description;
	return true;
}

bool RFChannel::RemoveLinkPeer(const std::string& peer)
{
	RFChannelDescription::link_role_t my_role;
	RFChannelDescription::link_role_t peer_role;
	my_role.flags=0;
	peer_role.flags=0;
	RFChannel* peer_channel=dynamic_cast<RFChannel*>(RFManager::GetSingleton()->GetInstance(peer));
	if(peer_channel)GetDescription()->GetLinkRoles(peer_channel->GetDescription(), &my_role, &peer_role);

	if(!(my_role.flags & RFChannelDescription::LINK_ROLE_FLAG_VIRTUAL)){
		int peer_address;
		int peer_channel;
		if(!RFManager::GetSingleton()->ParseAddress(peer, &peer_address, &peer_channel))return false;
		LowLevelRemoveLinkPeer(peer_address, peer_channel);
		DeleteStoredValues((peer_address<<8)|peer_channel);
	}
	RFManager::GetSingleton()->ReportUpdate(GetSerial(), RFManager::UPDATE_HINT_LINKS);
	return true;
}

bool RFChannel::LowLevelRemoveLinkPeer(int peer_address, int peer_channel)
{
	link_peers.erase((peer_address<<8)|peer_channel);
	RequestSave();
	RemoveConfigData(peer_address, peer_channel);
	BidcosFrame frame;
	frame.SetType(BidcosFrame::FT_CONFIG_PEER_REMOVE);
	frame.SetCtrl(BidcosFrame::CTRL_BIDI |  BidcosFrame::CTRL_RPT_ENABLE);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_CHANNEL, index);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_A, peer_address);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_CH_A, peer_channel);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_CH_B, 0);
	if( (!SendFrame(&frame)) || (!frame.GetResponse()) ||
		(frame.GetResponse()->IsNack() && !frame.GetResponse()->MatchType(BidcosFrame::FT_NACK_TARGET_INVALID))){
		link_peers_dirty=true;
		GetDevice()->CheckConfigPendingEvent();
		return false;
	}else{
		return true;
	}
}
bool RFChannel::LowLevelRemoveLinkPeer(int peer_address, int peer_channel_a,int peer_channel_b)
{
	link_peers.erase((peer_address<<8)|peer_channel_a);
	link_peers.erase((peer_address<<8)|peer_channel_b);
	RequestSave();
	RemoveConfigData(peer_address, peer_channel_a);
	RemoveConfigData(peer_address, peer_channel_b);
	BidcosFrame frame;
	frame.SetType(BidcosFrame::FT_CONFIG_PEER_REMOVE);
	frame.SetCtrl(BidcosFrame::CTRL_BIDI |  BidcosFrame::CTRL_RPT_ENABLE);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_CHANNEL, index);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_A, peer_address);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_CH_A, peer_channel_a);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_CH_B, peer_channel_b);
	if( (!SendFrame(&frame)) || (!frame.GetResponse()) ||
		(frame.GetResponse()->IsNack() && !frame.GetResponse()->MatchType(BidcosFrame::FT_NACK_TARGET_INVALID))){
		link_peers_dirty=true;
		GetDevice()->CheckConfigPendingEvent();
		return false;
	}else{
		return true;
	}
}
int RFChannel::GetOtherPairIndex()
{
	int index=GetDescription()->GetOtherPairIndex(GetIndex());
	//check if the other index really exists
	if(!GetDevice()->GetInstance(index))index=0;
	//LOG(Logger::LOG_DEBUG, "GetOtherPairIndex(%d)=%d", GetIndex(), index);
	return index;
}

RFChannel* RFChannel::GetOtherPairChannel()
{
	int index=GetDescription()->GetOtherPairIndex(GetIndex());
	return index?dynamic_cast<RFChannel*>(GetDevice()->GetInstance(index)):NULL;
}

bool RFChannel::AddLinkPeer(const std::string& peer)
{
	return AddLinkPeer(peer, true);
}

bool RFChannel::AddLinkPeer(const std::string& peer, bool pair)
{
	//LOG(Logger::LOG_DEBUG, "%s::AddLinkPeer(%s)", GetSerial().c_str(), peer.c_str());
	int peer_address;
	RFChannel* peer_channel=dynamic_cast<RFChannel*>(RFManager::GetSingleton()->GetInstance(peer));
	if(!peer_channel) { return false; }
	else if(IsLinkedTo(peer_channel->GetDevice()->GetAddress(), peer_channel->GetIndex()) ) {
		return true;
	}
	RFChannel* peer_paired_channel=peer_channel->GetOtherPairChannel();
	peer_address=peer_channel->GetDevice()->GetAddress();
	int ab_channels[2]={0,0};
	if(peer_channel->GetFunction()==RFChannelDescription::FUNCTION_AB){
		ab_channels[0]=peer_channel->GetIndex();
		ab_channels[1]=peer_channel->GetIndex();
	}else{
		ab_channels[peer_channel->GetFunction()]=peer_channel->GetIndex();
		if(peer_paired_channel)ab_channels[peer_paired_channel->GetFunction()]=peer_paired_channel->GetIndex();
	}

	for(RFChannel* ch=this;ch;){
		RFChannelDescription::link_role_t my_role;
		RFChannelDescription::link_role_t peer_role;
		my_role.flags=0;
		peer_role.flags=0;
		GetDescription()->GetLinkRoles(peer_channel->GetDescription(), &my_role, &peer_role);

		if(!(my_role.flags & RFChannelDescription::LINK_ROLE_FLAG_VIRTUAL)){
				if(ch->LowLevelAddLinkPeer(peer_address, ab_channels[RFChannelDescription::FUNCTION_A], ab_channels[RFChannelDescription::FUNCTION_B])==ADD_PEER_FAILED)return false;
			//try to read the parameter set for the new link from the device
			//and generate a default parameter set if this fails
			XmlRpcValue dummy;
			if(!ch->GetParamsetValues(peer_channel->GetSerial(), &dummy)){
				ch->GenerateDefaultLinkset(peer_channel);
			}else{
			//	LOG(Logger::LOG_DEBUG, "Linkset=%s", dummy.toText().c_str());
			}
			ch->SetEnforcedParameters(peer_channel);
			if(peer_paired_channel){
				if(!ch->GetParamsetValues(peer_paired_channel->GetSerial(), &dummy))ch->GenerateDefaultLinkset(peer_paired_channel);
				ch->SetEnforcedParameters(peer_paired_channel);
			}
		}

		RFManager::GetSingleton()->ReportUpdate(ch->GetSerial(), RFManager::UPDATE_HINT_LINKS);
		if(!pair)break;
		if(ch==GetOtherPairChannel())ch=NULL;
		else ch=GetOtherPairChannel();
	}

	return true;
}

bool RFChannel::SetEnforcedParameters(RFChannel* peer_channel)
{
	RFParamset* ps=GetDescription()->GetParamset("LINK");
	if(!ps)return false;
	ps->SetEnforcedValues(this, peer_channel);
	int peer_address=peer_channel->GetDevice()->GetAddress();
	RFConfigData& cd=config_data[(peer_address<<8)|(peer_channel->GetIndex()&0xff)];
	cd.CommitToDevice(this);
	if(cd.IsDevDirty())config_data_dirty=true;
	RequestSave();
	return true;
}
void RFChannel::InitDefaultConfig(void)
{
	config_data[0].InitDefault();	
}
int RFChannel::LowLevelAddLinkPeer(int peer_address, int peer_channel_a, int peer_channel_b)
{
	RequestSave();
	if(peer_channel_a>0){
		//add channel A to our internal peer map
		link_peers[(peer_address<<8)|peer_channel_a];
		//reset the invalid flag for this peer, just in case it was marked invalid before
		RFConfigData* cd=GetConfigData(peer_address, peer_channel_a, -1);
		cd->SetValid();
	}
	if(peer_channel_b>0){
		//add channel A to our internal peer map
		link_peers[(peer_address<<8)|peer_channel_b];
		//reset the invalid flag for this peer, just in case it was marked invalid before
		RFConfigData* cd=GetConfigData(peer_address, peer_channel_b, -1);
		cd->SetValid();
	}

	BidcosFrame frame;
	frame.SetType(BidcosFrame::FT_CONFIG_PEER_ADD);
	frame.SetCtrl(BidcosFrame::CTRL_BIDI |  BidcosFrame::CTRL_RPT_ENABLE);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_CHANNEL, index);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_A, peer_address);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_CH_A, peer_channel_a);
	frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_CH_B, peer_channel_b);
	if(!SendFrame(&frame)){
		//we didn't receive a response. Adding this peer will be tried again later.
		link_peers_dirty=true;
		GetDevice()->CheckConfigPendingEvent();
		return ADD_PEER_DEFERED;
	}
	BidcosFrame* response=frame.GetResponse();
	if(!response){
		//should never happen
		return ADD_PEER_FAILED;
	}
	if(!response->MatchType(BidcosFrame::FT_ACK_OR_NACK)){
		//should also never happen. Correctly implemented devices will always answer with ACK or NACK.
		return ADD_PEER_FAILED;
	}

	unsigned char subtype=response->GetByteData(0x09);
	if(!(subtype&0x80)){
		//We simply got an ACK from the device. Any ACK subtype is OK for us.
		return ADD_PEER_OK;
	}
	switch(subtype){
		case 0x80://NACK
		case 0x81://NACK_BUSY
			//we received a general NACK or a device busy indication. Adding this peer will be tried again later.
			link_peers_dirty=true;
			GetDevice()->CheckConfigPendingEvent();
			return ADD_PEER_DEFERED;
		case 0x82://NACK_MEMFULL
		case 0x83://NACK_MEMFULL_PART
		case 0x84://NACK_TARGET_INVALID
		case 0x85://NACK_CHANNEL_INVALID
			//something went terribly wrong. Remove the peer from the peer map and don't try again adding it later
			LOG(Logger::LOG_WARNING, "%s memory full. Discarding new link.", GetSerial().c_str());
			//Remove the peer that caused the memory to become full.
			//Maybe this should be catched earlier by checking the maximum number of link peers per channel / device
			if(peer_channel_a>0){
				link_peers.erase((peer_address<<8)|peer_channel_a);
				RemoveConfigData(peer_address, peer_channel_a);
			}
			if(peer_channel_b>0){
				link_peers.erase((peer_address<<8)|peer_channel_b);
				RemoveConfigData(peer_address, peer_channel_b);
			}
			link_peers_dirty=true;
			GetDevice()->CheckConfigPendingEvent();
			return ADD_PEER_FAILED;
	}
	return ADD_PEER_FAILED;
}

bool RFChannel::Describe(XmlRpc::XmlRpcValue* descr)
{
	const std::string& serial=parent_dev->GetSerial();
	(*descr)["ADDRESS"]=GetSerial();
	(*descr)["TYPE"]=GetDescription()->GetType();
	(*descr)["PARENT_TYPE"]=GetDevice()->GetType();
	(*descr)["PARENT"]=RFManager::GetSingleton()->BuildStringAddress(serial);
	(*descr)["INDEX"]=GetIndex();
	(*descr)["PARAMSETS"].assertArray(0);
	(*descr)["LINK_SOURCE_ROLES"]=GetDescription()->GetLinkSourceRoles();
	(*descr)["LINK_TARGET_ROLES"]=GetDescription()->GetLinkTargetRoles();
	(*descr)["DIRECTION"]=GetDescription()->GetDirection();
	(*descr)["AES_ACTIVE"]=aes;
	(*descr)["VERSION"]=GetDevice()->GetDeviceDescription()->GetVersion();
	(*descr)["FLAGS"]=GetDescription()->GetFlags();
	int other_pair_index=GetOtherPairIndex();
	if(other_pair_index>0){
		(*descr)["GROUP"]=RFManager::GetSingleton()->BuildStringAddress(serial, other_pair_index);
	}
	if(team_channel){
		(*descr)["TEAM"]=team_channel->GetSerial();
	}
	const std::string& team_tag=GetDescription()->GetTeamTag();
	if(team_tag.size()){
		(*descr)["TEAM_TAG"]=team_tag;
	}
	GetDescription()->ListParamsets(&((*descr)["PARAMSETS"]));
	return GetDescription()->GetAdditionalDescription()->Describe(descr);
}

bool RFChannel::GenerateDefaultLinkset(RFChannel* peer_channel)
{
	RFParamset* ps=GetDescription()->GetParamset("LINK");
	if(!ps)return false;
	return ps->SetDefaultValues(this, peer_channel);

	return true;
}
bool RFChannel::PushDefaultConfig()
{
	bool retval = false;
		link_peer_set_t dev_peers;
		std::map<uint32_t,link_peer_set_t> peers_per_dev;
		std::map<uint32_t,link_peer_set_t>::iterator dev_iterator;
		link_peer_set_t::iterator it1;
		if(!GetLinkPeersFromDevice(&dev_peers))return false;
		for(it1=dev_peers.begin();it1!=dev_peers.end();++it1)
		{
			if((int)(*it1 >> 8) != parent_dev->address)
			peers_per_dev[*it1 >> 8].insert(*it1);
		}
		for(dev_iterator = peers_per_dev.begin();dev_iterator != peers_per_dev.end();++dev_iterator)
		{
			for(it1=dev_iterator->second.begin();it1 != dev_iterator->second.end();++it1)
			{
				if(dev_iterator->second.upper_bound(*it1) != dev_iterator->second.end())
				{
					LowLevelRemoveLinkPeer(dev_iterator->first,(*it1 & 0xff),(*(dev_iterator->second.upper_bound(*it1)) & 0xff));
					it1++;
				}
				else
				{
					LowLevelRemoveLinkPeer((*it1 >> 8),(*it1 & 0xff));
				}
			}
		}
		link_peers_valid=true;
		UpdateAESFlag();
		config_data_t::iterator confIt;
		for(confIt=config_data.begin();confIt!=config_data.end();confIt++)
		{
			if(!confIt->second.CommitToDevice(this))
			{
				retval=false;
			}
			else
			{
				config_data_dirty=false;
			}
		}
		return retval;
}
bool RFChannel::CommitPendingConfig()
{
	//make sure the "MASTER" parameter set is cached
	try{
		XmlRpcValue dummy;
		GetParamsetValues("MASTER", &dummy);
	}catch(XmlRpcException){}
	UpdateAESFlag();
	if(config_data_dirty){
		config_data_t::iterator it;
		for(it=config_data.begin();it!=config_data.end();it++){
			//transmit the channel master config_data first
			if(it->second.IsForPeer(0, 0) && !it->second.CommitToDevice(this))
			    return false;
		}
	}
	if(!link_peers_valid){
		link_peer_set_t dev_peers;
		if(!GetLinkPeersFromDevice(&dev_peers))
		    return false;
		for(link_peer_set_t::iterator it=dev_peers.begin();it!=dev_peers.end();it++){
			link_peers[*it];
		}
		link_peers_valid=true;
	}else if(link_peers_dirty){
		link_peer_set_t dev_peers;
		if(!GetLinkPeersFromDevice(&dev_peers))return false;
		for(link_peer_set_t::iterator it=dev_peers.begin();it!=dev_peers.end();it++){
			if(link_peers.find(*it)==link_peers.end()){
				if(!LowLevelRemoveLinkPeer((*it)>>8, (*it)&0xff))return false;
			}
		}
		for(link_peer_map_t::iterator it=link_peers.begin();it!=link_peers.end();it++){
			if(dev_peers.find(it->first)==dev_peers.end()){
				if(LowLevelAddLinkPeer(it->first>>8, it->first&0xff, 0)!=ADD_PEER_OK)return false;
			}
		}
		link_peers_dirty=false;
	}
	CreateTeam();
	if(config_data_dirty){
		config_data_t::iterator it;
		for(it=config_data.begin();it!=config_data.end();it++){
			//now transmit the link config_data
			if(!it->second.IsForPeer(0, 0) && !it->second.CommitToDevice(this))return false;
		}
		config_data_dirty=false;
		if(behaviourChangePending) {
			behaviourChangePending = false;
			RFManager::GetSingleton()->ReportNewDevice(this->GetDevice());
		}
	}

	//make sure that all link parameter sets are cached
	if(GetDescription()->HasLinkPeers()){
		for(link_peer_map_t::iterator it=link_peers.begin();it!=link_peers.end();it++){
			int address=it->first>>8;
			int channel=it->first&0xff;
			std::string peer_address=RFManager::GetSingleton()->BuildStringAddress(address, channel);
			try{
				XmlRpcValue dummy;
				GetParamsetValues(peer_address, &dummy);
			}catch(XmlRpcException){}
		}
	}
	return true;
}
bool RFChannel::SetDefaultConfig(void)
{
	return GetDescription()->SetDefaultConfig(this);
}
bool RFChannel::SaveToXml(XMLNode* node)
{
	char buffer[16];
	snprintf(buffer, sizeof(buffer), "%d", GetIndex());
	node->addAttributeConst("index", buffer);
	node->addAttributeConst("type", GetDescription()->GetType().c_str());
	if(link_peers_dirty)node->addAttributeConst("peers_dirty", "true");
	if(config_data_dirty)node->addAttributeConst("config_dirty", "true");
	if(behaviour>0) {
		snprintf(buffer, sizeof(buffer), "%d", behaviour);
		node->addAttributeConst("behaviour", buffer);
	}
	if(behaviourChangePending)node->addAttributeConst("behaviour_change_pending", "true");
	if(!link_peers_valid)node->addAttributeConst("peers_valid", "false");
	if(aes)node->addAttributeConst("aes", "true");
	else node->addAttributeConst("aes", "false");
	if(GetDescription()->IsAesCbcSupported()) {
		snprintf(buffer, sizeof(buffer), "%u", aes_cbc_counter);
		node->addAttributeConst("aes_cbc_cnt", buffer);
	}
	XMLNode config_node=node->addChildConst("config");
	if(!config_data[0].SaveToXml(&config_node))return false;
	link_peer_map_t::iterator it;
	for(it=link_peers.begin();it!=link_peers.end();it++)
	{
		int peer_address=it->first>>8;
		int peer_channel=it->first&0xff;
		XMLNode peer_node=node->addChildConst("peer");
		snprintf(buffer, sizeof(buffer), "0x%06X", peer_address);
		peer_node.addAttributeConst("address", buffer);
		snprintf(buffer, sizeof(buffer), "%d", peer_channel);
		peer_node.addAttributeConst("channel", buffer);
		peer_node.addAttributeConst("link_name", it->second.name.c_str());
		XMLNode description_node=peer_node.addChildConst("description");
		description_node.addTextConst(it->second.description.c_str());
		XMLNode peer_config_node=peer_node.addChildConst("config");
		if(!config_data[it->first].SaveToXml(&peer_config_node)) {
			LOG(Logger::LOG_ERROR, "Error saving channels config data %d of device %s.", GetIndex(), GetDevice()->GetSerial().c_str());
			return false;
		}
	}
	XMLNode values_node=node->addChildConst("values");
	if(!ValueStore::SaveToXml(&values_node)) {
		LOG(Logger::LOG_ERROR, "Error saving channels value data %d of device %s.", GetIndex(), GetDevice()->GetSerial().c_str());
		return false;
	}

	for(t_value_usage_map::iterator it=value_usage_map.begin();it!=value_usage_map.end();it++){
		XMLNode usage_node=node->addChildConst("value_usage");
		usage_node.addAttributeConst("id", it->first.c_str());
		snprintf(buffer, sizeof(buffer), "%d", it->second);
		usage_node.addAttributeConst("refcount", buffer);
	}

	return true;
}

bool RFChannel::LoadFromXml(XMLNode& node)
{
//	LOG(Logger::LOG_DEBUG, "RFChannel::LoadFromXml() index=%d, type=%s", GetIndex(), GetDescription()->GetType().c_str());

	const char* temp;
	temp=node.getAttribute("config_dirty");
	config_data_dirty=(temp!=NULL) && (temp[0]=='t');
	temp=node.getAttribute("behaviour");
	if(temp) {
		sscanf(temp, "%d", &behaviour);
	}
	temp=node.getAttribute("behaviour_change_pending");
	behaviourChangePending=(temp!=NULL) && (temp[0]=='t');
	temp=node.getAttribute("peers_dirty");
	link_peers_dirty=(temp!=NULL) && (temp[0]=='t');
	temp=node.getAttribute("peers_valid");
	link_peers_valid=(temp==NULL) || (temp[0]=='f');
	temp=node.getAttribute("aes");
	if(temp){
		aes=(temp[0]=='t');
	}
	temp=node.getAttribute("aes_cbc_cnt");
	if(temp) {
		sscanf(temp, "%u", &aes_cbc_counter);
	}

	XMLNode config_node=node.getChildNode("config");
	if(config_node.isEmpty()) {
		LOG(Logger::LOG_WARNING, "RFChannel::LoadFromXml(): No config node!");
		return false;
	}
	if(!config_data[0].LoadFromXml(config_node)) {
		LOG(Logger::LOG_ERROR, "RFChannel::LoadFromXml(): Error reading config node.");
		return false;
	}
	int i=0;
	XMLNode peer_node=node.getChildNode("peer", &i);
	while(!peer_node.isEmpty()){
		temp=peer_node.getAttribute("address");
		if(!temp) {
			LOG(Logger::LOG_ERROR, "RFChannel::LoadFromXml(): Error reading address.");
			return false;
		}
		int peer_address=strtol(temp, NULL, 0);
		temp=peer_node.getAttribute("channel");
		if(!temp) {
			LOG(Logger::LOG_ERROR, "RFChannel::LoadFromXml(): Error reading channel.");
			return false;
		}
		int peer_channel=strtol(temp, NULL, 0);
//		LOG(Logger::LOG_DEBUG, "Loaded peer: address=%06X, channel=%d", peer_address, peer_channel);
		link_t& peer_struct=link_peers[(peer_address<<8)|peer_channel];
		temp=peer_node.getAttribute("link_name");
		if(temp)peer_struct.name=temp;
		XMLNode description_node=peer_node.getChildNode("description");
		if(!peer_node.isEmpty()){
			temp=description_node.getText();
			if(temp)peer_struct.description=temp;
		}
		
		XMLNode peer_config_node=peer_node.getChildNode("config");
		if(peer_config_node.isEmpty()) {
			LOG(Logger::LOG_ERROR, "RFChannel::LoadFromXml(): Error reading peer-config.");
			return false;
		}
		RFConfigData& cd=config_data[(peer_address<<8)|peer_channel];
		cd.SetPeer(peer_address, peer_channel);
		if(!cd.LoadFromXml(peer_config_node)) {
			LOG(Logger::LOG_ERROR, "RFChannel::LoadFromXml(): Error reading peer config node.");
			return false;
		}
		peer_node=node.getChildNode("peer", &i);
	}

	i=0;
	XMLNode usage_node=node.getChildNode("value_usage", &i);
	while(!usage_node.isEmpty()){
		temp=usage_node.getAttribute("refcount");
		if(!temp) {
			LOG(Logger::LOG_ERROR, "RFChannel::LoadFromXml(): Error reading refcount.");
			return false;
		}
		int count=strtol(temp, NULL, 0);
		temp=usage_node.getAttribute("id");
		if(!temp) {
			LOG(Logger::LOG_ERROR, "RFChannel::LoadFromXml(): Error reading refcount id.");
			return false;
		}
		value_usage_map[temp]=count;
		usage_node=node.getChildNode("value_usage", &i);
	}
	XMLNode values_node=node.getChildNode("values");
	if(!values_node.isEmpty()){
		if(!ValueStore::LoadFromXml(values_node)) {
			LOG(Logger::LOG_ERROR, "RFChannel::LoadFromXml(): Error reading value store.");
			return false;
		}
	}
	CreateTeam();
	return true;
}

void RFChannel::OnConfigReadFromDevice(RFConfigData* cd, int list)
{
	if(cd->IsForPeer(0, 0)){
		if(list==1 && GetDevice()->GetDeviceDescription()->SupportsAES() && !GetDescription()->GetAESAlways()){
			cd->PutValue(1, 8, 0, 0, 1, aes?1:0);
			config_data_dirty|=cd->IsDevDirty(1, 8);
		}
	}
	if(config_data_dirty){
		GetDevice()->ScheduleConfig(false);
	}
}

void RFChannel::UpdateAESFlag()
{
	if(!GetDevice()->GetDeviceDescription()->SupportsAES())return;
	if(GetDescription()->GetAESAlways())return;

	//get global channel config and activate AES for the channel
	RFConfigData* cd=GetConfigData(0, 0, 1);
	if(!cd)return;
	if(GetDevice()->IsNewDevice()){
		if(cd->GetValue(1,8,0,0,1)){
			aes=true;
		}
	}
	cd->PutValue(1, 8, 0, 0, 1, aes?1:0);
	config_data_dirty|=cd->IsDevDirty(1, 8);

	GetDevice()->SetAesPolicy();

	//set EXPECT_AES flag for each of our link paramsets

	RFDevice* central_dev=RFCentral::GetSingleton();
	for(link_peer_map_t::iterator it=link_peers.begin();it!=link_peers.end();it++){
		int peer_address=it->first>>8;
		int peer_channel=it->first&0xff;
		if(peer_address==central_dev->GetAddress()){
			RFChannel* peer=dynamic_cast<RFChannel*>(central_dev->GetInstance(peer_channel));
			if(peer)SetEnforcedParameters(peer);
		}
	}
	RequestSave();
}

void RFChannel::RequestSave()
{
	GetDevice()->RequestSave();
}

bool RFChannel::IsConfigPending()
{
//	LOG(Logger::LOG_DEBUG, "RFChannel::IsConfigPending() index=%d, type=%s, %d, %d, %d", GetIndex(), GetDescription()->GetType().c_str(), (int)link_peers_dirty, (int)config_data_dirty, (int)link_peers_valid);
	if(link_peers_dirty || config_data_dirty || !link_peers_valid)return true;
	config_data_t::iterator it;
	for(it=config_data.begin();it!=config_data.end();it++){
		if(it->second.IsDevDirty()){
			config_data_dirty=true;
			return true;
		}
	}
	return false;
}

bool RFChannel::ClearConfigCache()
{
	config_data.clear();
	link_peers_dirty=false;
	config_data_dirty=false;
	behaviourChangePending = false;
	link_peers_valid=false;
	link_peers.clear();
	return true;
}

bool RFChannel::ActivateLinkParamset(const std::string& peer, bool longpress)
{
	//LOG(Logger::LOG_DEBUG, "RFChannel::ActivateLinkParamset() peer=%s, long=%s", peer.c_str(), longpress?"true":"false");
	int peer_address;
	int peer_channel;
	if(!RFManager::GetSingleton()->ParseAddress(peer, &peer_address, &peer_channel))return false;

	BidcosFrame frame;
	frame.SetType(BidcosFrame::FT_SIMULATION);
	frame.SetIntValue(BidcosFrame::FIELD_SIM_TYPE, BidcosFrame::FT_SWITCH);
	frame.SetIntValue(BidcosFrame::FIELD_SIM_SENDER, peer_address);
	frame.SetIntValue(BidcosFrame::SimField(BidcosFrame::FIELD_SWITCH_CHANNEL), peer_channel);
	frame.SetIntValue(BidcosFrame::SimField(BidcosFrame::FIELD_SWITCH_COUNTER), rand()>>8);
	frame.SetIntValue(BidcosFrame::SimField(BidcosFrame::FIELD_SWITCH_DURATION), longpress);
	return SendFrame(&frame);
}

bool RFChannel::SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event/*=false*/)
{
	try{
		if(name=="AES" && !GetDescription()->GetAESAlways()){
			this->aes=(bool&)val!=0;
			//UpdateAESFlag();
			CommitPendingConfig();//TWIST-1067 ; (CommitPendingConfig calls UpdateAESFlag);
			RequestSave();
		}
		else if(name=="CHANNEL_FUNCTION" || name=="BEHAVIOUR") {
			SetBehaviour((int&)val);
			//CommitPendingConfig();
			RequestSave();
		}
		else{
			LOG(Logger::LOG_WARNING, "Tried to set unknown internal value %s", name.c_str());
		}
	}catch(XmlRpcException& e){
		LOG(Logger::LOG_WARNING, "SetInternalValue() exception %s", e.getMessage().c_str());
		return false;
	}
	if(fire_event)this->SendInternalValueEvent(name, val);
	return true;
}

bool RFChannel::GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val)
{
	try{
		if(name=="AES"){
			(bool&)(*val)=this->aes;
		}else if(name=="FUNCTION"){
			(int&)(*val)=GetFunction();
		}
		else if( (name.compare("CHANNEL_FUNCTION") == 0) || name=="BEHAVIOUR") {
			(int&)(*val)=GetBehaviour();
			//LOG(Logger::LOG_ALL, "RFChannel::GetInternalValue(): Got behaviour %d", (int)val);
		}
		else{
			LOG(Logger::LOG_WARNING, "Tried to get unknown internal value %s", name.c_str());
		}
	}catch(XmlRpcException& e){
		LOG(Logger::LOG_WARNING, "GetInternalValue() exception %s", e.getMessage().c_str());
		return false;
	}
	return true;
}

bool RFChannel::IsLinkedTo(int peer_address, int peer_channel)
{
	return link_peers.find((peer_address<<8)|peer_channel)!=link_peers.end();
}

bool RFChannel::ReportValueUsage(const std::string& value, int count)
{
	if(!GetDescription()->GetAutoregisterCentral())return true;
	value_usage_map[value]=count;
	int total_count=0;
	for(t_value_usage_map::iterator it=value_usage_map.begin();it!=value_usage_map.end();it++){
		if(it->second > 0)total_count+=it->second;
	}

	RFCentral* central_device=RFCentral::GetSingleton();
	if(!central_device){
		LOG(Logger::LOG_ERROR, "No central device defined");
		return false;
	}
	int central_address=central_device->GetAddress();
	int central_channel=central_device->GetListenerChannel()->GetIndex();
	if(total_count && !IsLinkedTo(central_address, central_channel)){
		AddLinkPeer(central_device->GetListenerChannel()->GetSerial(), false);
		central_device->GetListenerChannel()->LowLevelAddLinkPeer(GetDevice()->GetAddress(), GetIndex());
	}else if(!total_count && IsLinkedTo(central_address, central_channel)){
		LowLevelRemoveLinkPeer(central_address, central_channel);
		central_device->GetListenerChannel()->LowLevelRemoveLinkPeer(GetDevice()->GetAddress(), GetIndex());
	}

	return !(config_data_dirty || link_peers_dirty);
}

bool RFChannel::UnpeerCentral()
{
	uint32_t central_address=RFManager::GetSingleton()->GetBidcosAddress();
	std::vector<int> peers_to_erase;
	for(link_peer_map_t::iterator it=link_peers.begin();it!=link_peers.end();it++){
		uint32_t peer_address=it->first>>8;
		uint32_t peer_channel=it->first&0xff;
		if(peer_address==central_address){
			BidcosFrame frame;
			frame.SetType(BidcosFrame::FT_CONFIG_PEER_REMOVE);
			frame.SetCtrl(BidcosFrame::CTRL_BIDI |  BidcosFrame::CTRL_RPT_ENABLE);
			frame.SetIntValue(BidcosFrame::FIELD_CONFIG_CHANNEL, index);
			frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_A, peer_address);
			frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_CH_A, peer_channel);
			frame.SetIntValue(BidcosFrame::FIELD_CONFIG_PEER_CH_B, 0);
			if(!SendFrame(&frame) || (!frame.GetResponse()) || frame.GetResponse()->IsNack()){
				link_peers_dirty=true;
				return false;
			}
			peers_to_erase.push_back((peer_address<<8)|peer_channel);
			RemoveConfigData(peer_address, peer_channel);
			DeleteStoredValues((peer_address<<8)|peer_channel);
			RFChannel* central_peer_channel=dynamic_cast<RFChannel*>(RFCentral::GetSingleton()->GetInstance(peer_channel));
			if(central_peer_channel)central_peer_channel->LowLevelRemoveLinkPeer(GetDevice()->GetAddress(), GetIndex());
		}
	}
	for(unsigned int i=0;i<peers_to_erase.size();i++){
		link_peers.erase(peers_to_erase[i]);
	}
	return true;
}

bool RFChannel::SendToPeers(BidcosFrame* frame)
{
	bool retval=true;
	std::set<RFDevice*> peer_devices;
	bool burst=false;
	for(link_peer_map_t::iterator it=link_peers.begin();it!=link_peers.end();it++){
		RFLogicalInstance* inst=RFManager::GetSingleton()->GetInstance((it->first)>>8, 0);
		RFDevice* dev=NULL;
		if(inst)dev=inst->GetDevice();
		if(!dev){
			LOG(Logger::LOG_WARNING, "Skipping unknown device 0x%06X", (it->first)>>8);
			continue;
		}
		if(dynamic_cast<RFCentral*>(dev))continue;
		peer_devices.insert(dev);
		burst|=dev->RxNeedsBurst();
	}

	const unsigned int peerCnt = peer_devices.size();
	frame->SetSenderAddress(GetDevice()->GetAddress());
	if(peerCnt > 1) {
		retval = RFManager::GetSingleton()->GetInterfaceConcentrator()->PerformMulticastSend(peer_devices, frame, burst);
	}
	else {
		//frame->SetSenderAddress(GetDevice()->GetAddress());
		std::set<RFDevice*>::iterator it=peer_devices.begin();
		if(it != peer_devices.end()) {
			retval = (*it)->SendFrame(frame);
		}
	}

	return retval;
}

void RFChannel::SetConfigDevDirty()
{
	RFLogicalInstance::SetConfigDevDirty();
	config_data_dirty=true;
	link_peers_dirty=true;
}

void RFChannel::ScheduleValueGet(const std::string& id)
{
	if(scheduled_get_values.empty()){
		int32_t timeout=(rand()/(RAND_MAX/SCHEDULED_GET_WAIT_RANGE))+SCHEDULED_GET_WAIT_MIN;
		RequestTimer(timeout, TIMER_SCHEDULED_GET);
	}
	else if(scheduled_get_values.find(id) != scheduled_get_values.end()) {//TWIST-344
		KillTimer(TIMER_SCHEDULED_GET);
		int32_t timeout=(rand()/(RAND_MAX/SCHEDULED_GET_WAIT_RANGE))+SCHEDULED_GET_WAIT_MIN;
		RequestTimer(timeout, TIMER_SCHEDULED_GET);
	}
	scheduled_get_values.insert(id);
}

void RFChannel::ProcessScheduledValueGet()
{
	if(scheduled_get_values.empty())return;
	std::string id=*(scheduled_get_values.begin());
	scheduled_get_values.erase(id);
	map<std::string, int>::iterator scheduledGetCntIterator = scheduled_get_valuse_cnt.find(id);
	if(scheduledGetCntIterator != scheduled_get_valuse_cnt.end()) {
		scheduledGetCntIterator->second++;
		//LOG(Logger::LOG_ALL, "************************ cnt %d", scheduledGetCntIterator->second);
		if(scheduledGetCntIterator->second > MAX_SCHEDULED_GET_CNT) {
			LOG(Logger::LOG_INFO, "RFChannel::ProcessScheduledValueGet(): Aborting scheduled get of %s for %s.", id.c_str(), GetSerial().c_str());
			scheduled_get_valuse_cnt.erase(scheduledGetCntIterator);
			return;
		}
	}
	else {
		scheduled_get_valuse_cnt[id] = 1;
		//LOG(Logger::LOG_ALL, "************************ cnt 1");
	}
	LOG( Logger::LOG_DEBUG, "Executing scheduled value get for %s.%s", GetSerial().c_str(), id.c_str());
	XmlRpcValue val;
	try{
		if(GetValue(id, &val)){
			ReportEvent(id, val);
			scheduled_get_valuse_cnt.erase(id);
		}
	}catch(XmlRpcException& e){
		LOG(Logger::LOG_ERROR, "RFChannel::ProcessScheduledValueGet(): Exception in GetValue(\"%s\"): %d, %s", id.c_str(), e.getCode(), e.getMessage().c_str());
	}
	if(scheduled_get_values.size()){
		int32_t timeout=(rand()/(RAND_MAX/SCHEDULED_GET_WAIT_RANGE))+SCHEDULED_GET_WAIT_MIN;
		RequestTimer(timeout, TIMER_SCHEDULED_GET);
	}
}

void RFChannel::OnTimer(uint32_t cookie)
{
	switch(cookie){
		case TIMER_SCHEDULED_GET:
			ProcessScheduledValueGet();
			break;
		default:
			RFLogicalInstance::OnTimer(cookie);
	}
}

void RFChannel::CreateTeam(RFDeviceDescription* team_description /*=NULL*/)
{
	if(!GetDescription()->HasTeam())return;
	if(!team_description)team_description=GetDevice()->GetDeviceDescription()->GetTeamDescription();
	if(!team_description)return;
	

	uint32_t team_address;
	link_peer_map_t::iterator it=link_peers.begin();
	if(it==link_peers.end()){
		//no team address set. So we make up our own brand-new team.
		team_address=GetDevice()->GetAddress();
		LowLevelAddLinkPeer(team_address, GetIndex(), 0);
	}else{
		team_address=it->first>>8;
	}
	RFDevice* team=RFManager::GetSingleton()->CreateTeamInstance(team_description, team_address, GetDevice());
	if(team){
		this->team_channel=dynamic_cast<RFTeamChannel*>(team->GetInstance(GetIndex()));
		this->team_channel->AddTeamChannel(this);
	}else{
		this->team_channel=NULL;
	}
	RequestSave();
}

bool RFChannel::SetTeam(RFTeamChannel* team)
{
	if(!GetDescription()->HasTeam())return false;
	if(!team){
		RFDevice* team_device=RFManager::GetSingleton()->CreateTeamInstance(GetDevice()->GetDeviceDescription()->GetTeamDescription(), GetDevice()->GetAddress(), GetDevice());
		if(!team_device)return false;
		team=dynamic_cast<RFTeamChannel*>(team_device->GetInstance(GetIndex()));
	}
	if(!team)return false;
	if(team->GetDescription()->GetTeamTag() != GetDescription()->GetTeamTag()){
		return false;
	}
	if(team_channel == team){
		//we already are in this team
		return true;
	}
	LOG(Logger::LOG_DEBUG, "Setting team for %s to %s", GetSerial().c_str(), team->GetSerial().c_str());
	if(team_channel){
		LowLevelRemoveLinkPeer(team_channel->GetDevice()->GetAddress(), team_channel->GetIndex());
		team_channel->RemoveTeamChannel(this);
		team_channel=NULL;
	}
	if(LowLevelAddLinkPeer(team->GetDevice()->GetAddress(), team->GetIndex(), 0)==ADD_PEER_FAILED){
		LOG(Logger::LOG_ERROR, "Setting team for %s to %s add link peer failed", GetSerial().c_str(), team->GetSerial().c_str());
		link_peers.clear();
		link_peer_set_t dev_peers;
		if(GetLinkPeersFromDevice(&dev_peers)){
			for(link_peer_set_t::iterator it=dev_peers.begin();it!=dev_peers.end();it++){
				link_peers[*it];
			}
			link_peers_valid=true;
		}
		CreateTeam();
		return false;
	}
	this->team_channel=team;
	this->team_channel->AddTeamChannel(this);
	RequestSave();
	LOG(Logger::LOG_DEBUG, "Done setting team for %s to %s", GetSerial().c_str(), team->GetSerial().c_str());
	return true;
}

void RFChannel::SetSerial(const std::string& s)
{
	if(s.empty()){
		serial.clear();
	}else{
		serial=RFManager::GetSingleton()->BuildStringAddress(s, index);
	}
}
bool RFChannel::GetConfig(XmlRpc::XmlRpcValue* c)
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
bool RFChannel::replaceChannel(RFChannel * oldChannel)
{

    std::vector<std::string> peers;
    oldChannel->GetLinkPeers(&peers);
    std::vector<std::string>::iterator it_peers;
    
    for(it_peers = peers.begin(); it_peers != peers.end();++it_peers)
    {
        RFChannel *ch = dynamic_cast<RFChannel*>(RFManager::GetSingleton()->GetInstance(*it_peers));
        if(ch)
        {
            RFDevice *device = ch->GetDevice();
            RFDevice *oldDev = oldChannel->GetDevice();

            if((device && oldDev) && device != oldDev)
            {
                int new_peer_address = device->GetAddress();
                int new_peer_channel = ch->GetIndex();
                //this->AddLinkPeer(*it_peers);
                this->link_peers[(new_peer_address << 8)| (new_peer_channel & 0xff)] = link_t(oldChannel->link_peers[(new_peer_address << 8)| (new_peer_channel & 0xff)]);
            }
        }
    }
    
    this->replaceRFConfigData(oldChannel);
    this->link_peers_valid = true;
    this->link_peers_dirty = true;
    this->config_data_dirty = true;
    return true;
}

void RFChannel::SetValueAsDefined(const std::string& name) {
	RFParamset* ps=GetDescription()->GetParamset("VALUES");
	if(!ps){
			return;
	}
		HSSParameter* param=ps->GetParameter(name);
		if(!param){
			return;
		}
		SetCurParamsetPeer(0);
		return param->SetUndefined(false);
}

void RFChannel::SetValueAsUndefined(const std::string& name) {
	RFParamset* ps=GetDescription()->GetParamset("VALUES");
	if(!ps){
			return;
	}
		HSSParameter* param=ps->GetParameter(name);
		if(!param){
			throw XmlRpcException("Unknown parameter", -5);
		}
		SetCurParamsetPeer(0);
		return param->SetUndefined(true);
}

bool RFChannel::replaceRFConfigData(RFLogicalInstance *oldInstance)
{
    config_data_t::iterator configIt;
    RFLogicalInstance::replaceRFConfigData(oldInstance);
    RFChannel *oldchannel = dynamic_cast<RFChannel*>(oldInstance);
    std::vector<int32_t> internalLinks;
    if (oldchannel == NULL)
    {
        return false;
    }
    unsigned int oldDeviceAddress = oldchannel->GetDevice()->GetAddress();

    for (configIt = oldchannel->config_data.begin();configIt != oldchannel->config_data.end(); ++configIt)
    {
        if ((configIt->first >> 8) == oldDeviceAddress)
        {
            internalLinks.push_back(configIt->first);
        }
    }

    std::vector<int32_t>::iterator internalLinkIt;
    for (internalLinkIt = internalLinks.begin();
            internalLinkIt != internalLinks.end(); ++internalLinkIt)
    {
      
        this->config_data[(this->GetDevice()->GetAddress() << 8) | (*internalLinkIt & 0xff)] = RFConfigData(this->config_data[*internalLinkIt]);
        this->config_data[(this->GetDevice()->GetAddress() << 8) | (*internalLinkIt & 0xff)].SetPeer(this->GetDevice()->GetAddress(), (*internalLinkIt  & 0xff));
        this->config_data[(this->GetDevice()->GetAddress() << 8) | (*internalLinkIt & 0xff)].SetDevDirty();
        this->config_data.erase(*internalLinkIt);
    }
    return true;

}
bool RFChannel::replacePeer(std::string old_peer, std::string new_peer)
{
	RFChannel *oldPeerChannel = dynamic_cast<RFChannel *>(RFManager::GetSingleton()->GetInstance(old_peer));
	RFChannel *newPeerChannel = dynamic_cast<RFChannel *>(RFManager::GetSingleton()->GetInstance(new_peer));
	int new_peer_address, new_peer_channel, old_peer_address, old_peer_channel;
	
	if(newPeerChannel == NULL || oldPeerChannel == NULL)
	{
		return false;
	}
	new_peer_address = newPeerChannel->GetDevice()->GetAddress();
	new_peer_channel = newPeerChannel->GetIndex();
	old_peer_address = oldPeerChannel->GetDevice()->GetAddress();
	old_peer_channel = oldPeerChannel->GetIndex();
	
	this->link_peers[(new_peer_address << 8)| (new_peer_channel & 0xff)] = this->link_peers[(old_peer_address << 8)| (old_peer_channel & 0xff)];
	this->link_peers.erase((old_peer_address << 8)| (old_peer_channel & 0xff));

	this->config_data[(new_peer_address << 8)| (new_peer_channel & 0xff)] = RFConfigData(this->config_data[(old_peer_address << 8)| (old_peer_channel & 0xff)]);
	//this->config_data[(old_peer_address << 8)| (old_peer_channel & 0xff)] = *(new RFConfigData()); // allte addresse mit dummydaten laden damit nicht die richtigen daten beim erase gel�scht werden
	this->config_data.erase((old_peer_address << 8)| (old_peer_channel & 0xff));
	this->config_data[(new_peer_address << 8)| (new_peer_channel & 0xff)].SetPeer(new_peer_address,new_peer_channel);
	this->config_data[(new_peer_address << 8)| (new_peer_channel & 0xff)].SetDevDirty(); // damit die daten zum Ger�t �bertragen werdenl

	this->link_peers_valid = true;
	this->link_peers_dirty = true;
	this->config_data_dirty = true;
	return true;
}

unsigned int RFChannel::getAesCbcCounter()
{
	return aes_cbc_counter;
}

void RFChannel::setAesCbcCounter(const unsigned int cbc_counter)
{
	aes_cbc_counter = cbc_counter;
	//TODO: Save every time or just X time
	/*if((aes_cbc_counter % 10) == 0) {

	}*/
	GetDevice()->Save();
}

int RFChannel::GetBehaviour()
{
	HSSParameter* p=description->GetBehaviourParam();
	if(!p){
		LOG(Logger::LOG_WARNING, "Don't know how to get channel behaviour");
		return behaviour;
	}
	try{
		SetCurParamsetPeer(0);
		//SetCurParamsetIndex(GetEEPromIndex());
		XmlRpcValue v;
		if(!p->GetValue(this, &v)){
			LOG(Logger::LOG_WARNING, "Could not get channel %d behaviour", GetIndex());
			return behaviour;
		}
		behaviour=(int&)v;
	}catch(XmlRpcException& e){
	}
	return behaviour;
}

bool RFChannel::SetBehaviour(const int b)
{
	//LOG(Logger::LOG_ALL, "RFChannel::SetBehaviour(%d)",b);
	if(behaviour==b)return true;
	HSSParameter* p=description->GetBehaviourParam();
	if(!p){
		LOG(Logger::LOG_WARNING, "Don't know how to set channel behaviour");
		return false;
	}

	//--- new
	SetCurParamsetPeer(0);
	XmlRpcValue v=b;
	//if(GetDevice()->RxAlways() || GetDevice()->RxNeedsBurst()) {
	//---
		//SetCurParamsetIndex(GetEEPromIndex());
	if(!p->SetValue(this, v)){// <-> Dat geht so nicht, da der rest nicht ausgeführt wird.
		LOG(Logger::LOG_WARNING, "Could not set channel behaviour");
		return false;
	}
	
	if(p->GetValue(this, &v)) {
		behaviour=(int&)v;
	}
	behaviourChangePending = true;
	return true;
}


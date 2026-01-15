/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HS485_CHANNEL_H_
#define _HS485_CHANNEL_H_

#include "HS485LogicalInstance.h"
#include "HS485ChannelDescription.h"
#include <xmlParser.h>

class HS485Device;

//! Jede Instanz dieser Klasse repräsentiert einen konkreten Kanal eines konkreten angelernten BidCoS-Wired-Gerätes
/*! Diese Klasse verwaltet die Informationen, die für einen konkreten Kanal relevant sind. Informationen,
 *  die eine Kanalklasse betreffen (also das, was aus der XML-Datei gelesen wird), werden von HS485ChannelDescription
 *  verwaltet. Jede Instanz von HS485Channel enthält einen Zeiger auf die zugehörige Instanz von HS485ChannelDescription.
 */
class HS485Channel : public HS485LogicalInstance
{
public:
	HS485Channel(void);
	virtual ~HS485Channel(void);
	bool GetParamsetValues(const std::string& key, XmlRpc::XmlRpcValue* set);
	bool PutParamsetValues(const std::string& key, XmlRpc::XmlRpcValue& set);
	bool GetParamsetDescription(const std::string& key, XmlRpc::XmlRpcValue* set);
	bool GetParamsetId(const std::string& key, std::string* id);

	bool GetValue(const std::string& name, XmlRpc::XmlRpcValue* val);
	bool SetValue(const std::string& name, XmlRpc::XmlRpcValue& val);

	virtual bool SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event=false);
	virtual bool GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val);

	void ReportEvent(const std::string& id, XmlRpc::XmlRpcValue& val, uint32_t burst_suppression=0);
	void ReportServiceMessage(const std::string& id, XmlRpc::XmlRpcValue& val);
	void ProcessIncomingFrame(HS485Frame& frame, FrameDescription* fd);
	void SetParent(HS485Device* parent, int index, HS485ChannelDescription* desc);
	bool SetEnforcedParameters();
	bool SendMessage(const std::string& msg, std::string* response);
	inline int GetIndex(){return index;};
	inline int GetPhysicalIndex(){return index+description->GetIndexOffset();};
	inline int GetEEPromIndex(){return index-description->GetIndex();};
	virtual HS485Device* GetDevice(){return parent_dev;};
	virtual bool GetLinkPeers(std::vector<std::string>* peers);
	virtual bool GetLinks(int flags, link_map_t* result);
	virtual bool AddLinkPeer(const std::string& peer);
	virtual bool RemoveLinkPeer(const std::string& peer);
	virtual bool IsLinkedTo(const std::string& peer);
	virtual bool SetLinkInfo(const std::string& peer, const std::string& name, const std::string& description);
	virtual bool GetLinkInfo(const std::string& peer, std::string* name, std::string* description);
	inline HS485ChannelDescription* GetDescription()
	{
		if(!behaviour)return description;
		return description->GetSubdescription(behaviour);
	}
	bool Describe(XmlRpc::XmlRpcValue* val);
	bool UnpeerCentral();
	virtual bool SendMessage(HS485CommMessage* msg);
	virtual bool SaveToXml(XMLNode* node);
	virtual bool LoadFromXml(XMLNode& node);
	inline const std::string& GetSerial(){return serial;};
	inline const std::string& GetCurParamsetPeer(){return cur_paramset_peer;};
	inline void SetCurParamsetPeer(const std::string& peer){cur_paramset_peer=peer;};
	bool ReportValueUsage(const std::string& value, int count);
	bool SendToPeers(HS485CommMessage* msg, unsigned int receiver_channel_field);
	int GetBehaviour();
	bool SetBehaviour(int b);
	bool ActivateLinkParamset(const std::string& peer, bool longpress);
	void SetDescription(HS485ChannelDescription* d){description=d;};
	bool GetConfig(XmlRpc::XmlRpcValue* c);
	bool RestoreConfig(XmlRpc::XmlRpcValue& c);
	bool replacePeer(const std::string& oldPeerSerial, const std::string& newPeerSerial);
	void SetSerial(const std::string& serial);


	virtual bool SetDefaultConfig() { return true; }
	HS485Device* parent_dev;
	
	//Method used by rfd (which is pure virtual in class LogicalInstance)
	virtual void SetValueAsDefined(const std::string&) {}
	//Method used by rfd (which is pure virtual in class LogicalInstance)
	virtual void SetValueAsUndefined(const std::string&) {}
	
protected:
	void RequestSave();
	//HS485Device* parent_dev;
	HS485ChannelDescription* description;
	int behaviour;
	int index;
	typedef std::map<std::string, int> t_value_usage_map;
	t_value_usage_map value_usage_map;
	typedef struct{
		std::string name;
		std::string description;
	}t_link_info;
	typedef std::map<std::string, t_link_info> t_link_info_map;
	t_link_info_map link_info_map;
	std::string serial;
	std::string cur_paramset_peer;
};
#endif //_HS485_CHANNEL_H_

/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485ChannelDescription.h: Schnittstelle für die Klasse HS485ChannelDescription.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HS485CHANNELDESCRIPTION_H__A3DD9C11_969F_42AD_8D8C_49E0A0E87430__INCLUDED_)
#define AFX_HS485CHANNELDESCRIPTION_H__A3DD9C11_969F_42AD_8D8C_49E0A0E87430__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>
#include "xmlParser.h"
#include "XmlRpc.h"
#include "HS485Paramset.h"
#include <HSSDescription.h>

class HS485DeviceDescription;
class HS485LogicalInstance;
class HS485Value;

class HS485Controller;

//! Diese Klasse enthält die für einen Kanaltypen aus der Gerätebeschreibungsdatei gelesenen Informationen
class HS485ChannelDescription  
{
public:
	enum{DIRECTION_NONE=0, DIRECTION_SENDER=1, DIRECTION_RECEIVER=2};
	enum{FLG_NONE=0, FLG_VISIBLE=(1<<0), FLG_INTERNAL=(1<<1)};
	inline void SetIndex(int index)
	{
		this->index=index;
	};
	inline int GetIndex()
	{
		return index;
	};
	inline int GetCount()
	{
		return count;
	};
	inline int GetIndexOffset(){
		return index_offset;
	};
	void ProcessIncomingFrame(HS485LogicalInstance* inst, HS485Frame& frame, FrameDescription* fd);
	bool SetEnforcedParameters(HS485LogicalInstance* inst);
	bool ListParamsets(XmlRpc::XmlRpcValue* list);
	HS485Paramset* GetParamset(const std::string& key);

	bool GetLinkPeers(HS485LogicalInstance* inst, std::vector<std::string>* peers);
	bool RemoveLinkPeer(HS485LogicalInstance* inst, const std::string& peer);
	bool AddLinkPeer(HS485LogicalInstance* inst, const std::string& peer);

	const std::string& GetType();
	void SetDevice(HS485DeviceDescription* device);
	HS485ChannelDescription();
	bool SetupInstance(HS485LogicalInstance* inst);
	virtual ~HS485ChannelDescription();
    virtual bool InitFromXml(XMLNode& node, XMLNode& root_node);
	const std::string& GetLinkSourceRoles(){return link_source_roles;};
	const std::string& GetLinkTargetRoles(){return link_target_roles;};
	inline HSSDescription* GetAdditionalDescription(){return &additional_description;};
	inline const std::string& GetCreationTag(){return creation_tag;};
	inline bool HasLinkPeers(){return direction!=DIRECTION_NONE;};
	inline bool LinksAreVirtual(){return virtual_links;};
	inline int GetDirection(){return direction;};
	inline bool GetAutoregisterCentral(){return autoregister_central;};
	HS485ChannelDescription* GetSubdescription(int index);
	bool HasSubdescriptions(){return !vec_subdescriptions.empty();};
	HSSParameter* GetBehaviourParam(){return behaviour_param;};
	inline int GetFlags(){return flags;};
	const std::string& GetTestParamsetFrame(bool longpress){return longpress?test_paramset_frame_long:test_paramset_frame_short;};
	inline bool IsHidden(){return hidden;};
protected:
	int index_offset;
	int index;
	int count;
	int direction;
	bool virtual_links;
	HS485DeviceDescription* device;
    std::string type;
	std::string link_source_roles;
	std::string link_target_roles;
	std::string test_paramset_frame_short;
	std::string test_paramset_frame_long;
	typedef std::map<std::string, HS485Paramset> paramsets_t;
	paramsets_t paramsets;
	HSSDescription additional_description;
	std::string creation_tag;
	bool autoregister_central;
	std::vector<HS485ChannelDescription*> vec_subdescriptions;
	HSSParameter* behaviour_param;
	int flags;
	bool hidden;
};

#endif // !defined(AFX_HS485CHANNELDESCRIPTION_H__A3DD9C11_969F_42AD_8D8C_49E0A0E87430__INCLUDED_)

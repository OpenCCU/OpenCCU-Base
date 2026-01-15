/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485DeviceDescription.h: Schnittstelle für die Klasse HS485DeviceDescription.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HS485DEVICEDESCRIPTION_H__41F2BBEB_C64D_4FB5_A126_713C3C9B7AAF__INCLUDED_)
#define AFX_HS485DEVICEDESCRIPTION_H__41F2BBEB_C64D_4FB5_A126_713C3C9B7AAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "HS485ChannelDescription.h"
#include "HS485Paramset.h"
#include <FrameDescription.h>

#include "xmlParser.h"

#include <vector>
#include <map>
#include <string>

#include <XmlRpc.h>


//! Diese Klasse verwaltet die Informationen aus den Gerätebeschreibungsdateien (XML-Dateien)
/*!
 *  Jede Instanz dieser Klasse liest eine XML-Datei ein und speichert die darin enthaltenen Informationen.
 */
class HS485DeviceDescription
{
public:
	enum{FLG_NONE=0, FLG_VISIBLE=(1<<0), FLG_DONTDELETE=(1<<3)};
	HS485Device* CreateDevice();
	std::string& GetCreationTag(){return creation_tag;};
	bool ListParamsets(XmlRpc::XmlRpcValue* list);
	bool SetEnforcedParameters(HS485LogicalInstance* inst);
	HS485Paramset* GetParamset(const std::string& key);
	unsigned int GetChannelCount();
	bool HasSerial(){return has_serial;};
	HS485ChannelDescription* GetChannel(int i);
	HS485ChannelDescription* GetChannel(const std::string& type);
	int Matches(HS485Frame& sysinfoFrame, std::string* type_id);
	virtual bool InitFromXml(XMLNode& node, XMLNode& root_node);
	inline bool ChannelsHaveSubdescriptions(){return channels_have_subdescriptions;};
	FrameDescription* GetFrameDescription(HS485Frame& frame, int* channel, int* iterator);
	FrameDescription* GetFrameDescription(const std::string& id)
	{
		framedefs_by_id_t::iterator it=framedefs_by_id.find(id);
		if(it==framedefs_by_id.end())return NULL;
		return &framedefs[it->second];
	};
	FrameDescription* GetFrameDescription(int type)
	{
		framedefs_by_type_t::iterator it=framedefs_by_type.find(type);
		if(it==framedefs_by_type.end())return NULL;
		return &framedefs[it->second];
	};
	class Type:public FrameDescription{
    public:
        Type(){
			type=-1;
			priority=0;
        };
        bool InitFromXml(XMLNode& node, XMLNode& root_node);
		int GetPriority(){return priority;};
	protected:
		std::string name;
		int priority;
    };
	HS485DeviceDescription();
	virtual ~HS485DeviceDescription();
	inline HSSDescription* GetAdditionalDescription(){return &additional_description;};
	inline int GetEEPSize(){return eep_size;};
	bool SupportsType(const std::string& type);
	int GetFlags(){return flags;};
	inline int GetVersion(){return version;};
protected:
    typedef std::vector<HS485ChannelDescription*> channels_t;
    channels_t channels;
    typedef std::vector<Type> types_t;
    types_t supported_types;
    typedef std::map<std::string, HS485Paramset> paramsets_t;
	paramsets_t paramsets;
    typedef std::map<int, int> framedefs_by_type_t;
	framedefs_by_type_t framedefs_by_type;
	typedef std::map<std::string, int> framedefs_by_id_t;
	framedefs_by_id_t framedefs_by_id;
	typedef std::vector<FrameDescription> framedefs_t;
	framedefs_t framedefs;
	std::string creation_tag;
	friend class HS485Paramset;
	HSSDescription additional_description;
	int eep_size;
	bool has_serial;
	bool channels_have_subdescriptions;
	int flags;
	int version;
};

#endif // !defined(AFX_HS485DEVICEDESCRIPTION_H__41F2BBEB_C64D_4FB5_A126_713C3C9B7AAF__INCLUDED_)

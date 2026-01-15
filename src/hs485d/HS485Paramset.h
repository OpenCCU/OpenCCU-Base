/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485Paramset.h: Schnittstelle für die Klasse HS485Paramset.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HS485PARAMSET_H__0815F594_247F_4C45_86DA_B8898697C969__INCLUDED_)
#define AFX_HS485PARAMSET_H__0815F594_247F_4C45_86DA_B8898697C969__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <string>
#include <map>
#include <XmlRpc.h>

#include "xmlParser.h"
#include <HSSParameter.h>
#include <HSSParamset.h>
#include "HS485LogicalInstance.h"
#include "HS485CommMessage.h"

class HS485DeviceDescription;
	
//! Zusammenfassung einer Menge von Objekten der Klasse HSSParameter für BidCoS-Wired
class HS485Paramset: public HSSParamset
{
public:
	bool Get(LogicalInstance* inst, const std::string& id, XmlRpc::XmlRpcValue *set);
	bool Put(LogicalInstance* inst, const std::string& id, XmlRpc::XmlRpcValue& set);
	virtual bool InitFromXml(XMLNode &node, XMLNode& root_node);
	bool SetEnforcedValues(HS485LogicalInstance* inst);
	bool ListPeers(HS485LogicalInstance* inst, std::vector<std::string>* peers);
	bool AddPeer(HS485LogicalInstance* inst, const std::string& peer);
	bool RemovePeer(HS485LogicalInstance* inst, const std::string& peer);
	HS485Paramset();
	virtual ~HS485Paramset();
	bool IsLinkset(){return count>1;};
protected:
	bool SetDefaultValues(HS485LogicalInstance* inst);
	bool SetIndexByPeer(HS485LogicalInstance* inst, const std::string& peer);
	int count;
	std::string channel_param_id;
	std::string peer_param_id;
};

#endif // !defined(AFX_HS485PARAMSET_H__0815F594_247F_4C45_86DA_B8898697C969__INCLUDED_)

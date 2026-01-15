/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HS485_MAINTENANCE_CHANNEL_H_
#define _HS485_MAINTENANCE_CHANNEL_H_

#include "HS485Channel.h"
#include <XmlRpc.h>

//! Spezialisierte Kanalklasse für \c MAINTENANCE Kanäle
/*!
 *  Wird verwendet bei Angabe von \c class="maintenance" in der XML-Datei
 *  Von dieser Klasse werden interne Werte gesondert behandelt. Sie spiegelt einfach
 *  die internen Werte des Geräteobjektes. Damit kann über entsprechende Konfiguration
 *  des Kanals in der XML-Datei auf die internen Werte des Gerätes zugegriffen werden.
 */
class HS485MaintenanceChannel :
	public HS485Channel, public LogicalInstance::EventReceiver
{
public:
	HS485MaintenanceChannel(void);
	~HS485MaintenanceChannel(void);
	virtual bool SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event=false);
	virtual bool GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val);
	bool RegisterInternalValueEvent(const std::string& id, LogicalInstance::EventReceiver* rec);
	void OnEvent(LogicalInstance* inst, const std::string& id, XmlRpc::XmlRpcValue& val);
	static bool CheckCreationTag(const char *tag);
	virtual bool CommitPendingConfig(){return true;};
	virtual bool SaveToXml(XMLNode* node);
	virtual bool LoadFromXml(XMLNode& node){return true;};
	virtual bool IsConfigPending(){return false;};
};
#endif //_HS485_MAINTENANCE_CHANNEL_H_

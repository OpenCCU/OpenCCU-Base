/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HS485CENTRAL_H_
#define _HS485CENTRAL_H_

#include "HS485Device.h"
#include <TimerTarget.h>
#include <map>
#include <string>

#include <XmlRpc.h>

//! Spezialisierte Geräteklasse, die das virtuelle Zentralengerät repräsentiert
/*! Die Kanäle dieses Gerätes sind die virtuellen Fernbedienungstasten sowie
 *  der LISTENER-Kanal (Nummer 63), an den die Nachrichten addressiert sind, die für die
 *  Zentrale bestimmt sind.
 */
class HS485Central:public HS485Device
{
public:
	//! Spezialisierte Kanalklasse für die virtuellen Fernbedienungstasten der CCU
	/*! Da es sich nicht um ein physikalisch vorhandenes Gerät handelt, sind die
	 *  Methoden, die in HS485Channel eine Buskommunikation auslösen, mit Versionen überladen,
	 *  die nur auf den internen Datenstrukturen von HS485Channel arbeiten.
	 */
	class HS485CentralChannel:public HS485Channel
	{
	public:
		HS485CentralChannel(void);
		~HS485CentralChannel(void);
		virtual bool GetLinkPeers(std::vector<std::string>* peers);
		virtual bool AddLinkPeer(const std::string& peer);
		virtual bool RemoveLinkPeer(const std::string& peer);
		virtual bool SendMessage(HS485CommMessage* msg);
		bool SetLinkInfo(const std::string& peer, const std::string& name, const std::string& description);
		HS485Central* GetDevice(void);
		static bool CheckCreationTag(const char *tag);
	protected:
	};
	HS485Central(void);
	~HS485Central(void);
	static HS485Central* GetSingleton(){return singleton;};
	virtual bool GetLinkPeers(std::vector<std::string>* peers);
	virtual bool AddLinkPeer(const std::string& peer);
	virtual bool RemoveLinkPeer(const std::string& peer);
	virtual bool SendMessage(HS485CommMessage* msg);
	bool GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val);
	bool SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event=false);
	static bool CheckCreationTag(const char *tag);
	void SetSerial(const std::string s){};
	void SetType(const std::string t){};
	HS485CentralChannel* GetListenerChannel();

	virtual inline bool SetDefaultConfig() { return true; }
	
		//Method used by rfd (which is pure virtual in class LogicalInstance)
	virtual void SetValueAsDefined(const std::string&) {}
	//Method used by rfd (which is pure virtual in class LogicalInstance)
	virtual void SetValueAsUndefined(const std::string&) {}
	
protected:
	void PeerListDirty();
	virtual void ReportEvent(const std::string& id, XmlRpc::XmlRpcValue& val, uint32_t burst_suppression=0) {};
	inline HS485Channel* CreateChannel(){return new HS485CentralChannel();};
	static HS485Central* singleton;
	HS485CentralChannel* listener_channel;
	friend class HS485CentralChannel;
};

#endif

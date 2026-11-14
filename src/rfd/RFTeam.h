/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RFTEAM_H_
#define _RFTEAM_H_

#include "RFDevice.h"
#include <set>
#include <string>

//! Klasse für die virtuellen Team-Kanäle der Team-Geräte
class RFTeamChannel:public RFChannel
{
public:
	RFTeamChannel(void);
	~RFTeamChannel(void);
	virtual bool ClearConfigCache(){return true;};
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("channel_class_team") erzeugen.
	 *  Das ist normalerweise nicht nötig, weil RFTeam::CreateChannel() Objekte dieser Klasse erzeugt.
	 */
	static bool CheckCreationTag(const char *tag);
	//! Setzt Name und Beschreibung für eine bestehende Verknüpfung
	virtual bool SetLinkInfo(const std::string& peer, const std::string& name, const std::string& description);
	//! Einen Kanal zum Team hinzufügen
	void AddTeamChannel(RFChannel* ch);
	//! Einen Kanal aus dem Team entfernen
	void RemoveTeamChannel(RFChannel* ch);
	//! Gibt die Anzahl der Kanäle im Team zurück
	int GetTeamChannelCount(){return set_team_channels.size();};
	bool ReplaceTeamChannel(std::string oldSerial, std::string newSerial);
	virtual bool replaceChannel(RFChannel * oldChannel);
protected:
	//! Dummyfunktion. Team-Kanäle haben keine direkten Verknüpfungen.
	virtual bool GetLinkPeersFromDevice(link_peer_set_t* peers){return true;};
	//! Erweiterung von RFChannel::Describe um das Feld \c TEAM_CHANNELS
	virtual bool Describe(XmlRpc::XmlRpcValue* descr);
	//! Typedef für das Set der Teammitglieder
	typedef std::set<std::string> t_set_team_channels;
	//! Set der Teammitglieder
	t_set_team_channels set_team_channels;
};

//! Klasse für die virtuellen Team-Geräte
class RFTeam :
	public RFDevice
{
public:
	RFTeam(void);
	~RFTeam(void);
	//! Dummyfunktion. Team-Geräte werden on-the-fly erzeugt und nicht gespeichert
	virtual bool Save(){return true;};
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("device_class_team") erzeugen
	 *  Für den &lt;team&gt;-Abschnitt in der XML-Datei ist dies voreingestellt
	 */
	static bool CheckCreationTag(const char *tag);
	//! Dummyfunktion. Team-Geräte sind virtuell und haben keine zu übertragenen Konfigurationsdaten.
	bool CommitPendingConfig();
	//! Dummyfunktion. Es sind nie Konfigurationsdaten zu übertragen
	bool IsConfigPending(){return false;};
	//! Dummyfunktion. Team-Geräte sind virtuell und lassen sich nicht zurücksetzen
	virtual bool FactoryReset(){return true;};
	//! Dummyfunktion. Team-Geräte sind virtuell und lassen sich nicht verknüpfen
	virtual bool UnpeerCentral(){return true;};
	//! Gibt \c false zurück, wenn sich noch Kanäle im Team befinden
	virtual bool Delete();
	virtual bool replaceDevice(RFDevice *oldDevice);
	//! Gibt \c true zurück, da es sich um ein team device (RFTeam) handelt.
	virtual bool IsTeamDeviceInstance();
protected:
	//! Erzeugt eine Instanz von RFTeamChannel
	virtual inline RFChannel* CreateChannel(){return new RFTeamChannel();};

};

#endif

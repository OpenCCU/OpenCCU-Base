/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RFCENTRAL_H_
#define _RFCENTRAL_H_

#include "RFDevice.h"
#include <TimerTarget.h>
#include <map>
#include <string>

#include <XmlRpc.h>

//! Spezialisierte Geräteklasse, die das virtuelle Zentralengerät repräsentiert
/*! Die Kanäle dieses Gerätes sind die virtuellen Fernbedienungstasten sowie
 *  der LISTENER-Kanal (Nummer 63), an den die Nachrichten addressiert sind, die für die 
 *  Zentrale bestimmt sind.
 */
class RFCentral:public RFDevice
{
public:
	//! Timer-IDs
	enum{
		TIMER_SAVE_PEERS=100 //!< Timer, der 10 Sekunden nach der letzten Änderung die Liste der Verknüpfungspartner speichert
	};
	//! Spezialisierte Kanalklasse für die virtuellen Fernbedienungstasten der CCU
	/*! Da es sich nicht um ein physikalisch vorhandenes Gerät handelt, sind die 
	 *  Methoden, die in RFChannel eine Funkkommunikation auslösen, mit Versionen überladen,
	 *  die nur auf den internen Datenstrukturen von RFChannel arbeiten.
	 */
	class RFCentralChannel:public RFChannel
	{
	public:
		//! Konstruktor
		RFCentralChannel(void);
		//! Destruktor
		~RFCentralChannel(void);
		//! Verknüpfungspartner als Array von Seriennummern zurückgeben
		/*! Liefert direkt die gespeicherten Partner zurück, ohne Funkkommunikation.
		 */
		virtual bool GetLinkPeers(std::vector<std::string>* peers);
		//! Fügt einen neuen Verknüpfungspartner hinzu
		virtual bool AddLinkPeer(const std::string& peer);
		//! Trägt einen neuen Verknüpfungspartner in \c RFChannel::link_peers ein
		virtual int LowLevelAddLinkPeer(int peer_address, int peer_channel);
		//! Entfernt einen Verknüpfungspartner
		virtual bool RemoveLinkPeer(const std::string& peer);
		//! Entfernt einen Verknüpfungspartner aus \c RFChannel::link_peers
		virtual bool LowLevelRemoveLinkPeer(int peer_address, int peer_channel);
		//! Dummyimplementierung, gibt \c true zurück
		virtual bool ClearConfigCache(){return true;};
		//! Dummymethode: Es können keine Konfigurationsdaten übertragen werden
		virtual void SetConfigDevDirty(){};
		//! Dummymethode: Keine Übertragung anstehender Konfigurationsdaten nötig
		virtual bool CommitPendingConfig(){return true;};
		//! Laden aus einer XML-Datei
		/*! Ruft RFChannel::LoadFromXml() auf und setzt zusätzlich RFChannel::link_peers_valid
		 *  auf \c true.
		 */
		virtual bool LoadFromXml(XMLNode& node);
		//! Sendet eine Nachricht an alle Verknüpfungspartner
		/*! Ruft dazu einfach RFChannel::SendToPeers() auf.
		 */
		bool SendFrame(BidcosFrame* frame);
		//! Spezialisierte Version von RFChannel::GetDevice(), die ein RFCentral zurückliefert
		RFCentral* GetDevice(void);
		//! Methode für die dynamische Erzeugung mit \c class="central" im XML-File
		static bool CheckCreationTag(const char *tag);
	protected:
	};
	//! Konstruktor
	RFCentral(void);
	//! Destruktor
	~RFCentral(void);
	//! Gibt die einzige Instanz dieser Klasse zurück (RFCentral::singleton)
	static RFCentral* GetSingleton(){return singleton;};
	//! Ruft RFController::SendFrame() auf, ohne sich um irgendwelche Flags zu kümmern
	bool SendFrame(BidcosFrame* frame);
	//! Gibt den LISTENER-Kanal zurück
	RFCentralChannel* GetListenerChannel();
	//! Kompatibilitätsfunktion zum Laden der alten Version der Verknüpfungspartnerliste
	/*! RFCentral wird inzwischen wie alle anderen Geräte auch in einer .dev-Datei gespeichert
	 *  und verwendet den selben Code. Diese Methode existiert, um die alte Version noch lesen
	 *  zu können.
	 */
	bool LoadPeerList();
	//! Abfragen eines internen Wertes
	/*! Implementiert LogicalInstance::GetInternalValue()
	 *  Unterstützte Werte für \c name:
	 *  - \c INSTALL_MODE: int, Anzahl Sekunden, die der Anlernmodus noch aktiv ist
	 *  - \c AES_KEY: int, Index des aktuell von der CCU verwendeten AES-Schlüssels
	 *  - \c ADDRESS: int, BidCoS-Funkadresse der CCU
	 */
	bool GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val);
	//! Setzen eines internen Wertes
	/*! Implementiert LogicalInstance::SetInternalValue()
	 *  Unterstützte Werte für \c name:
	 *  - \c INSTALL_MODE: int, Anzahl Sekunden, die der Anlernmodus noch aktiv ist. \c 0 für aus.
	 */
	bool SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event=false);
	//! Methode für die dynamische Erzeugung mit \c class="central" im XML-File
	static bool CheckCreationTag(const char *tag);
	//! Liefert die Firmware-Version der CCU zurück
	virtual std::string GetFirmwareVersion();
	//! Dummymethode, tut nichts, da der Typ des virtuellen Zentralengerätes fest \c HM-RCV-50 ist
	void SetType(const std::string t){};
	//! Dummymethode, tut nichts, da die Seriennummer des virtuellen Zentralengerätes fest \c BidCoS-RF ist
	void SetSerial(const std::string s){};
	//! Dummymethode: Es können keine Konfigurationsdaten übertragen werden
	virtual void SetConfigDevDirty(){};
	//! Dummymethode: Es können keine Konfigurationsdaten übertragen werden
	virtual bool CommitPendingConfig(){return true;};

protected:
	//! Dispatchmethode für Timer
	void OnTimer(uint32_t cookie);
	//! Fordert den Timer TIMER_SAVE_PEERS an
	void PeerListDirty();
	virtual void ReportEvent(const std::string& id, XmlRpc::XmlRpcValue& val, uint32_t burst_suppression=0) {};
	//! Erzeugt ein Objekt der Klasse RFCentralChannel
	inline RFChannel* CreateChannel(){return new RFCentralChannel();};
	//! Die einzige Instanz dieser Klasse
	static RFCentral* singleton;
	//! Der LISTENER-Kanal, an den z.B. Tasterdrücke gesendet werden
	RFCentralChannel* listener_channel;
	friend class RFCentralChannel;
};

#endif

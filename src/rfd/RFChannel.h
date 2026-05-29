/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RF_CHANNEL_H_
#define _RF_CHANNEL_H_

#include "RFLogicalInstance.h"
#include "RFChannelDescription.h"
#include "BidcosFrame.h"
#include <TimerTarget.h>
#include <set>
#include <xmlParser.h>

#ifdef ReportEvent
#undef ReportEvent
#endif


class RFDevice;
class RFTeam;
class RFTeamChannel;
class JSONObject;


//! Jede Instanz dieser Klasse repr�sentiert einen konkreten Kanal eines konkreten angelernten BidCoS-RF-Ger�tes
/*! Diese Klasse verwaltet die Informationen, die f�r einen konkreten Kanal relevant sind. Informationen,
 *  die eine Kanalklasse betreffen (also das, was aus der XML-Datei gelesen wird), werden von RFChannelDescription
 *  verwaltet. Jede Instanz von RFChannel enth�lt einen Zeiger auf die zugeh�rige Instanz von RFChannelDescription.
 */
class RFChannel : public RFLogicalInstance
{
public:
	//! Timer-Cookies
	enum{
		TIMER_SCHEDULED_GET=1000 //!< Timer f�r das geplante Abfragen eines Wertes nach AES-Verletzung
	};
	//! Zeitkonstanten
	enum{
		EVENT_SUPPRESSION_TIME=2000 //!< Zeit in ms f�r die nach einem Event kein zweites Event f�r den selben Wert erzeugt wird
	};
	//! R�ckgabewerte f�r LowLevelAddLinkPeer()
	enum{
		ADD_PEER_OK=1, //!< Hinzuf�gen des Partners erfolgreich
		ADD_PEER_FAILED=0, //!< Hinzuf�gen des Partners fehlgeschlagen
		ADD_PEER_DEFERED=-1 //!< Hinzuf�gen des Partners erfolgreich, wird sp�ter an das Ger�t �bertragen
	};
	//! Konstruktor
	RFChannel(void);
	//! Destruktor
	virtual ~RFChannel(void);
	bool SetDefaultConfig(void);
	//! Implementierung von LogicalInstance::GetParamsetValues()
	/*! Ermittelt das entsprechende Paramset und ruft dann RFParamset::Get() auf
	 */
	bool GetParamsetValues(const std::string& key, XmlRpc::XmlRpcValue* set);
	bool GetParamsetValues(const std::string& key,int mode, XmlRpc::XmlRpcValue* set);
	//! Implementierung von LogicalInstance::PutParamsetValues()
	/*! Ermittelt das entsprechende Paramset und ruft dann RFParamset::Put() auf
	 */
	bool PutParamsetValues(const std::string& key, XmlRpc::XmlRpcValue& set);
	//! Implementierung von LogicalInstance::GetParamsetDescription()
	/*! Ermittelt das entsprechende Paramset und ruft dann RFParamset::GetDefinition() auf
	 */
	bool GetParamsetDescription(const std::string& key, XmlRpc::XmlRpcValue* set);
	//! Implementierung von LogicalInstance::GetParamsetId()
	/*! Ermittelt das entsprechende Paramset und ruft dann RFParamset::GetId() auf
	 */
	bool GetParamsetId(const std::string& key, std::string* id);
	//! Implementierung von LogicalInstance::DetermineParameter()
	/*! Ermittelt das entsprechende Paramset und ruft dann RFParamset::Determine() auf
	 */
	bool DetermineParameter(const std::string& key, const std::string& parameter);

	//! Implementierung von LogicalInstance::GetValue()
	/*! Ermittelt das entsprechende Paramset und den entsprechenden Parameter und ruft 
	 *  dann HSSParameter::GetValue() auf
	 */
	bool GetValue(const std::string& name, XmlRpc::XmlRpcValue* val);
	bool ReadValue(const std::string& name,int mode, XmlRpc::XmlRpcValue* val);
	//! Implementierung von LogicalInstance::SetValue()
	/*! Ermittelt das entsprechende Paramset und den entsprechenden Parameter und ruft 
	 *  dann HSSParameter::SetValue() auf
	 */
	bool SetValue(const std::string& name, XmlRpc::XmlRpcValue& val);
	//! Erzeugt Defaultwerte f�r ein Verkn�pfungsparameterset
	/*! Ruft RFParamset::SetDefaultValues() mit dem Parameter \c peer_channel auf, um abh�ngig 
	 *  von der Funktion des Partners die Parameter des entsprechenden Verkn�pfungsparametersets
	 *  auf Vorgabewerte zu setzen.
	 */
	bool GenerateDefaultLinkset(RFChannel* peer_channel);
	//! Meldet ein Ereignis an den Logikprozess
	/*! Ruft RFManager::ReportEvent() auf, wenn das letzte Ereignis f�r den Wert \c id l�nger als
	 *  \c burst_suppression ms zur�ckliegt oder sich der Wert seit dem letzten Ereignis ge�ndert hat.
	 */
	void ReportEvent(const std::string& id, XmlRpc::XmlRpcValue& val, uint32_t burst_suppression=0);
	//! Meldet ein Serviceereignis
	/*! Ruft RFManager::ReportServiceMessage() auf
	 */
	void ReportServiceMessage(const std::string& id, XmlRpc::XmlRpcValue& val);
	//! Verarbeitung einer eingehenden Nachricht
	/*! Wird von RFDevice::ProcessIncomingFrame() aufgerufen. 
	 *  Ruft RFChannelDescription::ProcessIncomingFrame() auf.
	 *  Gibt true zurück, wenn der Frame verarbeitet werden konnte und ggf. die Authentifizierung ok war.
	 */
	bool ProcessIncomingFrame(BidcosFrame& msg, FrameDescription* fd);

	void ProcessForwardedFrame(BidcosFrame& msg, FrameDescription* fd);

	//! Wird w�hrend der Initialisierung aufgerufen, um Ger�t, Kanalnummer und Kanalbeschreibung zu setzen
	virtual void SetParent(RFDevice* parent, int index, RFChannelDescription* desc);
	//!Setzt die default Konfiguration so dass diese zum ger�t �bertragen werden und kein vorheriges abfragen statfindet
	void InitDefaultConfig(void);
	//! Setzen der erzwungenen Konfigurationsparameter f�r den Kanal nach dem Anlernen
	/*! Ruft RFChannelDescription::SetEnforcedParameters() auf
	 */
	bool SetEnforcedParameters();
	//! Setzen der erzwungenen Konfigurationsparameter f�r eine Verkn�pfung nach dem Verkn�pfen
	/*! Ruft RFParamset::SetEnforcedValues() f�r das Parameterset \c LINK und den als \c peer_channel
	 *  �bergebenen Partner auf
	 */
	bool SetEnforcedParameters(RFChannel* peer_channel);
	//! Senden einer Nachricht an das zugeh�rige Ger�t
	/*! Der Aufruf wird unver�ndert an RFDevice::SendFrame() weitergeleitet.
	 */
	virtual bool SendFrame(BidcosFrame* frame);
	//! Gibt das zum Kanal geh�rige Ger�teobjekt zur�ck
	virtual RFDevice* GetDevice(){return parent_dev;};
	//! Verkn�pfungspartner als Array von Seriennummern zur�ckgeben
	virtual bool GetLinkPeers(std::vector<std::string>* peers);
	//! Liefert alle direkten Verkn�pfungen zur�ck, an denen das Kanalobjekt beteiligt ist
	virtual bool GetLinks(int flags, link_map_t* result);
	//! F�gt einen neuen Verkn�pfungspartner hinzu
	/*! Falls es sich bei \c this um eine Taste eines Tastenpaares handelt und \c pair==true
	 *  ist, werden f�r beide Tasten des Tastenpaares Verkn�pfungen angelegt.
	 */
	virtual bool AddLinkPeer(const std::string& peer, bool pair);
	//! F�gt einen neuen Verkn�pfungspartner hinzu
	/*! Implementierung von LogicalInstance::AddLinkPeer(). Ruft AddLinkPeer(peer, true) auf.
	 */
	virtual bool AddLinkPeer(const std::string& peer);
	//! L�scht einen Verkn�pfungspartner
	virtual bool RemoveLinkPeer(const std::string& peer);
	//! Setzt Name und Beschreibung f�r eine bestehende Verkn�pfung
	virtual bool SetLinkInfo(const std::string& peer, const std::string& name, const std::string& description);
	//! Ermittelt Namen und Beschreibung f�r eine bestehende Verkn�pfung
	virtual bool GetLinkInfo(const std::string& peer, std::string* name, std::string* description);
	//! Weist ein Ger�t an, das zu einer Verkn�pfung geh�rende Parameterset (Profil) auszuf�hren
	virtual bool ActivateLinkParamset(const std::string& peer, bool longpress);
	//! Gibt die zum Kanal geh�rige Kanalbeschreibung zur�ck
	inline RFChannelDescription* GetDescription()
	{
		if(behaviour<=0)return description;
		return description->GetSubdescription(behaviour);
	}

	//! Gibt die Kanalnummer zur�ck
	inline int GetIndex(){return index;};
	//! Gibt die Funktion des Kanals zur�ck
	/*! \return RFChannelDescription::FUNCTION_A, RFChannelDescription::FUNCTION_B, RFChannelDescription::FUNCTION_AB 
	 */
	inline int GetFunction(){return GetDescription()->GetFunction(index, GetOtherPairIndex());};
	//! Gibt bei Tastenpaaren die Kanalnummer der anderen Taste zur�ck, \c 0 sonst.
	int GetOtherPairIndex();
	//! Gibt bei Tastenpaaren das Kanalobjekt der anderen Taste zur�ck, \c NULL sonst.
	RFChannel* GetOtherPairChannel();
	//! Speichert in \c *descr die Kanalbeschreibung wie an der XmlRpc-Schnittstelle erwartet
	virtual bool Describe(XmlRpc::XmlRpcValue* descr);
	//! Pr�ft, ob noch Konfigurationsdaten f�r den Kanal an das Ger�t �bertragen werden m�ssen
	virtual bool IsConfigPending();
	//! �bertragung anstehender Konfigurationsdaten f�r den Kanal an das Ger�t
	virtual bool CommitPendingConfig();
	//! Speichern der persistenten Daten des Kanals
	/*! Wird im Rahmen von RFDevice::SaveToXml() aufgerufen.
	 */
	virtual bool SaveToXml(XMLNode* node);
	//! Laden der persistenten Daten des Kanals
	/*! Wird im Rahmen von RFDevice::LoadFromXml() aufgerufen.
	 */
	virtual bool LoadFromXml(XMLNode& node);
	//! Speichern des Ger�teobjektes anfordern
	/*! Der Aufruf wird unver�ndert an RFDevice::RequestSave() weitergeleitet.
	 */
	void RequestSave();
	//! L�scht alle im Schnittstellenprozess gespeicherten Daten zur Kanalkonfiguration
	/*! Wird von RFDevice::ClearConfigCache() aufgerufen.
	 */
	virtual bool ClearConfigCache();
	//! Setzen eines internen Wertes
	/*! Implementiert LogicalInstance::SetInternalValue()
	 *  Unterst�tzte Werte f�r \c name:
	 *  - \c AES
	 */
	virtual bool SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event=false);
	//! Abfragen eines internen Wertes
	/*! Implementiert LogicalInstance::GetInternalValue()
	 *  Unterst�tzte Werte f�r \c name:
	 *  - \c AES
	 *  - \c FUNCTION
	 */
	virtual bool GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val);
	//! Zeigt die Verwendung eines Wertes in der Logikschicht an
	/*! Siehe XmlRpcMethodReportValueUsage
	 *  F�hrt zum automatischen Herstellen und L�sen der Verkn�pfung mit dem Kanal 63 der CCU
	 *  bei Kan�len mit RFChannelDescription::autoregister_central=true.
	 */
	virtual bool ReportValueUsage(const std::string& value, int count);
	//! L�st alle evtl. bestehenden Verkn�pfungen mit Kan�len der CCU
	bool UnpeerCentral();
	//! Nachricht an alle Verkn�pfungspartner �bertragen
	/*! Wird beim Senden von simulierten Nachrichten aufgerufen. Sendet \c msg an alle Verkn�pfungspartner
	 *  unter Ber�cksichtigung von Kommunikationsparametern wie Burst.
	 */
	bool SendToPeers(BidcosFrame* frame);
	//! Gibt die Kanalseriennummer (=Adresse an der XmlRpc-Schnittstelle) zur�ck
	inline const std::string& GetSerial(){return serial;};
	//! Markiert alle Konfigurationsdaten als noch an das Ger�t zu �bertragen
	virtual void SetConfigDevDirty();
	//! Gibt zur�ck, ob AES f�r den Kanal aktiviert ist
	inline bool GetAES(){return aes;};
	//! Plant das automatische Abfragen eines Wertes
	/*! Der Wert \c id wird automatisch nach einer zuf�lligen Zeit zwischen 2 und 4 Sekunden
	 *  vom Ger�t abgefragt. Nach der Abfrage wird ein Ereignis ausgel�st.
	 */
	void ScheduleValueGet(const std::string& id);
	//! Dispatchmethode f�r Timer
	void OnTimer(uint32_t cookie);
	//! Liefert das Team zur�ck, zu dem der Kanal geh�rt, bzw. \c NULL, falls es kein Team gibt.
	RFTeamChannel* GetTeamChannel(){return team_channel;};
	//! Erzeugt f�r das zum Kanal geh�rige Team ein Team-Objekt
	/*! Kehrt sofort zur�ck, wenn der Kanal kein Team unterst�tzt.
	 *  Abh�ngig vom in \c link_peers eingetragenen Verkn�pfungspartner wird bei Bedarf ein Team-Objekt
	 *  (RFTeamChannel) erzeugt und die gegenseitigen Zeiger zwischen Team und Kanal gesetzt.
	 *  Existiert kein Verkn�pfungspartner, wird zuerst eine Verkn�pfung auf den Kanal selber angelegt.
	 */
	void CreateTeam(RFDeviceDescription* team_description=NULL);
	//! Weist dem Kanal ein neues Team zu
	/*! Die Liste der Verkn�pfungspartner wird entsprechend angepasst und an das Ger�t �bertragen.
	 *  Falls das vorherige Team keine Mitglieder mehr hat, wird es gel�scht.
	 */
	bool SetTeam(RFTeamChannel* team);
	//! Weist dem Kanal w�hrend der Initialisierung seine Kanalseriennummer zu
	void SetSerial(const std::string& s);
	virtual bool PushDefaultConfig(void);
	bool GetConfig(XmlRpc::XmlRpcValue* c);
	virtual bool replaceChannel(RFChannel * oldChannel);
	bool replacePeer(std::string old_peer, std::string new_peer);
	virtual void SetValueAsDefined(const std::string& name);
	virtual void SetValueAsUndefined(const std::string& name);

	unsigned int getAesCbcCounter();
	virtual void setAesCbcCounter(const unsigned int cbc_counter);

	int GetBehaviour();
	bool SetBehaviour(const int b);

	/** \brief Arbeitet einen weitergeleiteten Frame ab.
	 * \details Sofern in der Gerätebeschreibung entsprechend angegeben, können Werte aus Frames, die für eine anderen Kanal eingehen zu anderen Kanälen weitergeleitet werden.
	 * Diese Methode arbeitet einen weitergeleiteten Frame ab.
	 * (Diese Methode wird aus RFDevice::ProcessIncomingFrame heraus aufgerufen.)
	 */
	void processForwardedFrame(BidcosFrame& msg, FrameDescription* fd);

protected:
	//! Struct f�r die Speicherung von Name und Beschreibung einer Verkn�pfung
	typedef struct{
		std::string name; //!< Verkn�pfungsname
		std::string description; //!< Verkn�pfungsbeschreibung
	}link_t;
	//! Typedef f�r die Zuordnung von Namen und Beschreibungen zu Verkn�pfungen
	typedef std::map<uint32_t, link_t> link_peer_map_t;
	//! Typedef f�r eine Menge von Verkn�pfungspartnern
	typedef std::set<uint32_t> link_peer_set_t;
	//! Erzeugt eine neue Verkn�pfung im Ger�t
	virtual int LowLevelAddLinkPeer(int peer_address, int peer_channel_a, int peer_channel_b);
	//! L�scht eine Verkn�pfung im Ger�t
	virtual bool LowLevelRemoveLinkPeer(int peer_address, int peer_channel);
	virtual bool LowLevelRemoveLinkPeer(int peer_address, int peer_channel_a,int peer_channel_b);
	//! Wird aufgerufen unmittelbar nachdem Konfigurationsdaten vom Ger�t gelesen wurden
	/*! Bietet einen Einsprungpunkt f�r das zwangsweise Setzen von Konfigurationsparametern.
	 *  Z.Zt. wird ein vom Ger�t gelesenes Flag f�r AES aktiv zwangsweise auf den Wert von 
	 *  \c aes gesetzt und diese �nderung auch wieder an das Ger�t �bertragen
	 */
	virtual void OnConfigReadFromDevice(RFConfigData* cd, int list);
	//! Holt nach Planung mit ScheduleValueGet() den Wert vom Ger�t
	void ProcessScheduledValueGet();
	//! Liest die Liste der Verkn�pfungspartner f�r den Kanal vom Ger�t
	virtual bool GetLinkPeersFromDevice(link_peer_set_t* peers);
	//! Pr�ft, ob der Kanal mit der angegebenen Ger�teadresse und Kanalnummer mit diesem Kanal verkn�pft ist
	bool IsLinkedTo(int peer_address, int peer_channel);
	//! F�hrt die nach dem �ndern von \c aes n�tigen Aktionen durch
	/*! - �bertr�gt die �nderung falls n�tig an das Ger�t
	 *  - Ruft RFDevice::EnableAESForChannel() oder RFDevice::DisableAESForChannel() auf
	 *  - Setzt bei allen Verkn�pfungspartnern das Flag \c EXPECT_AES entsprechend
	 */
	void UpdateAESFlag();

	virtual bool performCBCAuthentification(BidcosFrame& bidcosFrame);

	//! Zeiger auf das zugeh�rige Ger�teobjekt
	RFDevice* parent_dev;
	//! Zeiger auf die zugeh�rige Kanalbeschreibung
	RFChannelDescription* description;
	//! Map der Verkn�pfungspartner, Namen und Beschreibungen
	link_peer_map_t link_peers;
	//! Gibt an, ob die Liste der Verkn�pfungspartner bereits vom Ger�t abgefragt wurde
	bool link_peers_valid;
	//! Gibt an, ob die Liste der Verkn�pfungspartner noch an das Ger�t �bertragen werden muss
	bool link_peers_dirty;
	//! Gibt an, ob noch Konfigurationsdaten an das Ger�t �bertragen werden m�ssen
	bool config_data_dirty;
	//! Kanalnummer
	int index;
	//! Flag f�r AES eingeschaltet
	bool aes;
	//! Zeiger auf das zugeh�rige Team oder NULL
	RFTeamChannel* team_channel;
	//! Typedef f�r Speicherung der Information �ber in der Logigschicht verwendete Werte
	typedef std::map<std::string, int> t_value_usage_map;
	//! Information �ber in der Logikschicht verwendete Werte f�r automatisches Verkn�pfen mit der CCU
	t_value_usage_map value_usage_map;
	//! Kanalseriennummer
	std::string serial;
	//! Menge der Werte, die noch geplant abgefragt werden m�ssen
	std::set<std::string> scheduled_get_values;
	//! Speichert die Anzahl durchgeführter geplanter Abfragen von Werten.
	std::map<std::string, int> scheduled_get_valuse_cnt;
	//! Speicher f�r versendete Ereignisse mit Zeitstempeln zur Unterdr�ckung von Bursts
	ValueStore event_store;

	unsigned int aes_cbc_counter;

	/** \brief Channel behaviour.*/
	int behaviour;

	/** \brief Change of behaviour pending.
	 * \details For non-permanent listening devices.
	 */
	bool behaviourChangePending;

	virtual bool replaceRFConfigData(RFLogicalInstance *oldInstance);

	/** \brief Fires an error event if a device responded with NAK.
	 * \details
	 * 0: RESERVED
	 * 	1: Allgemeiner Fehler / Unknown error
	 * 	2: Gerät beschäftigt / Busy
	 * 	3: Gerätespeicher voll / MemFull
	 *	4: Zieladresse nicht vorhanden / target invalid
	 * 	5: Kanal nicht vorhanden / invalid channel
	 *	Beispiel event(0123456789:1, ERROR, {1, 'Unknown Error'})
	 */
	void checkAndFireNAKErrorEvent(BidcosFrame* requestFrame);

	/** \brief Called by SetValue if parameter has write dependencies (see HSSParameter)
	* \param param Parameter to set value for.
	* \param value Value to set to parameter.
	*/
	bool setValueWithWriteDependencies(HSSParameter* param, XmlRpc::XmlRpcValue& value);
	bool callSetValue(const JSONObject& jsonObj, const std::string& paramName, HSSLogicalType* pLType);
};
#endif //_RF_CHANNEL_H_

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// RFDevice.h: Schnittstelle f�r die Klasse RFDevice.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RFDEVICE_H__31222142_578A_4D7D_917B_5F9694706BFF__INCLUDED_)
#define AFX_RFDEVICE_H__31222142_578A_4D7D_917B_5F9694706BFF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RFParamset.h"
#include "RFChannel.h"
#include "RFLogicalInstance.h"
#include "BidcosFrame.h"
#include "RFFirmwareManager.h"
#include <TimerTarget.h>
#include <vector>
#include <queue>
#include <typedefs.h>

//! Jede Instanz dieser Klasse k�mmert sich um ein konkretes angelerntes BidCoS-RF-Ger�t
/*! Diese Klasse verwaltet die Informationen, die f�r ein konkretes Ger�t zutreffen. Dies sind 
 *  Zustandsinformationen und Adressierungsinformationen. Informationen, die eine Ger�teklasse
 *  betreffen (also das, was aus der XML-Datei gelesen wird), werden von RFDeviceDescription
 *  verwaltet. Jede Instanz von RFDevice enth�lt einen Zeiger auf die zugeh�rige Instanz von
 *  RFDeviceDescription.
 *  Die Ger�tekan�le werden analog dazu von RFChannel / RFChannelDescription verwaltet. Jede
 *  Instanz von RFDevice enth�lt dazu eine Map von Instanzen von RFChannel und RFDeviceDescription
 *  enth�lt einen Vektor mit Instanzen von RFChannelDescription.
 */
class RFDevice : public RFLogicalInstance
{
public:
	//! Intern verwaltete Flags, die Servicemeldungen erzeugen
	enum{
		FLAG_UNREACH=(1<<0), //!< Ger�t ist aktuell nicht erreichbar
		FLAG_STICKY_UNREACH=(1<<1), //!< Ger�t war nicht erreichbar
		FLAG_LOWBAT=(1<<2), //!< Ger�tebatterie fast leer
		FLAG_DUTYCYCLE=(1<<3) //!< Dutycycle des Ger�tes zu mindestens 90% ausgesch�pft
	};

	//! Konstanten f�r Zeiten
	enum{
		BURST_ACTIVE_INTERVAL=1800, //!< Zeit in ms f�r die ein Ger�t nach einer Burst-Aussendung ohne weiteren Burst angesprochen werden kann
		FAIL_COUNTER_RESET_TIME=5000 //!< Zeit in ms nach einem fehlerhaften Sendeversuch, in der keine neue Sendung versucht wird
	};
	//! Implementierung von LogicalInstance::GetParamsetValues()
	/*! Ermittelt das entsprechende Paramset und ruft dann RFParamset::Get() auf
	 */
	bool SetDefaultConfig(void);
	bool PushDefaultConfig();
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
	bool GetParamsetId(const std::string& type, std::string* id);
	//! Implementierung von LogicalInstance::DetermineParameter()
	/*! Ermittelt das entsprechende Paramset und ruft dann RFParamset::Determine() auf
	 */
	bool DetermineParameter(const std::string& key, const std::string& parameter);

	//! Implementierung von LogicalInstance::GetValue()
	/*! Gibt immer false zur�ck, da Ger�te keine Werte unterst�tzen.
	 */
	bool GetValue(const std::string& name, XmlRpc::XmlRpcValue* val);
	bool ReadValue(const std::string& name,int mode, XmlRpc::XmlRpcValue* val);
	//! Implementierung von LogicalInstance::SetValue()
	/*! Gibt immer false zur�ck, da Ger�te keine Werte unterst�tzen.
	 */
	bool SetValue(const std::string& name, XmlRpc::XmlRpcValue& val);
	//! Sendet eine Kommunikationsnachricht an das zugeordnete Ger�t
	/*! \c msg wird durch Eintragen der Empf�ngeradresse und Setzen der notwendigen Flags
	 *  sowie des korrekten Telegrammz�hlers modifiziert. Falls n�tig, wird auf eine Antwort 
	 *  gewartet. Die internen Flags \c UNREACH und \c STICKY_UNREACH werden aktualisiert.
	 *  Falls der letzte fehlerhafte Sendeversuch nicht l�nger als \c FAIL_COUNTER_RESET_TIME
	 *  zur�ckliegt, wird nicht erneut versucht, zu senden.
	 */
	virtual bool SendFrame(BidcosFrame* frame);
	//! Verarbeitung einer asynchron eingehenden Nachricht vom Ger�t
	/*! Folgende Aktionen werden durchgef�hrt:
	 *  - Falls das Ger�t zum L�schen markiert ist, wird es gel�scht
	 *  - Falls noch Konfigurationsdaten an das Ger�t zu �bertragen sind, wird die �bertragung
	 *    f�r den n�chstm�glich Zeitpunkt vorgemerkt.
	 *  - Falls es sich um ein neu angelerntes Ger�t handelt, wird der beim Anlernen �bertragene
	 *    Sysinfo-Rahmen verwendet, um interne Datenstrukturen zu initialisieren. Au�erdem
	 *    wird die Zentralenadresse als Konfigurationsparameter an das Ger�t �bertragen. Falls
	 *    ein Schl�sseltausch n�tig ist, wird dieser durchgef�hrt.
	 *  - Falls es sich um eine unaufgeforderte Mitteilung von Konfigurationsdaten handelt,
	 *    RFChannel::ProcessAsyncParamInfo() des entsprechenden Kanals aufgerufen.
	 *  - Es werden die internen Flags f�r \c LOWBAT, \c DUTYCYCLE, \c UNREACH, etc. aktualisiert.
	 *  - Es wird versucht, die eingehende Nachricht einer FrameDescription zuzuordnen. Falls das klappt,
	 *    wird RFChannel::ProcessIncomingFrame() des entsprechendne Kanals aufgerufen.
	 *  - Es werden die gespeicherten RSSI-Informationen aktualisiert.
	 *  
	 */
	bool ProcessIncomingFrame(BidcosFrame& msg);
	//! Setzen des zugeordneten Objektes der Klasse RFDeviceDescription
	/*! Wird von RFManager w�hrend der Initialisierung eines Objektes dieser Klasse aufgerufen.
	 */
	virtual void SetDeviceDescription(RFDeviceDescription* description)
	{
		this->description=description;
	}
	//! Erzeugt f�r jeden Kanal ein Objekt der Klasse RFChannel
	/*! Verwendet dazu Informationen aus RFDeviceDescription.
	 */
	virtual void CreateChannels();
	//! Liefert das zugeordneten Objektes der Klasse RFDeviceDescription zur�ck
	RFDeviceDescription* GetDeviceDescription();
	//! Setzen der BidCoS-Adresse
	/*! Wird von RFManager w�hrend der Initialisierung eines Objektes dieser Klasse aufgerufen.
	 */
	inline void SetAddress(int address){this->address=address;}
	//! Abfragen der BidCoS-Adresse
	inline int GetAddress(){return this->address;}
	//! Index des aktuellen AES-Schl�ssels abfragen
	inline int GetAESKey(){return cur_aes_key;}
	//! Liefert eine Ger�tebeschreibung wie an der XmlRpc-Schnittstelle erwartet
	bool Describe(XmlRpc::XmlRpcValue* val);
	//! Erzwungene Werte in allen Parametersets setzen
	/*! Ruft rekursiv RFChannel::SetEnforcedParameters() aller Kan�le auf
	 */
	bool SetEnforcedParameters();
	//! Liefert ein Kanal- oder Ger�tebojekt zur Kanalnummer zur�ck
	/*! Implementiert RFLogicalInstance::GetInstance()
	 *  Liefert f�r \c channel_index>=0 das entsprechende Kanalobjekt und f�r \c channel_index==-1
	 *  das Ger�teobjekt zur�ck.
	 */
	RFLogicalInstance* GetInstance(int channel_index);
	//! Liefert das Ger�tebojekt zur�ck
	/*! Implementiert RFLogicalInstance::GetDevice()
	 */
	virtual RFDevice* GetDevice(){return this;}
	//! Konstruktor
	RFDevice();
	//! Destruktor
	virtual ~RFDevice();
	//! Ger�tetyp (Kurzbezeichnung) setzen
	virtual void SetType(const std::string t){type=t;}
	//! Seriennummer (=Adresse f�r XmlRpc-Schnittstelle) setzen
	virtual void SetSerial(const std::string s);
	//! Setzen eines internen Wertes
	/*! Implementiert LogicalInstance::SetInternalValue()
	 *  Unterst�tzte Werte f�r \c name:
	 *  - \c STICKY_UNREACH
	 *  - \c LOWBAT
	 *  - \c DUTYCYCLE
	 */
	virtual bool SetInternalValue(const std::string& name, XmlRpc::XmlRpcValue& val, bool fire_event=false);
	//! Abfragen eines internen Wertes
	/*! Implementiert LogicalInstance::GetInternalValue()
	 *  Unterst�tzte Werte f�r \c name:
	 *  - \c CONFIG_PENDING
	 *  - \c UNREACH
	 *  - \c STICKY_UNREACH
	 *  - \c LOWBAT
	 *  - \c DUTYCYCLE
	 *  - \c AES_KEY
	 *  - \c NEEDS_BURST
	 */
	virtual bool GetInternalValue(const std::string& name, XmlRpc::XmlRpcValue* val);
	//! Liefert den Ger�tetypen (Kurzbezeichnung)
	const std::string& GetType(){return type;}
	//! Liefert die Ger�teseriennummer (=Adresse f�r XmlRpc-Schnittstelle)
	const std::string& GetSerial(){return serial;}
	//! �bertr�gt noch nicht �bertragene Konfigurationsdaten an das Ger�t
	/*! Ruft RFChannel::CommitPendingConfig f�r alle Kan�le auf
	 */
	virtual bool CommitPendingConfig();
	//! Setzt den internen Wert \c CONFIG_PENDING
	/*! Pr�ft, ob sich seit dem letzten Aufruf das Ergebnis von IsConfigPending() ge�ndert hat.
	 *  Setzt den internen Wert \c CONFIG_PENDING entsprechend und erzeugt darauf ein Ereignis falls
	 *  n�tig.
	 *  Ruft CheckWakeup() auf.
	 */
	void CheckConfigPendingEvent(bool force = false);
	//! Pr�ft, ob Konfigurationsdaten zur �bertragung an das Ger�t anstehen
	/*! Die R�ckgabe von RFChannel::IsConfigPending() der Kan�le wird mit einbezogen.
	 */
	virtual bool IsConfigPending();
	//! Speichern der persistenten Daten f�r das Ger�t und alle Kan�le in eine XML-Datei
	bool SaveToXml(XMLNode* node);
	//! Laden der persistenten Daten des Ger�tes und aller Kan�le aus einer XML-Datei
	bool LoadFromXml(XMLNode& node);
	//! Speichern des Ger�tes
	/*! Legt eine Datei \c /etc/config/rfd/&lt;Seriennummer&gt;.dev an und speichert durch Aufruf
	 *  von SaveToXml() das Ger�t darin ab. Dabei wird mit Kopieren und Umbenennen gearbeitet,
	 *  damit zu jedem Zeitpunkt eine intakte Version der Ger�tedatei vorhanden ist.
	 */
	virtual bool Save();
	//! L�scht das Ger�t
	/*! Das Ger�teobjekt wird aus den Listen f�r AES-Ger�te und Ger�te, die ein Wakeup ben�tigen
	 *  in RFController ausgetragen. Die Datei mit den persistenten Ger�tedaten (siehe Save())
	 *  wird gel�scht.
	 */	 
	virtual bool Delete();
	//! Bricht das L�schen des Ger�ts ab
	/*! Setzt das Flag DEFERRED_DELETE zur�ck.
	 */
	virtual bool AbortDelete();	
	//! L�scht alle im Schnittstellenprozess gespeicherten Daten zur Ger�tekonfiguration
	/*! Diese Daten werden neu vom Ger�t abgefragt, wenn sie das n�chste mal ben�tigt werden.
	 *  Kann �ber \c devconfig.cgi aufgerufen werden. Bitte nur zum Debuggen verwenden.
	 */
	bool ClearConfigCache();
	//! Setzt einen Timer f�r sofort zum �bertragen von Konfigurationsdaten
	/*! \param set_enforced_parameters Gibt an, ob vor dem �bertragen der Konfigurationsdaten
	 *  die erzwungenen Werte gesetzt werden sollen (siehe SetEnforcedParameters())
	 */
	void ScheduleConfig(bool set_enforced_parameters);
	//! Setzt einen Timer f�r setzen der Defaultwerte und �bertragen der Konfigurationen
	/*! \param
	 *
	*/
	void ScheduleInclusionPushMode(void);
	//! Liefert eine Liste der belegten Kanalnummern zur�ck
	std::vector<int> ListChannels();
	//! F�hrt einen Schl�sseltausch durch
	bool ChangeAESKey();
	//! Vom Ger�t verwendeten Schl�sselindex setzen
	/*! Wird von RFManager aufgerufen, nachdem vom Ger�t eine AES-Aufgabe empfangen wurde, bei
	 *  der das Ger�t den aktuell verwendeten Schl�sselindex �bertr�gt.
	 */
	void SetKeyIndex(int key_index);
	//! Setzt f�r das Ger�t den Schl�sselindex und die AES-aktiven Kan�le im Interface-Concentrator
	/*! Wird nach dem Erzeugen / Laden eines Ger�tes aufgerufen und wenn sich an einem Kanal der AES-Status ge�ndert hat.
	 */
	bool SetAesPolicy();
	//! AES-�bertragung f�r einen Kanal ausschalten
	/*! Wird von RFChannel aufgerufen, um die AES-�bertragung f�r den Kanal zu deaktivieren.
	 *  Ruft RFController::RemoveAESDevice() auf und setzt die Instanzvariable \c aes neu.
	 */
	void DisableAESForChannel(int ch_index);
	//! Stellt fest, ob f�r das Ger�t ein Schl�sseltausch n�tig ist
	/*! Vergleicht dazu den Schl�sselindex des Ger�tes mit dem von RFManager
	 */
	bool NeedsAESKeyChange();
	//! Ger�t in Werkseinstellungen zur�cksetzen
	/*! Setzt das Ger�t durch Senden des entsprechenden Funkbefehls auf Werkseinstellungen zur�ck
	 */
	virtual bool FactoryReset();
	//! L�scht direkten Verkn�pfungen mit der Zentrale
	/*! Wird beim Ablernen eines Ger�tes aufgerufen, um f�r alle Ger�tekan�le die
	 *  Verkn�pfungen mit Kan�len der CCU zu l�schen.
	 */
	virtual bool UnpeerCentral();
	//! Liefert die direkten Verkn�pfungen aller Kan�le zur�ck
	/*! Implementiert LogicalInstance::GetLinks()
	 */
	bool GetLinks(int flags, link_map_t* result);
	//! Liefert die beim Anlernen aus dem Sysinfo-Rahmen entnommene Firmwareversion zur�ck
	virtual inline std::string GetFirmwareVersion(){return firmware_version;}
	//! Markiert alle Konfigurationsdaten des Ger�te und aller Kan�le f�r �bertragung zum Ger�t
	/*! Wird von RestoreConfigToDevice() aufgerufen, um alle Konfigurationsdaten als im Ger�t
	 *  ung�ltig zu markieren.
	 */
	virtual void SetConfigDevDirty();
	//! Alle Konfigurationsdaten erneut an das Ger�t �bertragen
	/*! Stellt die komplette Konfiguration eines Ger�tes aus den in der CCU gespeicherten Daten
	 *  wieder her. Bedient sich dazu SetConfigDevDirty() und CommitPendingConfig()
	 */
	bool RestoreConfigToDevice();
	//! Markiert ein Ger�t zum L�schen, sobald es erreichbar ist
	/*! Soll ein Ger�t aus der CCU gel�scht werden, ist aber nicht erreichbar, kann der Anwender
	 *  ausw�hlen, dass das Ger�t bei n�chster Gelegenheit gel�scht werden soll. Das Markieren f�r
	 *  "bei n�chster Gelegenheit l�schen" �bernimmt diese Methode.
	 *  \param flags Flags, die von RFManager �bergeben werden. Diese werden gespeichert und beim
	 *  tats�chlichen L�schen wieder an RFManager::DeleteDevice() �bergeben.
	 */
	void ScheduleDelete(int flags);
	//! Setzt einen Timer f�r sofort zum Speichern des Ger�tes
	/*! Um mehrfaches Speichern bei mehreren zusammenh�ngenden �nderungen am Ger�t oder der
	 *  Ger�tekonfiguration zu vermeiden, wird das Speichern �ber einen Timer zu einem sp�teren
	 *  Zeitpunkt aus der Hauptschleife heraus durchgef�hrt.
	 */
	void RequestSave();
	//! Stellt eine Nachricht in die Warteschlange f�r nach dem Aufwachen eines Ger�tes zu �bertragende Nachrichten
	/*! W�hrend Nachrichten f�r zu �bertragende Konfigurationsdaten unmittelbar vor dem Senden neu erzeugt
	 *  werden, werden Nachrichten f�r Zustands�nderungen an Ger�ten, die nur nach einem Wakeup anzusprechen sind,
	 *  in einer Warteschlange mit maximal 10 Eintr�gen verwaltet. Die �ltesten Eintr�ge fallen dabei zuerst
	 *  heraus.
	 */
	void QueueAfterWakeupFrame(BidcosFrame& frame);
	//! Gibt den beim Anlernen empfangenen Sysinfo-Rahmen zur�ck
	BidcosFrame* GetStoredSysinfo()
	{
		return &sysinfo_frame;
	}
	//! Gibt an, ob sich das Ger�t noch im Anlernvorgang befindet
	/*! 
	 *  Gibt nur \c true zur�ck, w�hrend das Ger�t erstmalig angelernt wird. Wird verwendet, um
	 *  zu verhindern, dass ein bereits angelerntes Ger�t den AES-Standardschl�ssel �bertragen
	 *  bekommt, weil dies im Zusammenhang mit dem vom Kunden w�hlbaren Tempor�rschl�ssel eine
	 *  gro�e Sicherheitsl�cke w�re.
	 */
	inline bool IsNewDevice(){return is_new_device;}
    //! Setzt das Interface, �ber das mit dem Ger�t kommuniziert wird.
    /*!
     *  Abgefragt werden kann diese Wert �ber die Ger�tebeschreibung
     *  \param interface_id Seriennummer des zu verwendenden Interfaces
     *  \param roaming Gibt an, ob die Interface-Zuordung abh�ngig von der Empfangsfeldst�rke im laufenden
     *         Betrieb ge�ndert werden soll.
     */
    bool SetBidcosInterface(const std::string& interface_id, bool roaming);
	//! Setzt den RSSI-Wert, der am Ger�t gemessen wurde.
	/*!
	 * \param rssi RSSI-Wert, der am Ger�t gemessen wurde.
	 */
	void SetDeviceRSSI(int rssi);
	//! Setzt den RSSI-Wert, der an der lokalen Antenne gemessen wurde.
	/*
	 * \param rssi RSSI-Wert, der an der lokalen Antenne gemessen wurde.
	 */
	void SetPeerRSSI(int rssi);
	//! F�hrt ein Firmwareupdate f�r das entsprechende Ger�t durch
	bool UpdateFirmware();
	//! Liefert die f�r dieses Ger�t verf�gbare Firmwareversion
	const std::string GetAvailableFirmware();
	//!
	bool Rebuild();
	bool GetConfig(XmlRpc::XmlRpcValue* c);
	void SetUpdatePennding(bool val);
	virtual bool replaceDevice(RFDevice *oldDevice);
	const std::vector<std::string> &GetReplaceHistory();
	void DeleteFromReplaceHistory(std::string serial);
	bool IsReplaceCompatible(RFDevice *newDevice);
	bool InitVirtualInstance(BidcosFrame &sysinfoFrame);
	virtual void SetValueAsDefined(const std::string& name);
	virtual void SetValueAsUndefined(const std::string& name);
protected:
	//! Verschiedene Timer-Cookies
	enum{
		TIMER_RESET_FAIL_COUNTER=1000, //!< Timer zum Aufheben der Sperre nach zwei fehlgeschlagenen Kommunikationsversuchen
		TIMER_COMMIT_CONFIG, //!< Timer f�r das Versenden von Konfigurationsdaten
		TIMER_INCLUDE_PUSH_DEFAULT,
		TIMER_CYCLIC_TIMEOUT, //!< Timer f�r Zeit�berschreitung bei sich zyklisch meldenden Ger�ten
		TIMER_SAVE //!< Timer f�r die Speicherung der Ger�tedaten
	};
	//! Flags f�r TIMER_COMMIT_CONFIG
	enum{
		SCHEDULE_COMMIT=(1<<0), //!< Allgemeines Flag f�r Konfigurationsdaten sollen �bertragen werden
		SCHEDULE_SET_ENFORCED=(1<<1) //!< Beim �bertragen von Konfigurationsdaten erzwungene Werte setzen
	};
	//! Interne Hilfsmethode zum Hinzuf�gen eines Kanals
	bool AddChannel(int index, const std::string& type);
	//! Interne Hilfsmethode zum Update der internen Werte \c LOWBAT und \c DUTY_CYCLE
	void UpdateDeviceFlags(BidcosFrame& frame);
	//! Interne Hilfsmethode zum Anpassen des AES-Schl�sselindex an den vom Ger�t verwendeten Schl�ssel
	/*! Wird nach jeder Sendung mit dem gesendeten Rahmen aufgerufen.
	 */
	void UpdateCurAESKey(BidcosFrame* frame);
	//! Dispatch- und Behandlungsmethode f�r Timer
	void OnTimer(uint32_t cookie);
	//! L�scht alle Kan�le
	void ClearChannels();
	//! Meldet das Ger�t je nach Bedarf bei RFController f�r einen Aufweckvorgang an- oder ab
	/*! Es wird gepr�ft, ob
	 *  - Das Ger�t einen Aufweckvorgang ben�tigt, um angesprochen werden zu k�nnen
	 *    (Siehe RFDeviceDescription::RxNeedsWakeup()) und
	 *  - Konfigurationsdaten zu senden sind oder
	 *  - sich Nachrichten in der \c after_wakeup_queue befinden.
	 */
	void CheckWakeup();
	//! Sendet die Nachrichten aus der \c after_wakeup_queue zum Ger�t
	void SendAfterWakeupFrames();
	virtual void ReportEvent(const std::string& id, XmlRpc::XmlRpcValue& val, uint32_t burst_suppression=0) {};
	//! Erzeugt eine Instanz von RFChannel
	/*! Kann in von RFDevice abgeleiteten Klassen �berladen werden, um spezialisierte Kanalobjekte zu erzeugen.
	 */
	virtual inline RFChannel* CreateChannel(){return new RFChannel();}
	//! Update der Flags \c UNREACH und \c STICKY_UNREACH nach dem Empfang einer Nachricht
	void UpdateUnreachFlags(BidcosFrame* frame);
	//! Gibt \c true zur�ck, wenn AES f�r mindestens einen Kanal des Ger�tes aktiviert ist
	bool GetAES(){return aes;}
	//! Gibt ein Bitfeld zur�ck, das angibt, f�r welche Kan�le AES aktiv ist
	uint64 GetChannelAESMask();
	//! Ermittelt, ob das Ger�t interne Tasten besitzt.
	bool HasInternalKeys();
	//! BidCoS-RF Adresse des Ger�tes
    int address;
	//! Anzahl aufeinanderfolgender fehlerhafter Versuche, das Ger�t anzusprechen
	unsigned int fail_counter;
	//! Flagfeld f�r interne Werte (\c FLAG_UNREACH, etc.)
	uint32_t maintenance_flags;
	BidcosFrame::unreach_reason_t unreach_reason;
	//! Zeiger auf das Beschreibungsobjekt f�r die Ger�teklasse
	RFDeviceDescription* description;
	//! Typedef f�r Ger�tekan�le
	typedef std::map<int, RFChannel*> channels_t;
	//! MAp der Ger�tekan�le
	channels_t channels;
	//! Typedef f�r das Speichern der RSSI-Werte
	typedef std::map<int, int> rssi_map_t;
	friend class RFChannel;
	//! Ger�tetyp (Kurzbezeichnung)
	std::string type;
	//! Ger�teseriennummer (Adresse an der XmlRpc-Schnittstelle)
	std::string serial;
	//! Bei \c true m�ssen noch Konfigurationsdaten an das Ger�t �bertragen werden
	bool config_data_dirty;
	//! Gespeicherte Flags f�r das Verz�gerte L�schen von Ger�ten
	int delete_deferred_flags;
	//! Flag f�r Pr�fung auf �nderung von \c config_pending
	bool last_config_pending;
	//! Flags f�r Verz�gerte �bertragung von Konfigurationsdaten
	int config_schedule;
	//! Bei \c true ist AES f�r mindestens einen Kanal des Ger�tes aktiv
	bool aes;
	//! Aktuell vom Ger�t verwendeter AES-Schl�ssel als Index
	int cur_aes_key;
	//! Ist nur w�hrend des ersten Anlernens \c true
	bool is_new_device;
	//! Zeitstempel zum Unterdr�cken mehrfacher Burst-Aussendungen. Wird mit jeder erfolgreichen Burstsendung auf t+BURST_ACTIVE_INTERVAL gesetzt.
	uint64_t burst_active_until;
	//! Firmwareversion des Ger�tes
	std::string firmware_version;
	//! Beim Anlernen des Ger�tes empfangener Sysinfo-Rahmen
	BidcosFrame sysinfo_frame;
	//! Queue f�r nach dem Aufwachen des Ger�tes zu sendende Nachrichten
	std::queue<BidcosFrame> after_wakeup_queue;
    //! Seriennummer des zu geordneten Transceivers (BidcosInterface)
    std::string bidcos_interface_id;
    //! Gibt an, ob sich zur Laufzeit abh�ngig von der Empfangsfeldst�rke der zugeordnete Transceiver �ndern soll
    bool roaming;
	//! Empfangsfeldst�rke, die im Ger�te gemessen wurde 
	int rssi_device;
	//! Empfangsfeldst�rke, die an der lokalen Antenne gemessen wurde
	int rssi_peer;
	int deviceInBootloader;
	int firmwareUpdatePeding;

	/** \brief Temporary RX_MODE setting.
	 * \details Currently used for one xmlrpc call (compare RFManager)
	 * This is part of dynamic rx_mode change feature for DTAG.
	 * Special value is 0, which means there is currently no temporary rx mode setting.
	 */
	int rx_mode_temporary;

	unsigned int scheduledRetriesCount;
	unsigned int scheduledRetriesExecuted;

	bool ProcessPendingUpdate();
	typedef std::vector<std::string> replaceHistory_t;
	replaceHistory_t replace_history;
	void initBasicDeviceParameter(BidcosFrame &sysinfoFrame);
	bool set100kDataRate(void);
	//bool SendFrame(BidcosFrame* frame, bool enforceWakeUP);

	inline bool TemporaryRxModeActive() {
		return (rx_mode_temporary != 0);
	}

	/**
	 * Schedule configuration send retry (for rx_always devices).
	 */
	void ScheduleConfig(bool set_enforced_parameters, int32_t delay);

public:
	void setLastBurstTime(uint64_t timeMillis);

	/** \brief Sets temporary RX_MODE.
	 * \param tempRxMode RX_MODE value as defined in enum. Special value 0 means temporary rx mode is off.
	 */
	void SetTemporaryRxMode(const int tempRxMode);

	//! Returns if the device is always ready to receive.
	bool RxAlways();//currently just calls RFDeviceDescription::RxAlways()

	//! Returns if the device can be reached by Wake-On-Radio
	bool RxNeedsBurst();

	//! Returns if the device is reachable after receiving its sysinfo frame.
	bool RxAfterConfig();//currently just calls RFDeviceDescription::RxAfterConfig()

	//! Returns if the device is reachable after sending a wakeup frame.
	bool RxNeedsWakeup();

	//! Returns if the device supports lazy config.
	bool RxSupportLazyConfig();//currently just calls RFDeviceDescription::RxSupportLazyConfig()

	//! Returns wether this device instance is of type RFTeam or not.
	virtual bool IsTeamDeviceInstance();

	/** \brief Retries to commit config to device.
	 * \details 2 restries allowed. First after 5 seconds, second after 10 seconds.
	 * \return True if retry was scheduled, false if not.
	 */
	bool ScheduleConfigRetry();

	/** \brief Checks if there is enough space in after_wakeup_queue before overwriting entries.
	 * \param desiredSpace Number of entries desired as free. Default 1
	 * \return True if desiredSpace entries are free in queue, otherwise false.
	 */
	bool IsEnoughSpaceLeftInWakeupFrameQueue(const unsigned int desiredSpace = 1);
};

#endif // !defined(AFX_RFDEVICE_H__31222142_578A_4D7D_917B_5F9694706BFF__INCLUDED_)

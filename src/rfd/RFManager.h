/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RF_MANAGER_H_
#define _RF_MANAGER_H_

#include <TimerTarget.h>
#include "RFDevice.h"
#include "RFSystemDescription.h"
#include "RFParamset.h"
#include "BidcosFrame.h"
#include "RFController.h"
#include "RFFirmwareManager.h"
#include <HSSManager.h>
#include "BidcosInterfaceConcentrator.h"
#include "RFReplaceMap.h"

#include <utils.h>

#include <map>
#include <string>

enum InstallModes
{
	INSTALL_OFF = 0,
	INSTALL_NORMAL,
	INSTALL_PUSH_DEFAULT_CONFIG,
	INSTALL_DEVICE_WHITELIST //Only the device with matching serialCurrently it's not a list, only one device with given serial can be installed.
};

//! Zentrale Verwaltungsklasse
/*!
 *  An dieser Klasse h�ngen die Ger�teobjekte. Sie �bernimmt folgende Aufgaben:
 *  - Laden der persistierten Ger�te beim Starten
 *  - Anlernen neuer Ger�te (Anlernmodus)
 *  - Verteilung der XmlRpc-Aufrufe auf die Ger�te und Kan�le
 *  - Verteilung von der Funkseite her eingehender Nachrichten an die Ger�te
 *  - Verteilung von Ereignissen an die Logikprozesse (siehe HSSManager)
 */
class RFManager:public HSSManager, public TimerTarget
{

friend class RFChannel;

public:
	//! Flags f�r DeleteDevice()
	enum{
		DELETE_FLAG_RESET=(1<<0), //!< Ger�t in Werkszustand zur�cksetzen
		DELETE_FLAG_FORCE=(1<<1), //!< L�schen erzwingen, auch wenn Ger�t nicht erreichbar
		DELETE_FLAG_DEFER=(1<<2)  //!< Wenn Ger�t nicht erreichbar, bei n�chster Gelegenheit l�schen
	};
	//! Konstanten
    enum{
        INVALID_RSSI_VALUE=65536 //!< Ung�ltiger RSSI-Wert
    };
    //!Devicse Replace level
    typedef enum DeviceReplaceLevel_e
    {
    	ALL_POSSIBLE,
    	DEV_TYPE_MATCH,
    	REPLACE_MAP
    }DeviceReplaceLevel_t;
	//! Konstruktor
	RFManager(void);
	//! Destruktor
	virtual ~RFManager(void);
	//! Werte eines Parametersets lesen
	/*!
	 *  \param address Ger�te- oder Kanalseriennummer
	 *  \param key Schl�ssel des Parametersets, also \c MASTER, \c VALUES oder die Kanalseriennummer eines
	 *         Verkn�pfungspartners
	 *  \param set Zeigt auf ein XmlRpc-Struct, das die Werte des Parametersets aufnimmt.
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodGetParamset()
	 */
	bool GetParamsetValues(const std::string address, const std::string& key,int mode, XmlRpc::XmlRpcValue* set);
	//! Werte eines Parametersets setzen
	/*!
	 *  \param address Ger�te- oder Kanalseriennummer
	 *  \param key Schl�ssel des Parametersets, also \c MASTER, \c VALUES oder die Kanalseriennummer eines
	 *         Verkn�pfungspartners
	 *  \param set XmlRpc-Struct, das die Werte des Parametersets enth�lt. In diesem Array nicht enthaltene
	 *         Werte werden auch nicht gesetzt.
	 *  \param rxmode Desired rx mode (dtag dynamic rx mode feature)
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodPutParamset()
	 */
	bool PutParamsetValues(const std::string address, const std::string& key, XmlRpc::XmlRpcValue& set, const std::string& rxmode);
	//! Wertebeschreibungen eines Parametersets lesen
	/*!
	 *  \param address Ger�te- oder Kanalseriennummer
	 *  \param key Schl�ssel des Parametersets, also \c MASTER, \c VALUES, \c LINK oder die Kanalseriennummer 
	 *         eines Verkn�pfungspartners
	 *  \param set Zeigt auf ein XmlRpc-Struct, das die Wertebeschreibungen des Parametersets aufnimmt.
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodGetParamsetDescription()
	 */
	bool GetParamsetDescription(const std::string address, const std::string& key, XmlRpc::XmlRpcValue* set);
	//! Id eines Parametersets abfragen
	/*!
	 *  \param address Ger�te- oder Kanalseriennummer
	 *  \param type Schl�ssel des Parametersets, also \c MASTER, \c VALUES oder \c LINK
	 *  \param id Zeigt auf die Variable, die die Id des Parametersets aufnimmt.
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodGetParamsetId()
	 */
	bool GetParamsetId(const std::string address, const std::string& type, std::string* id);
	//! Automatische Ermittlung eines Parameterwertes
	/*!
	 *  \param address Ger�te- oder Kanalseriennummer
	 *  \param key Schl�ssel des Parametersets, also \c MASTER, \c VALUES, \c LINK oder die Kanalseriennummer 
	 *         eines Verkn�pfungspartners
	 *  \param parameter Id des zu ermittelnden Wertes
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodDetermineParameter()
	 */
	bool DetermineParameter(const std::string address, const std::string& key, const std::string& parameter);
	//! Abfrage eines einzelnen Wertes aus dem Parameterset \c VALUES
	/*!
	 *  \param address Ger�te- oder Kanalseriennummer
	 *  \param name Id des abzufragenden Wertes
	 *  \param val Zeiger auf die Variable, die den gelesenen Wert aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodGetValue()
	 */
	bool GetValue(const std::string address, const std::string& name,int mode, XmlRpc::XmlRpcValue* val);
	//! Setzen eines einzelnen Wertes im Parameterset \c VALUES
	/*!
	 *  \param address Ger�te- oder Kanalseriennummer
	 *  \param name Id des zu setzenden Wertes
	 *  \param val Referenz auf die Variable, die den zu setzenden Wert enth�lt
	 *  \param rxmode Desired rx mode (dtag dynamic rx mode feature)
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodSetValue()
	 */
	bool SetValue(const std::string address, const std::string& name, XmlRpc::XmlRpcValue& val, const std::string& rxMode);
	//! Gibt alle angelernten Ger�te, Kan�le und Teams in Form eines XmlRpc-Arrays mit Ger�tebeschreibungen zur�ck
	/*!
	 *  \param devs Zeiger auf Variable, die das Array der Ger�te-, Kanal- und Teambeschreibungen
	 *         in der an der XmlRpc-Schnittstelle erwarteten Form aufnimmt.
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodListDevices
	 */
	bool ListDevices(XmlRpc::XmlRpcValue* devs);
	//! Gibt alle angelernten Teams in Form eines XmlRpc-Arrays mit Ger�tebeschreibungen zur�ck
	/*!
	 *  \param devs Zeiger auf Variable, die das Array der Teambeschreibungen
	 *         in der an der XmlRpc-Schnittstelle erwarteten Form aufnimmt.
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodListTeams
	 */
	bool ListTeams(XmlRpc::XmlRpcValue* devs);
	//! Gibt eine Ger�te-, Kanal- oder Teambeschreibung zur�ck
	/*!
	 *  \param address Ger�te- oder Kanalseriennummer
	 *  \param descr Zeiger auf Variable, die die Ger�te-, Kanal- oder Teambeschreibung
	 *         in der an der XmlRpc-Schnittstelle erwarteten Form aufnimmt.
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodGetDeviceDescription
	 */
	bool GetDeviceDescription(const std::string& address, XmlRpc::XmlRpcValue* descr);
	//! Anlernen eines Ger�tes anhand der Seriennummer
	/*!
	 *  \param serial_number Seriennummer des anzulernenden Ger�tes
	 *  \param descr Zeiger auf Variable, die die Ger�tebeschreibung des neuen Ger�tes in der von 
	 *         der XmlRpc-Schnittstelle erwarteten Form aufnimmt.
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodAddDevice
	 */
	bool AddDevice(InstallModes mode,const std::string& serial_number, XmlRpc::XmlRpcValue* descr);
	//! Gibt die angelernten Verkn�pfungspartner f�r einen Kanal als Vektor von Seriennummern zur�ck
	/*!
	 *  \param address Kanalseriennummer des abzufragenden Kanals
	 *  \param peers Zeiger auf Variable, die die Kanalseriennummern der Verkn�pfungspartner aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodGetLinkPeers
	 */
	bool GetLinkPeers(const std::string& address, std::vector<std::string>* peers);
	//! Gibt die f�r einen Kanal, ein Ger�t oder den Schnittstellenprozess existierenden Verkn�pfungen zur�ck
	/*!
	 *  \param address Seriennummer des abzufragenden Ger�tes oder Kanals oder "" f�r alle Verkn�pfungen
	 *  \param flags Flags, die n�her bestimmen, was zur�ckgegeben werden soll
	 *  \param result Zeiger auf ein XmlRpc-Struct, das das Ergebnis aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodGetLinks
	 */
	bool GetLinks(const std::string& address, int flags, XmlRpc::XmlRpcValue* result);
	//! F�gt eine neue direkte Verkn�pfung zwischen zwei Kan�len hinzu
	/*!
	 *  \param sender_address Kanalseriennummer des ersten Verkn�pfungspartners
	 *  \param receiver_address Kanalseriennummer des zweiten Verkn�pfungspartners
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodAddLink
	 */
	bool AddLink(const std::string& sender_address, const std::string& receiver_address);
	//! Setzt Namen und Beschreibung zu einer bestehenden Verkn�pfung
	/*!
	 *  \param sender_address Kanalseriennummer des einen Verkn�pfungspartners
	 *  \param receiver_address Kanalseriennummer des anderen Verkn�pfungspartners
	 *  \param name Zu setzender Verkn�pfungsname
	 *  \param description Zu setzende Beschreibung
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodSetLinkInfo
	 */
	bool SetLinkInfo(const std::string& sender_address, const std::string& receiver_address, const std::string& name, const std::string& description);
	//! Ermittelt Namen und Beschreibung zu einer bestehenden Verkn�pfung
	/*!
	 *  \param sender_address Kanalseriennummer des einen Verkn�pfungspartners
	 *  \param receiver_address Kanalseriennummer des anderen Verkn�pfungspartners
	 *  \param name Zeiger auf Variable, die den Verkn�pfungsnamen aufnimmt
	 *  \param description Zeiger auf Variable, die die Beschreibung aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodGetLinkInfo
	 */
	bool GetLinkInfo(const std::string& sender_address, const std::string& receiver_address, std::string* name, std::string* description);
	//! L�scht eine bestehende Verkn�pfung
	/*!
	 *  \param sender_address Kanalseriennummer des einen Verkn�pfungspartners
	 *  \param receiver_address Kanalseriennummer des anderen Verkn�pfungspartners
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodRemoveLink
	 */
	bool RemoveLink(const std::string& sender_address, const std::string& receiver_address);
	//! Ordnet einen Kanal einem Team zu (z.B. Rauchmeldergruppe)
	/*!
	 *  \param channel_address Kanalseriennummer des dem Team zuzuordnenden Kanals
	 *  \param team_address Kanalseriennummer des Teams, dem der Kanal zugeordnet werden soll
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodSetTeam
	 */
	bool SetTeam(const std::string& channel_address, const std::string& team_address);
	//! �berpr�ft ob es sich bei dem Ger�t um den master eines Teams handelt
	    /*! Es wird ermittelt ob duch diese Ger�t ein Teamger�t erzeugt wurde
	     *
	     *  \param dev Ger�t das �berpr�ft werden soll
	     *  \param team_address Kanalseriennummer des Teams, dem der Kanal zugeordnet werden soll
	     *  \return \c Das Teamger�t wenn von dev ein Teamger�t erzeugt wurde ansonsten NULL
	     */
	RFDevice * IsTeamMaster(RFDevice *dev);
	//! Verarbeitung und Verteilung eingehender Funknachrichten
	/*!
	 *  Im Anlernmodus wird bei einer eingehenden Sysinfo-Nachricht f�r ein noch nicht bekanntes Ger�t
	 *  dieses Ger�t neu angelernt.
	 *
	 *  Die Nachricht wird an RFDevice::ProcessIncomingFrame() des Absenderger�tes �bergeben.
	 *
	 *  Eine Gruppennachricht wird zus�tzlich an RFDevice::ProcessIncomingFrame() des virtuellen
	 *  Gruppenger�tes �bergeben.
	 *
	 *  Dem Display-Prozess wird eine UDP-Nachricht gesendet, damit dieser das Antennensymbol f�r zwei
	 *  Sekunden aktiviert.
	 *
	 *  Wurde vom Ger�t eine Authentifizierungsaufforderung empfangen, wird der darin �bertragene
	 *  Schl�sselindex �ber RFDevice::SetKeyIndex() dem Ger�teobjekt mitgeteilt.
	 *
	 *  \param msg Die vom ARM7 empfangene Nachricht
	 */
	virtual void ProcessIncomingFrame(BidcosFrame& msg);
	//! �bertr�gt an ein Ger�t den Befehl zur Aktivierung eines Verkn�pfungsparametersets
	/*!
	 *  Wird zum Testen von Verkn�pfungen verwendet.
	 *
	 *  \param address Kanalseriennummer des Kanals, der ein Parameterset aktivieren soll
	 *  \param peer Kanalseriennummer des Verkn�pfungspartners, dessen PArameterset aktiviert 
	 *         werden soll
	 *  \param longpress Bei \c false wird die H�lfte des Parametersets f�r den kurzen Tastendruck aktiviert,
	 *         bei \c true die H�lfte f�r den langen Tastendruck.
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodActivateLinkParamset
	 */
	bool ActivateLinkParamset(const std::string address, const std::string& peer, bool longpress);
	//! Initialisierungsmethode. Wird beim Starten des Prozesses aufgerufen
	/*!
	 *  - �bertr�gt die BidCoS-Adresse der CCU an den ARM7
	 *  - Liest den Index der aktuellen AES-Schl�ssels aus dem ARM7 aus
	 *  - L�dt die Ger�tebeschreibungen
	 *  - L�dt die angelernten Ger�te
	 *  - Erzeugt das CCU-Ger�t (RFCentralDevice) falls erforderlich
	 *  - L�dt die in der RAM-Disk gespeicherte Liste der Logikprozesse
	 */
	virtual bool Init(const char* config_filename);
	//! Gibt das einzige Objekt dieser Klasse zur�ck
	static RFManager* GetSingleton(){
		return singleton;
	}
	//! L�scht die in der CCU gespeicherten Konfigurationsdaten zu einem Ger�t
	/*!
	 *  Die Konfigurationsdaten werden vom Ger�t erneut abgefragt, sobald sie ben�tigt werden.
	 *
	 *  \param address Ger�teseriennummer
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodClearConfigCache
	 */
	bool ClearConfigCache(const std::string& address);
	//! �bertr�gt die in der CCU zu einem Ger�t gespeicherten Konfigurationsdaten erneut an das Ger�t
	/*!
	 *  \param address Ger�teseriennummer
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodRestoreConfigToDevice
	 */
	bool RestoreConfigToDevice(const std::string& address);
	//! Erzeugt eine f�r die XmlRpc-Schnittstelle verwendbare Adresse
	/*!
	 *  \param address Ger�teseriennummer
	 *  \param channel Kanalnummer oder \c -1 f�r eine Ger�teadresse
	 *  \return Kanal- oder Ger�teadresse
	 */
	static std::string BuildStringAddress(const std::string& address, int channel=-1);
	//! Erzeugt eine f�r die XmlRpc-Schnittstelle verwendbare Adresse aus einer BidCoS-Adresse
	/*!
	 *  \param address BidCoS-Ger�teadresse
	 *  \param channel Kanalnummer oder \c -1 f�r eine Ger�teadresse
	 *  \return Kanal- oder Ger�teadresse
	 */
	std::string BuildStringAddress(int address, int channel=-1);
	//! Extrahiert aus einer Adresse der XmlRpc-Schnittstelle Ger�teseriennummer und Kanalnummer
	/*!
	 *  \param address Adresse von der XmlRpc-Schnittstelle
	 *  \param dev_address Zeiger auf Variable f�r die Ger�teseriennummer
	 *  \param channel Zeiger auf Variable f�r die Kanalnummer; Ist \c address eine Ger�teadresse,
	 *         dann wird \c *channel=-1.
	 */
	bool ParseAddress(const std::string& address, std::string * dev_address, int * channel);
	//! Extrahiert aus einer Adresse der XmlRpc-Schnittstelle BidCoS-Adresse und Kanalnummer
	/*!
	 *  \param address Adresse von der XmlRpc-Schnittstelle
	 *  \param dev_address Zeiger auf Variable f�r die BidCoS-Funkadresse
	 *  \param channel Zeiger auf Variable f�r die Kanalnummer; Ist \c address eine Ger�teadresse,
	 *         dann wird \c *channel=-1.
	 */
	bool ParseAddress(const std::string& address, int * dev_address, int * channel);
	//! Informiert die Logikprozesse �ber ein neues Ger�t
	void ReportNewDevice(RFDevice* dev);
	//! Informiert die Logikprozesse �ber ein gel�schtes Ger�t
	void ReportDeletedDevice(RFDevice* dev);
	//! Informiert die Logikprozesse �ber ein Ger�tetausch
	void ReportReplaceDevcie(RFDevice *newDev);
	//! Aktiviert oder deaktiviert den Anlernmodus
	/*!
	 *  \param seconds Zeit in Sekunden, die der Anlernmodus aktiv sein soll. \c 0 f�r Anlernmodus
	 *         deaktivieren.
	 */

	void ReportReAddedDevice(RFDevice* dev);
	
	/**\brief Methode zum (de-)aktivieren des InstallMode im ModusINSTALL_DEVICE_WHITELIST*/
	void SetInstallMode(InstallModes mode, int seconds, const std::string& devSerial);
	void SetInstallMode(InstallModes mode,int seconds);
	void SetInstallMode(int seconds);
	//! Ermittelt die Zeit in Sekunden, die der Anlernmodus noch aktiv ist
	/*!
	 *  \return Zeit in Sekunden, die der Anlernmodus noch aktiv ist. \c 0 f�r Anlernmodus
	 *          nicht aktiv.
	 */
	int GetInstallMode();
	int GetInstallMode(InstallModes *mode);
	//! Liefert ein RFDevice anhand seiner BidCoS-Adresse
	/*
	 *  Falls kein Ger�t mit der angegebenen Adresse gefunden wurde, wird NULL zur�ckgegeben.
	 *  \param address BidCoS-Adresse des gesuchten Ger�ts.
	 *  \return RFDevice oder NULL
	 */
	RFDevice* GetRFDevice(int address);
	//! Gibt zu einer Seriennummer das Ger�te- oder Kanalobjekt zur�ck
	RFLogicalInstance* GetInstance(const std::string& address);
	//! Gibt zu einer BidCoS-Adresse und einer Kanalnummer das Ger�te- oder Kanalobjekt zur�ck
	RFLogicalInstance* GetInstance(int address, int channel);
	//! Gibt zu einer BidCoS-Adresse und einer Kanalnummer das Teamobjekt zur�ck
	RFLogicalInstance* GetTeamInstance(int address, int channel);
	//! Gibt das Objekt zur�ck, dass die Ger�tebeschreibungen verwaltet
	RFSystemDescription* GetSystemDescription(){return &system_description;}
	//! L�schen eines Ger�tes (Ablernen von der CCU)
	/*!
	 *  \param address Seriennummer des zu l�schenden Ger�tes
	 *  \param flags Flags, die angeben, wie mit Fehlern umgegangen werden soll
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodDeleteDevice
	 */
	bool DeleteDevice(const std::string& address, int flags);
	//! L�schen eines Ger�tes (Ablernen von der CCU)
	/*!
	 *  Wird verwendet, wenn ein Ger�t "bei n�chster Gelegenheit" gel�scht werden soll und die
	 *  Gelegenheit gekommen ist.
	 *  \param dev Das zu l�schende Ger�t
	 *  \param flags Flags, die angeben, wie mit Fehlern umgegangen werden soll
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodDeleteDevice
	 */
	bool DeleteDevice(RFDevice* dev, int flags);
	//! Bricht das L�schen eines Ger�tes (Ablernen von der CCU) ab
	/*!
	 *  \param address Seriennummer des zu l�schenden Ger�tes
	 *
	 *  \see XmlRpcMethodAbortDeleteDevice
	 */
	bool AbortDeleteDevice(const std::string& address);	
	//! Gibt den Index das aktuell verwendeten AES-Schl�ssels zur�ck
	int GetCurAESKey(){return aes_key_index_current;}
	//! Gibt den Index das tempor�ren AES-Schl�ssels zur�ck
	int GetTempAESKey(){return aes_key_index_temp;}
	//! �ndert den aktuellen AES-Schl�ssel
	/*!
	 *  �ndert den aktuellen AES-Schl�ssel im ARM7. Schl�gt fehl, wenn der alte aktuelle Schl�ssel noch
	 *  nicht an alle Ger�te �bertragen wurde.
	 *
	 *  Versucht, den neuen aktuellen Schl�ssel auch an die angelernten Ger�te zu �bertragen. Bei Ger�ten
	 *  bei denen dies nicht klappt, wird das CONFIG_PENDING-Flag gesetzt.
	 *
	 *  \param passphrase Passwort dessen MD5-Hash als neuer Schl�ssel verwendet wird
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodChangeKey
	 */
	bool ChangeAESKey(const std::string& passphrase);
	//! Setzt den tempor�ren AES-Schl�ssel
	/*!
	 *  Der tempor�re AES-Schl�ssel wird zum Anlernen von Ger�ten verwendet, wenn der vom Ger�t verwendete
	 *  Schl�ssel der CCU nicht bekannt ist.
	 *
	 *  \param passphrase Passwort dessen MD5-Hash als neuer Schl�ssel verwendet wird
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodSetTempKey
	 */
	bool SetTempAESKey(const std::string& passphrase);
	//! Wird vom Logikprozess aufgerufen, um die Verwendung eines Wertes mitzuteilen
	/*!
	 *  \param address Kanalseriennummer des Kanals, zu dem der Wert geh�rt
	 *  \param value Id des verwendeten Wertes aus dem Parameterset \c VALUES
	 *  \param count Gibt an, wie oft der Wert verwendet wird
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 *
	 *  \see XmlRpcMethodReportValueUsage
	 */
	bool ReportValueUsage(const std::string& address, const std::string& value, int count);
	//! Gibt die Seriennummer eines Ger�tes zur�ck, das aufgrund eines unbekannten AES-Schl�ssels nicht angelernt werden konnte
	/*!
	 *  \param reset Bei \c true wird die Seriennummer auf \c "" zur�ckgesetzt
	 *  \return Seriennummer des Ger�tes, das nicht angelernt werden konnte oder \c "" falls kein solches existiert
	 *
	 *  \see XmlRpcMethodGetKeyMismatchDevice
	 */
	std::string GetKeyMismatchDevice(bool reset);
	//! Seriennummer eines Ger�tes setzen, das aufgrund eines unbekannten AES-Schl�ssels nicht angelernt werden konnte
	/*!
	 *  Wird von RFDevice aufgerufen, wenn das Anlernen aufgrund eines unbekannten AES-Schl�ssels fehlschl�gt.
	 */
	void SetKeyMismatchDevice(const std::string& serial){key_mismatch_device=serial;}
	//! Liefert die gespeicherten Empfangsfeldst�rkedaten von allen Ger�ten
	/*!
	 *  \param info Variable, die ein zweidimensionales assoziatives Array aufnimmt, dessen Schl�ssel die 
	 *         Ger�teseriennummern sind. Die Felder des assoziativen Arrays sind Tupel, die die
	 *         Empfangsfeldst�rken zwischen beiden  Schl�sselger�ten f�r beide Richtungen in dbm angeben. 
	 *         Ein Wert von 65536 bedeutet, dass keine Informationen vorliegen.
	 *
	 *  \see XmlRpcMethodRSSIInfo
	 */
	void GetRSSIInfo(XmlRpc::XmlRpcValue* info);
	//! Teilt den Logikprozessen �nderungen an einem Ger�t oder Kanal mit, z.B. Anzahl der Verkn�pfungen
	/*!
	 *  \param address Ger�te- oder Kanalseriennummer
	 *  \param hint Hinweis auf die �nderung (siehe Konstanten UPDATE_HINT_*)
	 */
	void ReportUpdate(const std::string& address, int hint);
	//! Stellt sicher, dass ein Team-Ger�t zu einer gegebenen BidCoS-Adresse existiert
	/*!
	 *  \param descr Beschreibung f�r das Team-Ger�t. Kann von einer Ger�tebeschreibung, die
	 *         Teambildung unterst�tzt mit RFDeviceDescription::GetTeamDescription() ermittelt werden.
	 *  \param address BidCoS-Funkadresse des Teams
	 *  \param master_candidate Ger�teobjekt, das als Team-Master in Frage kommt. Muss �bergeben werden,
	 *         wenn w�hrend des Anlernen eines Ger�tes ein Team erzeugt wird, weil dann das Ger�teobjekt
	 *         noch nicht in die entsprechenden Containerdatenstrukturen von RFManager eingetragen ist.
	 *  \return Zeiger auf das neu erstellte oder bereits vorhandene Team-Ger�t, \c NULL im Fehlerfall.
	 */
	RFDevice* CreateTeamInstance(RFDeviceDescription* descr, int address, RFDevice* master_candidate);
	//! Liefert die BidCoS-Adresse der Zentrale
	unsigned int GetBidcosAddress();
	//! Liefert den Interface-Concentrator
	BidcosInterfaceConcentrator* GetInterfaceConcentrator();
    //! Listet die vorhandenen Interfaces des Interface-Concentrators auf
    /*!
     *  \see XmlRpcMethodListBidcosInterfaces
     */
    bool ListBidcosInterfaces(XmlRpc::XmlRpcValue* result);
    //! Setzt das Bidcos-Interface f�r ein Ger�t
    /*!
     *  \see XmlRpcMethodSetBidcosInterface
     */
    bool SetBidcosInterface(const std::string& device_address, const std::string& interface_id, bool roaming);
    //! Liefert das Verzeichnis der Ger�tedateien zur�ck
    std::string GetDeviceFilesDir();
    //! Aktualisiert einen Wert in den gespeicherten RSSI-Informationen
    void UpdateRssiInfo( int sender_address, const std::string& receiver_serial, int rssi);
    //! Aktualisiert einen Wert in den gespeicherten RSSI-Informationen
    void UpdateRssiInfo( const std::string& sender_serial, const std::string& receiver_serial, int rssi);
	
	/*! Setzt die UTC Zeit f�r den CCU2 Coprozessor und ap�ter vll. noch ein paar weitere Interfaces.
	* \param utcSeconds Anzahl Sekunden seit 01.01.1970 00:00 Uhr (UTC)
	* \param offsetMinutes Offset in Minuten entsprechend der Zeitzone.*/
	bool SetInterfaceClock(const unsigned int utcSeconds, const int offsetMinutes);
	//! Liefert einen Pointer auf den Firmwaremanager
	RFFirmwareManager *GetFirmwareManager();
	/*! Startet das Firmwareupdaten f�r ein Ger�t
	* \param address  Adresse des Ger�tes das upgedatet werden soll*/
	bool UpdateFirmware(const std::string& address);
	bool ReplaceDevice(const std::string& old_address, const std::string& new_address);
    virtual bool IsDeviceReplaced(const std::string &oldDevieceAddress, std::string &newDeviceAddress);

    bool AddVirtualDeviceInstance(std::vector<unsigned char> rawSysinfo);
    bool ListReplaceableDevices(std::string addressNewDeviceToReplace, XmlRpc::XmlRpcValue* outDevs,DeviceReplaceLevel_t replaceLevel=RFManager::REPLACE_MAP);

    /** \brief Sets info LED of HomeMatic RF-Lan Gateway (rfd internal type: HMLGW2)
     * param state 0: off; 1: on; 2: blink slow (1 second); 3: blink fast (500ms)
     */
    bool SetRFLGWInfoLED(const unsigned int state);
    /** \brief Refreshes list of user deployed device firmware files.*/
    void RefreshDeployedDeviceFirmwareList();

    /** \brief Returns value of fireNAKerrorEvent.*/
    bool FireNACKErrorEventEnabled();

    /** \brief Returns value of callUpdateDeviceOnOTAUDeviceRebuild.*/
    bool CallUpdateDeviceOnOTAUDeviceRebuild();

private:
	//! Typedef zum Cachen des zuletzt an der XmlRpc-Schnittstelle verwendeten Ger�tes
	typedef struct{
		std::string address; //!< Ger�teseriennummer
		RFDevice* dev; //!< Zeiger auf das zugeh�rige Ger�teobjekt
	}dev_cache_t;
	//! Timer-ID zum automatischen Deaktivieren des Anlernmodus
	enum{TIMER_INSTALL_MODE};
	//! Maximale Dauer in Sekunden des Anlernmodus
	enum{INSTALL_MODE_MAX_TIME=600};
	//! Abfragen der aktuell verwendeten AES-Schl�ssel in Form von Indizes von ARM7
	/*!
	 *  Speichert die Schl�sselindizes in \c aes_keys.
	 *  \param response Hier kann die Antwortnachricht auf einen Befehl zum Setzen
	 *         eines AES-Schl�ssels �bergeben werden. Dann werden die aktuellen Schl�ssel
	 *         aus dieser Nachricht extrahiert. Wird hier \c NULL �bergeben, dann werden
	 *         die aktuellen Schl�ssel aktiv vom ARM7 abgefragt.
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
    //! Berechnet einen MD5-Hash aus einen String.
    std::string CalculateMD5(const std::string& s);
	//! Dispatchmethode f�r Timer
	void OnTimer(uint32_t cookie);
	//! Laden der Ger�teliste
	/*!
	 *  L�dt die Dateien mit der Endung \c .xml aus dem Verzeichnis \c DEVICE_FILES_PATH als angelernte
	 *  Ger�te.
	 */
	bool LoadDeviceList();
	//! Liest die aktuellen AES-Schl�ssel-Indizes von RFController
    bool ReadAESKeys();
	//! Speichert die aktuellen AES-Schl�ssel und -Indizes in die Datei rfd/keys
    bool WriteAESKeys();
    //! Speichert die eigene BidCoS-Adresse in die durch \c ADDRESS_FILE spezifizierte Datei
    bool PersistBidcosAddress();
	//! Objekt f�r die Verwaltung der Ger�tebeschreibungen
	RFSystemDescription system_description;
	//! Typedef f�r Map Seriennummer -> RFDevice zur Verwaltung der Ger�teobjekte
	typedef std::map<std::string, RFDevice*> t_dev_instances;
	//! Map Seriennummer -> RFDevice zur Verwaltung der Ger�teobjekte
	t_dev_instances dev_instances;
	//! Map Seriennummer -> RFDevice zur Verwaltung der Teamobjekte
	t_dev_instances team_instances;
	//! Typedef f�r Map BidCoS-Adresse -> RFDevice zur Verwaltung der Ger�teobjekte
	typedef std::map<int, RFDevice*> t_address_map;
	//! Map BidCoS-Adresse -> RFDevice zur Verwaltung der Ger�teobjekte
	t_address_map dev_address_map;
	//! Map BidCoS-Adresse -> RFDevice zur Verwaltung der Teamobjekte
	t_address_map team_address_map;
	//! Die einzige Instanz dieser Klasse
	static RFManager* singleton;
	//! Ist \c true solange der Anlernmodus aktiv ist
	InstallModes install_mode;
	//! Zeitstempel bis wann der Anlernmodus aktiv ist
	uint64_t install_mode_expires;
	//! Cache f�r das zuletzt an der XmlRpc-Schnittstelle verwendete Ger�t
	dev_cache_t dev_cache;
	//! Index des tempor�ren AES-Schl�ssels
	int aes_key_index_temp;
	//! Index des aktuellen AES-Schl�ssels
	int aes_key_index_current;
	//! Index des vorherigen AES-Schl�ssels
	int aes_key_index_previous;
	//! Konfigurationsflag. Gibt an, ob die AES-Schl�ssel in einer Datei gespeichert werden
    bool persist_aes_keys;
    //! Konfigurationsflag. Gibt an, ob die virtuelle Fernbedienung in der Ger�teliste auftaucht.
    bool has_virtual_remote;
    //! Seriennummer des letzten Ger�tes, das aufgrund eines unbekannten AES-Schl�ssels nicht angelernt werden konnte
	std::string key_mismatch_device;
	//! Verwaltet die Bidcos-Interfaces
	BidcosInterfaceConcentrator interface_concentrator;
	//! Bidcos-Adresse der Zentrale
	unsigned int bidcos_address;
    //! typedef f�r Historie der AES-Schl�ssel
    typedef std::map<int, std::string> t_map_aes_keys;
    //! Historie der AES-Schl�ssel
    t_map_aes_keys map_aes_keys;
    //! typedef f�r RSSI-Werte aller angelernten Ger�te
    /*! Der Schl�ssel besteht aud der Seriennummer des Senders gefolgt von der Seriennummer des Empf�ngers.
     *  Trennzeichen ist ein Slash (/)
     */
    typedef std::map<std::string, int> t_map_rssi;
    //! RSSI-Werte aller angelernten Ger�te
    t_map_rssi map_rssi;
	//! Ist gesetzt, w�hrend der Destruktor ausgef�hrt wird, damit dann bestimmte gef�hrliche Operationen unterbunden werden k�nnen.
	bool destructing;
	//! Verwaltung der verf�gbaren Firmwareupdates
	RFFirmwareManager firmwareManager;

	typedef std::map<std::string, RFDevice*> t_map_replace_history;
	t_map_replace_history replace_history;

	/**\brief Used in install mode INSTALL_DEVICE_WHITELIST
	* \details Currently it's just one serial/device (not a list) that can be installed.
	* whenever the INSTALL_DEVICE_WHITELIST is active.
	*/
	std::string installWhiteListDeviceSerial;

	/** \brief If true, an event like event(0123456789:1, ERROR, {1, 'Unknown Error'}) is fired if a device response is a NAK.*/
	bool fireNACKErrorEvents;

	/** \brief If true, Device->Rebuild() calls XmlRpc method updateDevice instead of newDevices. Default is false. */
	bool callUpdateDeviceOnOTAUDeviceRebuild;

	void initReplaceHistory();
	bool CheckReplaceCompatibility(RFDevice *instOld, RFDevice *instNew,DeviceReplaceLevel_t replaceLevel);


	RFReplaceMap replaceMap;
};
#endif //_RF_MANAGER_H_

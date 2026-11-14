/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RF_LOGICAL_INSTANCE_H_
#define _RF_LOGICAL_INSTANCE_H_

#include <string>
#include <XmlRpc.h>
#include "RFConfigData.h"
#include <LogicalInstance.h>

class RFDevice;
class BidcosFrame;

//! Abstrakte Basisklasse, die Gemeinsamkeiten von RFDevice und RFChannel zusammenfasst
/*!
 *  Insbesondere werden von dieser Klasse die Konfigurationsdatenobjekte RFConfigData
 *  zu einem Ger�t oder Kanal verwaltet.
 */
class RFLogicalInstance: public LogicalInstance
{
public:
	//! Kontstruktor
	RFLogicalInstance(void);
	//! Destruktor
	virtual ~RFLogicalInstance(void);
	//! Senden einer Nachricht an das Ger�t
	virtual bool SendFrame(BidcosFrame* frame)=0;
	//! Das Ger�teobjekt ermiteln
	virtual RFDevice* GetDevice()=0;
	//! Verkn�pfungspartner f�r nachfolgende Operationen auf Konfigurationsdaten setzen
	/*!
	 *  \param peer_address BidCoS-Adresse des Verkn�pfungspartners oder \c 0 wenn auf Ger�te-
	 *         oder Kanalparametersets zugegriffen werden soll
	 *  \param peer_channel Kanalnummer des Verkn�pfungspartners oder \c 0 wenn auf Ger�te-
	 *         oder Kanalparametersets zugegriffen werden soll
	 */
	inline void SetCurParamsetPeer(uint32_t peer_address, uint32_t peer_channel){cur_paramset_peer=(peer_address<<8)|peer_channel;};
	//! Verkn�pfungspartner f�r Operationen auf Konfigurationsdaten abfragen
	/*!
	 *  \return BidCoS-Adresse und Kanalnummer des Partners in einen 32bit-Wert gepackt. Die 3 h�chstwertigen
	 *          Bytes geben die Adresse an und das niederwertigste Byte die Kanalnummer. Beim Zugriff auf
	 *          Ger�te- oder Kanalparametersets kommt \c 0 zur�ck.
	 */
	inline uint32_t GetCurParamsetPeer(){return cur_paramset_peer;};
	//! Verkn�pfungspartner f�r nachfolgende Operationen auf Konfigurationsdaten setzen
	/*!
	 *  \param peer BidCoS-Adresse und Kanalnummer des Partners in einen 32bit-Wert gepackt. Die 3 h�chstwertigen
	 *          Bytes geben die Adresse an und das niederwertigste Byte die Kanalnummer. Beim Zugriff auf
	 *          Ger�te- oder Kanalparametersets wird \c 0 �bergeben.
	 */
	inline void SetCurParamsetPeer(uint32_t peer){cur_paramset_peer=peer;};
	//! Verkn�pfungspartner f�r nachfolgende Operationen auf Konfigurationsdaten setzen
	/*!
	 *  \param peer Seriennummer des Verkn�pfungspartners oder \c "" wenn auf Ger�te-
	 *         oder Kanalparametersets zugegriffen werden soll
	 */
	void SetCurParamsetPeer(const std::string& peer);
	//! Gibt das Konfigurationsdatenobjekt zur�ck, das zuvor mit SetCurParamsetPeer() ausgew�hlt wurde
	/*!
	 *  \param list Index der Parameterliste, auf die zugegriffen werden soll. Die Methode stellt sicher, dass
	 *              diese Parameterliste bereits vom Ger�t gelesen wurde.
	 *  \return Zeiger auf das gew�nschte Konfigurationsdatenobjekt oder \c NULL im Fehlerfall
	 */
	RFConfigData* GetCurConfigData(int list);
	//! Gibt das Konfigurationsdatenobjekt f�r einen bestimmten Verkn�pfungspartner zur�ck
	/*!
	 *  \param peer_address BidCoS-Adresse des Verkn�pfungspartners oder \c 0 wenn auf Ger�te-
	 *         oder Kanalparametersets zugegriffen werden soll
	 *  \param peer_channel Kanalnummer des Verkn�pfungspartners oder \c 0 wenn auf Ger�te-
	 *         oder Kanalparametersets zugegriffen werden soll
	 *  \param list Index der Parameterliste, auf die zugegriffen werden soll. Die Methode stellt sicher, dass
	 *         diese Parameterliste bereits vom Ger�t gelesen wurde.
	 *  \return Zeiger auf das gew�nschte Konfigurationsdatenobjekt oder \c NULL im Fehlerfall
	 */
	RFConfigData* GetConfigData(int peer_address, int peer_channel, int list);
	//! Speichert in \c *descr die Kanal- oder Ger�tebeschreibung wie an der XmlRpc-Schnittstelle erwartet
	virtual bool Describe(XmlRpc::XmlRpcValue* descr)=0;
	//! Zeigt die Verwendung eines Wertes in der Logikschicht an
	/*! \see XmlRpcMethodReportValueUsage
	 */
	virtual bool ReportValueUsage(const std::string& value, int count){return true;};
	//! Verarbeitet eine vom Ger�t empfangene asynchrone Parameter-�nderungs-Mitteilungsnachricht
	/*!
	 *  Ruft am entsprechenden Konfigurationsdatenobjekt RFConfigData::ProcessAsyncParamInfo() auf.
	 */
	bool ProcessAsyncParamInfo(BidcosFrame& frame);
	//! Markiert alle Konfigurationsdatenobjekte als noch an das Ger�t zu senden
	virtual void SetConfigDevDirty();
	//! Abfrage eines einzelnen Wertes aus dem Parameterset \c "VALUES"
	/*! Entspricht dem XmlRpc-Aufruf \c GetValue()
	 *  Muss von abgeleiteten Klassen implementiert werden.
	 *  \param name Id des abzufragenden Wertes
	 *  \param val Zeiger auf die Variable, die den gelesenen Wert aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool GetValue(const std::string& name, XmlRpc::XmlRpcValue* val) = 0;
	//! Abfrage eines einzelnen Wertes aus dem Parameterset \c "VALUES"
	/*! Entspricht GetValue() wirft allerdings bei nicht lesbaren Parameter eine Exception
	 *  Muss von abgeleiteten Klassen implementiert werden.
	 *  \param name Id des abzufragenden Wertes
	 *  \param val Zeiger auf die Variable, die den gelesenen Wert aufnimmt
	 *  \param mode gibt den Lesemodus an 0 = nur der Wert wird gelesen; 1= wert und undifined wird ausgewertet
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool ReadValue(const std::string& name,int mode, XmlRpc::XmlRpcValue* val) = 0;
	virtual bool GetParamsetValues(const std::string& key, XmlRpc::XmlRpcValue* set)=0;
	virtual bool GetParamsetValues(const std::string& key,int mode, XmlRpc::XmlRpcValue* set)=0;
protected:
	//! Wird aufgerufen unmittelbar nachdem Konfigurationsdaten vom Ger�t gelesen wurden
	/*! Bietet f�r abgeleitete Klassen einen Einsprungpunkt f�r das zwangsweise Setzen von 
	 *  Konfigurationsparametern.
	 */
	virtual void OnConfigReadFromDevice(RFConfigData* cd, int list){};
	//! L�scht die zu einem Partner gespeicherten Konfigurationsdaten
	/*!
	 *  Wird beim L�schen einer Verkn�pfung aufgerufen.
	 */
	bool RemoveConfigData(int peer_address, int peer_channel);
	//! Verkn�pfungspartner f�r Operationen auf Konfigurationsdaten
	uint32_t cur_paramset_peer;
	//! Typedef f�r Map mit Konfigurationsdatenobjekten
	typedef std::map<uint32_t, RFConfigData> config_data_t;
	//! Map mit Konfigurationsdatenobjekten; Schl�ssel ist die Partneradresse in gepackter Form
	config_data_t config_data;
public:
	//! Gespeicherten Wert setzen f�r einen bestimmten Verkn�pfungspartner
	/*!
	 *  Verwendet ValueStore::SetStoredValue(). Die ID, unter der gespeichert wird, ergibt sich aus
	 *  \c id und \c peer: &lt;id&gt;[&lt;peer&gt;]. Beispiel:
	 *
	 *  \c UI_HINT[0x12345601]
	 *
	 *  \param id Id, unter der gespeichert werden soll
	 *  \param peer Gepackte Adresse und Kanalnummer des Partners
	 *  \param param Zu speichernder Wert
	 *  \param flags Flags f�r ValueStore::SetStoredValue()
	 */
	using ValueStore::SetStoredValue;
	bool SetStoredValue(const std::string& id, uint32_t peer, XmlRpc::XmlRpcValue& param, int flags=0);
	//! Gespeicherten Wert abfragen f�r einen bestimmten Verkn�pfungspartner
	/*!
	 *  Verwendet ValueStore::GetStoredValue(). Die ID, f�r die abgefragt wird, ergibt sich aus
	 *  \c id und \c peer: &lt;id&gt;[&lt;peer&gt;]. Beispiel:
	 *
	 *  \c UI_HINT[0x12345601]
	 *
	 *  \param id Id des Wertes, der abgefragt werden soll
	 *  \param peer Gepackte Adresse und Kanalnummer des Partners
	 *  \param param Zeiger auf Variable, die den abgefragten Wert aufnimmt
	 */
	using ValueStore::GetStoredValue;
	bool GetStoredValue(const std::string& id, uint32_t peer, XmlRpc::XmlRpcValue* param);
	//! L�scht alle f�r einen bestimmten Verkn�pfungspartner gespeicherten Werte
	/*!
	 *  Wird beim L�schen einer Verkn�pfung aufgerufen.
	 *  \param peer Gepackte Adresse und Kanalnummer des Partners
	 */
	bool DeleteStoredValues(uint32_t peer);
	virtual bool replaceRFConfigData(RFLogicalInstance *oldInstance);
};
#endif //_RF_LOGICAL_INSTANCE_H_

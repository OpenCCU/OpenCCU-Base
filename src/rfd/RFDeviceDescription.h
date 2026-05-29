/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// RFDeviceDescription.h: Schnittstelle f�r die Klasse RFDeviceDescription.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RFDEVICEDESCRIPTION_H__41F2BBEB_C64D_4FB5_A126_713C3C9B7AAF__INCLUDED_)
#define AFX_RFDEVICEDESCRIPTION_H__41F2BBEB_C64D_4FB5_A126_713C3C9B7AAF__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "RFChannelDescription.h"
#include "RFParamset.h"
#include <FrameDescription.h>
#include <HSSDescription.h>

#include "xmlParser.h"

#include <vector>
#include <map>
#include <string>

#include <XmlRpc.h>

//! Diese Klasse verwaltet die Informationen aus den Ger�tebeschreibungsdateien (XML-Dateien)
/*!
 *  Jede Instanz dieser Klasse liest eine XML-Datei ein und speichert die darin enthaltenen Informationen.
 */
class RFDeviceDescription
{
public:
	//! Aufz�hlung f�r Empfangsmodus (wann und wie kann das Ger�t angesprochen werden)
	enum{
		RX_ALWAYS=(1<<0), //!< Empf�nger ist dauerhaft eingeschaltet
		RX_BURST=(1<<1),  //!< Empf�nger ist im Wake-On-Radio-Modus dauerhaft eingeschaltet
		RX_CONFIG=(1<<2), //!< Empf�nger kann nach dem Senden eines Sysinfo-Rahmens angesprochen werden
		RX_WAKEUP=(1<<3), //!< Empf�nger kann nach einer eigenen Sendung per Wakeup-Nachricht angesprochen werden
		RX_LAZY_CONFIG=(1<<4) //!< Empf�nger kann durch ein CallCCU telegram in der zweiten H�lfte der Repeaterl�cke erreicht werden
	};

	enum {
		RX_BURST_AND_WAKEUP=0x0A //!<  Empfaenger unterstuetzt WAKEUP und BURST zugleich. !!!(Dieser Wert wird nur zur Vereinfachung einer Abfrage verwendet)!!!
	};

	/* \brief Burst type.*/
	enum BurstMode {
		BURST_MODE_SINGLE,
		BURST_MODE_TRIPLE 
	};

	//! Flags f�r die XmlRpc-Schnittstelle
	enum{
		FLG_NONE=0, //!< Kein Flag gesetzt
		FLG_VISIBLE=(1<<0), //!< Ger�t ist sichtbar
		FLG_DONTDELETE=(1<<3) //!< Ger�t kann nicht gel�scht werden
	};

	//! Listet die definierten Parametersets in Form eines Arrays auf, wie f�r die XmlRpc-Schnittstelle ben�tigt
	bool ListParamsets(XmlRpc::XmlRpcValue* list);
	//! Ruft SetEnforcedParameters f�r alle Parametersets auf
	bool SetEnforcedParameters(RFLogicalInstance* inst);
	//! Ruft SetDefaultConfig des Parameterset "MASTER" auf  
	bool SetDefaultConfig(RFLogicalInstance *inst);
	//! Liefert das Parameterset mit dem Typen key zur�ck
	/*! Key ist der Typ des Parametersets aus der XML-Datei. Derzeit nur "MASTER".
	 */
	RFParamset* GetParamset(const std::string& key);
	//! Liefert die Anzahl der Kanalbeschreibungen zur�ck
	/*!
	 *  Dies ist nicht unbedingt die Anzahl der Kan�le am konkreten Ger�t. Eine Kanaldefinition kann
	 *  mehrere konkrete Kan�le definieren.
	 */
	unsigned int GetChannelCount();
	//! Gibt die Kanalbeschreibung mit Index \c i zur�ck, \c NULL falls diese nicht existiert
	RFChannelDescription* GetChannelDescription(int i);
	//! Gibt die Kanalbeschreibung mit Typ \c type zur�ck, \c NULL falls diese nicht existiert
	RFChannelDescription* GetChannelDescription(const std::string& type);
	//! Pr�ft, ob die Ger�tebeschreibung das Ger�t zum �bergebenen Sysinfo-Rahmen unterst�tzt
	/*!
	 *  Mit Hilfe dieser Methode wird zu einem anzulernenden Ger�t die passende Ger�tebeschreibung
	 *  ausgew�hlt.
	 *
	 *  \param sysinfo_frame Vom anzulernenden Ger�t empfangener Sysinfo-Rahmen
	 *  \param type_id In diese Variable wird im Erfolgsfall die Ger�tekurzbezeichnung gespeichert
	 *  \return Priorit�t der Ger�tebeschreibung in Bezug auf das anzulernende Ger�t. Falls
	 *          mehrere Ger�tebeschreibungen (=XML-Dateien) zu einem anzulernenden Ger�t passen,
	 *          wird diejenige ausgew�hlt, die hier die h�chste Priorit�t zur�ckgibt. Es kommt
	 *          \c -1 zur�ck, wenn die Ger�tebeschreibung nicht passt.
	 */
	int Matches(BidcosFrame& sysinfo_frame, std::string* type_id);
	//! Liest eine komplette XML-Datei ein
	virtual bool InitFromXml(XMLNode& node, XMLNode& root_node);
	//! Gibt zur�ck, ob das Ger�t dauerhaft empfangsbereit ist
	inline bool RxAlways(){return (rx_modes & RX_ALWAYS)!=0;};
	//! Gibt zur�ck, ob das Ger�t im Wake-On-Radio-Modus angesprochen werden kann
	bool RxNeedsBurst();
	//! Gibt zur�ck, ob das Ger�t nach Senden eines Sysinfo-Rahmens angesprochen werden kann
	inline bool RxAfterConfig(){return (rx_modes & (RX_ALWAYS | RX_CONFIG))!=0;};
	//! Gibt zur�ck, ob das Ger�t einen Wakeup-Rahmen braucht, um angesprochen werden zu k�nnen
	bool RxNeedsWakeup();
	//! Gibt zur�ck, ob das Ger�t LazyConfig unterst�tzt
	inline bool RxSupportLazyConfig(){return (rx_modes & RX_LAZY_CONFIG)!=0;};
	//! Gibt zur�ck, ob im vom Ger�t beim Anlernen an die CCU �bertragenen Sysinof-Rahmen eine Kanalnummer erwartet wird
	inline bool PeeringSysinfoExpectChannel(){return peering_sysinfo_expect_channel;};
	//! Gibt zur�ck, ob das Ger�t AES unterst�tzt
	inline bool SupportsAES(){return supports_aes;};
	//! Gibt zur�ck, ob bei der zeitabh�ngigen Erreichbarkeitspr�fung (zyklischer Sender) nur AES-Authentifizierte Pakete akzeptiert werden
	inline bool UnreachCheckAES(){return unreach_check_aes;};
	//! Gibt die Zeit in Sekunden zur�ck, nach der ein Ger�t als nicht erreichbar betrachtet wird, wenn keine Nachricht vom Ger�t empfangen wurde
	/*! 
	 *  Bei \c 0 findet keine zeitabh�ngige Erreichbarkeitspr�fung statt.
	 */
	inline uint32_t GetCyclicTimeout(){return cyclic_timeout;};
	//! Gibt die Versionsnummer der XML-Datei zur�ck
	inline int GetVersion(){return version;};

	//!Returns rx_modes value - Use in RFDevice only!!!!!
	int GetRxMode(){return rx_modes;}//be careful if bypassing RFDevice with this method.

	//!Returns rx_mode_default value. Compare rx_mode_default variable documentation for more information.
	int GetRxModeDefault() { return rx_mode_default; }

	//! Jede Instanz beschreibt einen unterst�tzten Ger�tetypen anhand des Sysinfo-Rahmens
	/*!
	 *  Diese Klasse ist von FrameDescription abgeleitet. Damit ist der Aufbau in der XML-Datei
	 *  der gleiche wie bei einer Rahmenbeschreibung. Dabei wird der Sysinfo-Rahmen des Ger�tes
	 *  beschrieben.
	 */
	class Type:public FrameDescription{
    public:
		//! Konstruktor
        Type(){
			type=-1;
			priority=0;
			isUpdatable = false;
        };
		//! Einlesen aus einer XML-Datei
        bool InitFromXml(XMLNode& node, XMLNode& root_node);
		//! Gibt die Priorit�t zur�ck. Bestimmt damit den R�ckgabewert von RFDeviceDescription::Matches().
		int GetPriority(){return priority;};
		bool IsUpdatable(){return isUpdatable;};
	protected:
		//! Ger�tename (englische Langbezeichnung aus der XML-Datei), wird derzeit nicht verwendet
		std::string name;
		//! Priorit�t
		int priority;
		bool isUpdatable;
    };
	//! Konstruktor
	RFDeviceDescription();
	//! Destruktor
	virtual ~RFDeviceDescription();
	//! Ermittelt die passende Rahmenbeschreibung f�r eine eingehende Nachricht
	/*!
	 *  Diese Methode kann in einer Schleife aufgerufen werden und gibt dann jeweils die
	 *  n�chste passende Rahmenbeschreibung zur�ck.
	 *
	 *  \param frame Die einer Rahmenbeschreibung zuzuordnende Nachricht
	 *  \param channel Zeiger auf Variable, die die aus dem Rahmen mittels der Rahmenbeschreibung
	 *         extrahierte Kanalnummer aufnimmt. Kann \c NULL sein, wenn die Kanalnummer nicht
	 *         ben�tigt wird.
	 *  \param iterator Zeiger auf Z�hlvariable. Muss beim ersten Aufruf auf \c 0 gesetzt werden
	 *                  und wird dann automatisch inkrementiert
	 *  \return Rahmenbeschreibung oder \c NULL, wenn keine passende Beschreibung (mehr) gefunden wurde.
	 */
	FrameDescription* GetFrameDescription(StructuredFrame& frame, int* channel, int* iterator);
	//! Gibt die Rahmenbeschreibung mit einer bestimmten ID zur�ck
	FrameDescription* GetFrameDescription(const std::string& id)
	{
		framedefs_by_id_t::iterator it=framedefs_by_id.find(id);
		if(it==framedefs_by_id.end())return NULL;
		return &framedefs[it->second];
	};
	//! Pr�ft, ob die Ger�tebeschreibung den �bergebenen Ger�tetypen unterst�tzt
	bool SupportsType(const std::string& type);
	//! Gibt das Objekt zur�ck, das zus�tzliche Felder f�r die Kanalbeschreibung an der XmlRpc-Schnittstelle enth�lt
	inline HSSDescription* GetAdditionalDescription(){return &additional_description;};
	//! Liefert die maximale Anzahl von Verkn�pfungspartnern f�r das Ger�t zur�ck. Wird derzeit nicht verwendet.
	inline int GetMaxLinkPeers(){return max_link_peers;};
	//! Erzeugt eine Instanz von RFDevice oder einer abgeleiteten Klasse wenn \c creation_tag gesetzt ist
	RFDevice* CreateDevice();
	//! Gibt die Flags f�r die Oberfl�che zur�ck
	int GetFlags(){return flags;};
	//! Ger�tebeschreibung f�r das virtuelle Team-Ger�t ermitteln
	/*!
	 *  Falls vom Ger�t Teams unterst�tzt werden (z.B. Rauchmeldergruppen) kommt hier die Ger�tebeschreibung 
	 *  f�r das virtuelle Team-Ger�t zur�ck.
	 *
	 *  \return Die Ger�tebeschreibung des virtuellen Team-Ger�tes oder \c NULL wenn vom Ger�t keine Teams
	 *          unterst�tzt werden.
	 */
	RFDeviceDescription* GetTeamDescription(){return team_description;};
	bool IsUpdatable(StructuredFrame &sysinfo);
	
	//! Gibt den Burst Mode des Gerätes zurück. Nur sinnvoll, bei RX_BURST.
	BurstMode GetBurstMode();

protected:
	//! Typedef f�r den Vektor von Kanalbeschreibungen
    typedef std::vector<RFChannelDescription*> channels_t;
	//! Vektor von Kanalbeschreibungen
    channels_t channels;
	//! Typedef f�r den Vektor von unterst�tzten Ger�tetypen
    typedef std::vector<Type> types_t;
	//! Vektor von unterst�tzten Ger�tetypen
    types_t supported_types;
	//! Typedef f�r die Map der in der XML-Datei definierten Parametersets
    typedef std::map<std::string, RFParamset> paramsets_t;
	//! Map der in der XML-Datei definierten Parametersets
	paramsets_t paramsets;

	//! Typedef f�r den Vektor von Rahmenbeschreibungen
	typedef std::vector<FrameDescription> framedefs_t;
	//! Vektor von Rahmenbeschreibungen
	framedefs_t framedefs;
	//! Typedef einer Map f�r den Zugriff auf Rahmenbeschreibungen �ber die ID
	typedef std::map<std::string, int> framedefs_by_id_t;
	//! Map f�r den Zugriff auf Rahmenbeschreibungen �ber die ID
	framedefs_by_id_t framedefs_by_id;
	friend class RFParamset;
	//! Vom Ger�t unterst�tzte Empfangsmodi
	int rx_modes;
	//! Zusatzfelder f�r die Kanalbeschreibung an der XmlRpc-Schnittstelle
	HSSDescription additional_description;
	//! Flag ob im vom Ger�t beim Anlernen an die CCU �bertragenen Sysinof-Rahmen eine Kanalnummer erwartet wird
	bool peering_sysinfo_expect_channel;
	//! Flag ob AES unterst�tzt wird
	bool supports_aes;
	//! Flag ob bei der zeitabh�ngigen Erreichbarkeitspr�fung (zyklischer Sender) nur AES-Authentifizierte Pakete akzeptiert werden
	bool unreach_check_aes;
	//! Maximale Anzahl von Verkn�pfungspartners (wird nicht verwendet)
	int max_link_peers;
	//! Zeit in Sekunden, nach der ein Ger�t als nicht erreichbar betrachtet wird, wenn keine Nachricht vom Ger�t empfangen wurde
	/*! 
	 *  Bei \c 0 findet keine zeitabh�ngige Erreichbarkeitspr�fung statt.
	 */
	uint32_t cyclic_timeout;
	//! Ist gesetzt, wenn eine andere Klasse als RFDevice f�r die Ger�teinstanz verwendet werden soll
	std::string creation_tag;
	//! Version der XML-Datei
	int version;
	//! Flags f�r die Oberfl�che
	int flags;
	//! Ger�tebeschreibung f�r das virtuelle Team-Ger�t
	RFDeviceDescription* team_description;
	//bool updatable;


	/** \brief Default RX_MODE setting.
	 * \details If device supports WAKEUP and BURST, and rx_default ist set in device description xml,
	 * rx_mode_default holds rx_default attributes value.
	 * This is part of dynamic rx_mode change feature for DTAG.
	 * Default value, if not set in device description xml, is RX_WAKEUP
	 */
	int rx_mode_default;

	/* \brief Burst mode of the device. */
	BurstMode burstMode;

};

#endif // !defined(AFX_RFDEVICEDESCRIPTION_H__41F2BBEB_C64D_4FB5_A126_713C3C9B7AAF__INCLUDED_)

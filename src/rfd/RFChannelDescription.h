/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// RFChannelDescription.h: Schnittstelle f�r die Klasse RFChannelDescription.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_RFCHANNELDESCRIPTION_H__A3DD9C11_969F_42AD_8D8C_49E0A0E87430__INCLUDED_)
#define AFX_RFCHANNELDESCRIPTION_H__A3DD9C11_969F_42AD_8D8C_49E0A0E87430__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <map>
#include <string>
#include "xmlParser.h"
#include "XmlRpc.h"
#include "RFParamset.h"
#include "BidcosFrame.h"

class RFDeviceDescription;
class RFLogicalInstance;
class RFController;

//! Diese Klasse enth�lt die f�r einen Kanaltypen aus der Ger�tebeschreibungsdatei gelesenen Informationen
class RFChannelDescription  
{
public:
	//! Aufz�hlung der Tastenfunktionen
	enum{
		FUNCTION_A=1, //!< Einschalten
		FUNCTION_B=0, //!< Ausschalten
		FUNCTION_AB=2 //!< Toggeln
	};
	//! Aufz�hlung f�r die Richtung des Informationsflusses in einer direkten Verkn�pfung
	enum{
		DIRECTION_NONE=0, //!< Keine Richtung, Kanal nicht verkn�pfbar
		DIRECTION_SENDER=1, //!< Kanal ist in einer direkten Verkn�pfung Sender
		DIRECTION_RECEIVER=2 //!< Kanal ist in einer direkten Verkn�pfung Empf�nger
	};
	//! Aufz�hlung von Flags f�r die Oberfl�che
	enum{
		FLG_NONE=0, //!< Kein Flag gesetzt
		FLG_VISIBLE=(1<<0), //!< Kanal ist sichtbar
		FLG_INTERNAL=(1<<1) //!< Kanal wird nur intern verwendet
	};
	//! Typedef f�r erzwungenen Werte
	/*! Erzwungene Werte werden von der CCU im Ger�t zwangsweise gesetzt. Sie werden so fr�h wie
	 *  m�glich an das Ger�t �bertragen. Das ist normalerweise unmittelbar nach dem Anlernen oder
	 *  unmittelbar nach dem Anlegen einer neuen direkten Verkn�pfung.
	 */
	typedef std::map<std::string, XmlRpc::XmlRpcValue> enforced_values_t;
	//! Flags f�r die Rolle des Kanals in einer direkten Verkn�pfung
	enum{
		LINK_ROLE_FLAG_SENDER=DIRECTION_SENDER, //!< Kanal ist in der direkten Verkn�pfung Sender
		LINK_ROLE_FLAG_RECEIVER=DIRECTION_RECEIVER, //!< Kanal ist in der direkten Verkn�pfung Empf�nger
		LINK_ROLE_FLAG_VIRTUAL=4, //!< Die Verkn�pfung ist virtuell, d.h. sie wird auf der Seite dieses Kanals nicht im Ger�t gespeichert
		LINK_ROLE_FLAG_TEAM=8 //!< Die Verkn�pfung dient der Teambildung
	};
	//! Typedef f�r die Rolle in einer direkten Verkn�pfung
	typedef struct {
		std::string name; //!< Name der Verkn�pfung
		int flags; //!< Flags, siehe oben
	}link_role_t;
	//! Verarbeitung einer eingehenden Nachricht f�r den Kanal inst
	/*! Wird im Rahmen der Verarbeitung der eingehenden Nachricht von RFChannel::ProcessIncomingFrame()
	 *  aufgerufen. Der Aufruf wird direkt durchgereicht an RFParamset::ProcessIncomingFrame() f�r das
	 *  Parameterset VALUES.
	 */
	void ProcessIncomingFrame(RFLogicalInstance* inst, BidcosFrame& frame, FrameDescription* fd);

	//! Verarbeitung einer weitergeleiteten eingehenden NAchricht
	void ProcessForwardedFrame(RFLogicalInstance* inst, BidcosFrame& frame, FrameDescription* fd);

	//! Setzt den Startindex f�r die durch dieses Objekt beschriebenen Kan�le
	void SetIndex(int index)
	{
		this->index=index;
	};
	//! Gibt den Startindex f�r die durch dieses Objekt beschriebenen Kan�le zur�ck
	int GetIndex()
	{
		return index;
	};
	bool SetDefaultConfig(RFLogicalInstance *inst);
	//! Gibt die Anzahl aufeinanderfolgender Kan�le zur�ck, die durch das Objekt beschrieben werden
	/*! Weil die Kanalanzahl vom Sysinfo-Frame abh�ngen kann, muss hier ein RFDevice �bergeben werden.
	 */
	int GetCount(RFDevice* dev);
	//! Ruft SetEnforcedParameters f�r alle Parametersets auf
	bool SetEnforcedParameters(RFLogicalInstance* inst);
	//! Listet die definierten Parametersets in Form eines Arrays auf, wie f�r die XmlRpc-Schnittstelle ben�tigt
	bool ListParamsets(XmlRpc::XmlRpcValue* list);
	//! Liefert das Parameterset mit dem Typen key zur�ck
	/*! Key kann "MASTER", "VALUES" oder "LINK" sein. Alle anderen Werte werden wie "LINK" behandelt, weil
	 *  angenommen wird, dass es sich um die Adresse eines Verkn�pfungspartners handelt.
	 */
	RFParamset* GetParamset(const std::string& key);
	//! Liefert den aus der XML-Datei gelesenen Kanaltyp zur�ck
	const std::string& GetType();
	//! Abfrage, ob der Kanal direkte Verkn�pfungen unterst�tzt
	inline bool HasLinkPeers(){return (direction!=DIRECTION_NONE) && !has_team;};
	//! Das dem Kanal �bergeordnete Ger�teobjekt setzen
	void SetDevice(RFDeviceDescription* device);
	//! Ermittelt die Funktion einer Taste innerhalb eines Tastenpaares
	/*! Liefert die Funktion der Taste mit der Kanalnumer \c index zur�ck. Falls
	 *  es sich um eine Taste eines Tastenpaares handelt, muss in \c other_pair_index
	 *  die Kanalnummer der anderen Taste �bergeben werden. Handelt es sich nicht um
	 *  ein Tastenpaar, wird hier \c 0 �bergeben.
	 */
	int GetFunction(int index, int other_pair_index);
	//! Konstruktor
	RFChannelDescription();
	//! Destruktor
	virtual ~RFChannelDescription();
	//! Einlesen der Kanalbeschreibung aus einer XML-Datei
    virtual bool InitFromXml(XMLNode& node, XMLNode& root_node);
	//! Gibt zur�ck, ob der Kanal bei Verwendung in der Logikschicht automatisch an den LISTENER-Kanal angelernt werden muss
	inline bool GetAutoregisterCentral(){return autoregister_central;};
	//! Liefert zu einer Kanalnummer einer Taste die Kanalnummer der anderen Taste im Tastenpaar
	/*! Gibt \c 0 zur�ck, wenn die �bergebene Kanalnummer nicht zu einem Tastenpaar geh�rt.
	 */
	int GetOtherPairIndex(int index)
	{
		if(paired)return ((index-this->index))%2?index-1:index+1;
		else return 0;
	}
	//! Liefert das Flag zur�ck, ob f�r den Kanal nach dem Anlernen an die Zentrale erstmal AES aktiviert werden soll
	inline bool GetAESDefault(){return aes_default || aes_always;};
	//! Liefert das Flag zur�ck, ob f�r den Kanal AES immer aktiv ist (z.B. Keymatic)
	inline bool GetAESAlways(){return aes_always;};
	//! Wird w�hrend der Initialisierung von \c inst aufgerufen
	/*! Ruft RFParamset::SetupInstance() f�r alle Parametersets auf, um diesen die Gelegenheit
	 *  zu geben, Defaultwerte mit ValueStore::SetStoredValue() in \c inst zu speichern.
	 */
	bool SetupInstance(RFLogicalInstance* inst);
	//! Gibt die Liste der Verkn�pfungen zur�ck, in denen der Kanal die Rolle Sender �bernehmen kann
	/*! \return Durch Leerzeichen getrennte Liste von Verkn�pfungs-Typ-Namen
	 *  �ber diese Liste kann die Oberfl�che beim Anlegen einer Verkn�pfung zueinander passende
	 *  Kan�le anbieten.
	 */
	std::string GetLinkSourceRoles();
	//! Gibt die Liste der Verkn�pfungen zur�ck, in denen der Kanal die Rolle Empf�nger �bernehmen kann
	/*! \return Durch Leerzeichen getrennte Liste von Verkn�pfungs-Typ-Namen
	 *  �ber diese Liste kann die Oberfl�che beim Anlegen einer Verkn�pfung zueinander passende
	 *  Kan�le anbieten.
	 */
	std::string GetLinkTargetRoles();
	//! Gibt den Wert des Attributes \c class aus der XML-Datei zur�ck
	/*! Wird verwendet, wenn nicht RFChannel sondern eine davon abgeleitete spezielle Klasse
	 *  f�r den Kanal verwendet werden soll. Dieser Wert wird dann mit dem Pr�fix \c channel_class
	 *  an hsscomm::type_registry::create() �bergeben.
	 */
	inline const std::string& GetCreationTag(){return creation_tag;};
	//! Gibt das Objekt zur�ck, das zus�tzliche Felder f�r die Kanalbeschreibung an der XmlRpc-Schnittstelle enth�lt
	inline HSSDescription* GetAdditionalDescription(){return &additional_description;};
	//! Gibt die maximale Anzahl von Verkn�pfungspartnern zur�ck. Wird derzeit nicht verwendet.
	inline int GetMaxLinkPeers(){return max_link_peers;};
	//! Gibt die Richtung des Informationsflusses in einer direkten Verkn�pfung zur�ck
	inline int GetDirection(){return direction;};
	//! Gibt die Flags f�r die Oberfl�che zur�ck
	inline int GetFlags(){return flags;};
	//! Gibt die erzwungenen Werte f�r die Gegenseite einer neuen Verkn�pfung zur�ck
	enforced_values_t& GetLinkEnforcements(){return link_enforcements;};
	//! Gibt das Flag zur�ck f�r das vollst�ndige Verstecken des Kanals an der XmlRpc-Schnittstelle
	/*! Ein Kanal, der dieses Flag gesetzt hat, existiert aus Sicht der Logikschicht nicht.
	 */
	inline bool IsHidden(){return hidden;};
	//! Ermittelt die Rollen zweier potentieller Verkn�pfungspartner
	/*! Ermittelt aus zwei Kanalbeschreibungen (\c this und \c peer) welche Rolle die beiden
	 *  Partner in einer Verkn�pfung einnehmen w�rden.
	 *  \param peer Zeiger auf die Kanalbeschreibung des potentiellen Partners
	 *  \param my_role Zeiger auf die Variable, die die Beschreibung unserer Seite der Verkn�pfung aufnimmt
	 *  \param peer_role Zeiger auf die Variable, die die Beschreibung der anderenr Seite der Verkn�pfung aufnimmt
	 */
	bool GetLinkRoles(RFChannelDescription* peer, link_role_t* my_role, link_role_t* peer_role);
	//! Gibt zur�ck, ob ein Kanal mit dieser Beschreibung ein Team unterst�tzt
	bool HasTeam(){return has_team;};
	//! Gibt den Schl�sseltag f�r die Teamzuordnung zur�ck
	/*! Soll ein Kanal einem Team zugeordnet werden, so ist das nur m�glich, wenn Kanal und Team
	 *  hier den selben String zur�ckgeben. Der String wird aus der XML-Datei gelesen.
	 */
	const std::string& GetTeamTag(){return team_tag;};

	bool IsAesCbcSupported();

	const std::vector<int>& getValueForwardingChannels() { return valueForwardingChannels; }

protected:
	//! Die beschriebenen Kan�le sind zu Tastenpaaren gruppiert
	bool paired;
	//! Zeiger auf die zugeh�rige Ger�tebeschreibung
	RFDeviceDescription* device;
	//! Startnummer der Kan�le mit dieser Beschreibung
    int index;
	//! Kanaltyp aus XML-Datei
    std::string type;
	//! Funktion der Taste eines Tastenpaares mit der kleineren Kanalnummer
	int pair_first_function;
	//! Funktion der Taste eines Tastenpaares mit der gr��eren Kanalnummer
	int pair_second_function;
	//! Funktion f�r eine Einzeltaste
	int function;
	//! Typedef f�r Map von Parametersets
	typedef std::map<std::string, RFParamset> paramsets_t;
	//! Map von Parametersets
	paramsets_t paramsets;
	//! Anzahl Kan�le zu diesr Beschreibung mit fortlaufenden Kanalnummern, falls die Anzahl fest ist.
	int count;
	//! Feld im Sysinfo-Rahmen, aus dem die Kanalanzahl gelesen wird. Nur bei \c count=0
	uint32_t sysinfo_count_field;
	//! Gibt an ob der Kanal bei Verwendung in der Logikschicht automatisch an den LISTENER-Kanal angelernt werden muss
	bool autoregister_central;
	//! Gibt an, ob AES nach dem Anlernen erstmal aktiv geschaltet wird
	bool aes_default;
	//! Gibt an, ob AES dauerhaft aktiv ist und nicht deaktiviert werden kann
	bool aes_always;
	//! Typedef f�r Verwaltung von Verkn�pfungsrollen
	typedef std::map<std::string, link_role_t> link_roles_t;
	//! Verkn�pfungsrollen, in denen der Kanal Sender ist
	link_roles_t link_source_roles;
	//! Verkn�pfungsrollen, in denen der Kanal Empf�nger ist
	link_roles_t link_target_roles;
	//! Tag f�r dynamische Erzeugung der Kanal-Klasse
	std::string creation_tag;
	//! Tag f�r zuordnung von Kan�len zu Teams
	std::string team_tag;
	//! Maximale Anzahl Verkn�pfungspartner
	int max_link_peers;
	//! Richtung des Informationsflusses in einer direkten Verkn�pfung
	int direction;
	//! Flags f�r die Oberfl�che
	int flags;
	//! Zusatzfelder f�r die Kanalbeschreibung an der XmlRpc-Schnittstelle
	HSSDescription additional_description;
	//! Erzwungene Werte f�r die andere Seite einer direkten Verkn�pfung
	enforced_values_t link_enforcements;
	//! Gibt an, ob der Kanal an der XmlRpc-Schnittstelle versteckt ist
	bool hidden;
	//! Gibt an, ob der Kanal Teambildung unterst�tzt
	bool has_team;
	//! Gibt an, ob das Gerät das Verfahren zu Authentifizierung von Wetterdaten unterstützt. (AES CBC)
	bool aes_cbc;

	/** \brief Channel subdescriptions used for behaviour feature also known from hs485d
	 * \details Holds alternative channel configurations.
	 */
	std::vector<RFChannelDescription*> vec_subdescriptions;

	/** \brief Special paramater 'BEHAVIOUR'
	 * \details Part of behaviour feature.
	 */
	HSSParameter* behaviour_param;

	std::vector<int> valueForwardingChannels;

public:
	//! Hilfsfunktion zur Umwandlung eines Zeichens in eine Funktionsspezifikation
	/*! - \c 'A' und \c 'a' -> \c FUNCTION_A
	 *! - \c 'B' und \c 'b' -> \c FUNCTION_B
	 *! - \c 'T' und \c 't' und alles andere -> \c FUNCTION_AB
	 */
	static int FunctionFromChar(char c);

	/** \brief Retrieves subdescription at given index or NULL if no such subdescription.
	 */
	RFChannelDescription* GetSubdescription(int index);

	/** \brief Checks if there are subdescriptions.
	 * \return True if there are subdescriptions, false otherwise.
	 */
	bool HasSubdescriptions();

	/**
	 * \brief Returns the BEHAVIOUR parameter (can be NULL).
	 */
	HSSParameter* GetBehaviourParam();
};

#endif // !defined(AFX_RFCHANNELDESCRIPTION_H__A3DD9C11_969F_42AD_8D8C_49E0A0E87430__INCLUDED_)

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RFPHYSICALDATAINTERFACECOMMAND_H_
#define _RFPHYSICALDATAINTERFACECOMMAND_H_

#include "HSSPhysicalDataInterface.h"
#include "FrameDescription.h"

//! Data-Interface für das Versenden und Empfangen von BidCoS-RF-Nachrichten

class RFPhysicalDataInterfaceCommand :
	public HSSPhysicalDataInterface
{
public:
	//! Zeitkonstanten
	enum{
		VALUE_CACHE_TIME=2000 //!< Maximales Alter des Wertes in ms für das Bedienen einer Abfrage aus dem Cache ohne Kommunikation
	};
	//! Aufzählung für Verfahrensweisen bei AES-Verletzungen
	/*!
	 *  Was wird mit einer empfangenen Nachricht gemacht, wenn AES für den Kanal aktiviert ist,
	 *  für die Nachricht aber kein AES-Zyklus durchlaufen wurde?
	 *  Dies ist relevant für empfangene Broadcast-Nachrichten oder mitgehörte Nachrichten an andere
	 *  Empfänger.
	 */
	enum{
		AUTH_VIOLATE_IGNORE, //!< Die Verletzung wird ignoriert und die Nachricht normal verarbeitet
		AUTH_VIOLATE_REJECT, //!< Die Nachricht wird verworfen
		AUTH_VIOLATE_GET //!< Die Nachricht wird verworfen und der betroffene Wert nach wenigen Sekunden von der Zentrale aktiv beim Gerät abgefragt
	};
	RFPhysicalDataInterfaceCommand(void);
	virtual ~RFPhysicalDataInterfaceCommand(void);
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	virtual bool SetDefaultConfig(LogicalInstance* inst,XmlRpc::XmlRpcValue val);
    virtual bool GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param);
    virtual bool PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param);
	virtual bool GetFromIncomingFrame(LogicalInstance* inst, StructuredFrame& frame, FrameDescription* fd, XmlRpc::XmlRpcValue* val);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("data_interface_command") erzeugen
	 */
	static bool CheckCreationTag(const char *tag);
	bool SetupInstance(LogicalInstance* inst);
protected:
	//! Beschreibt die Verwendung einer FrameDescription für zu sendende oder als Antwort empfangende Kommandonachrichten
	class frame_t
	{
	public:
		//! Konstruktor
		frame_t(){
			auth_violate_policy=AUTH_VIOLATE_IGNORE;
			process_as_event=false;
		}
		//! Referenz auf die Id der FrameDescription
		std::string id;
		//! Was passiert mit einer eingehenden Nachricht bei einer AES-Verletzung
		int auth_violate_policy;
		//! Soll eine eingehende Antwortnachricht zusätzlich wie eine asynchron empfangene Nachricht verarbeitet werden
		bool process_as_event;
	};
	//! Beschreibt die Verwendung einer FrameDescription für asynchron empfangende Ereignisnachrichten
	/*!
	 *  Ermöglicht es, aufgrund einer empfangenen Nachricht einen Wert automatisch nach einer berechneten
	 *  Zeit auf einen definierten Wert zu setzen (für adaptive Sendeintervalle bei PIRs)
	 */
	class event_frame_t:public frame_t
	{
	public:
		//! Konstruktor
		event_frame_t(){
		}
		//! Id eines Integer-Wertes des Kanals, der die Verzögerung in Sekunden für das automatische Setzen angibt
		std::string domino_delay_id;
		//! Der nach Ablauf der Verzögerung zu setzende Wert
		XmlRpc::XmlRpcValue domino_value;
	};
	//! Typedef für Rahmenbeschreibungen in Setzbefehlen
	typedef std::vector<frame_t> frames_t;
	//! Typedef für Rahmenbeschreibungen in Ereignissen
	typedef std::vector<event_frame_t> event_frames_t;
	//! Vektor der zum Setzen eines Wertes versandten Nachrichten
	frames_t set_request_frames;
	//! Die zum Abfragen eines Wertes versandte Nachricht
	frame_t get_request_frame;
	//! Die als Antwort auf eine Abfrage erwartete Nachricht
	frame_t get_response_frame;
	//! Vektor der als Ereignis verarbeiteten Nachrichten
	event_frames_t event_frames;
	//! Zuordnung zum Parameter innerhalb einer FrameDescription und Id für das Speichern am Kanalobjekt
	std::string value_id;
	//! Die am Kanalobjekt gespeicherte Kopie des Wertes wird beim Starten nicht auf den Standardwert initialisiert
	bool no_init;
	//! Vektor von Werten des Kanals, die nach dem Senden auf den jeweiligen Standardwert zurückgesetzt werden.
	std::vector<std::string> reset_after_send_params;
};
#endif

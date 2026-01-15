/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HS485PHYSICALDATAINTERFACECOMMAND_H_
#define _HS485PHYSICALDATAINTERFACECOMMAND_H_

#include "HSSPhysicalDataInterface.h"
#include "HS485PhysicalAddress.h"

//! Data-Interface für das Versenden und Empfangen von BidCoS-Wired-Nachrichten
class HS485PhysicalDataInterfaceCommand :
	public HSSPhysicalDataInterface
{
public:
	//! Zeitkonstanten
	enum{
		VALUE_CACHE_TIME=2000 //!< Maximales Alter des Wertes in ms für das Bedienen einer Abfrage aus dem Cache ohne Kommunikation
	};
	HS485PhysicalDataInterfaceCommand(void);
	virtual ~HS485PhysicalDataInterfaceCommand(void);
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
    virtual bool GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param);
    virtual bool PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param);
	virtual bool GetFromIncomingFrame(LogicalInstance* inst, StructuredFrame& frame, FrameDescription* fd, XmlRpc::XmlRpcValue* val);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch dynamic::create("data_interface_command") erzeugen
	 */
	static bool CheckCreationTag(const char *tag);
	bool SetupInstance(LogicalInstance* inst);

	virtual bool SetDefaultConfig(LogicalInstance* inst, XmlRpc::XmlRpcValue val) { return true; }
protected:
	//! Beschreibt die Verwendung einer FrameDescription für zu sendende, als Antwort empfangende oder als Eeignis eingehende Nachrichten
	typedef struct{
		std::string id; //!< Referenz auf die Id der FrameDescription
		bool process_as_event; //!< Soll eine eingehende Antwortnachricht zusätzlich wie eine asynchron empfangene Nachricht verarbeitet werden
	} frame_t;
	//! Typedef für Rahmenbeschreibungen in Setzbefehlen
	typedef std::vector<frame_t> frames_t;
	//! Vektor der zum Setzen eines Wertes versandten Nachrichten
	frames_t set_request_frames;
	//! Die zum Abfragen eines Wertes versandte Nachricht
	frame_t get_request_frame;
	//! Die als Antwort auf eine Abfrage erwartete Nachricht
	frame_t get_response_frame;
	//! Vektor der als Ereignis verarbeiteten Nachrichten
	frames_t event_frames;
	//! Zuordnung zum Parameter innerhalb einer FrameDescription und Id für das Speichern am Kanalobjekt
	std::string value_id;
	//! Die am Kanalobjekt gespeicherte Kopie des Wertes wird beim Starten nicht auf den Standardwert initialisiert
	bool no_init;
};
#endif

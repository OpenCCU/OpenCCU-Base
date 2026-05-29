/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSPhysicalType.h: Schnittstelle für die Klasse HSSPhysicalType.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSPHYSICALTYPE_H_
#define _HSSPHYSICALTYPE_H_

#include "dllexport.h"


#include <vector>
#include <string>
#include <XmlRpc.h>
#include "xmlParser.h"
#include "HSSPhysicalDataInterface.h"

class  HSSParameter;
//! Basisklasse für von HSSParameter verwendeten physikalische Typen
/*! Diese Klasse dient dazu, die physikalische Darstellung eines Parameters
 *  vom Gerät zu lesen, auf das Gerät zu schreiben und aus eingehenden
 *  Nachrichten zu extrahieren.
 *  Um unabhängig davon zu sein, auf welche Art die physikalischen Daten mit 
 *  dem Gerät kommuniziert werden, wird die tatsächliche Kommunikation an eine
 *  von HSSPhysicalDataInterface abgeleitete Klasse delegiert.
 */
class DLLEXPORT HSSPhysicalType  
{
public:
	//! Initialisierung aus einer XML-Datei
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Konstruktor
	HSSPhysicalType();
	//! Destruktor
	virtual ~HSSPhysicalType();
	//! Ruft die SetDefaultConfig methode des data_interface auf
	virtual bool SetDefaultConfig(LogicalInstance *inst,XmlRpc::XmlRpcValue val);
	//! Gibt den Typen als String zurück ("integer", "string", etc.)
    const std::string& GetType();
	//! Liefert den aktuellen physikalischen Wert zurück
	/*! Bedient sich dazu HSSPhysicalDataInterface::GetData()
	 *  \param inst Geräte- oder Kanalobjekt, auf das sich die Aktion bezieht
	 *  \param val Zeiger auf Variable, die den Wert aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
    virtual bool Get(LogicalInstance* inst, XmlRpc::XmlRpcValue* val);
	//! Setzt den aktuellen physikalischen Wert
	/*! Bedient sich dazu HSSPhysicalDataInterface::PutData()
	 *  \param inst Geräte- oder Kanalobjekt, auf das sich die Aktion bezieht
	 *  \param val Referenz auf Variable, die den zu setzenden Wert enthält
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
    virtual bool Put(LogicalInstance* inst, XmlRpc::XmlRpcValue& val);
	//! Initiiert das selbstständige Ermitteln des Wertes durch das Gerät
	/*! Bedient sich dazu HSSPhysicalDataInterface::DetermineValue()
	 *  \param inst Geräte- oder Kanalobjekt, auf das sich die Aktion bezieht
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
    virtual bool Determine(LogicalInstance* inst);
	//! Wird während der Verarbeitung einer asynchron eingehenden Nachricht aufgerufen
	/*! Bedient sich dazu HSSPhysicalDataInterface::GetFromIncomingFrame()
	 *  \param inst Geräte- oder Kanalobjekt, auf das sich die eingehende Nachricht bezieht
	 *  \param msg Referenz auf die empfangene Nachricht
	 *  \param fd Zeiger auf die zu \c msg passende abstrakte Beschreibung
	 *  \param val Zeiger auf die Variable, die den aus \c msg extrahierten Wert aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool GetFromIncomingFrame(LogicalInstance* inst, StructuredFrame& frame, FrameDescription* fd, XmlRpc::XmlRpcValue* val);
	//! Wird beim Erzeugen eines Geräte- oder Kanalobjekts für alle zugeordneten Parameter aufgerufen
	/*! Dieser Aufruf wird weitergereicht an HSSPhysicalDataInterface::SetupInstance(). Der Parameter bekommt hier
	 *  die Gelegenheit, in dem zugeordneten Geräte- oder Kanalobjekt abgelegte Werte 
	 *  (siehe LogicalInstance::SetStoredValue()) zu initialisieren.
	 *  \param inst Zeiger auf das zugeordnete Geräte- oder Kanalobjekt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool SetupInstance(LogicalInstance* inst){return data_interface->SetupInstance(inst);};
	//! Wird während der Initialisierung aufgerufen, um das übergeordnete Objekt der Klasse HSSParameter zu setzen
	inline void SetParent(HSSParameter* param){this->parent_param=param;};
	//! Gibt das übergeordnete Objekt der Klasse HSSParameter zurück
	inline HSSParameter* GetParent(){return parent_param;};
protected:
	//! Der während der Initialisierung aus der XML-Datei gelesene Typ
    std::string type;
	//! Objekt, an das die tatsächliche Kommunikation delegiert wird
	HSSPhysicalDataInterface* data_interface;
	//! Zeiger auf das übergeordnete Objekt der Klasse HSSParameter
	HSSParameter* parent_param;
};

#endif // _HSSPHYSICALTYPE_H_

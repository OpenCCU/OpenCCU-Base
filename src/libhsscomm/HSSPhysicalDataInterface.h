/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HSSPHYSICALDATAINTEHSSACE_H_
#define _HSSPHYSICALDATAINTEHSSACE_H_

#include "dllexport.h"

#include "CommMessage.h"
#include "LogicalInstance.h"
#include "FrameDescription.h"
#include <xmlParser.h>
#include <XmlRpc.h>

class  HSSPhysicalType;
//! Abstrakte Basisklasse für Klassen, die die Verbindung zwischen einem HSSPhysicalType und einem Gerät herstellen
/*! Von dieser Klasse abgeleitete Klassen implementieren verschiedene Methoden, um physikalische Werte zu lesen
 *  oder zu schreiben.
 */
class DLLEXPORT HSSPhysicalDataInterface
{
public:
	//!Konstruktor
	HSSPhysicalDataInterface(void);
	//!Destruktor
	virtual ~HSSPhysicalDataInterface(void);
	virtual bool SetDefaultConfig(LogicalInstance* inst, XmlRpc::XmlRpcValue val)=0;
	//! Initialisierung aus einer XML-Datei
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Fragt einen Wert vom Gerät ab
	/*! \param inst Geräte- oder Kanalobjekt, auf das sich die Aktion bezieht
	 *  \param param Zeiger auf Variable, die den Wert aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
    virtual bool GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param)=0;
	//! Überträgt einen Wert an das Gerät
	/*! \param inst Geräte- oder Kanalobjekt, auf das sich die Aktion bezieht
	 *  \param param Zeiger auf Variable, die den zu setzenden Wert enthält
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
    virtual bool PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param)=0;
	//! Weist das Gerät an, einen Wert selstständig zu ermitteln
	/*! \param inst Geräte- oder Kanalobjekt, auf das sich die Aktion bezieht
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool DetermineValue(LogicalInstance* inst){return false;};
	//! Wird während der Verarbeitung einer asynchron eingehenden Nachricht aufgerufen
	/*! \param inst Geräte- oder Kanalobjekt, auf das sich die eingehende Nachricht bezieht
	 *  \param msg Referenz auf die empfangene Nachricht
	 *  \param fd Zeiger auf die zu \c msg passende abstrakte Beschreibung
	 *  \param val Zeiger auf die Variable, die den aus \c msg extrahierten Wert aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool GetFromIncomingFrame(LogicalInstance* inst, StructuredFrame& frame, FrameDescription* fd, XmlRpc::XmlRpcValue* val)
	{
		return false;
	};
	//! Wird während der Initialisierung aufgerufen, um das übergeordnete Objekt der Klasse HSSPhysicalType zu setzen
	inline void SetParent(HSSPhysicalType* parent){parent_type=parent;};
	//! Gibt das übergeordnete Objekt der Klasse HSSPhysicalType zurück
	inline HSSPhysicalType* GetParent(){return parent_type;};
	//! Wird beim Erzeugen eines Geräte- oder Kanalobjekts für alle zugeordneten Parameter aufgerufen
	/*! Der Parameter bekommt hier die Gelegenheit, in dem zugeordneten Geräte- oder Kanalobjekt 
	 *  abgelegte Werte (siehe LogicalInstance::SetStoredValue()) zu initialisieren.
	 *  \param inst Zeiger auf das zugeordnete Geräte- oder Kanalobjekt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual inline bool SetupInstance(LogicalInstance* inst){return true;};
protected:
	//! Zeiger auf das übergeordnete Objekt der Klasse HSSPhysicalType
	HSSPhysicalType* parent_type;
};

#endif

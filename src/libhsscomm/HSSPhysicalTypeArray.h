/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSPhysicalTypeArray.h: Schnittstelle für die Klasse HSSPhysicalTypeArray.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSPHYSICALTYPEARRAY_H_
#define _HSSPHYSICALTYPEARRAY_H_

#include "dllexport.h"


#include "HSSPhysicalType.h"
#include "HSSIntegerValueMap.h"
#include "LogicalInstance.h"

//! Stellt einen physikalischen Typen dar, der ein Array bestehen aus von HSSPhysicalType abgeleiteten Objekten ist
/*! Wird z.B. verwendet zur Darstellung von BidCoS-Wired Kanaladressen. Das Array besteht dann aus 2 Integers und zwar
 *  der Busadresse sowie der Kanalnummer.
 */
class DLLEXPORT HSSPhysicalTypeArray : public HSSPhysicalType  
{
public:
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("physical_type_array") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	//! Liest die Beschreibung dieses physikalischen Typs aus einer XML-Datei ein
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Liefert den aktuellen physikalischen Wert zurück
	/*! Überschreibt HSSPhysicalType::Get()
	 *  Ruft in einer Schleife HSSPhysicalType::Get() für alle Elemente auf und bildet
	 *  aus den Werten ein Array.
	 *  \param inst Geräte- oder Kanalobjekt, auf das sich die Aktion bezieht
	 *  \param val Zeiger auf Variable, die das Array aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
    virtual bool Get(LogicalInstance* inst, XmlRpc::XmlRpcValue* val);
	//! Setzt den aktuellen physikalischen Wert
	/*! Überschreibt HSSPhysicalType::Put()
	 *  Erwartet ein Array als \c val. Ruft in einer Schleife HSSPhysicalType::Put() für alle
	 *  Elemente mit den Werten aus \c val auf.
	 *  \param inst Geräte- oder Kanalobjekt, auf das sich die Aktion bezieht
	 *  \param val Referenz auf Array mit zu setzenden Werten
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
    virtual bool Put(LogicalInstance* inst, XmlRpc::XmlRpcValue& val);
	//! Wird während der Verarbeitung einer asynchron eingehenden Nachricht aufgerufen
	/*! Überschreibt HSSPhysicalType::GetFromIncomingFrame()
	 *  Ruft in einer Schleife HSSPhysicalType::GetFromIncomingFrame() für alle Elemente auf und bildet
	 *  aus den Werten ein Array.
	 *  \param inst Geräte- oder Kanalobjekt, auf das sich die eingehende Nachricht bezieht
	 *  \param msg Referenz auf die empfangene Nachricht
	 *  \param fd Zeiger auf die zu \c msg passende abstrakte Beschreibung
	 *  \param val Zeiger auf die Variable, die das Array mit den den aus \c msg extrahierten Werten aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool GetFromIncomingFrame(LogicalInstance* inst, StructuredFrame& frame, FrameDescription* fd, XmlRpc::XmlRpcValue* val);
	//! Wird beim Erzeugen eines Geräte- oder Kanalobjekts für alle zugeordneten Parameter aufgerufen
	/*! Überschreibt HSSPhysicalType::SetupInstance().
	 *  Ruft in einer Schleife HSSPhysicalType::SetupInstance() für alle Elemente auf.
	 *  \param inst Zeiger auf das zugeordnete Geräte- oder Kanalobjekt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	virtual bool SetupInstance(LogicalInstance* inst);
	//! Konstruktor
	HSSPhysicalTypeArray();
	//! Destruktor
	virtual ~HSSPhysicalTypeArray();
protected:
	//! Typedef für Vektor der Elemente
	typedef std::vector<HSSPhysicalType*> elements_t;
	//! Vektor der Elemente
	elements_t elements;
};

#endif // _HSSPHYSICALTYPEARRAY_H_

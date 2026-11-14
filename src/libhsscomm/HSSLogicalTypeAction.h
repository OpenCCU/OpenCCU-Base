/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSLogicalTypeAction.h: Schnittstelle für die Klasse HSSLogicalTypeAction.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSLOGICALTYPEACTION_H_
#define _HSSLOGICALTYPEACTION_H_

#include "dllexport.h"


#include "HSSLogicalType.h"
//! Beschreibt eine logische Aktion, z.B. einen Tastendruck
/*! Entsprechung in der XML-Datei: &lt;logical type="action"&gt;
 *  Entsprechung in der XmlRpc-Schnittstelle: TYPE=ACTION
 *  Eine Aktion wird auf XmlRpc-Seite als Boolean dargestellt, wobei der Wert keine Bedeutung hat.
 *  Folgende Konventionen gelten für den Wert:\n
 *  Bei Lesen kommt immer \c false zurück, beim Ereignis wird immer \c true gemeldet und 
 *  beim Schreiben ist der geschriebene Wert egal.
 */
class DLLEXPORT HSSLogicalTypeAction : public HSSLogicalType  
{
public:
	//! Gibt eine logische Parameterbeschreibung gemäß XmlRpc-Schnittstelle zurück
	virtual bool GetDescription(XmlRpc::XmlRpcValue* val);
	//! Sorgt für die Einhaltung der Konventionen für den Wert
	virtual bool EnforceConstraints(LogicalInstance* inst, XmlRpc::XmlRpcValue* val, operation_t op);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("logical_type_action") erzeugen
	 */
    static bool CheckCreationTag(const char* tag);
	//! Liest die Beschreibung dieses logischen Typs aus einer XML-Datei ein
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Konstruktor
	HSSLogicalTypeAction();
	//! Desctruktor
	virtual ~HSSLogicalTypeAction();
	//! Default-Wert zurückgeben
	/*! Diese Methode gibt einen leeren XmlRpcValue zurück, der dann von EnforceConstraints in ein
	 *  \c false umgewandelt wird.
	 */
	XmlRpc::XmlRpcValue GetDefault();
protected:
};

#endif // _HSSLOGICALTYPEACTION_H_

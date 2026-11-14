/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSLogicalTypeBoolean.h: Schnittstelle für die Klasse HSSLogicalTypeBoolean.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSLOGICALTYPEBOOLEAN_H_
#define _HSSLOGICALTYPEBOOLEAN_H_

#include "dllexport.h"


#include "HSSLogicalType.h"

//! Beschreibt den logischen Typ Boolean
/*! Entsprechung in der XML-Datei: &lt;logical type="boolean"&gt;
 *  Entsprechung in der XmlRpc-Schnittstelle: TYPE=BOOL
 */
class DLLEXPORT HSSLogicalTypeBoolean : public HSSLogicalType  
{
public:
	//! Gibt eine logische Parameterbeschreibung gemäß XmlRpc-Schnittstelle zurück
	virtual bool GetDescription(XmlRpc::XmlRpcValue* val);
	//! Läßt einen übergebenen Wert vom Typ Boolean unverändert und konvertiert Integer und String in Boolean
	virtual bool EnforceConstraints(LogicalInstance* inst, XmlRpc::XmlRpcValue* val, operation_t op);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("logical_type_boolean") erzeugen
	 */
    static bool CheckCreationTag(const char* tag);
	//! Liest die Beschreibung dieses logischen Typs aus einer XML-Datei ein
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Konstruktor
	HSSLogicalTypeBoolean();
	//! Destruktor
	virtual ~HSSLogicalTypeBoolean();
	//! Gibt den in der XML-Beschreibung angegebenen Defaultwert zurück
	XmlRpc::XmlRpcValue GetDefault();
protected:
	//! Defaultwert aus der XML-Beschreibung
    bool default_value;
};

#endif // _HSSLOGICALTYPEBOOLEAN_H_

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSLogicalTypeString.h: Schnittstelle für die Klasse HSSLogicalTypeString.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSLOGICALTYPESTRING_H_
#define _HSSLOGICALTYPESTRING_H_

#include "dllexport.h"


#include "HSSLogicalType.h"

//! Beschreibt den logischen Typ String
/*! Entsprechung in der XML-Datei: &lt;logical type="string"&gt;
 *  Entsprechung in der XmlRpc-Schnittstelle: TYPE=STRING
 */
class DLLEXPORT HSSLogicalTypeString : public HSSLogicalType  
{
public:
	//! Gibt eine logische Parameterbeschreibung gemäß XmlRpc-Schnittstelle zurück
	virtual bool GetDescription(XmlRpc::XmlRpcValue* val);
	//! Stellt sicher, dass es sich bei dem übergebenen Wert um einen String handelt
	virtual bool EnforceConstraints(LogicalInstance* inst, XmlRpc::XmlRpcValue* val, operation_t op);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("logical_type_string") erzeugen
	 */
    static bool CheckCreationTag(const char* tag);
	//! Liest die Beschreibung dieses logischen Typs aus einer XML-Datei ein
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Konstruktor
	HSSLogicalTypeString();
	//! Destruktor
	virtual ~HSSLogicalTypeString();
	//! Gibt den in der XML-Datei angegebenen Defaultwert zurück
	XmlRpc::XmlRpcValue GetDefault();
protected:
	//! Defaultwert aus der XML-Datei
	std::string default_value;
};

#endif // _HSSLOGICALTYPESTRING_H_

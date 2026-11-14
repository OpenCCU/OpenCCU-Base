/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSLogicalTypeInteger.h: Schnittstelle für die Klasse HSSLogicalTypeInteger.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSLOGICALTYPEINTEGER_H_
#define _HSSLOGICALTYPEINTEGER_H_

#include "dllexport.h"


#include "HSSLogicalType.h"

//! Beschreibt den logischen Typ Integer
/*! Entsprechung in der XML-Datei: &lt;logical type="integer"&gt;
 *  Entsprechung in der XmlRpc-Schnittstelle: TYPE=INTEGER
 *  Der logische Typ Integer hat einen kontinuierlichen Wertebereich, der durch \c min und \c max
 *  begrenzt ist. Zuätzlich kann es außerhalb dieses Bereiches diskrete Spezialwerte mit einer besonderen Bedeutung
 *  geben.\n
 *  Für ein Beispiel siehe HSSLogicalTypeFloat.
 */
class DLLEXPORT HSSLogicalTypeInteger : public HSSLogicalType  
{
public:
	//! Gibt eine logische Parameterbeschreibung gemäß XmlRpc-Schnittstelle zurück
	virtual bool GetDescription(XmlRpc::XmlRpcValue* val);
	//! Begrenzt den übergebenen Wert auf \c min und \c max
	/*! Zusätzlich wird ein übergebener String vorher in einen Integer konvertiert.
	 */
	virtual bool EnforceConstraints(LogicalInstance* inst, XmlRpc::XmlRpcValue* val, operation_t op);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("logical_type_integer") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	//! Liest die Beschreibung dieses logischen Typs aus einer XML-Datei ein
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Konstruktor
	HSSLogicalTypeInteger();
	//! Destruktor
	virtual ~HSSLogicalTypeInteger();
	//! Gibt den in der XML-Datei angegebenen Defaultwert zurück
	XmlRpc::XmlRpcValue GetDefault();

protected:
	//! Minimalwert aus der XML-Datei
	int min;
	//! Maximalwert aus der XML-Datei
	int max;
	//! Minimalwert aus der XML-Datei erweitert um Spezialwerte, die kleiner sind als der Minimalwert
	int effective_min;
	//! Maximalwert aus der XML-Datei erweitert um Spezialwerte, die größer sind als der Maximalwert
	int effective_max;
	//! Defaultwert aus der XML-Datei
	int default_value;
	//! Array diskreter Spezialwerte mit einer speziellen Bedeutung außerhalb des Wertebereichs
	/*! Dieses Array besteht aus XmlRpc-Structs. Jeder XmlRpc-Struct hat zwei Felder, \c VALUE und 
	 *  \c ID. Damit kann dieses Array direkt von GetDescription() zurückgegeben werden.
	 */
	XmlRpc::XmlRpcValue special_values;
};

#endif // _HSSLOGICALTYPEINTEGER_H_

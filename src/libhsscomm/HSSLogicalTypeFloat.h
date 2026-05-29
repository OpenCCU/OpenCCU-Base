/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSLogicalTypeFloat.h: Schnittstelle für die Klasse HSSLogicalTypeFloat.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSLOGICALTYPEFLOAT_H_
#define _HSSLOGICALTYPEFLOAT_H_

#include "dllexport.h"


#include "HSSLogicalType.h"

//! Beschreibt den logischen Typ Float
/*! Entsprechung in der XML-Datei: &lt;logical type="float"&gt;
 *  Entsprechung in der XmlRpc-Schnittstelle: TYPE=FLOAT
 *  Der logische Typ Float hat einen kontinuierlichen Wertebereich, der durch \c min und \c max
 *  begrenzt ist. Zuätzlich kann es außerhalb dieses Bereiches diskrete Spezialwerte mit einer besonderen Bedeutung
 *  geben.\n
 *  Beispiel Winmatic: Die Kippstellung des Fensters entspricht dem kontinuierlichen Wertebereich von
 *  0.0 bis 1.0. Zusätzlich bedeutet der Spezialwert -0.05 "Fenster verriegelt".
 */
class DLLEXPORT HSSLogicalTypeFloat : public HSSLogicalType  
{
public:
	//! Gibt eine logische Parameterbeschreibung gemäß XmlRpc-Schnittstelle zurück
	virtual bool GetDescription(XmlRpc::XmlRpcValue* val);
	//! Begrenzt den übergebenen Wert auf \c min und \c max
	/*! Zusätzlich wird ein übergebener String vorher in einen Float konvertiert.
	 */
	virtual bool EnforceConstraints(LogicalInstance* inst, XmlRpc::XmlRpcValue* val, operation_t op);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("logical_type_float") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	//! Liest die Beschreibung dieses logischen Typs aus einer XML-Datei ein
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Konstruktor
	HSSLogicalTypeFloat();
	//! Destruktor
	virtual ~HSSLogicalTypeFloat();
	//! Gibt den in der XML-Beschreibung angegebenen Defaultwert zurück
	XmlRpc::XmlRpcValue GetDefault();

protected:
	//! Minimalwert aus der XML-Datei
	double min;
	//! Maximalwert aus der XML-Datei
	double max;
	//! Minimalwert aus der XML-Datei erweitert um Spezialwerte, die kleiner sind als der Minimalwert
	double effective_min;
	//! Maximalwert aus der XML-Datei erweitert um Spezialwerte, die größer sind als der Maximalwert
	double effective_max;
	//! Defaultwert aus der XML-Datei
	double default_value;
	//! Array diskreter Spezialwerte mit einer speziellen Bedeutung außerhalb des Wertebereichs
	/*! Dieses Array besteht aus XmlRpc-Structs. Jeder XmlRpc-Struct hat zwei Felder, \c VALUE und 
	 *  \c ID. Damit kann dieses Array direkt von GetDescription() zurückgegeben werden.
	 */
	XmlRpc::XmlRpcValue special_values;
};

#endif // _HSSLOGICALTYPEFLOAT_H_

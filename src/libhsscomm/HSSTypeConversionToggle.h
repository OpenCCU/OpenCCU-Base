/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionScript.h: Schnittstelle für die Klasse HSSTypeConversionScript.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSTYPECONVERSIONTOGGLE_H_
#define _HSSTYPECONVERSIONTOGGLE_H_

#include "dllexport.h"


#include "HSSTypeConversion.h"
#include "HSSIntegerValueMap.h"
#include <XmlRpc.h>

//! Umschalten des Parameterwertes zwischen zwei Zuständen.
/* Prüft, ob der physische Wert off entspricht. Falls ja, wird on zurückgegeben,
 * ansonsten off.
 */
class DLLEXPORT HSSTypeConversionToggle : public HSSTypeConversion  
{
public:
	//! Konvertierung vom physikalischen in den logischen Wert.
	/*! out = (${value} == off) ? on : off;
	 */
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);
	
	//! Konvertierung vom logischen in den physikalischen Wert.
	/*! nicht definiert
	 */	
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out);
	
	//! Initialisierung aus einer XML-Datei
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("type_conversion_script") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	
	//! Konstruktor
	HSSTypeConversionToggle();
	//! Destruktor
	virtual ~HSSTypeConversionToggle();
protected:
	//! Bezeichnung des verknüpften physikalischen Parameters.
	std::string m_valueId;
	//! Logischer Wert des Zustandes "Ein"
	int m_on;
	//! Logischer Wert des Zustandes "Aus"; Prüfwert des physikalischen Parameter.
	int m_off;
};

#endif // _HSSTYPECONVERSIONTOGGLE_H_

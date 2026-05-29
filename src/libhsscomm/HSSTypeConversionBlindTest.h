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

#ifndef _HSSTYPECONVERSIONBLINDTEST_H_
#define _HSSTYPECONVERSIONBLINDTEST_H_

#include "dllexport.h"


#include "HSSTypeConversion.h"
#include "HSSIntegerValueMap.h"
#include <XmlRpc.h>

//! Konvertierung für den Gerätetest von Jalousieaktoren
class DLLEXPORT HSSTypeConversionBlindTest : public HSSTypeConversion  
{
public:
	//! Konvertierung vom physikalischen in den logischen Wert
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);
	
	//! Konvertierung vom logischen in physikalischen Wert
	/*! Wird nicht verwendet
	 */
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out);
	
	//! Initialisierung aus einer XML-Datei
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("type_conversion_script") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	
	//! Konstruktor
	HSSTypeConversionBlindTest();
	//! Destruktor
	virtual ~HSSTypeConversionBlindTest();
protected:
	int m_value;
};

#endif // _HSSTYPECONVERSIONBLINDTEST_H_

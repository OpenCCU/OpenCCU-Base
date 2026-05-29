/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionIntegerIntegerMap.h: Schnittstelle für die Klasse HSSTypeConversionIntegerIntegerMap.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSTypeConversionIntegerIntegerMap_H_
#define _HSSTypeConversionIntegerIntegerMap_H_

#include "dllexport.h"


#include "HSSTypeConversion.h"
#include "HSSIntegerValueMap.h"
#include <XmlRpc.h>

//! Assoziative Konvertierung zwischen Integer als logischem Datentyp und Integer als physikalischem Datentyp
/*! Bedient sich der Klasse HSSIntegerValueMap für die Konvertierung
 */
class DLLEXPORT HSSTypeConversionIntegerIntegerMap : public HSSTypeConversion  
{
public:
	//! Physikalischen Wert durch Aufruf von HSSIntegerValueMap::MapFromDevice() konvertieren
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);
	//! Logischen Wert durch Aufruf von HSSIntegerValueMap::MapToDevice() konvertieren
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out);
	//! Initialisierung aus XML-Datei
	/*! Ruft HSSIntegerValueMap::InitFromXml() auf
	 */
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("type_conversion_integer_integer_map") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	//! Konstruktor
	HSSTypeConversionIntegerIntegerMap();
	//! Destruktor
	virtual ~HSSTypeConversionIntegerIntegerMap();
protected:
	//! Objekt, das das Mapping durchführt
	HSSIntegerValueMap value_map;
};

#endif // _HSSTypeConversionIntegerIntegerMap_H_

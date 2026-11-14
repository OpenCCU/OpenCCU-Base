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

#ifndef _HSSTYPECONVERSIONRC19DISPLAY_H_
#define _HSSTYPECONVERSIONRC19DISPLAY_H_

#include "dllexport.h"


#include "HSSTypeConversion.h"
#include "HSSIntegerValueMap.h"
#include <XmlRpc.h>

//! Konvertierung für das Display der 19-Tasten-Fernbedienung HM-RC-19.
/*! Bei der Konvertierung werden folgende Aktionen durchgeführt:
 *    - Ermitteln, ob das Komma gesetzt werden muss (COMMA setzen) 
 *    - Texte, die Komma enthalten, am Komma ausrichten (rechtsbündig)
 *    - Komma aus Text entfernen
 */
class DLLEXPORT HSSTypeConversionRc19Display : public HSSTypeConversion  
{
public:
	//! Wird nicht verwendet
	/*! \c out = \c in
	 */
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);

	//! Text für die Anzeige konvertieren.
	/*! Bei der Konvertierung werden folgende Aktionen durchgeführt:
	 *    - Ermitteln, ob das Komma gesetzt werden muss (COMMA setzen) 
	 *    - Texte, die Komma enthalten, am Komma ausrichten (rechtsbündig)
	 *    - Komma aus Text entfernen
	 */
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out);
	
	//! Initialisierung aus einer XML-Datei
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("type_conversion_script") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	
	//! Konstruktor
	HSSTypeConversionRc19Display();
	//! Destruktor
	virtual ~HSSTypeConversionRc19Display();
};

#endif // _HSSTYPECONVERSIONRC19DISPLAY_H_

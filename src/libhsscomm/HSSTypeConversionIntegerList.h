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

#ifndef _HSSTYPECONVERSIONINTEGERLIST_H_
#define _HSSTYPECONVERSIONINTEGERLIST_H_

#include "dllexport.h"


#include "HSSTypeConversion.h"
#include "HSSIntegerValueMap.h"
#include <XmlRpc.h>

//! Konvertierung für Integer-Listen.
/*! z.B. für MP3 Funkgong
 */
class DLLEXPORT HSSTypeConversionIntegerList : public HSSTypeConversion  
{
public:
	//! Wird nicht verwendet
	/*! \c out = \c in
	 */
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);

	//! Kovertierung.
	/*! String "1,2,3,.." --> 0x01 0x02 x03
	 */
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out);
	
	//! Initialisierung aus einer XML-Datei
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("type_conversion_script") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	
	//! Konstruktor
	HSSTypeConversionIntegerList();
	//! Destruktor
	virtual ~HSSTypeConversionIntegerList();
protected:
	int size;
};

#endif // _HSSTYPECONVERSIONRC19DISPLAY_H_

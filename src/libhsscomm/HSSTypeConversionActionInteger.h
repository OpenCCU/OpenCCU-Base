/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionActionInteger.h: Schnittstelle für die Klasse HSSTypeConversionActionInteger.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSTYPECONVERSIONACTIONINTEGER_H_
#define _HSSTYPECONVERSIONACTIONINTEGER_H_

#include "dllexport.h"


#include "HSSTypeConversion.h"
#include <XmlRpc.h>

//! Konvertierung zwischen Boolean (ACTION) als logischem Datentyp und Integer als physikalischem Datentyp
/*! Der Typ \c ACTION an der XmlRpc-Schnittstelle wird auf XmlRpc-Ebene als Boolean behandelt.
 *  Da es bei \c ACTION nur um das Ereignis geht und nicht um den Wert, wird unabhängig vom Eingabewert
 *  konvertiert.
 */
class DLLEXPORT HSSTypeConversionActionInteger : public HSSTypeConversion  
{
public:
	//! Konvertiert jeden Wert nach \c false
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);
	//! Konvertiert \c 0 nach \c false und alles andere nach \c true
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out);
	//! Initialisierung aus einer XML-Datei
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("type_conversion_action_integer") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	//! Konstruktor
	HSSTypeConversionActionInteger();
	//! Destruktor
	virtual ~HSSTypeConversionActionInteger();
protected:
};

#endif // _HSSTYPECONVERSIONACTIONINTEGER_H_

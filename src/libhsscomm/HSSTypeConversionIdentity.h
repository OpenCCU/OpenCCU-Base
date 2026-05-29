/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionIdentity.h: Schnittstelle für die Klasse HSSTypeConversionIdentity.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSTYPECONVERSIONIDENTITY_H_
#define _HSSTYPECONVERSIONIDENTITY_H_

#include "dllexport.h"


#include "HSSTypeConversion.h"
#include <XmlRpc.h>

//! Diese Konvertierungsklasse konvertiert jeden Wert auf sich selbst
/*! HSSParameter benötigt immer mindestens ein Konvertierungsobjekt, selbst wenn nichts zu tun ist.
 *  In diesem Fall wird von HSSParameter ein Objekt dieser Klasse erzeugt.
 */
class DLLEXPORT HSSTypeConversionIdentity : public HSSTypeConversion  
{
public:
	//! Reicht den Eingabewert unverändert durch
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);
	//! Reicht den Eingabewert unverändert durch
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out);
	//! Diese Klasse ist nicht parametrierbar, also passiert hier nichts
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("type_conversion_&lt;type&gt;_&lt;type&gt;"),
	 *  also mit gleichem logischen und physikalischen Typ, erzeugen.
	 */
	static bool CheckCreationTag(const char* tag);
	//! Konstruktor
	HSSTypeConversionIdentity();
	//! Destruktor
	virtual ~HSSTypeConversionIdentity();
};

#endif // _HSSTYPECONVERSIONIDENTITY_H_

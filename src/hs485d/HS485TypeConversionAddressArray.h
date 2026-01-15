/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485TypeConversionAddressArray.h: Schnittstelle für die Klasse HS485TypeConversionAddressArray.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HS485TYPECONVERSIONADDRESSARRAY_H__99234C71_B989_49C6_9E7F_68B1E91CDF54__INCLUDED_)
#define AFX_HS485TYPECONVERSIONADDRESSARRAY_H__99234C71_B989_49C6_9E7F_68B1E91CDF54__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <HSSTypeConversion.h>
#include <XmlRpc.h>

//! Konvertierung zwischen String (ADDRESS) als logischem Datentyp und einem Array von zwei Integern 
/*! Der Typ \c ADDRESS an der XmlRpc-Schnittstelle ist eine Geräte- oder
 *  Kanalseriennummer.
 *
 *  Der physikalische Typ ist ein Array von zwei Integern. Der erste Integer ist die Busadresse und der
 *  zweite die Kanalnummer oder \c -1 bei einem Gerät.
 */
class HS485TypeConversionAddressArray : public HSSTypeConversion  
{
public:
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out);
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch dynamic::create("type_conversion_address_array") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	HS485TypeConversionAddressArray();
	virtual ~HS485TypeConversionAddressArray();
};

#endif // !defined(AFX_HS485TYPECONVERSIONADDRESSARRAY_H__99234C71_B989_49C6_9E7F_68B1E91CDF54__INCLUDED_)

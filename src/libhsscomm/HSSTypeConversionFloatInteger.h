/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionFloatInteger.h: Schnittstelle für die Klasse HSSTypeConversionFloatInteger.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSTYPECONVERSIONFLOATINTEGER_H_
#define _HSSTYPECONVERSIONFLOATINTEGER_H_

#include "dllexport.h"


#include "HSSTypeConversion.h"
#include <XmlRpc.h>

    
//! Konvertierung zwischen Float als logischem Datentyp und Integer als physikalischem Datentyp
/*! Konvertiert zwischen Float und Integer mit optionalem Faktor und Offset. Rundet bei der
 *  Konvertierung kaufmännisch.
 */
class DLLEXPORT HSSTypeConversionFloatInteger : public HSSTypeConversion  
{
public:
	//! Konvertierung von Integer nach Float
	/*! out=(in/factor)-offset;
	 */
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);
	//! Konvertierung von Float nach Integer
	/*! out=(in+offset)*factor;
	 */
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out);
	//! Initialisierung aus einer XML-Datei
	/*! Folgende Attribute werden gelesen:
	 *  - \c factor -> optional, Float. Faktor für die Konvertierung. Default ist \c 1.0
	 *  - \c offset -> optional, Float. Offset für die Konvertierung. Default ist \c 0.0
	 */
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("type_conversion_float_integer") oder
	 *  hsscomm::type_registry::create("type_conversion_float_integer_scale") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	//! Konstruktor
	HSSTypeConversionFloatInteger();
	//! Destruktor
	virtual ~HSSTypeConversionFloatInteger();

protected:
	//! Konvertierungsfaktor
    double factor;
	//! Offset für Konvertierung
	double offset;
};

#endif // _HSSTYPECONVERSIONFLOATINTEGER_H_

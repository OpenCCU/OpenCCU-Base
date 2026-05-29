/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionIntegerIntegerScale.h: Schnittstelle für die Klasse HSSTypeConversionIntegerIntegerScale.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSTYPECONVERSIONBOOLEANINTEGER_H_
#define _HSSTYPECONVERSIONBOOLEANINTEGER_H_

#include "dllexport.h"


#include "HSSTypeConversion.h"
#include <XmlRpc.h>

//! Skalierende Konvertierung zwischen Integer als logischem Datentyp und Integer als physikalischem Datentyp
/*! Konvertiert zwischen Integer und Integer durch Multiplikation mit einer rationalen Zahl und Addition
 *  eines ganzzahligen Offsets
 */
class DLLEXPORT HSSTypeConversionIntegerIntegerScale : public HSSTypeConversion  
{
public:
	//! Konvertierung vom physikalischen zum logischen Wert
	/*! out=((in-offset)*div)/mul;
	 */
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);
	//! Konvertierung vom logischen zum physikalischen Wert
	/*! out=((in*mul)/div)+offset);
	 */
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out);
	//! Initialisierung aus einer XML-Datei
	/*! Folgende Attribute werden gelesen:
	 *  - \c mul -> optional, Integer, Zähler der Rationalzahl für die Skalierung, Default ist 1
	 *  - \c div -> optional, Integer, Nenner der Rationalzahl für die Skalierung, Default ist 1
	 *  - \c offset -> optional, Integer, Wert für die konstante Verschiebung, Default ist 0
	 */
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("type_conversion_integer_integer_scale") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	//! Konstruktor
	HSSTypeConversionIntegerIntegerScale();
	//! Destruktor
	virtual ~HSSTypeConversionIntegerIntegerScale();
protected:
	//! Zähler der Rationalzahl für die Skalierung
	int mul;
	//! Nenner der Rationalzahl für die Skalierung
	int div;
	//! Wert für die konstante Verschiebung
	int offset;

};

#endif // _HSSTYPECONVERSIONBOOLEANINTEGER_H_

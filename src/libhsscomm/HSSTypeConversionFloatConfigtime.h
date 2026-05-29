/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionFloatConfigtime.h: Schnittstelle für die Klasse HSSTypeConversionFloatConfigtime.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSTYPECONVERSIONFLOATINTEGER_H_
#define _HSSTYPECONVERSIONFLOATINTEGER_H_

#include "dllexport.h"


#include "HSSTypeConversion.h"
#include <XmlRpc.h>
#include <vector>

//class HSSLogicalTypeFloat;
//class HSSPhysicalTypeInteger;

//! Konvertierung zwischen Float als logischem Datentyp und Integer (Zeitangabe) als physikalischem Datentyp
/*! Setzt die von den BidCoS-Geräten verwendete Zeitdarstellung um in eine Float-Darstellung in Sekunden
 *  und umgekehrt. Die BidCoS-Zeitdarstellung verwendet einen Integer, von dem eine variable Anzahl von Bits
 *  einen Faktor angeben und die restlichen Bits einen Multiplikator für den Faktor. Siehe 
 *  BidCoS-Protokollspezifikation. Diese Klasse wird parametriert über eine Liste von Faktoren und die 
 *  Größe in Bits des Multiplikators.
 *  Der Multiplikator liegt immer in den LSB.
 */
class DLLEXPORT HSSTypeConversionFloatConfigtime : public HSSTypeConversion  
{
public:
	//! BidCoS-Zeitangabe in einen Float konvertieren
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);
	//! Float in BidCoS-Zeitangabe konvertieren
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out);
	//! Initialisierung aus einer XML-Datei
	/*! Folgende Attribute werden gelesen:
	 *  - \c factors -> optional, komma-getrennte Liste von Float-Werten. Default ist \c "0.1, 1.0, 5.0, 10.0, 60.0, 300.0, 600.0, 3600.0"
	 *  - \c value_size -> optional, Größe in \c "Bits" oder \c "Bytes.Bits" des Multiplikators. Default ist 5 Bits.
	 */
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("type_conversion_float_configtime") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	//! Kontruktor
	HSSTypeConversionFloatConfigtime();
	//! Destruktor
	virtual ~HSSTypeConversionFloatConfigtime();

protected:
	//! Vektor für die Multiplikationsfaktoren
	std::vector<double> factors;
	//! Größe in Bits des Multiplikators
	int value_size;
};

#endif // _HSSTYPECONVERSIONFLOATINTEGER_H_

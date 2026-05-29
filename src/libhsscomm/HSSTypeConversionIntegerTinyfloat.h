/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionIntegerTinyfloat.h: Schnittstelle für die Klasse HSSTypeConversionIntegerTinyfloat.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSTypeConversionIntegerTinyfloat_H_
#define _HSSTypeConversionIntegerTinyfloat_H_

#include "dllexport.h"


#include "HSSTypeConversion.h"
#include "HSSIntegerValueMap.h"
#include <XmlRpc.h>

//! Konvertierung zwischen Integer als logischem Datentyp und Integer (Tinyfloat) als physikalischem Datentyp
/*! Ein Tinyfloat ist eine Fließkommazahl mit relativ wenigen Bits. Diese Zahlen werden in der Kommunikation
 *  mit HomeMatic-Geräten verwendet, wenn ein großer Wertebereich übertragen werden soll, aber mit zunehmender
 *  Größe der Werte auf Genauigkeit verzichtet werden kann.
 *  Parametriert wird die Klasse mit den Größen in Bits der Mantisse und des Exponenten sowie der Größe
 *  der Mantisse und des Exponenten im Integer auf der physikalischen Seite
 */
class DLLEXPORT HSSTypeConversionIntegerTinyfloat : public HSSTypeConversion  
{
public:
	//! Konvertiert den physikalischen Tinyfloat in einen Integer
	/*! Extrahiert die Mantisse und den Exponenten und rechnet dann:
	 *  *out=Mantisse * (2 ^ Exponent)
	 */
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);
	//! Konvertiert den logischen Integer in den genauestmöglichen physikalischen Tinyfloat
	/*! Berechnet den kleinstmöglichen Exponenten, mit dem sich der logische Wert darstellen läßt.
	 *  Berechnet die Mantisse zu in / ( 2 ^ Exponent)
	 *  Packt beide Werte durch Bitoperationen zusammen in einen Integer.
	 */
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out);
	//! Initialisierung aus einer XML-Datei
	/*! Folgende Attribute werden gelesen:
	 *  - \c mantissa_start -> optional, Position des Startbits auf der physikalischen Seite für die Mantisse, Defaultwert ist 0
	 *  - \c mantissa_size -> Größe in Bits der Mantisse auf der physikalischen Seite
	 *  - \c exponent_start -> optional, Position des Startbits auf der physikalischen Seite für den Exponenten, Defaultwert ist \c mantissa_start+mantissa_size
	 *  - \c exponent_size -> Größe in Bits des Exponenten auf der physikalischen Seite
	 */
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("type_conversion_integer_tinyfloat") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	//! Konstruktor
	HSSTypeConversionIntegerTinyfloat();
	//! Destruktor
	virtual ~HSSTypeConversionIntegerTinyfloat();
protected:
	//! Größe in Bits des Exponenten auf der physikalischen Seite
	int exponent_size;
	//! Größe in Bits der Mantisse auf der physikalischen Seite
	int mantissa_size;
	//! Position des Startbits auf der physikalischen Seite für den Exponenten
	int exponent_start;
	//! Position des Startbits auf der physikalischen Seite für die Mantisse
	int mantissa_start;
};

#endif // _HSSTypeConversionIntegerTinyfloat_H_

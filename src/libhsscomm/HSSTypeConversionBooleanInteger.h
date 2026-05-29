/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionBooleanInteger.h: Schnittstelle für die Klasse HSSTypeConversionBooleanInteger.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSTYPECONVERSIONBOOLEANINTEGER_H_
#define _HSSTYPECONVERSIONBOOLEANINTEGER_H_

#include "dllexport.h"


#include "HSSTypeConversion.h"
#include <XmlRpc.h>

//! Konvertierung zwischen Boolean als logischem Datentyp und Integer als physikalischem Datentyp
/*! Die Konvertierung wird über Attribute aus der XML-Datei parametriert. Siehe InitFromXml().
 */
class DLLEXPORT HSSTypeConversionBooleanInteger : public HSSTypeConversion  
{
public:
	//! Konvertiert einen Integer in \c in in einen Boolean in \c *out
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);
	//! Konvertiert einen Boolean in \c in in einen Integer in \c *out
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out);
	//! Initialisierung aus einer XML-Datei
	/*! Folgende Attribute werden gelesen:
	 *  - \c range -> optional, \c "Untergrenze-Obergrenze". Integer-Werte von Untergrenze bis einschließlich 
	 *    Obergrenze werden zu \c true konvertiert, alle anderen zu \c false. Wird als Untergrenze bzw. Obergrenze
	 *    ein Leerstring angegeben, so gelten INT_MIN bzw. INT_MAX. Als Default gelten 1 und INT_MAX.
	 *  - \c threshold -> optional, alias für \c range
	 *  - \c true -> optional, Integer-Wert für die konvertierung des boolschen Wertes \c true. Default ist \c "1".
	 *  - \c false -> optional, Integer-Wert für die konvertierung des boolschen Wertes \c false. Default ist \c "0".
	 *  - \c invert -> optional, bei \c "true" wird der boolsche Wert bei der Konvertierung invertiert. Default ist \c "false"
	 */
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("type_conversion_boolean_integer") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	//! Konstruktor
	HSSTypeConversionBooleanInteger();
	//! Destruktor
	virtual ~HSSTypeConversionBooleanInteger();
protected:
	//! Inclusiv-Untergrenze für als \c true interpretierte Integer-Werte
	int lower_threshold;
	//! Inclusiv-Obergrenze für als \c true interpretierte Integer-Werte
	int upper_threshold;
	//! Flag für Verwendung umgekehrter Logik
	bool invert;
	//! Integer-Wert für Konvertierung von \c false
	int false_val;
	//! Integer-Wert für Konvertierung von \c true
	int true_val;

};

#endif // _HSSTYPECONVERSIONBOOLEANINTEGER_H_

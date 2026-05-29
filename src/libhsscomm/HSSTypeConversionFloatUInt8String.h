/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionFloatUInt8String.h: Schnittstelle f�r die Klasse HSSTypeConversionFloatUInt8String.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSTYPECONVERSIONFLOATUINT8STRING_H_
#define _HSSTYPECONVERSIONFLOATUINT8STRING_H_

#include "dllexport.h"


#include "HSSTypeConversion.h"
#include <XmlRpc.h>

    
//! Konvertierung zwischen Float als logischem Datentyp und einem vorzeichenlosen Integer (mit maximal 64 Bit) als physikalischem Datentyp (verpackt als byte array in einem string)
/*! Konvertiert zwischen Float und einem vorzeichenlosen Integer, dessen bytes im Big Endian Format in einem Byte Array (als Container ein string) abgelegt werden.
 *  Die Konvertierung erfolgt mit optionalem Faktor und Offset. 
 *  Bei der Konvertierung wird kaufmännisch gerundet.
 */
class DLLEXPORT HSSTypeConversionFloatUInt8String : public HSSTypeConversion
{
public:

	typedef uint64_t uint8;
	

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
	 *  - \c factor -> optional, Float. Faktor f�r die Konvertierung. Default ist \c 1.0
	 *  - \c offset -> optional, Float. Offset f�r die Konvertierung. Default ist \c 0.0
	 */
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Hilfsmethode f�r die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("type_conversion_float_integer") oder
	 *  hsscomm::type_registry::create("type_conversion_float_integer_scale") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	//! Konstruktor
	HSSTypeConversionFloatUInt8String();
	//! Destruktor
	virtual ~HSSTypeConversionFloatUInt8String();

protected:
	//! Konvertierungsfaktor
    double factor;
	//! Offset f�r Konvertierung
	double offset;
	unsigned int lengthBytes;
	//! Number of unused bits (8-lengthBytes)
	int preShiftBits;
};

#endif // _HSSTYPECONVERSIONFLOATUINT8_H_

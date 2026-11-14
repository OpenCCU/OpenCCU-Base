/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HSS_TYPE_CONVERSION_SAME_KEY_COUNTER_H_
#define _HSS_TYPE_CONVERSION_SAME_KEY_COUNTER_H_

#include "dllexport.h"

#include "HSSTypeConversion.h"
#include <XmlRpc.h>


class DLLEXPORT HSSTypeConversionSameKeyCounter : public HSSTypeConversion  
{
public:
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out);
	//! Initialisierung aus einer XML-Datei
	/*! Folgende Attribute werden gelesen:
	 *  - \c sim_counter -> optional, wird in \c sim_counter_id gespeichert
	 *  - \c counter_size -> optional, dient zur Berechnung von \c counter_mask und \c sim_counter_offset
	 */
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create(tag) erzeugen. Es wird jeder Wert für \c tag
	 *  akzeptiert, der mit \c "type_conversion_" beginnt und mit \c "_key_counter" endet.
	 */
	static bool CheckCreationTag(const char* tag);
	//! Konstruktor
	HSSTypeConversionSameKeyCounter(void);
	//! Destruktor
	virtual ~HSSTypeConversionSameKeyCounter(void);
protected:
	//! Id für die Speicherung des simulierten Tastendruckzählers am Kanalobjekt
	std::string sim_counter_id;
};
#endif //_RF_TYPE_CONVERSION_SAME_KEY_COUNTER_H_

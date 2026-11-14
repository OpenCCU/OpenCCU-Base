/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSPhysicalTypeBoolean.h: Schnittstelle für die Klasse HSSPhysicalTypeBoolean.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSPHYSICALTYPEBOOLEAN_H_
#define _HSSPHYSICALTYPEBOOLEAN_H_

#include "dllexport.h"


#include "HSSPhysicalType.h"

//! Physikalischer Typ für Boolsche Werte
/*! Wird verwendet zum Zugriff auf interne Werte (siehe HSSPhysicalDataInterfaceInternal), 
 *  insbesondere den internen Wert "AES".
 *  Diese Klasse implementiert kein über HSSPhysicalType hinausgehendes Verhalten. Sie dient nur
 *  der dynamischen Erzeugung für \c &ltphysical \c type="boolean" in der XML-Datei
 */
class DLLEXPORT HSSPhysicalTypeBoolean : public HSSPhysicalType  
{
public:
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("physical_type_boolean") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	//! Konstruktor
	HSSPhysicalTypeBoolean();
	//! Destruktor
	virtual ~HSSPhysicalTypeBoolean();
protected:
};

#endif // _HSSPHYSICALTYPEBOOLEAN_H_

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSPhysicalTypeInteger.h: Schnittstelle für die Klasse HSSPhysicalTypeInteger.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSPHYSICALTYPEINTEGER_H_
#define _HSSPHYSICALTYPEINTEGER_H_

#include "dllexport.h"


#include "HSSPhysicalType.h"

//! Physikalischer Typ für Ganzzahlige Werte
/*! Wird verwendet zum Zugriff auf fast alle Werte und Parameter der Sensoren und Aktoren
 *  Diese Klasse implementiert kein über HSSPhysicalType hinausgehendes Verhalten. Sie dient nur
 *  der dynamischen Erzeugung für \c &ltphysical \c type="integer" in der XML-Datei
 */
class DLLEXPORT HSSPhysicalTypeInteger : public HSSPhysicalType  
{
public:
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("physical_type_integer") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	//! Konstruktor
	HSSPhysicalTypeInteger();
	//! Destruktor
	virtual ~HSSPhysicalTypeInteger();
protected:
};

#endif // _HSSPHYSICALTYPEINTEGER_H_

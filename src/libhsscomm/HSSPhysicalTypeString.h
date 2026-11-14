/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSPhysicalTypeString.h: Schnittstelle für die Klasse HSSPhysicalTypeString.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSPHYSICALTYPESTRING_H_
#define _HSSPHYSICALTYPESTRING_H_

#include "dllexport.h"


#include "HSSPhysicalType.h"

//! Physikalischer Typ für Zeichenketten
/*! Wird verwendet für den zentraleninternen Parameter "UI_HINT" in den Geräteprofilen
 *  Diese Klasse implementiert kein über HSSPhysicalType hinausgehendes Verhalten. Sie dient nur
 *  der dynamischen Erzeugung für \c &ltphysical \c type="integer" in der XML-Datei
 */

class DLLEXPORT HSSPhysicalTypeString : public HSSPhysicalType  
{
public:
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("physical_type_string") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);
	//! Konstruktor
	HSSPhysicalTypeString();
	//! Destruktor
	virtual ~HSSPhysicalTypeString();
protected:
};

#endif // _HSSPHYSICALTYPESTRING_H_

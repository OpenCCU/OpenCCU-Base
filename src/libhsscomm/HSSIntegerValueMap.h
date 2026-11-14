/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSIntegerValueMap.h: Schnittstelle für die Klasse HSSIntegerValueMap.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSINTEGERVALUEMAP_H_
#define _HSSINTEGERVALUEMAP_H_

#include "dllexport.h"


#include <map>
#include <vector>
#include "xmlParser.h"

//! Hilfsklasse für die Umwandlung zwischen physikalischem Datentyp und logischem Datentyp in Form eines Mappings
/*! Unterstützt folgende Arten des Mappings, auch gemischt:
 *  1 Eins-zu-Eins-Zuordnung: einem logischen Wert entspricht genau ein physikalischer Wert in beide Richtungen
 *  2 Eins-zu-Eins-Zuordnung mit bitweiser Maskierung auf der physikalischen Seite: Wie zuvor, jedoch kann angegeben
 *    werden, dass für die Richtung physikalisch -> logisch bestimmte Bits auf der physikalischen Seite nicht beachtet
 *    werden.
 *  3 Richtungsabhängige Eins-zu-Eins-Zuordnung: wie 1), aber Zuordnung für beide Richtungen getrennt
 *  4 Defaultwert für nicht-zugeordnete Werte
 */
class DLLEXPORT HSSIntegerValueMap  
{
public:
	//! Einen vom physikalischen Gerät kommenden Wert abbilden
	int MapFromDevice(int val);
	//! Einen für das physikalische Gerät bestimmten Wert abbilden
	int MapToDevice(int val);
	//! Abbildungsregeln aus einer XML-Datei einlesen
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Konstruktor
	HSSIntegerValueMap();
	//! Destruktor
	virtual ~HSSIntegerValueMap();
protected:
	//! Typedef für eine maskierbare Abbildungsregel
	/*! Über \c mask lassen sich für die Abbildung vom Gerät kommender Daten irrelevante Bits ausblenden
	 */
	typedef struct{
		int mask; //!< Gibt an, welche Bits des vom Gerät kommenden Wertes in MapFromDevice() berücksichtigt werden
		int dev_value; //!< Wert, den das Gerät versteht (physikalischer Wert)
		int param_value; //!< Wert, mit dem die restliche Software arbeitet (logischer Wert)
	} masked_value_t;
	//! Typedef für nicht-maskierbare Abbildungen
    typedef std::map<int, int> map_t;
	//! Nicht-maskierbare Abbildungen in Richtung zum Gerät
    map_t to_device_map;
	//! Nicht-maskierbare Abbildungen in Richtung vom Gerät
    map_t from_device_map;
	//! Typedef für maskierbare Abbildungen in Richtung vom Gerät
	typedef std::vector<masked_value_t> masked_vec_t;
	//! Maskierbare Abbildungen in Richtung vom Gerät
	masked_vec_t masked_vec;
};

#endif // _HSSINTEGERVALUEMAP_H_

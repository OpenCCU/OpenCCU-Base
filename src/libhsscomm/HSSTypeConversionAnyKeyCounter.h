/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HSS_TYPE_CONVERSION_ANY_KEY_COUNTER_H_
#define _HSS_TYPE_CONVERSION_ANY_KEY_COUNTER_H_

#include "dllexport.h"

#include "HSSTypeConversion.h"
#include <XmlRpc.h>

//! Konvertierung zwischen beliebigem logischen Datentyp und Integer als physikalischem Datentyp
/*! Diese Klasse führt keine tatsächliche Konvertierung durch, sondern verwaltet Tastendruckzähler.
 *  Auf der physikalischen Seite wird der empfangene Tastendruckzähler erwartet bzw. ein sinnvoller Zählerwert
 *  für einen simulierten Tastendruck generiert.
 *  Der letzte empfangene Tastendruckzähler wird am Kanalobjekt im stored value "_COUNTER" gespeichert.
 *  Der Zähler für simulierte Tastendrücke wird am Kanalobjekt als stored value unter \c sim_counter_id
 *  gespeichert.
 */
class DLLEXPORT HSSTypeConversionAnyKeyCounter : public HSSTypeConversion  
{
public:
	//! Überprüfung eines vom Gerät empfangenen Tastendruckzählers
	/*! Erwartet den von Gerät empfangenen Tastendruckzähler als Integer in \c in.
	 *  Prüft diesen gegen den am Kanalobjekt \c inst unter "_COUNTER" gespeicherten Wert. Gibt
	 *  \c false zurück, wenn dieser Tastendruckzähler dem zuletzt empfangenen entspricht, womit
	 *  die weitere Verarbeitung der eingehenden Nachricht unterbunden wird.
	 *  Speichert den aktuellen Tastendruckzähler wieder unter "_COUNTER" in \c inst.
	 *  Falls \c sim_counter_id gesetzt ist, wird der empfangene Tastendruckzähler plus dem halben
	 *  Wertebereich des Zählers (zur Kollisionsvermeidung) unter dieser Id zusätzlich gespeichert.
	 *  Bei \c event=false wird einfach \c true zurückgegeben und nichts weiter gemacht. Das pasiert, wenn
	 *  der logische Wert für einen Tastendruck über XmlRpc abgefragt wird, was nicht vorkommen sollte.
	 *
	 *  \see LogicalInstance::GetStoredValue()
	 *  \see LogicalInstance::SetStoredValue()
	 */
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);
	//! Generierung eines Zählers für einen simulierten Tastendruck
	/*! Ein von der Zentrale gesendeter simulierter Tastendruck benötigt einen Tastendruckzähler, der
	 *  nicht mit dem tatsächlichen Zähler des simulierten Kanals kollidieren darf. Diese Methode versucht
	 *  diese Kollision zu vermeiden.
	 *  Der Zähler für den simulierten Tastendruck wird nicht über \c out zurückgegeben, sondern am
	 *  Kanalobjekt \c inst mit der Id \c sim_counter_id gespeichert.
	 *  Von dieser Methode wird der unter \c sim_counter_id gespeicherte Wert unter Berücksichtigung des Wertebereichs
	 *  (also mit Überlauf) inkrementiert.
	 *  
	 *  \see LogicalInstance::GetStoredValue()
	 *  \see LogicalInstance::SetStoredValue()
	 */
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
	HSSTypeConversionAnyKeyCounter(void);
	//! Destruktor
	virtual ~HSSTypeConversionAnyKeyCounter(void);
protected:
	//! Id für die Speicherung des simulierten Tastendruckzählers am Kanalobjekt
	std::string sim_counter_id;
	//! Maske der gültigen Bits des Zählers. Wir für Modulo-Inkrementierung verwendet
	unsigned int counter_mask;
	//! Offset zwischen empfangenem und simuliertem Zähler. Halber Wertebereich des Zählers.
	unsigned int sim_counter_offset;
};
#endif //_RF_TYPE_CONVERSION_ANY_KEY_COUNTER_H_

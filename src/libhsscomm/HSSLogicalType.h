/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSLogicalType.h: Schnittstelle für die Klasse HSSLogicalType.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSLOGICALTYPE_H_
#define _HSSLOGICALTYPE_H_

#include "dllexport.h"


#include <string>
#include "xmlParser.h"
#include <XmlRpc.h>

class  LogicalInstance;

//! Basisklasse für die Beschreibung des logischen Typs eines HSSParameters
/*! Diese Klasse bietet Zugriff auf die Eigenschaften, die jeder logische Typ hat, z.B. Datentyp,  Maßeinheit, 
 *  Min- und Maxwerte.
 *  Entspricht dem Tag &lt;logical&gt; unterhalb von &lt;parameter&gt; in der XML-Datei
 */
class DLLEXPORT HSSLogicalType  
{
public:
	//! Aufzählung für die unterstützten Operationen
	typedef enum{ 
		OP_READ, //!< Parameter kann gelesen werden
		OP_WRITE, //!< Parameter kann geschrieben werden
		OP_EVENT //!< Parameter kann ein Ereignis erzeugen
	} operation_t;
	//! Einlesen aus einer XML-Datei
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Konstruktor
	HSSLogicalType();
	//! Destruktor
	virtual ~HSSLogicalType();
	//! Gibt den Datentyp als String zurück
    const std::string& GetType();
	//! Einhaltung von Randbedingungen für einen Wert erzwingen
	/*! Mit dieser Methode erhalten abgeleitete Klassen die Gelegenheit, den übergebenen Wert
	 *  zu modifizieren oder die weitere Verarbeitung abzubrechen. In einem einfachen Fall wird z.B.
	 *  der übergebene Wert einfach im Wertebereich auf die Minimal- und Maximalgrenze beschränkt.
	 *  In einem komplexeren Fall wird für einen eingehenden Tastendruck geprüft, ob dieser bereits
	 *  verarbeitet wurde oder für einen simulierten Tastendruck die Zählerverwaltung implementiert.
	 *  \param inst Zeiger auf das Gerät oder den Kanal, dem der Parameter zugeordnet ist
	 *  \param val Zeiger auf den zu überprüfenden oder zu modifizierenden Wert
	 *  \param op Operation, die zum Aufruf der Methode geführt hat (Lesen, Schreiben oder Ereignis)
	 *  \return Bei \c false wird die weitere Verarbeitung abgebrochen
	 *  Viele abgeleitete Klassen führen in dieser Methode eine implizite Typkonvertierung durch,
	 *  soweit diese eindeutig und sinnvoll ist.
	 */
    virtual bool EnforceConstraints(LogicalInstance* inst, XmlRpc::XmlRpcValue* val, operation_t op){return true;};
	//! Gibt die Beschreibung für den Parameter in der für die XmlRpc-Schnittstelle benötigten Form zurück
	virtual bool GetDescription(XmlRpc::XmlRpcValue* val);
	//! Gibt den Default-Wert zurück
	virtual XmlRpc::XmlRpcValue GetDefault()=0;
	//! Gibt die Maßeinheit zurück
	virtual const std::string& GetUnit(){return unit;};
	//! Gibt an, ob Fehler beim Lesen des Parameters ignoriert werden und statt dessen der Defaultwert zurückgegeben wird
	bool UseDefaultOnFailure(){return use_default_on_failure;};
protected:
	//! Aus der XML-Datei gelesener Datentyp
    std::string type;
	//! Aus der XML-Datei gelesene Maßeinheit
    std::string unit;
	//! Aus der XML-Datei gelesener Wert des Attributes \c use_default_on_failure
	/*! Gibt an, ob Fehler beim Lesen des Parameters ignoriert werden und statt dessen der Defaultwert zurückgegeben wird
	 */
	bool use_default_on_failure;

};

#endif // _HSSLOGICALTYPE_H_

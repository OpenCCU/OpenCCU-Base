/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSLogicalTypeOption.h: Schnittstelle für die Klasse HSSLogicalTypeOption.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSLOGICALTYPEOPTION_H_
#define _HSSLOGICALTYPEOPTION_H_

#include "dllexport.h"


#include "HSSLogicalType.h"

//! Beschreibt den logischen Typ Optionsliste
/*! Entsprechung in der XML-Datei: &lt;logical type="option"&gt;
 *  Entsprechung in der XmlRpc-Schnittstelle: TYPE=ENUM
 */
class DLLEXPORT HSSLogicalTypeOption : public HSSLogicalType  
{
public:
	//! Gibt eine logische Parameterbeschreibung gemäß XmlRpc-Schnittstelle zurück
	virtual bool GetDescription(XmlRpc::XmlRpcValue* val);
	//! Begrenzt den übergebenen Wert gemäß Anzahl der Optionen
	/*! Zusätzlich wird versucht, einen übergebenen String einem Optionsnamen zuzuordnen und 
	 *  in den entsprechenden Integer zu konvertieren.
	 */
	virtual bool EnforceConstraints(LogicalInstance* inst, XmlRpc::XmlRpcValue* val, operation_t op);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("logical_type_option") erzeugen
	 */
    static bool CheckCreationTag(const char* tag);
	//! Liest die Beschreibung dieses logischen Typs aus einer XML-Datei ein
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Konstruktor
	HSSLogicalTypeOption();
	//! Destruktor
	virtual ~HSSLogicalTypeOption();
	//! Gibt den in der XML-Datei angegebenen Defaultwert zurück
	XmlRpc::XmlRpcValue GetDefault();
protected:
	//! Defaultwert aus der XML-Datei
    unsigned int default_option;
	//! Typedef für die Liste der Optionsnamen
    typedef std::vector<std::string> options_t;
	//! Liste der Optionsnamen
    options_t options;
};

#endif // _HSSLOGICALTYPEOPTION_H_

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef __HSS_DESCRIPTION_H_
#define __HSS_DESCRIPTION_H_

#include "dllexport.h"

#include <map>
#include <string>
#include "xmlParser.h"
#include "XmlRpc.h"

//! Hilfsklasse, um aus einer (XML-)Gerätebeschreibungsdatei getypte Name-Werte-Paare in eine Map einzulesen
/*! Wird von verschiedenen anderen Klassen verwendet. Hierüber können beliebige Name-Werte-Paare aus einer
 *  Gerätebeschreibungsdatei eingelesen werden, die dann später an der XmlRpc-Schnittstelle in Form einer
 *  DeviceDescription durchgereicht werden.
 */
class DLLEXPORT HSSDescription
{
public:
	//! Trägt alle enthaltenen Werte in ein XmlRpc-Struct ein
	/*! Diese Methode wird beim Abfragen einer DeviceDescription über die XmlRpc-Schnittstelle aufgerufen und
	 *  sorgt dafür, dass die in dieser Klasse gespeicherten Werte z.B. in einer DeviceDescription auftauchen.
	 */
	bool Describe(XmlRpc::XmlRpcValue* descr);
	//! Liest die Name-Werte-Paare aus einer Gerätebeschreibungsdatei ein
	bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Konstruktor
	HSSDescription(void);
	//! Destruktor
	~HSSDescription(void);
protected:
	//! Typedef der Map für die Datenhaltung
	typedef std::map<std::string, XmlRpc::XmlRpcValue> description_fields_t;
	//! Map für die Datenhaltung
	description_fields_t description_fields;
};
#endif //__HSS_DESCRIPTION_H_

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HSSPHYSICALDATAINTERFACEINTERNAL_H_
#define _HSSPHYSICALDATAINTERFACEINTERNAL_H_

#include "dllexport.h"

#include "CommMessage.h"
#include "LogicalInstance.h"
#include "FrameDescription.h"
#include "HSSPhysicalDataInterface.h"
#include <xmlParser.h>
#include <XmlRpc.h>

class  HSSPhysicalType;

//! Implementiert das Interface HSSPhysicalDataInterface für Interne Werte der Geräte- und Kanalobjekte
/*! Es wird über LogicalInstance::GetInternalValue() sowie LogicalInstance::SetInternalValue() auf die
 *  Werte zugegriffen. Beispiel für diese Methode des Zugriffs sind UNREACH, STICKY_UNREACH, LOWBAT, ...
 */
class DLLEXPORT HSSPhysicalDataInterfaceInternal : public HSSPhysicalDataInterface, LogicalInstance::EventReceiver
{
public:
	//! Konstruktor
	HSSPhysicalDataInterfaceInternal(void);
	virtual bool SetDefaultConfig(LogicalInstance* inst,XmlRpc::XmlRpcValue val);
	//! Destruktor
	virtual ~HSSPhysicalDataInterfaceInternal(void);
	//! Initialisierung aus einer XML-Datei
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Wert mit LogicalInstance::GetInternalValue() lesen
    virtual bool GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param);
	//! Wert mit LogicalInstance::SetInternalValue() schreiben
    virtual bool PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param);
	//! Wird beim Erzeugen eines Geräte- oder Kanalobjekts für alle zugeordneten Parameter aufgerufen
	/*! Das Objekt dieser Klasse registriert sich bei \c inst für Änderungsmitteilungen des zugeordneten
	 *  internen Wertes.
	 */
	virtual bool SetupInstance(LogicalInstance* inst);
	//! Wird von \c inst aufgerufen, wenn sich der zugeordnete interne Wert ändert.
	/*! \param inst zugeordnetes Geräte- oder Kanalobjekt
	 *  \param id Identifier des geänderten Wertes
	 *  \param val Referenz auf den neuen Wert
	 */
	void OnEvent(LogicalInstance* inst, const std::string& id, XmlRpc::XmlRpcValue& val);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("data_interface_internal") erzeugen
	 */
	static bool CheckCreationTag(const char *tag);
protected:
	//! Aus der XML-Datei gelesener Identifier des zugeordneten Wertes
	std::string value_id;
};

#endif

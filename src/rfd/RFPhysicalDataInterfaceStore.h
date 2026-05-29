/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RFPHYSICALDATAINTERFACESTORE_H_
#define _RFPHYSICALDATAINTERFACESTORE_H_

#include "HSSPhysicalDataInterface.h"
#include "FrameDescription.h"

//! Data-Interface für in der Zentrale gespeicherte Werte

class RFPhysicalDataInterfaceStore :
	public HSSPhysicalDataInterface
{
public:
	RFPhysicalDataInterfaceStore(void);
	virtual ~RFPhysicalDataInterfaceStore(void);
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	virtual bool SetDefaultConfig(LogicalInstance* inst,XmlRpc::XmlRpcValue val);
    virtual bool GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param);
    virtual bool PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("data_interface_store") erzeugen
	 */
	static bool CheckCreationTag(const char *tag);
	bool SetupInstance(LogicalInstance* inst);
protected:
	//! Id für das Speichern am Kanalobjekt
	std::string id;
	//! Bei Änderungen an diesem Wert wird das Geräteobjekt gespeichert
	bool save_on_change;
	//! Dieser Wert wird in der .dev-Datei gespeichert
	bool persistent;
	//! Der am Kanalobjekt gespeicherte Wert wird beim Starten nicht auf den Standardwert initialisiert
	bool no_init;
};
#endif

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RFPHYSICALDATAINTERFACECOMMAND_H_
#define _RFPHYSICALDATAINTERFACECOMMAND_H_

#include "HSSPhysicalDataInterface.h"
#include "FrameDescription.h"

//! Data-Interface für das Versenden von BidCoS-RF Konfigurationsdaten

class RFPhysicalDataInterfaceConfig :
	public HSSPhysicalDataInterface
{
public:
	RFPhysicalDataInterfaceConfig(void);
	virtual ~RFPhysicalDataInterfaceConfig(void);
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
    virtual bool GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param);
    virtual bool PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param);
	virtual bool SetDefaultConfig(LogicalInstance* inst,XmlRpc::XmlRpcValue val);
    virtual bool DetermineValue(LogicalInstance* inst);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("data_interface_config") erzeugen
	 */
	static bool CheckCreationTag(const char *tag);
protected:
	//! Parameterliste, in der der Parameter definiert ist
	int list;
	//! Index in Bytes innerhalb der Parameterliste
	int by_index;
	//! Index in Bits innerhalb des Bytes
	int bi_index;
	//! Größe in Bytes
	int by_size;
	//! Gibt die relevanten Bits an
	uint32_t mask;
	//! Größe in Bits zusätzlich zu by_size
	int bi_size;
};
#endif

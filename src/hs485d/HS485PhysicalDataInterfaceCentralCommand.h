/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HS485PHYSICALDATAINTERFACECENTRALCOMMAND_H_
#define _HS485PHYSICALDATAINTERFACECENTRALCOMMAND_H_

#include "HS485PhysicalDataInterfaceCommand.h"
#include "FrameDescription.h"

//! Data-Interface für das Versenden von virtuellen Tastendrücken

class HS485PhysicalDataInterfaceCentralCommand :
	public HS485PhysicalDataInterfaceCommand
{
public:
	HS485PhysicalDataInterfaceCentralCommand(void);
	virtual ~HS485PhysicalDataInterfaceCentralCommand(void);
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
    virtual bool GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param);
    virtual bool PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch dynamic::create("data_interface_central_command") erzeugen
	 */
	static bool CheckCreationTag(const char *tag);

protected:
	//! Id unter der der Tastendruckzähler am Kanalobjekt gespeichert wird
	std::string counter_id;
};
#endif

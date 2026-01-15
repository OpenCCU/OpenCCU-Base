/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HS485PHYSICALDATAINTERFACEEEPROM_H_
#define _HS485PHYSICALDATAINTERFACEEEPROM_H_

#include "HSSPhysicalDataInterface.h"
#include "HS485PhysicalAddress.h"

//! Data-Interface für den Zugriff auf BidCoS-Wired Konfigurationsdaten im Geräte-EEProm

class HS485PhysicalDataInterfaceEEProm :
	public HSSPhysicalDataInterface
{
public:
	HS485PhysicalDataInterfaceEEProm(void);
	virtual ~HS485PhysicalDataInterfaceEEProm(void);
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
    virtual bool GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param);
    virtual bool PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param);
	//! Hilfsmethode für die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch dynamic::create("data_interface_config") erzeugen
	 */
	static bool CheckCreationTag(const char *tag);

	virtual bool SetDefaultConfig(LogicalInstance* inst, XmlRpc::XmlRpcValue val) { return true; }
protected:
	//! Berechnet aus dem Index des gewünschten Parametersets die Adresse im EEProm
	bool CalculateAddress(unsigned int index, unsigned int* byte, unsigned int* bit);
	//! Typedef für Vektor von Adressspezifikationen
    typedef std::vector<HS485PhysicalAddress> addresses_t;
	//! Vektor von Adressspezifikationen
    addresses_t addresses;
	//! Größe in Bytes
	int by_size;
	//! Größe in Bits
	int bi_size;
	//! Bytereihenfolge
	bool little_endian;
	//! Anzahl der zu lesenden Bytes
	int read_size;
};
#endif

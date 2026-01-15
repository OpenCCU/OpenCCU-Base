/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HS485PhysicalAddress.h: Schnittstelle für die Klasse HS485PhysicalAddress.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HS485PHYSICALADDRESS_H__428EEFC3_7C24_4586_8E9D_0E865A7F72FA__INCLUDED_)
#define AFX_HS485PHYSICALADDRESS_H__428EEFC3_7C24_4586_8E9D_0E865A7F72FA__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "xmlParser.h"

//! Beschreibt eine Adresse im Geräte-EEProm, an der ein Konfigurationsdatum gespeichert ist
class HS485PhysicalAddress  
{
public:
	bool CalculateAddress(unsigned int index, unsigned int* byte, unsigned int* bit);
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	HS485PhysicalAddress();
	virtual ~HS485PhysicalAddress();

protected:
	int index_condition;
	int byte_increment;
	int bit_increment;
    unsigned int byte_address;
    unsigned int bit_address;
};

#endif // !defined(AFX_HS485PHYSICALADDRESS_H__428EEFC3_7C24_4586_8E9D_0E865A7F72FA__INCLUDED_)

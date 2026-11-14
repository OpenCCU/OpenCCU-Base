/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// Hexfile.h: Schnittstelle für die Klasse Hexfile.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HEXFILE_H_
#define _HEXFILE_H_

#include "dllexport.h"
#include "typedefs.h"	// Hinzugefügt von der Klassenansicht
#include <string>

class ELVUTILS_DLLEXPORT Hexfile  
{
public:
	const std::string& GetFilename();
	int Modify(int address, const ucVec& data);
	int GetStart();
	ucVec& GetBuffer();
	int Read(const std::string& filename);
	Hexfile();
	virtual ~Hexfile();

protected:
	unsigned char Hexbyte(const char* s);
	int start;
	ucVec buffer;
	std::string filename;
};

#endif

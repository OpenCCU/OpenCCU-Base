/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _UPDATEFILE_H_
#define _UPDATEFILE_H_
#include "dllexport.h"
#include "typedefs.h"	// Hinzugef�gt von der Klassenansicht
#include <string>
#include <vector>
class ELVUTILS_DLLEXPORT UpdateFile
{
private:
	//! Pfad zu Firmwaredatei
	std::string filename;
	//! Ger�te Typen Nummer f�r die diese Firmware geeignet ist 
	int deviceTypeNumber;
	//! Anzhal der Update Rahmen
	int updateFrameCount;
	//! Typedef f�r f�r den Vector in dem die Updaterahmen abgelegt sind
	typedef std::vector<std::string> frameVec;
	std::string emtyFrame;
	//! Vector in dem die Updaterahmen abgelegt sind
	frameVec updateFrames;
	//! umwandlung einer Hexzahl die im Ascci Format dargestellt ist 
	unsigned char Hexbyte(const char* s);
	//! Gibt an, ob die Firmware Datei erfolgreich geladen werden konnte (es wird keine CRC Prüfung durchgeführt)
	bool initialized;

public:
	//! Default Konstrukter
	UpdateFile(void);
	UpdateFile(const std::string &filename, int TypeNumber);
	//! Destruktor
	virtual ~UpdateFile(void);
	//! Liest eine Firmwaredatei ein
	virtual bool Read(const std::string &filenem, int TypeNumber);
	//! Liefert einen Updaterahmen
	virtual const std::string &getUpdateFrame(int frameIndex);
	virtual int getFrameLength(int frameIndex);
	virtual int getUpdateFrameCount();
	//! Gibt an, ob die Firmware Datei korrekt geladen werden konnte (keine CRC Prüfung).
	bool isInitialized()  { return initialized; }

};

#endif //_UPDATEFILE_H_

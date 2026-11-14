/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "UpdateFile.h"
#include "typedefs.h"
#include "Crc16.h"
#include <vector>
#include <fstream>
#include <iostream>
#include "Logger.h"

UpdateFile::UpdateFile(void)
  : deviceTypeNumber(0),
    updateFrameCount(0),
    emtyFrame(""),
    initialized(false)
{
}
UpdateFile::UpdateFile(const std::string &filename, int TypeNumber)
  : deviceTypeNumber(0),
    updateFrameCount(0),
    emtyFrame(""),
    initialized(false)
{
	Read(filename,TypeNumber);
}
UpdateFile::~UpdateFile(void)
{
}
bool UpdateFile::Read(const std::string &filename, int TypeNumber)
{
	std::fstream f;
	//Crc16 crcCheck(0xffff,0x8005);
	int frameLength;
	//int crcInitVal = 0;
	unsigned char ascciByte[5],*frameBuffer = NULL;
	this->updateFrameCount = 0;
	this->deviceTypeNumber = TypeNumber;
	LOG(Logger::LOG_DEBUG,"UpdateFile::Read(): Lade Firmwaredatei %s",filename.c_str());
	f.open(filename.c_str(),std::ios::in);
    
	if(!f.is_open())
	{
		return false;
	}
	this->filename = filename;
//	std::string::size_type dotpos = filename.find_last_of('.');
//	if (dotpos != std::string::npos && filename.substr(dotpos) == ".enc")
//	{
//		crcInitVal = 0;
//	}
//	else if(dotpos != std::string::npos && filename.substr(dotpos) == ".eq3")
//	{
//		crcInitVal = 0xffff;
//	}

	while(!f.eof())
	{
		if(!f.read((char *)ascciByte,4))
		{
			f.close();
			return false;
		}
		frameLength = (Hexbyte((char *)ascciByte) << 8)| (Hexbyte((char*) &ascciByte[2]));
		std::string updateFrame = "";

		std::cout << "Adresse des "<<updateFrameCount <<".: " <<updateFrame <<"\n";	
		updateFrame.append(1,Hexbyte((char *)ascciByte));
		updateFrame.append(1,Hexbyte((char *)&ascciByte[2]));
		
		frameBuffer = new unsigned char[frameLength * 2];

		if(!f.read((char *)frameBuffer,frameLength*2))
		{
			delete[] frameBuffer;
			frameBuffer = NULL;
			f.close();
			return false;
		}
		unsigned char *pframe = frameBuffer;
		//crcCheck.Init(crcInitVal);
		for(int i = 0;i<frameLength;++i)
		{
			updateFrame.append(1, Hexbyte((char*)pframe));
			//crcCheck.update(updateFrame[i+2]);
			pframe += 2;
		}
		delete[] frameBuffer;
		frameBuffer = NULL;

//		if(crcCheck.getCrcValue() != 0)
//		{
//			f.close();
//			return false;
//		}
		updateFrames.push_back(updateFrame);
		++updateFrameCount;
		f.get();
		if(!f.eof())
		{
			f.unget();
		}
	}
	LOG(Logger::LOG_DEBUG,"UpdateFile::Read(): ------- Datei geladen -------");
	f.close();
	initialized = true;
	return true;
}
const std::string &UpdateFile::getUpdateFrame(int frameIndex)
{
	if(frameIndex < updateFrameCount)
	{
		return updateFrames[frameIndex];	
	}

	return emtyFrame;
}
unsigned char UpdateFile::Hexbyte(const char* s)
{
	unsigned char b=0;
    unsigned char i;
    for(i=0;i<2;++i){
        b<<=4;
        if(*s>='0' && *s<='9')b+=*s-'0';
        else if(*s>='a' && *s<='f')b+=*s-'a'+0x0a;
        else if(*s>='A' && *s<='F')b+=*s-'A'+0x0a;
        ++s;
    }
    return b;
}
int UpdateFile::getUpdateFrameCount()
{
	return this->updateFrameCount;
}

int UpdateFile::getFrameLength(int frameIndex)
{
	if(frameIndex < updateFrameCount)
	{
		return updateFrames[frameIndex].length();
	}

	return -1;
}

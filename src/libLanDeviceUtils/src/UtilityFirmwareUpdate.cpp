/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "UtilityFirmwareUpdate.h"
#include "UtilityDeviceConfiguration.h"
#include "UtilitySendCommand.h"
#include <Protocol.h>
#include <Constants.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <LocalPointerGuard.h>

using namespace LDU;

UtilityFirmwareUpdate::UtilityFirmwareUpdate(void)
{
}

UtilityFirmwareUpdate::~UtilityFirmwareUpdate(void)
{
}

bool UtilityFirmwareUpdate::doFirmwareUpdate(LDU::LanDevice &dev, std::string file)
{
	const unsigned char OPCODE_INIT_UPDATE = 'U';
	const unsigned char OPCODE_WRITE_UPDATE = 'W';
	const int DATA_LENGTH = 0;

	LocalPointerGuard<Protocol> pProt(Protocol::createProtocol( dev.getProtocolType()), false );
	if (pProt->getProtocolName() != "eQ3Config")
	{
		return false;
	}

	// read firmware file
	std::vector<FirmwareFrame> frameList;
	if (!readFirmwareFile(file, &frameList))
	{
		return false;
	}

	// set IV
	unsigned char data[DATA_LENGTH + 1] = { 0x00 };
	if(UtilitySendCommand::createCommand(*pProt, dev, OPCODE_INIT_UPDATE, data, DATA_LENGTH))
	{
		for(unsigned int i = 0; i < frameList.size(); i++)
		{
			int frameSize = frameList.at(i).length + 4;
			unsigned char* frame = new unsigned char[frameSize];
			frame[0] = frameList.at(i).index >> 8;
			frame[1] = frameList.at(i).index;
			frame[2] = frameList.at(i).length >> 8;
			frame[3] = frameList.at(i).length;
			// printf("%02X %02X %02X %02X\n", frame[0], frame[1], frame[2], frame[3]);
			
			for(int j = 0; j < frameList.at(i).length; j++)
			{
				frame[j + 4] = frameList.at(i).data[j];
			}

			if(UtilitySendCommand::lastSendWasEncrypted)
			{
				// send firmware encrypted
				if(!UtilitySendCommand::createEncryptedCommand(*pProt, dev, OPCODE_WRITE_UPDATE, frame, frameSize, true))
				{	
					return false;
				}
			}
			else
			{
				// send firmware unencrypted
				if(!UtilitySendCommand::createCommand(*pProt, dev, OPCODE_WRITE_UPDATE, frame, frameSize, true))
				{					
					return false;
				}
			}
		}

		return true;
	}
	else
	{		
		return false;
	}
}

bool UtilityFirmwareUpdate::readFirmwareFile(std::string file, std::vector<FirmwareFrame>* frameList)
{
	int size = 0;
	// check if firmware file exists
	std::ifstream fileStream;
	fileStream.open(file.c_str(), std::ifstream::in); 
	if (!fileStream.good())
	{
		printf("error: firmware file not found.\n");
		return false;
	}

	// read file content
	char* temp = new char[ 2 ];
	fileStream.seekg(0, std::ios::end);
	size = fileStream.tellg();
	fileStream.seekg(0, std::ios::beg);
	int index = 0;
	int counter = 0;
	while (counter < size)
	{
		FirmwareFrame tempFrame;
		tempFrame.index = index++;
		tempFrame.length = 0;
		fileStream.read( temp, 2 );
		tempFrame.length = ConvertHexStringToByte(temp[0], temp[1]) * 256;
		fileStream.read( temp, 2 );
		tempFrame.length += ConvertHexStringToByte(temp[0], temp[1]);
		tempFrame.data = new unsigned char[tempFrame.length];
		counter += (tempFrame.length + 2) * 2;
		for(int i = 0; i < tempFrame.length; i++)
		{
			fileStream.read( temp, 2 );
			tempFrame.data[i] = ConvertHexStringToByte(temp[0], temp[1]);
		}

		frameList->push_back(tempFrame);
	}

	return true;
}

unsigned char UtilityFirmwareUpdate::ConvertHexStringToByte(char high, char low)
{
	unsigned char highvalue;
	unsigned char lowvalue;
	unsigned char result;
	
	highvalue = ConvertHexCharToByte(high);
	lowvalue = ConvertHexCharToByte(low);	
	
	result = highvalue * 16 + lowvalue;
	return result;
}

unsigned char UtilityFirmwareUpdate::ConvertHexCharToByte(char value)
{
	unsigned char byte = 0x00;
	if(value >= 'a')
	{
		byte = value - 'a' + 10;
	}
	else if(value >= 'A')
	{
		byte = value - 'A' + 10;
	}
	else if(value != ':')
	{	
		byte = value - '0';
	}
	
	return byte;
}

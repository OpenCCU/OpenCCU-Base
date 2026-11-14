/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifdef WIN32
	#include <Windows.h>
#else
	#include <unistd.h>
#endif

#include "UtilityDeviceConfiguration.h"
#include "UtilitySendCommand.h"
#include "LanDeviceUtils.h"
#include <Protocol.h>
#include <Constants.h>
#include <UDPDatagramSender.h>
#include <cstdio>
#include <sstream>
#include <iostream>
#include <fstream>
#include <vector>
#include <utils.h>
#include <LocalPointerGuard.h>


using namespace LDU;

UtilityDeviceConfiguration::UtilityDeviceConfiguration(void)
{
}

UtilityDeviceConfiguration::~UtilityDeviceConfiguration(void)
{
}

bool UtilityDeviceConfiguration::loadTestStatus(LanDevice& dev)
{
	const unsigned char OPCODE = 't';
	const int DATA_LENGTH = 0;

	LocalPointerGuard<Protocol> pProt(Protocol::createProtocol( dev.getProtocolType() ), false);
	if (pProt->getProtocolName() != "eQ3Config")
	{
		return false;
	}

	unsigned char data[DATA_LENGTH + 1] = { 0x00 };
	bool success = UtilitySendCommand::createCommand(*pProt, dev, OPCODE, data, DATA_LENGTH);

	if (success)
	{
		TestStatusConfiguration testStatusConfiguration;
		pProt->parseTestStatus(UtilitySendCommand::lastResponse, dev.getSerialNumber(), testStatusConfiguration);
		dev.setTestStatusConfiguration(testStatusConfiguration);
		return true;
	}
	else
	{
		return false;
	}
}

bool UtilityDeviceConfiguration::changeTestStatus(LanDevice& dev, const TestStatusConfiguration& testStatusConfiguration)
{
	const unsigned char OPCODE = 'T';
	const int DATA_LENGTH = 1;

	LocalPointerGuard<Protocol> pProt(Protocol::createProtocol( dev.getProtocolType()), false );
	if (pProt->getProtocolName() != "eQ3Config")
	{
		return false;
	}
	
	unsigned char data[DATA_LENGTH] = { testStatusConfiguration.getTestStatus() };
	return UtilitySendCommand::createCommand(*pProt, dev, OPCODE, data, DATA_LENGTH);
}

bool UtilityDeviceConfiguration::rebootDevice(LanDevice& dev)
{
	const unsigned char OPCODE = 'R';
	const int DATA_LENGTH = 0;

	LocalPointerGuard<Protocol> pProt(Protocol::createProtocol( dev.getProtocolType()), false );
	if (pProt->getProtocolName() != "eQ3Config")
	{
		return false;
	}
	
	unsigned char data[DATA_LENGTH + 1] = { 0x00 };
	return UtilitySendCommand::createCommand(*pProt, dev, OPCODE, data, DATA_LENGTH);
}

bool UtilityDeviceConfiguration::resetDevice(LanDevice& dev)
{
	const unsigned char OPCODE = 'F';
	const int DATA_LENGTH = 0;

	LocalPointerGuard<Protocol> pProt(Protocol::createProtocol( dev.getProtocolType()), false );
	if (pProt->getProtocolName() != "eQ3Config")
	{
		return false;
	}
	
	unsigned char data[DATA_LENGTH + 1] = { 0x00 };
	return UtilitySendCommand::createCommand(*pProt, dev, OPCODE, data, DATA_LENGTH);
}

bool UtilityDeviceConfiguration::sendUserDataToDevice(LanDevice& dev, unsigned char userData[], const unsigned char AMOUNT)
{
	const unsigned char OPCODE = 'P';

	LocalPointerGuard<Protocol> pProt(Protocol::createProtocol( dev.getProtocolType()), false );
	if (pProt->getProtocolName() != "eQ3Config")
	{
		return false;
	}
	
	return UtilitySendCommand::createCommand(*pProt, dev, OPCODE, userData, AMOUNT);
}

bool UtilityDeviceConfiguration::enterBootloader(LDU::LanDevice &dev)
{
	const unsigned char OPCODE = 'B';
	const int DATA_LENGTH = 0;

	LocalPointerGuard<Protocol> pProt(Protocol::createProtocol( dev.getProtocolType()), false );
	if (pProt->getProtocolName() != "eQ3Config")
	{
		return false;
	}
	
	// checks if the bootloader is already started
	bool blAlreadyStarted = (dev.getType().find("Bl") != std::string::npos);
	if (blAlreadyStarted)
	{
		return true;
	}

	unsigned char data[DATA_LENGTH + 1] = { 0x00 };
	if(!UtilitySendCommand::createCommand(*pProt, dev, OPCODE, data, DATA_LENGTH))
	{
		return false;
	}

	int counter = 0;
	do
	{
		if (validateDeviceState(dev.getSerialNumber(), DEVICE_STATE_BOOTLOADER, dev))
		{
			return true;
		}

		counter++;
//		Sleep(500);
		usleep(500*1000);
	}
	while (counter < 10);

	return false;
}

bool UtilityDeviceConfiguration::enterApplication(LDU::LanDevice &dev)
{	
	const unsigned char OPCODE = 'A';
	const int DATA_LENGTH = 0;

	LocalPointerGuard<Protocol> pProt(Protocol::createProtocol( dev.getProtocolType()), false );
	if (pProt->getProtocolName() != "eQ3Config")
	{
		return false;
	}
	
	unsigned char data[DATA_LENGTH + 1] = { 0x00 };
	if (!UtilitySendCommand::createCommand(*pProt, dev, OPCODE, data, DATA_LENGTH))
	{
		return false;
	}

	int counter = 0;
	do
	{
		if (UtilityDeviceConfiguration::validateDeviceState(dev.getSerialNumber(), DEVICE_STATE_APP, dev))
		{
			return true;
		}

		counter++;
		//Sleep(500);
		usleep(500*1000);
	}
	while (counter < 5);

	return false;
}

bool UtilityDeviceConfiguration::validateDeviceState(std::string serial, DeviceState deviceState, LDU::LanDevice &dev)
{
	LanDeviceUtils ldu;
	std::vector<LanDevice> devs;
	std::vector<std::string> devTypeFilters;

	devTypeFilters.push_back( std::string("*") );
	int protocols = PROTOCOL_EQ3CONFIG;
	int routingSchemes = (int) (ROUTINGSCHEME_BROADCAST);
	if (!ldu.searchDevicesByType(devTypeFilters, protocols, routingSchemes, "", "", devs))
	{
		return false;
	}

	for (unsigned int i = 0; i < devs.size(); i++)
	{
		if (devs.at(i).getSerialNumber() == serial)
		{
			LanDevice tempDevice = devs.at(i);
			std::string type = tempDevice.getType();
			dev.setType(tempDevice.getType());
			std::string version = tempDevice.getFirmwareVersion();
			dev.setFirmwareVersion(version);

			if (deviceState == DEVICE_STATE_APP)
			{
				return (type.find("App") != std::string::npos) ? true : false;
			}
			else
			{
				return (type.find("Bl") != std::string::npos) ? true : false;
			}
		}
	}

	return false;
}

bool UtilityDeviceConfiguration::keyExchange(LDU::LanDevice &dev, unsigned char newKey[], const unsigned char AES_KEY_LENGTH)
{
	const unsigned char OPCODE_INIT = 'K';
	const unsigned char OPCODE_EXCHANGE = 'E';
	const int DATA_LENGTH = 0;

	LocalPointerGuard<Protocol> pProt( Protocol::createProtocol( dev.getProtocolType()), false );
	if (pProt->getProtocolName() != "eQ3Config")
	{
		return false;
	}
	
	unsigned char data[DATA_LENGTH + 1] = { 0x00 };
	if(UtilitySendCommand::createCommand(*pProt, dev, OPCODE_INIT, data, DATA_LENGTH))
	{		
		if(UtilitySendCommand::lastSendWasEncrypted)
		{
			if(UtilitySendCommand::createEncryptedCommand(*pProt, dev, OPCODE_EXCHANGE, newKey, AES_KEY_LENGTH))
			{
				dev.setAesKey(newKey, AES_KEY_LENGTH);
				return true;
			}
		}
		else
		{
			if(UtilitySendCommand::createCommand(*pProt, dev, OPCODE_EXCHANGE, newKey, AES_KEY_LENGTH))
			{
				dev.setAesKey(newKey, AES_KEY_LENGTH);
				return true;
			}
		}
	}

	return false;
}

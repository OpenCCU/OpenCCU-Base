/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "UtilityNetworkConfiguration.h"
#include "UtilitySendCommand.h"
#include <Protocol.h>
#include <Constants.h>
#include <UDPDatagramSender.h>
#include <LocalPointerGuard.h>

using namespace LDU;

UtilityNetworkConfiguration::UtilityNetworkConfiguration(void)
{
}

UtilityNetworkConfiguration::~UtilityNetworkConfiguration(void)
{
}

bool UtilityNetworkConfiguration::loadRuntimeNetworkConfiguration(LanDevice& dev)
{
	const unsigned char OPCODE = 'n';
	const int DATA_LENGTH = 0;
	std::string response;
	bool done;

	LocalPointerGuard<Protocol> pProt(Protocol::createProtocol( dev.getProtocolType()), false );
	std::string serial(dev.getSerialNumber());
	if(serial.empty())
	{
		return false;
	}

	if (pProt->getProtocolName() == "eQ3Config")
	{
		unsigned char data[DATA_LENGTH + 1] = { 0x00 };
		done = UtilitySendCommand::createCommand(*pProt, dev, OPCODE, data, DATA_LENGTH);
		if(done)
		{
			response = UtilitySendCommand::lastResponse;
		}
	}
	else
	{
		std::string msg = pProt->getRuntimeIPConfigRequestFrame(dev.getType(), dev.getSerialNumber() );
		UDPDatagramSender sender(Constants::MULTICAST_ADDRESS, pProt->getProtocolDefaultPort(), pProt->getProtocolDefaultReplyPort(), dev.getReachedByRoutingScheme());
		std::vector<std::string> responses;
		done = sender.send(msg, responses, 2000, 1);
		if(done && responses.size() > 0)
		{
			response = responses.at(0);
		}
		else
		{
			return false;
		}
	}

	if (done)
	{
		RuntimeIPConfiguration ipConfig;
		done = pProt->parseRuntimeIPConfigRequestResponse( response, dev.getSerialNumber(), ipConfig);
		if(!done)
		{
			return false;
		}
		
		dev.setRuntimeIPConfiguration(ipConfig);
		return true;
	}
	else
	{
		return false;
	}
}

bool UtilityNetworkConfiguration::loadNetworkConfiguration(LanDevice& dev) 
{
	const unsigned char OPCODE = 'c';
	const int DATA_LENGTH = 0;
	std::string response;
	bool done;
	
	LocalPointerGuard<Protocol> pProt(Protocol::createProtocol( dev.getProtocolType()), false );
	std::string serial(dev.getSerialNumber());
	if(serial.empty())
	{
		return false;
	}

	if (pProt->getProtocolName() == "eQ3Config")
	{
		unsigned char data[DATA_LENGTH + 1] = { 0x00 };
		done = UtilitySendCommand::createCommand(*pProt, dev, OPCODE, data, DATA_LENGTH);
		if(done)
		{
			response = UtilitySendCommand::lastResponse;
		}
	}
	else
	{
		std::string msg = pProt->getGetNetworkConfigurationFrame( dev.getType(), dev.getSerialNumber());
		std::string addr = dev.getRuntimeIPConfiguration().getIPAddress();
		if(addr.empty()) 
		{
			return false;
		}

		UDPDatagramSender sender(addr.c_str(), pProt->getProtocolDefaultPort(), pProt->getProtocolDefaultReplyPort(), ROUTINGSCHEME_UNICAST);
		std::vector<std::string> responses;
		done = sender.send( msg, responses, 2000, 1);
		
		if(done && responses.size() > 0)
		{
			response = responses.at(0);
		}
		else
		{
			return false;
		}
	}
	
	if (done)
	{
		IPConfiguration ipConfig;
		done = pProt->parseGetNetworkConfigurationFrameResponse( response, dev.getSerialNumber(), ipConfig );
		if(!done)
		{
			return false;
		}

		dev.setIPConfiguration(ipConfig);
		return true;
	}
	else
	{
		return false;
	}
}

bool UtilityNetworkConfiguration::changeNetworkConfiguration(LanDevice& dev, const IPConfiguration& newIPConfiguration) 
{
	const unsigned char OPCODE = 'C';
	bool done;

	// ToDo: support eQ3LanIf-protocol :/
	LocalPointerGuard<Protocol> pProt(Protocol::createProtocol( dev.getProtocolType()), false );
	std::string serial(dev.getSerialNumber());
	if(serial.empty())
	{
		return false;
	}

	if (pProt->getProtocolName() == "eQ3Config")
	{
		unsigned char data[1000];
		int indexData = 0;
		
		UtilityNetworkConfiguration::getNetworkConfigData(data, &indexData, newIPConfiguration);

		done = UtilitySendCommand::createCommand(*pProt, dev, OPCODE, data, indexData);
		if(done)
		{
			dev.setIPConfiguration(newIPConfiguration);
			return true;
		}
		else
		{
			return false;
		}
	}
	else
	{
		std::string msg = pProt->getSetNetworkConfigurationFrame( dev.getType(), dev.getSerialNumber(), newIPConfiguration);
		std::string addr = dev.getRuntimeIPConfiguration().getIPAddress();
		if(addr.empty()) {
			return false;
		}

		UDPDatagramSender sender(addr.c_str(), pProt->getProtocolDefaultPort(), pProt->getProtocolDefaultReplyPort(), ROUTINGSCHEME_UNICAST);
		std::vector<std::string> responses;
		done = sender.send( msg, responses, 2000, 1);
		if(done && (responses.size() > 0)) 
		{
			char requestOpcode;
			int returnCode;
			done = pProt->parseAckRespone( responses.at(0), &requestOpcode, &returnCode );

			if(done && returnCode == 1) 
			{
				//TODO Returncode auswerten
				dev.setIPConfiguration(newIPConfiguration);
				return true;
			}
			else
			{
				return false;
			}
		}
		else
		{
			return false;
		}
	}
}

void UtilityNetworkConfiguration::getNetworkConfigData(unsigned char* data, int* indexData, const IPConfiguration& newIPConfiguration)
{
	// 4 byte ip adress
	UtilityNetworkConfiguration::appendAdress(data, *indexData, newIPConfiguration.getIPAddress());
	(*indexData) += 4;

	// 4 byte gateway
	UtilityNetworkConfiguration::appendAdress(data, *indexData, newIPConfiguration.getDefaultGateway());
	(*indexData) += 4;

	// 4 byte netmask
	UtilityNetworkConfiguration::appendAdress(data, *indexData, newIPConfiguration.getNetmask());
	(*indexData) += 4;

	// 4 byte dns1
	UtilityNetworkConfiguration::appendAdress(data, *indexData, newIPConfiguration.getPrimaryDNS());
	(*indexData) += 4;

	// 4 byte dns2
	UtilityNetworkConfiguration::appendAdress(data, *indexData, newIPConfiguration.getSecondaryDNS());
	(*indexData) += 4;

	//1 byte ip-flags (dhcp & autoip)
	char tempC = (char) (newIPConfiguration.isDHCPEnabled() ? 0x01 : 0x00);//dhcp
	tempC = tempC | (char) (newIPConfiguration.isAutoIPEnabled() ? 0x02 : 0x00);//autoip
	data[(*indexData)++] = tempC;

	// 1 byte crypt
	data[(*indexData)++] = newIPConfiguration.isCryptEnabled() ? 0x01 : 0x00;

	// n byte dns-name
	std::string dnsName = newIPConfiguration.getDNSName();
	
	// terminate with zero if string is empty
	if (dnsName.empty())
	{
		data[(*indexData)++] = 0x00;
	}
	else
	{
		for (unsigned int i = 0; i < dnsName.length(); i++)
		{
			data[(*indexData)++] = dnsName.at(i);
		}

		data[(*indexData)++] = 0x00;
	}
}

void UtilityNetworkConfiguration::appendAdress(unsigned char* data, int indexData, std::string adress)
{
	int a, b, c, d;
	extractIPAddressComponents(adress, a, b, c, d);
	
	data[indexData++] = (char) a;
	data[indexData++] = (char) b;
	data[indexData++] = (char) c;
	data[indexData++] = (char) d;
}

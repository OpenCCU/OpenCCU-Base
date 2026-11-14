/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "UtilitySendCommand.h"
#include <EQ3ConfigProtocol.h>
#include <Protocol.h>
#include <Constants.h>
#include <UDPDatagramSender.h>
#include <cstdio>
#include <sstream>
#include <iostream>

using namespace LDU;

UtilitySendCommand::UtilitySendCommand(void)
{	
}

UtilitySendCommand::~UtilitySendCommand(void)
{
}

std::string UtilitySendCommand::lastResponse;
bool UtilitySendCommand::resentCommandEncrypted;
bool UtilitySendCommand::resentResponseEmpty;
bool UtilitySendCommand::lastSendWasEncrypted;
const int MAX_SEND_TRIES = 3;	

std::string UtilitySendCommand::getMessage(const std::string& devType, const std::string& serial, const unsigned char opcode, const unsigned char* data, const int dataLength)
{
	std::stringstream ss;

	int length = 0;
	std::string frame(EQ3ConfigProtocol::assembleHeaderFrameBase(devType, serial, length));
	frame.append(1, opcode);

	for (int i = 0; i < dataLength; i++)
	{
		ss << data[i];

		frame.append(ss.str());

		ss.str("");
		ss.clear();
	}
	
	return frame;
}

bool UtilitySendCommand::createCommand(
	Protocol* pProt,
	LanDevice& dev,
	unsigned char opcode,
	unsigned char* data,
	int dataLength, 
	bool resendWithSameCounter)
{
	std::string msg = getMessage(dev.getType(), dev.getSerialNumber(), opcode, data, dataLength);
		
	for (int i = 0; i < MAX_SEND_TRIES; i++)
	{
		UtilitySendCommand::resentCommandEncrypted = false;
		UtilitySendCommand::resentResponseEmpty = false;
		UtilitySendCommand::lastSendWasEncrypted = false;

		bool result = sendCommand(pProt, dev, msg, opcode);

		if (result)
		{
			return true;
		}
		else if (UtilitySendCommand::resentCommandEncrypted)
		{
			msg = dev.encryptMessage(opcode, data, dataLength);
			UtilitySendCommand::lastSendWasEncrypted = true;
			return sendCommand(pProt, dev, msg, opcode);
		}
		else if (!UtilitySendCommand::resentResponseEmpty)
		{
			return false;
		}

		if(!resendWithSameCounter)
		{
			msg = getMessage(dev.getType(), dev.getSerialNumber(), opcode, data, dataLength);
		}
	}
	
	return false;
}

bool UtilitySendCommand::createEncryptedCommand(
	Protocol* pProt,
	LanDevice& dev,
	unsigned char opcode,
	unsigned char* data,
	int dataLength, 
	bool resendWithSameCounter)
{
	std::string msg = dev.encryptMessage(opcode, data, dataLength);
		
	for (int i = 0; i < MAX_SEND_TRIES; i++)
	{
		UtilitySendCommand::resentResponseEmpty = false;
		UtilitySendCommand::lastSendWasEncrypted = true;
			
		bool result = sendCommand(pProt, dev, msg, opcode);
		
		if (!UtilitySendCommand::resentResponseEmpty)
		{
			return result;
		}
		
		if(!resendWithSameCounter)
		{
			msg = dev.encryptMessage(opcode, data, dataLength);
		}
	}

	return false;
}

bool UtilitySendCommand::sendCommand(
	Protocol* pProt,
	LanDevice& dev,
	std::string msg,
	unsigned char opcode)
{
	const unsigned char OPCODE_LOAD_NETWORK_CONFIGURATION = 'n';
	const unsigned char OPCODE_READ_NETWORK_CONFIGURATION = 'c';
	const unsigned char OPCODE_WRITE_NETWORK_CONFIGURATION = 'C';
	const unsigned char OPCODE_WRITE_FIRMWARE = 'W';
	bool done;
	
	std::vector<std::string> responses;
	std::string response = "";
	unsigned int timeout = 2000;
	if(opcode == OPCODE_WRITE_FIRMWARE)
	{
		timeout = 5000;
	}

	if (opcode == OPCODE_LOAD_NETWORK_CONFIGURATION)
	{
		std::string targetAddress;
		switch(dev.getReachedByRoutingScheme()) {
			case ROUTINGSCHEME_MULTICAST:
				targetAddress = Constants::MULTICAST_ADDRESS;
				break;
			case ROUTINGSCHEME_BROADCAST:
				targetAddress = Constants::BROADCAST_ADDRESS;
				break;
			case ROUTINGSCHEME_UNICAST:
				targetAddress = dev.getRuntimeIPConfiguration().getIPAddress();
				break;

		}
		UDPDatagramSender sender(targetAddress.c_str(), pProt->getProtocolDefaultPort(), pProt->getProtocolDefaultReplyPort(), dev.getReachedByRoutingScheme());
		done = sender.send( msg, responses, timeout, 1 );
	}
	else
	{
		std::string addr = dev.getRuntimeIPConfiguration().getIPAddress();
		if((useBroadcast(addr) && dev.getReachedByRoutingScheme() != ROUTINGSCHEME_UNICAST) && (opcode == OPCODE_READ_NETWORK_CONFIGURATION || opcode == OPCODE_WRITE_NETWORK_CONFIGURATION))
		{
			// printf("Send broadcast\n");
			UDPDatagramSender sender(Constants::MULTICAST_ADDRESS, pProt->getProtocolDefaultPort(), pProt->getProtocolDefaultReplyPort(), dev.getReachedByRoutingScheme());
			done = sender.send( msg, responses, timeout, 1 );
		}
		else if(addr.empty())
		{
			return false;
		}
		else
		{
			//printf("IP: %s\n", addr.c_str());
			UDPDatagramSender sender(addr.c_str(), pProt->getProtocolDefaultPort(), pProt->getProtocolDefaultReplyPort(), ROUTINGSCHEME_UNICAST);
			done = sender.send( msg, responses, timeout, 1 );
		}
	}
	
	int returnCode = -1;
	if(responses.size() > 0)
	{
		//printf("responses: %i\n", responses.size());
		for(unsigned int i = 0; i < responses.size(); ++i)
		{
			char reqOpcode;
			done = pProt->parseAckRespone( responses.at(i), &reqOpcode, &returnCode );
			UtilitySendCommand::lastResponse = responses.at(i);
			if(!done && returnCode != 3 && returnCode != 0)
			{
				//printf("Error Parse Ack\n", reqOpcode, opcode);
				continue;
			}

			if(reqOpcode != opcode)
			{
				//printf("ReqOpcode %c expected %c\n", reqOpcode, opcode);
				continue;
			}

			if(msg.at(4) != responses.at(i).at(4))
			{
				//printf("Wrong counter %i expected %i\n", responses.at(i).at(4), msg.at(4));
				continue;
			}

			response = responses.at(i);
			break;
		}
	}

	if(!response.empty())
	{
		//printf("returncode: %d\n", returnCode);
		switch (returnCode)
		{
			// error (0 general, 3 encryption needed)
			case 0:
				return false;

			// success (1 general, 2 with following reboot)
			case 1:
			case 2:
				return true;

			// encryption needed
			case 3:
				dev.setIv(response);
				UtilitySendCommand::resentCommandEncrypted = true;
				return false;

			default:
				return false;
		}
	}
	else
	{
		//printf("Response empty (opcode %c)\n", opcode);
		UtilitySendCommand::resentResponseEmpty = true;
	}

	return false;
}

bool UtilitySendCommand::useBroadcast(std::string ipAdress)
{
	if (ipAdress.find("10") == 0 || ipAdress.find("192.168") == 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}

bool UtilitySendCommand::getLastResponse(int* length, unsigned char* response)
{
	// opcode ">" + 1 byte original opcode + 1 byte return code --> begin of message
	const std::string::size_type BEGIN_OF_MESSAGE = lastResponse.find(">") + 2;

	std::string lastResponse = UtilitySendCommand::lastResponse;

	*length = 0;
  if(lastResponse.at( BEGIN_OF_MESSAGE) != 0x01 && lastResponse.at( BEGIN_OF_MESSAGE) != 0x02 )
  {
        return false;
  }

	for (std::string::size_type i = BEGIN_OF_MESSAGE + 1; i < lastResponse.length(); i++)
	{
		unsigned char currentByte = lastResponse.at(i);

		response[i - (BEGIN_OF_MESSAGE + 1)] = currentByte;
		(*length)++;
	}

	return true;
}

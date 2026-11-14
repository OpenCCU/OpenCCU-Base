/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "../include/udpframe.h"
#include <stdio.h>

// Initialize the udpframe with the data string
udpframe::udpframe(std::string& data)
{
	std::string zeroStr;
	zeroStr.append(1, (char)0x00);
	size_t offset = 0;
	
	// Protocoll version, should be 2
	if(data.size() <= offset) { return; }
	this->version = (unsigned char)data.at(offset++);
	if(this->version > 1)
	{
		// Senderid of the sender and message counter
		if(data.size() <= offset+3) { return; }
		this->senderid = (unsigned char)data.at(offset++) << 16;
		this->senderid += (unsigned char)data.at(offset++) << 8;
		this->senderid += (unsigned char)data.at(offset++);
		this->counter = (unsigned char)data.at(offset++);
	}
	else
	{
		this->senderid = 0;
		this->counter = 0;
	}

	// Zero terminated devicetype
	std::string::size_type index = data.find(zeroStr, offset);
	if(index != std::string::npos)
	{
		this->devicetype = data.substr(offset, index - offset);
		offset += this->devicetype.size() + 1;
	}

	// Zero terminated serial number
	index = data.find(zeroStr, offset);
	if(index != std::string::npos)
	{
		this->serialnumber = data.substr(offset, index - offset);
		offset += this->serialnumber.size() + 1;
	}

	// Opcode of the telegramm
	if(data.size() <= offset) { return; }
	this->opcode = data.at(offset++);

	// Payload data of the telegramm
	if(data.size() <= offset) { return; }
	this->payload = data.substr(offset);
}

udpframe::~udpframe()
{
    //dtor
}

std::string udpframe::Getdevicetype() const
{
    return devicetype;
}

std::string udpframe::Getserialnumber() const
{
    return serialnumber;
}

std::string udpframe::Getpayload() const
{
    return payload;
}

unsigned char udpframe::Getversion() const
{
    return version;
}

unsigned char udpframe::Getcounter() const
{
    return counter;
}

unsigned int udpframe::Getsenderid() const
{
    return senderid;
}

char udpframe::Getopcode() const
{
    return opcode;
}

// Check if the device is target for the frame
bool udpframe::IsTarget(const std::string type, const std::string serial) const
{
	// Check if the device has the correct type
	for(unsigned int i = 0; i < this->devicetype.size() && i < type.size(); i++)
	{
		if(devicetype.at(i) == '*')
		{
			break;
		}
		else if(type.at(i) != this->devicetype.at(i))
		{
			return false;
		}
	}

	// Check if the device has the correct serial number
	for(unsigned int i = 0; i < this->serialnumber.size() && i < serial.size(); i++)
	{
		if(serialnumber.at(i) == '*')
		{
			break;
		}
		else if(serial.at(i) != this->serialnumber.at(i))
		{
			return false;
		}
	}

	return true;
}

// Get the header for the response
std::string udpframe::GetResponseHeader(const std::string type, const std::string serial) const
{
	std::string resultframe;
	// Append counter and senderid to the resultframe if version is 2 or higher
	resultframe.append(1, (char)this->version);
	if(this->version > 1)
	{
		resultframe.append(1, (char)((this->senderid >> 16) & 0xff));
		resultframe.append(1, (char)((this->senderid >> 8) & 0xff));
		resultframe.append(1, (char)(this->senderid & 0xff));
		resultframe.append(1, (char)this->counter);
	}

	// Append device information (type and serial) to the resultframe
	resultframe.append(type);
	resultframe.append(1, (char)0x00);
	resultframe.append(serial);
	resultframe.append(1, (char)0x00);

	// Append response opcode to the resultframe
	resultframe.append(1, '>');

	return resultframe;
}

/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <HMWLGWCommand.h>
#include <Logger.h>
#include <HMWLGWUtils.h>

unsigned char HMWLGWCommand::frameCounterValue = 0;
/*
HMWLGWCommand::HMWLGWCommand(const HMWLGWCommandType cmdType)
:frameCounter((char)0xff)
,command(cmdType)
,hmwControlByte(0x00)
,hmwReceiverAddress(0)
,hmwSenderAddress(0)
{
}
*/

HMWLGWCommand::HMWLGWCommand()
:frameCounter((char)0xFF)
,command(LGW_CMD_UNDEFINED)
,hmwControlByte(0x00)
,hmwReceiverAddress(0)
,hmwSenderAddress(0)
,hmwTimeout(0)
{
}


HMWLGWCommand::HMWLGWCommand(const std::string& cmdRawData)
:frameCounter((char)0xFF)
,command(LGW_CMD_UNDEFINED)
,hmwControlByte(0x00)
,hmwReceiverAddress(0)
,hmwSenderAddress(0)
,hmwTimeout(0)
{
	parseFromRawData(cmdRawData);
}

HMWLGWCommand::~HMWLGWCommand()
{
}

bool HMWLGWCommand::parseFromRawData(const std::string& cmdRawData)
{
	if(cmdRawData.size() < 2) {//Must be at least Counter and OPCODE
		return false;
	}
	bool done = true;
	//Parse counter and opcode
	frameCounter = (unsigned char)cmdRawData.at(0);
	command = (HMWLGWCommandType)cmdRawData.at(1);
	switch(command) {
	case LGW_CMD_KEEPALIVE:
	case LGW_CMD_SEND:
	case LGW_CMD_DISCOVERY:
	case LGW_CMD_SET_LOG_LEVEL:
		LOG(Logger::LOG_ERROR, "HMWLGWCommand::parseRawData(): Command only valid for outgoing messages.");
		done = false;
		break;
	case LGW_RPL_RESPONSE:
		if(cmdRawData.size() >= 3) {
			//LOG(Logger::LOG_ALL, "HMWLGWCommand::parseFromRawData() cmdRawData: %s", toDebugHexStr(cmdRawData).c_str());
			hmwControlByte = (unsigned char)cmdRawData.at(2);
			if(cmdRawData.size() >= 4) {
				commandData = cmdRawData.substr(3);
			}
		}
		else {
			LOG(Logger::LOG_ERROR, "HMWLGWCommand::parseRawData(): Error parsing response data. No control byte.");
			done = false;
		}
		break;
	case LGW_RPL_ANSWER:
		if(cmdRawData.size() == 3) {
			commandData.assign(1, cmdRawData.at(2));
		}
		else {
			LOG(Logger::LOG_ERROR, "HMWLGWCommand::parseRawData(): Error parsing answer data. Wrong amount of data.");
			done = false;
		}
		break;
	case LGW_RPL_DISCOVERY_RESULT:
		if(cmdRawData.size() >= 6) {//minimum of 2 byte + 1 * 4 byte address
			commandData = cmdRawData.substr(2);
			//LOG(Logger::LOG_DEBUG, "HMWLGWCommand::parseRawData(): Discovery result data: %s", toDebugHexStr(commandData).c_str());
		}
		else {
			//LOG(Logger::LOG_DEBUG, "HMWLGWCommand::parseRawData(): Discovery result empty.");
			//it's not an error...
		}
		break;
	case LGW_EVT_DISCOVERY_END:
		if(cmdRawData.size() == 5) {
			commandData = cmdRawData.substr(2);
			//LOG(Logger::LOG_DEBUG, "HMWLGWCommand::parseRawData(): Discovery end data: %s", toDebugHexStr(commandData).c_str());
		}
		else {
			LOG(Logger::LOG_ERROR, "HMWLGWCommand::parseRawData(): Error parsing discovery end data. Wrong amount of data.");
			done = false;
		}
		break;
	case LGW_EVT_EVENT:
		if(cmdRawData.size() >= 11) { //2 Bytes + 4 recv addr + 1 byte ctrl + 4 sender addr (+ n bytes data)
			hmwReceiverAddress = convertBigEndianStringToUnsignedInt(cmdRawData.substr(2, 4));
			hmwControlByte = cmdRawData.at(6);
			hmwSenderAddress = convertBigEndianStringToUnsignedInt(cmdRawData.substr(7, 4));
			if(cmdRawData.size() > 11) {
				commandData.assign(cmdRawData.substr(11));
			}
		}
		else {
			LOG(Logger::LOG_ERROR, "HMWLGWCommand::parseRawData(): Error parsing event data. Too few data.");
			done = false;
		}
		break;
	default:
		LOG(Logger::LOG_ERROR, "HMWLGWCommand::parseRawData(): Command only valid for outgoing messages.");
		done = false;
		break;
	}
	return done;
}

std::string HMWLGWCommand::assembleCmdRawData()
{
	std::string rawData;
	frameCounterValue++;
	frameCounter = frameCounterValue;
	rawData.append(1, frameCounter);
	rawData.append(1, (unsigned char)command);
	if(command == LGW_CMD_SEND) {
		rawData.append(1, (char)hmwTimeout);
		rawData.append(convertUnsignedIntToBigEndianString(hmwReceiverAddress));
		rawData.append(1, hmwControlByte);
		rawData.append(convertUnsignedIntToBigEndianString(hmwSenderAddress));
	}
	rawData.append(commandData);
	return rawData;
}

HMWLGWCommandType HMWLGWCommand::getCommandType()
{
	return command;
}

void HMWLGWCommand::setCommandType(const HMWLGWCommandType& cmdType)
{
	command = cmdType;
}

const std::string& HMWLGWCommand::getCommandData()
{
	return commandData;
}


void HMWLGWCommand::setCommandData(const std::string& payload)
{
	commandData = payload;
}




void HMWLGWCommand::setSenderAddress(const unsigned int address)
{
	hmwSenderAddress = address;
}


void HMWLGWCommand::setReceiverAddress(const unsigned int address)
{
	hmwReceiverAddress = address;
}


unsigned int HMWLGWCommand::getReceiverAddress()
{
	return hmwReceiverAddress;
}


unsigned int HMWLGWCommand::getSenderAddress()
{
	return hmwSenderAddress;
}


void HMWLGWCommand::setCtrlByte(unsigned char ctrl)
{
	hmwControlByte = ctrl;
}

unsigned char HMWLGWCommand::getCtrlByte()
{
	return hmwControlByte;
}

unsigned int HMWLGWCommand::getTimeout()
{
	return hmwTimeout;
}

void HMWLGWCommand::setTimeout(unsigned int timeout)
{
	hmwTimeout = timeout;
}

bool HMWLGWCommand::isResponseOf(const HMWLGWCommand responseCmd)
{
	if(frameCounter == responseCmd.frameCounter)
	{
		return true;//FIXME mybe further checks...
	}
	return false;
}

bool HMWLGWCommand::transformResponseToEvent(HMWLGWCommand& requestCommand)
{
	if(command == LGW_RPL_RESPONSE) {
		hmwReceiverAddress = requestCommand.getSenderAddress();
		hmwSenderAddress = requestCommand.getReceiverAddress();
		command = LGW_EVT_EVENT;
	}
	return false;
}


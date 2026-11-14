/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <UnifiedLanProtocolMessage.h>
#include <UnifiedLanProtocolTypeConverter.h>


using namespace ulc;

//unsigned char UnifiedLanProtocolMessage::counter = 0;

//----------------------------------------------------------------------------------------------

UnifiedLanProtocolMessage::UnifiedLanProtocolMessage()	
:messageCounter((char)0x00)
,messageCommand(ULP_CMD_UNDEFINED)
,messageData("")
,msgEndChars("")
{
	msgEndChars.append(1, (char)0x0D);//CR
	msgEndChars.append(1, (char)0x0A);//LF
}

//----------------------------------------------------------------------------------------------

UnifiedLanProtocolMessage::UnifiedLanProtocolMessage(const UnifiedLanProtocolCmd cmd, const unsigned char messageCounter)
:messageCounter(messageCounter)
,messageCommand(cmd)
,messageData("")
{
	msgEndChars.append(1, (char)0x0D);//CR
	msgEndChars.append(1, (char)0x0A);//LF
}

//----------------------------------------------------------------------------------------------

bool UnifiedLanProtocolMessage::appendDataFromDevice(const std::string data, std::string& nextMessageData)
{
	nextMessageData.clear(); 
	//TODO implement me
	std::string::size_type index = data.find(msgEndChars);
	if(index == std::string::npos) {
		messageData.append(data);
		return false;
	}
	else {
		messageData.append(data.substr(0, index+2));
		if(index+2 < data.size()) {
			nextMessageData.append(data.substr(index+2));
		}
		parseReceivedData();
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------------

bool UnifiedLanProtocolMessage::parseReceivedData()
{
	if(messageData.size() < 3) {//OPCODE 1 Byte + counter 2 bytes (hex)
		return false;
	}
	messageCommand = (UnifiedLanProtocolCmd)messageData.at(0);
	messageCounter = hexStringToUChar(messageData.substr(1,2));
	for(unsigned int i = 3; i < messageData.size(); i++) {
		const char c = messageData.at(i);
		if(c == ',' && ((i+1) < messageData.size())) {
			parameterIndices.push_back(i+1);
		}
	}
	return true;
}

//----------------------------------------------------------------------------------------------

void UnifiedLanProtocolMessage::addParam(const std::string& hexStr)
{
	messageData.append(1, ',');
	messageData.append(hexStr);
}

//----------------------------------------------------------------------------------------------

unsigned int UnifiedLanProtocolMessage::getMessageParameterCount() const
{
	return parameterIndices.size();
}

//----------------------------------------------------------------------------------------------

std::string UnifiedLanProtocolMessage::getParameterAt(const unsigned int index) const
{
	std::string paramData;
	if(index < parameterIndices.size()) {
		int startIndex = parameterIndices.at(index);
		int length = 0;
		if((index+1) == parameterIndices.size()) {//last param
			length = messageData.size() - parameterIndices.at(index) - 2;
		}
		else {
			length = parameterIndices.at(index+1) - startIndex - 1;
		}
		paramData = messageData.substr(startIndex, length);
	}
	return paramData;
}

//----------------------------------------------------------------------------------------------

std::string UnifiedLanProtocolMessage::getMessageStringToSend() const
{
	std::string str(1, (char)messageCommand);
	//counter++;
	str.append(ucharToHexStr(messageCounter));
	str.append(messageData);
	str.append(msgEndChars);
	return str;
}

//----------------------------------------------------------------------------------------------

void UnifiedLanProtocolMessage::clear()
{
	messageCounter = (char)0x00;
	messageCommand = ULP_CMD_UNDEFINED;
	messageData.clear();
	parameterIndices.clear();
}

//----------------------------------------------------------------------------------------------

UnifiedLanProtocolCmd UnifiedLanProtocolMessage::getMessageCommand() const 
{
	return messageCommand;
}

//----------------------------------------------------------------------------------------------
unsigned char UnifiedLanProtocolMessage::getMessageCounter() const
{
	return messageCounter;
}

//----------------------------------------------------------------------------------------------

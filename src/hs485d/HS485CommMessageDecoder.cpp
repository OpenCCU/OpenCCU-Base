/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HS485CommMessageDecoder.h"
#include "HS485CommMessage.h"
#include <cstring>
#include <stdio.h>
#include <cinttypes>

HS485CommMessageDecoder::FrameDescription HS485CommMessageDecoder::FrameDescriptions[]=
{
};

HS485CommMessageDecoder::HS485CommMessageDecoder(void)
{
}

HS485CommMessageDecoder::~HS485CommMessageDecoder(void)
{
}

static const char* CtrlToString(int ctrl)
{
	static char s[32];
	if(!(ctrl&0x01)){
		snprintf(s, sizeof(s), "I[%d](%d", (ctrl>>1)&0x03, (ctrl>>5)&0x03);
		if(ctrl&(1<<7))strcat(s, ",Y");
		if(ctrl&(1<<4))strcat(s, ",F");
		if(ctrl&(1<<3))strcat(s, ",B");
		strcat(s, ")");
	}else if((ctrl&0x07)==0x03){
		snprintf(s, sizeof(s), "DISCOVERY(%d)", (ctrl>>3)+1);
	}else if((ctrl&0x97)==0x11){
		snprintf(s, sizeof(s), "ACK(%d", (ctrl>>5)&0x03);
		if(ctrl&(1<<3))strcat(s, ",B");
		strcat(s, ")");
	}else{
		snprintf(s, sizeof(s), "(%02X)", ctrl);
	}
	return s;
}

std::string HS485CommMessageDecoder::Format(const StructuredFrame& frame, std::string::size_type start, bool start_with_char)
{
	std::string result;
	char buffer[4];
	for(unsigned int i=start;i<frame.GetSize();i++){
		int c=(int)frame.GetByteData(i);
		if(start_with_char && i==start){
			if(c>' ' && c<=0x7e){
				result.append(1, (char)c);
				result+=' ';
				continue;
			}
		}
		snprintf(buffer, sizeof(buffer), "%02X ", c);
		result+=buffer;
	}
	return result;
}

std::string HS485CommMessageDecoder::ToString(HS485CommMessage* msg)
{
	static char buffer[256];
	std::string result;

	switch(msg->GetCommand()){
		case HS485CommMessage::CMD_DISCOVERY:
		{
			snprintf(buffer, sizeof(buffer), "Discovery #%d: ", msg->GetID()&0xff);
			result=buffer;
			result+=Format(*msg, 0, false);
			return result;
		}
		case HS485CommMessage::CMD_DISC_END:
			return "Discovery end";
		case HS485CommMessage::CMD_DISC_RESULT:
		{
			snprintf(buffer, sizeof(buffer), "Discovery result #%d: ", msg->GetID()&0xff);
			result=buffer;
			result+=Format(*msg, 0, false);
			return result;
		}
		case HS485CommMessage::CMD_ERROR:
		{
			snprintf(buffer, sizeof(buffer), "Error #%d: ", msg->GetID()&0xff);
			result=buffer;
			result+=Format(*msg, 0, false);
			return result;
		}
		case HS485CommMessage::CMD_EVENT:
		case HS485CommMessage::CMD_SEND:
				snprintf(buffer, sizeof(buffer), "%s #%d %08" PRIX32 " -> %08" PRIX32 ": ", CtrlToString(msg->GetCtrl()), msg->GetID(), msg->GetSenderAddress(), msg->GetReceiverAddress());
				result=buffer;
				result+=Format(*msg, msg->MapIndex(9), true);
				break;
		case HS485CommMessage::CMD_BOOT_SEND:
				snprintf(buffer, sizeof(buffer), "Bootloader %s #%d -> %08" PRIX32 ": ", CtrlToString(msg->GetCtrl()), msg->GetID(), msg->GetReceiverAddress());
				result=buffer;
				result+=Format(*msg, msg->MapIndex(9), true);
				break;
		case HS485CommMessage::CMD_RESPONSE:
		{
			snprintf(buffer, sizeof(buffer), "Response %s on #%d : ", CtrlToString(msg->GetCtrl()), msg->GetID());
			result=buffer;
			if((int)msg->GetSize()>msg->MapIndex(9)){
				result+=Format(*msg, msg->MapIndex(9), false);
			}
			break;
		}
	}
	if(!result.empty())return result;
	snprintf(buffer, sizeof(buffer), "#%d Undecoded frame cmd=0x%02X:\n", msg->GetID(), msg->GetCommand());
	return buffer;
}

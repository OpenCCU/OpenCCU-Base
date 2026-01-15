/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <HMWLGWTransportFrame.h>
#include <Logger.h>
#include <HMWLGWUtils.h>


HMWLGWTransportFrame::HMWLGWTransportFrame()
: expectedMsgSize(-1)
, deEscapePending(false)
{
}

HMWLGWTransportFrame::~HMWLGWTransportFrame()
{
}

std::string HMWLGWTransportFrame::getPayload()
{
	return payload;
}

void HMWLGWTransportFrame::setPayload(const std::string& payload)
{
	this->payload = payload;
}


std::string HMWLGWTransportFrame::getFrameData()
{
	std::string frame;
	frame.append(1, frameStartChar);
	frame.append(1, (unsigned char)(payload.size()&0xFF));
	frame.append( escape(payload) );
	return frame;
}

std::string HMWLGWTransportFrame::escape(const std::string& data) {
	std::string escapedStr;
	for(unsigned int i = 0; i < data.size() ; i++) {
		const char d = data.at(i);
		if( (d == frameStartChar) || (d == escapeChar) ) {
			escapedStr.append(1, escapeChar);
			escapedStr.append(1, (char)(d & 0x7f));
		}
		else {
			escapedStr.append(1, d);
		}
	}
	return escapedStr;
}


void HMWLGWTransportFrame::deEscapeChar(char* c)
{
	if(deEscapePending) {
		(*c) = (*c) | (char)0x80;
		deEscapePending = false;
	}
	else {
		if((*c) == escapeChar) {
			deEscapePending = true;
		}
	}
}

bool HMWLGWTransportFrame::addIncomingFrameData(const std::string& frameData, std::string& leftOver)
{
//	LOG(Logger::LOG_ALL, "HMWLGWTransportFrame::addIncomingFrameData()");
	for(unsigned int i = 0; i < frameData.size() ; i++) {
		char c = frameData.at(i);
		if(c == frameStartChar && payload.size() > 0) {//in case of connection loss it can happen...
			leftOver = frameData.substr(i);//...that there is an incomplete message which...
			payload.clear();//... will never be complete.
			expectedMsgSize = 0;
			return true;//finish empty message
		}
		deEscapeChar( &c );
		if(deEscapePending) {
			continue;
		}
		if( (payload.size() == 0) && (c != frameStartChar) ) {
			//new frame
			continue;//drop any characters not belonging to this message
		}
		payload.append(1, c);
		if(payload.size() == 2) {
			expectedMsgSize = (int)((unsigned char)payload.at(1));
			expectedMsgSize += 2; // start character + 1 byte length = 2 bytes
		}
		if( ((int)payload.size() >= expectedMsgSize) && (expectedMsgSize != -1)) {
			//if( (i+1) < frameData.size() ) {
			//	frameData.substr( (i+1) );
			//}
			leftOver.clear();
			leftOver.append( frameData.substr(i) );

			//extract payload
			//payload = extractPayload(payload);
			payload.erase(0, 2);////1 byte start char + 1 byte length
			return true;
		}
	}
	return false;
}



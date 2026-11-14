/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <CCU2SerialFrame.h>

#include <stdio.h>
#include <cstring>
#include <Logger.h>
#include <HM2Utils.h>

using namespace HM2;

CCU2SerialFrame::CCU2SerialFrame() 
: expectedMsgSize(-1)
, escapePending(false)
{                                 
}

//CCU2SerialFrame::CCU2SerialFrame(const std::string& serialFrameData)
//{
//    CCU2SerialFrame::payload = CCU2SerialFrame::extractPayload(serialFrameData);
//}
       
CCU2SerialFrame::~CCU2SerialFrame() 
{   
}

void CCU2SerialFrame::reset() {
	expectedMsgSize = -1;
	escapePending = false;
	payload.clear();
}

const std::string& CCU2SerialFrame::getPayload() const 
{
    return payload;
} 

void CCU2SerialFrame::setPayload(const std::string& payload) 
{
    this->payload = payload;
}

std::string CCU2SerialFrame::getFrameData() const {
    return assembleFrame();
}

std::string CCU2SerialFrame::assembleFrame() const 
{
    std::string frame;
    frame.append(1, frameStartChar);//start character
	frame.append(1, (char)((payload.size() >> 8) & 0xFF));//length, high byte
	frame.append(1, (char)(payload.size() & 0xFF));//length, low byte
    frame.append(payload);//payload
    //CRC16
    char crcLowByte, crcHighByte;
    calculateCRC(frame.c_str(), 1, (int)(payload.size()+2), crcHighByte, crcLowByte);
    frame.append(1, crcHighByte);
    frame.append(1, crcLowByte);
	frame = escape( frame );
    return frame;
}

std::string CCU2SerialFrame::toString(const int i) 
{
    char* buffer = new char[11];
    memset(buffer, 0, 11);
    snprintf(buffer, 11, "%d", i);
    std::string s(buffer);
    delete[] buffer;
    return s;
}

void CCU2SerialFrame::calculateCRC(const char* msg, const unsigned int offset, const unsigned int length, char& outHighByte, char& outLowByte) 
{
	unsigned short crc = 0xffff; //Initial value
	for(unsigned int i = offset; i < length+offset; i++) {
		crc = crc16_update(crc, msg[i]);
	}
	outLowByte = crc & 0xFF;
	outHighByte = (crc >> 8) & 0xFF;
}

std::string CCU2SerialFrame::extractPayload(const std::string& serialFrameData) 
{
    std::string pl;
	//std::string deEscapedStr = deEscape( serialFrameData );
    if(serialFrameData.size() > 5) {//start char + 2 length + 2 crc
		//deescape message
		
        //check crc
        char crcHighByte, crcLowByte;
        calculateCRC(serialFrameData.c_str(), 1, serialFrameData.size()-3, crcHighByte, crcLowByte);//size - 2 crc - startChar
        char msgHighByte = serialFrameData.at(serialFrameData.size()-2);
        char msgLowByte = serialFrameData.at(serialFrameData.size()-1);
        if( (crcHighByte == msgHighByte) && (crcLowByte == msgLowByte) ) {
               
/*          Length already checked in CCU2CommController  
            //extract length
            highByte = serialFrameData.at(1);
            lowByte = serialFrameData.at(2);
            unsigned short temp = 0;
            pChar = (char*)&temp;
            *pChar = lowByte;
            *(pChar+1) = highByte;
            int length = (int)temp;
            temp = 0;
*/
            pl = serialFrameData.substr(3, serialFrameData.size()-5);
        }
		else {
			LOG(Logger::LOG_WARNING, "CCU2SerialFrame::extractPayload(): Checksum error. Payload: %s", toDebugHexStr(serialFrameData).c_str());
		}
    }
    return pl;    
}

unsigned short CCU2SerialFrame::crc16_update(unsigned short crc_reg, unsigned char Val )
{
    unsigned char i;
    for (i = 0; i < 8; i++)
    {
      if (((crc_reg & 0x8000) >> 8) ^ (Val & 0x80))
      {
        crc_reg = (crc_reg << 1) ^ CRC16_POLY;
      }
      else
      {
        crc_reg = (crc_reg << 1);
      }
      Val <<= 1;
    }
    return crc_reg;
}

std::string CCU2SerialFrame::escape(const std::string& data) {
	std::string escapedStr(1, frameStartChar);
	for(unsigned int i = 1; i < data.size() ; i++) {
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

void CCU2SerialFrame::deEscapeChar(char* c) 
{
	(*c) = (*c) | (char)0x80;
}

//#include <HM2Utils.h>
bool CCU2SerialFrame::addFrameData(const std::string& frameData, std::string& leftOver) 
{
	//LOG(Logger::LOG_ALL, "CCU2SerialFrame::addFrameData(): Processing %s", toDebugHexStr(frameData).c_str());
	for(unsigned int i = 0; i < frameData.size() ; i++) {
		char c = frameData.at(i);
		switch(c)
		{
		case frameStartChar:
			if((!payload.empty())) {
				//incomplete frame detected -> drop
				LOG(Logger::LOG_ALL, "CCU2SerialFrame::addFrameData(): Corrupt data detected. Frame start character unexpected.");
				reset();
			}
			payload.assign(1, c);
			continue;
		case escapeChar:
			escapePending = true;
			continue;
		default:
			if(escapePending) {
				deEscapeChar(&c);
				escapePending = false;
			}
			payload.append(1, c);
			if(payload.size() == 3) {
				expectedMsgSize = 0;
				int high = ((int) payload.at(1)) & 0xff;
				int low =  ((int) payload.at(2)) & 0xff;
				expectedMsgSize = (high << 8) | low;
				expectedMsgSize += 5; // start character + 2 byte length + 2 byte crc
			}
			if( ((int)payload.size() >= expectedMsgSize) && (expectedMsgSize != -1)) {
				leftOver.clear();
				if(frameData.size() > (i+1))  {
					leftOver.append( frameData.substr(i+1) );
				}
				payload = extractPayload(payload);
				return true;
			}
			break;
		}
	}
	return false;
}


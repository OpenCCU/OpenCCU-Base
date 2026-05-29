/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <CCU2SerialFrameMod.h>

#include <stdio.h>
#include <cstring>

using namespace HM2Mod;

CCU2SerialFrameMod::CCU2SerialFrameMod()
: expectedMsgSize(-1)
, deEscapePending(false)
{                                 
}

//CCU2SerialFrame::CCU2SerialFrame(const std::string& serialFrameData)
//{
//    CCU2SerialFrame::payload = CCU2SerialFrame::extractPayload(serialFrameData);
//}
       
CCU2SerialFrameMod::~CCU2SerialFrameMod()
{   
}

void CCU2SerialFrameMod::reset() {
	expectedMsgSize = -1;
	deEscapePending = false;
	payload.clear();
}

const std::string& CCU2SerialFrameMod::getPayload() const
{
    return payload;
} 

void CCU2SerialFrameMod::setPayload(const std::string& payload)
{
    this->payload = payload;
}

std::string CCU2SerialFrameMod::getFrameData() const {
    return assembleFrame();
}

std::string CCU2SerialFrameMod::assembleFrame() const
{
    std::string frame;
    frame.append(1, frameStartChar);//start character
    unsigned short usLength = (unsigned short) payload.size();
    char* pChar = (char*)&usLength;
    frame.append(1, *(pChar+1));//length, high byte
    frame.append(1, *pChar);//length, low byte
    frame.append(payload);//payload
    //CRC16
    char crcLowByte, crcHighByte;
    calculateCRC(frame.c_str(), 1, (int)(payload.size()+2), crcHighByte, crcLowByte);
    frame.append(1, crcHighByte);
    frame.append(1, crcLowByte);
	frame = escape( frame );
    return frame;
}

std::string CCU2SerialFrameMod::toString(const int i)
{
    char* buffer = new char[11];
    memset(buffer, 0, 11);
    snprintf(buffer, 11, "%d", i);
    std::string s(buffer);
    delete[] buffer;
    return s;
}

void CCU2SerialFrameMod::calculateCRC(const char* msg, const unsigned int offset, const unsigned int length, char& outHighByte, char& outLowByte)
{
	unsigned short crc = 0xffff; //Initial value
	for(unsigned int i = offset; i < length+offset; i++) {
		crc = crc16_update(crc, msg[i]);
	}
	char* pChar = (char*)&crc;
    outLowByte = *pChar;
    outHighByte = *(pChar+1);
}

std::string CCU2SerialFrameMod::extractPayload(const std::string& serialFrameData)
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
            pl = serialFrameData.substr(3, serialFrameData.size()-5);
        }
    }
    return pl;    
}

unsigned short CCU2SerialFrameMod::crc16_update(unsigned short crc_reg, unsigned char Val )
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

std::string CCU2SerialFrameMod::escape(const std::string& data) {
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


void CCU2SerialFrameMod::deEscapeChar(char* c)
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

//#include <HM2Utils.h>
bool CCU2SerialFrameMod::addFrameData(const std::string& frameData, std::string& leftOver)
{
	//LOG(Logger::LOG_ALL, "CCU2SerialFrame::addFrameData(): Processing %s", toDebugHexStr(frameData).c_str());
		for(unsigned int i = 0; i < frameData.size() ; i++) {
			char c = frameData.at(i);
			switch(c)
			{
			case frameStartChar:
				if((!payload.empty())) {

					reset();
				}
				payload.assign(1, c);
				continue;
			case escapeChar:
				deEscapePending = true;
				continue;
			default:
				if(deEscapePending) {
					deEscapeChar(&c);
					deEscapePending = false;
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
					if((int)frameData.size() > (i+1))  {
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


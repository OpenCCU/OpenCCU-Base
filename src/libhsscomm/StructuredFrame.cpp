/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// StructuredFrame.cpp: Implementierung der Klasse StructuredFrame.
//
//////////////////////////////////////////////////////////////////////
#include "StructuredFrame.h"
#include <stdlib.h>
#include <stdio.h>

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

StructuredFrame::StructuredFrame()
{
	type_index=-1;
}

StructuredFrame::StructuredFrame(const std::string& data):data(data)
{
	type_index=-1;
}

StructuredFrame::~StructuredFrame()
{
}

void StructuredFrame::SetULongData(int address, uint32_t v)
{
	if(data.size()<address+sizeof(v))data.resize(address+sizeof(v));
	for(unsigned int i=address;i<address+sizeof(v);i++){
		data[i]=(v>>((sizeof(v)-1)*8))&0xff;
		v<<=8;
	}
}

uint32_t StructuredFrame::GetULongData(int address) const
{
	uint32_t retval=0;
	
	if(data.size()<address+sizeof(retval))return 0;
	for(unsigned int i=address;i<address+sizeof(retval);i++){
		retval<<=8;
		retval|=data[i];
	}
	return retval;
}

void StructuredFrame::SetUShortData(int address, unsigned short v)
{
	if(data.size()<address+sizeof(v))data.resize(address+sizeof(v));
	for(unsigned int i=address;i<address+sizeof(v);i++){
		data[i]=(v>>((sizeof(v)-1)*8))&0xff;
		v<<=8;
	}
}

unsigned short StructuredFrame::GetUShortData(int address) const
{
	unsigned short retval=0;
	
	if(data.size()<address+sizeof(retval))return 0;
	for(unsigned int i=address;i<address+sizeof(retval);i++){
		retval<<=8;
		retval|=data[i];
	}
	return retval;
}

void StructuredFrame::SetByteData(int address, unsigned char v)
{
	if(data.size()<address+sizeof(v))data.resize(address+sizeof(v));
	for(unsigned int i=address;i<address+sizeof(v);i++){
		data[i]=(v>>((sizeof(v)-1)*8))&0xff;
		v<<=8;
	}
}

unsigned char StructuredFrame::GetByteData(int address) const
{
	unsigned char retval=0;
	
	if(data.size()<address+sizeof(retval))return 0;
	for(unsigned int i=address;i<address+sizeof(retval);i++){
		retval<<=8;
		retval|=data[i];
	}
	return retval;
}

bool StructuredFrame::SetIntValue(int index_bytes, int index_bits, int size_bytes, int size_bits, uint32_t value)
{
	if(index_bytes<0)return false;
	if(!size_bytes){
		int mask=(0xff>>(8-size_bits));
		mask <<= index_bits;
		value <<= index_bits;
		value &= mask;
		int v=GetByteData(index_bytes);
		v&=~mask;
		v|=value;
		SetByteData(index_bytes, v);
	}else{
		for(int i=index_bytes+size_bytes-1;i>=index_bytes;i--){
			SetByteData(i, value&0xff);
			value>>=8;
		}
	}
	return true;
}

bool StructuredFrame::GetIntValue(int index_bytes, int index_bits, int size_bytes, int size_bits, uint32_t* value) const
{
	if(index_bytes<0)return false;
	if(!size_bytes){
		unsigned int v=GetByteData(index_bytes);
		unsigned int mask=(0xff>>(8-size_bits));
		v >>= index_bits;
		v&=mask;
		*value=v;
	}else{
		*value=0;
		for(int i=index_bytes;i<index_bytes+size_bytes;i++){
			unsigned int v=GetByteData(i);
			*value<<=8;
			*value|=v;
		}
		if(size_bits){
			unsigned int v=GetByteData(index_bytes+size_bytes);
			*value <<= 8;
			*value |= v;
			*value &= 0xffffffff >> (32-size_bytes*8-size_bits);
		}
	}
	return true;
}

bool StructuredFrame::GetStringValue(int index, int size, std::string* value) const
{
	value->clear();
	for(int i=index;i<index+size;i++)value->append(1, (char)GetByteData(i));
	return true;
}

bool StructuredFrame::SetStringValue(int index, int size, const std::string& value)
{
	for(int i=0;i<size;i++){
		if(i<(int)value.size())SetByteData(index+i, value[i]);
		else SetByteData(index+i, 0);
	}
	return true;
}

/*static*/ unsigned int StructuredFrame::FieldIdFromString(const std::string& s)
{
	int by_pos=0;
	int bi_pos=0;
	int by_size=1;
	int bi_size=0;

	std::string::size_type colon_pos=s.find(':');
	std::string::size_type dot_pos=s.find('.');
	if(dot_pos>=colon_pos){
		by_pos=strtoul(s.substr(0, colon_pos).c_str(), NULL, 0);
	}else{
		by_pos=strtoul(s.substr(0, dot_pos).c_str(), NULL, 0);
		bi_pos=strtoul(s.substr(dot_pos+1, colon_pos-dot_pos).c_str(), NULL, 0);
	}
	if(colon_pos!=std::string::npos){
		dot_pos=s.find('.', colon_pos);
		if(dot_pos==std::string::npos){
			by_size=strtoul(s.substr(colon_pos+1).c_str(), NULL, 0);
		}else{
			by_size=strtoul(s.substr(colon_pos+1, dot_pos-colon_pos-1).c_str(), NULL, 0);
			bi_size=strtoul(s.substr(dot_pos+1).c_str(), NULL, 0);
		}
	}
	return STRUCTURED_FRAME_FIELD_INT(by_pos, bi_pos, by_size, bi_size);
}

std::string StructuredFrame::ToString() const
{
	std::string s;
	char buffer[4];
	for(unsigned int i=0;i<data.size();i++){
		snprintf(buffer, sizeof(buffer), "%02X", (int)(unsigned char)data[i]);
		s+=buffer;
	}
	return s;
}

bool StructuredFrame::FromString(const std::string& s)
{
	data.clear();
	for(unsigned int i=0;(2*i+1)<s.size();i++){
		data.append(1, (char)strtol(s.substr(2*i, 2).c_str(), NULL, 16));
	}
	return true;
}

unsigned int StructuredFrame::GetSize() const
{
	return data.size();
}

bool StructuredFrame::Resize(unsigned int new_size)
{
	data.resize(new_size);
	return true;
}

bool StructuredFrame::SetType(unsigned int type)
{
	if(type_index<0)return false;
	SetByteData(type_index, type&0xff);
	if((type>>8)&0xff)SetByteData((type>>8)&0xff, (type>>16)&0xff);
	return true;
}

bool StructuredFrame::MatchType(unsigned int type) const
{
	if(type_index<0)return false;
	if( GetByteData(type_index) != (type&0xff) )return false;
	if( (((type>>8)&0xff) != 0) && (GetByteData((type>>8)&0xff) != ((type>>16)&0xff)))return false;
	return true;
}

StructuredFrame::ReceiverType StructuredFrame::GetReceiverType() const
{
	return RCV_UNKNOWN;
}

bool StructuredFrame::operator==(const StructuredFrame& sf) const
{
	return data.compare(sf.data)==0;
}

bool StructuredFrame::operator!=(const StructuredFrame& sf) const
{
	return !(*this == sf);
}

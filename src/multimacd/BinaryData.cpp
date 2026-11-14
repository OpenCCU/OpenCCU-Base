/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BinaryData.h"
#include <stdio.h>
#include <string.h>

BinaryData::BinaryData(void)
{
}

BinaryData::BinaryData(const uint8_t* data, size_t size)
{
	assign( data, data + size );
}

BinaryData::BinaryData(const char* s)
{
	assign( (const uint8_t*)s, (const uint8_t*)s + strlen(s) );
}

BinaryData::BinaryData(const std::vector<uint8_t>& data)
{
	this->assign( data.begin(), data.end() );
}

BinaryData::BinaryData( uint8_t v)
{
	SetUInt8Value(0, v);
}

BinaryData::BinaryData( uint16_t v)
{
	SetUInt16Value(0, v);
}

BinaryData::BinaryData( uint32_t v)
{
	SetUInt32Value(0, v);
}

BinaryData::~BinaryData(void)
{
}

uint8_t BinaryData::GetUInt8Value( size_t index )const
{
	uint8_t v = 0;
	size_t value_size = 1;
	for( size_t i=index; i<index+value_size && i<size(); i++ )
	{
		v <<= 8;
		v |= at(i);
	}
	return v;
}

uint16_t BinaryData::GetUInt16Value( size_t index )const
{
	uint16_t v = 0;
	size_t value_size = 2;
	for( size_t i=index; i<index+value_size && i<size(); i++ )
	{
		v <<= 8;
		v |= at(i);
	}
	return v;
}

uint32_t BinaryData::GetUInt24Value( size_t index )const
{
	uint32_t v = 0;
	size_t value_size = 3;
	for( size_t i=index; i<index+value_size && i<size(); i++ )
	{
		v <<= 8;
		v |= at(i);
	}
	return v;
}

uint32_t BinaryData::GetUInt32Value( size_t index )const
{
	uint32_t v = 0;
	size_t value_size = 4;
	for( size_t i=index; i<index+value_size && i<size(); i++ )
	{
		v <<= 8;
		v |= at(i);
	}
	return v;
}

std::string BinaryData::GetStringValue( size_t index )const
{
	std::string s;
	for( size_t i=index; i<size() && at(i); i++ )
	{
		s.append(1, (char)at(i) );
	}
	return s;
}


BinaryData BinaryData::GetRange( size_t index, size_t size )const
{
	BinaryData result;
	result.resize( size );
	for( size_t i=index; i<index+size && i<this->size(); i++ )
	{
		result[i-index] = at(i);
	}
	return result;
}

void BinaryData::GetBinaryData( uint8_t* buffer, size_t index, size_t size )const
{
	for( size_t i=index; i<index+size; i++ )
	{
		if( i<this->size() )buffer[i-index] = at(i);
		else buffer[i-index] = 0;
	}
}


void BinaryData::SetUInt8Value( size_t index, uint8_t value )
{
	size_t value_size = 1;
	if( size() < index + value_size )resize( index + value_size );

	for( size_t i = 0; i < value_size; i++ )
	{
		at(i + index) = value >> ( (value_size - i - 1) * 8 );
	}
}

void BinaryData::SetUInt16Value( size_t index, uint16_t value )
{
	size_t value_size = 2;
	if( size() < index + value_size )resize( index + value_size );

	for( size_t i = 0; i < value_size; i++ )
	{
		at(i + index) = value >> ( (value_size - i - 1) * 8 );
	}
}

void BinaryData::SetUInt24Value( size_t index, uint32_t value )
{
	size_t value_size = 3;
	if( size() < index + value_size )resize( index + value_size );

	for( size_t i = 0; i < value_size; i++ )
	{
		at(i + index) = (uint8_t)(value >> ( (value_size - i - 1) * 8 ));
	}
}

void BinaryData::SetUInt32Value( size_t index, uint32_t value )
{
	size_t value_size = 4;
	if( size() < index + value_size )resize( index + value_size );

	for( size_t i = 0; i < value_size; i++ )
	{
		at(i + index) = (uint8_t)(value >> ( (value_size - i - 1) * 8 ));
	}
}

void BinaryData::SetStringValue( size_t index, const std::string& s )
{
	if( size() < index + s.size() )resize( index + s.size() );

	for( size_t i = 0; i < s.size(); i++ )
	{
		at(i + index) = s[i];
	}
}

void BinaryData::SetRange( size_t index, const BinaryData& range )
{
	if( size() < index + range.size() )resize( index + range.size() );

	for( size_t i = 0; i < range.size(); i++ )
	{
		at(i + index) = range[i];
	}
}

void BinaryData::Fill( size_t start, size_t length, uint8_t c )
{
	if( size() < start + length )resize( start + length );
	for( size_t i = 0; i < length; i++ )
	{
		at(i + start) = c;
	}
}


std::string BinaryData::ToString()const
{
	char buffer[8];
	std::string s;
	for( size_t i=0; i<size(); i++ )
	{
		if( i>0 ) s += " ";
		snprintf(buffer, sizeof(buffer), "%02X", at(i) );
		s += buffer;
	}
	return s;
}

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BidcosAesKey.h"
#include <Logger.h>

#define OBFUSCATED_DEFAULT_KEY "\xA7\x0B\x9F\x08\xEA\xEA\x34\xAF\x55\x91\x9F\x98\x26\xE7\xD0\xFA"


BidcosAesKey::BidcosAesKey()
{
	_index = 0;
	_data.resize( 16 );
}

BidcosAesKey::BidcosAesKey(uint8_t index, const BinaryData& data)
{
	_index = index;
	_data = data;
	_data.resize( 16 );
}

BidcosAesKey::~BidcosAesKey(void)
{
}

uint8_t BidcosAesKey::GetIndex()const
{
	return _index;
}

uint8_t* BidcosAesKey::GetSessionKey( const BinaryData& random )const
{
	//LOG( Logger::LOG_DEBUG, "BidcosAesKey::GetSessionKey() base key = %s", _data.ToString().c_str() );

	uint8_t* buffer = new uint8_t[16];
	if( _index > 0 )
	{
		_data.GetBinaryData( buffer, 0, 16 );
	}else{
		uint32_t v = 0xc04db6d9;
		for( int i=0; i<16; i++ )
		{
			v = v * 0xbcbc5910 + 0x5b570db7;
			buffer[i] = (OBFUSCATED_DEFAULT_KEY[i] ^ (v >> 19)) & 0xff;
			v += buffer[i];
		}
	}
	for( int i=0; i<6; i++ )
	{
		buffer[i] ^= random.GetUInt8Value(i);
	}
	return buffer;
}

const BinaryData& BidcosAesKey::GetBytes()const
{
	return _data;
}

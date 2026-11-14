/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BidcosPeerContext.h"


BidcosPeerContext::BidcosPeerContext()
{
	_rfAddress = 0;
	_aesKeyIndex = 0;
	_flags = 0;
	_aesChannelMask = 0;
}


BidcosPeerContext::~BidcosPeerContext(void)
{
}

void BidcosPeerContext::Init(uint32_t rfAddress, uint8_t aesKeyIndex, int flags)
{
	_rfAddress = rfAddress;
	_aesKeyIndex = aesKeyIndex;
	_flags = flags | Flag_Active;
	if( _flags & Flag_NeedsWakeup )
	{
		_flags |= Flag_FirstWakeup;
	}
	_aesChannelMask = 0;
}

uint32_t BidcosPeerContext::GetRfAddress()const
{
	return _rfAddress;
}

void BidcosPeerContext::Update( uint8_t aesKeyIndex, int flags )
{
	_aesKeyIndex = aesKeyIndex;
	if( (flags & Flag_NeedsWakeup) && !(_flags & Flag_NeedsWakeup) )
	{
		flags |= Flag_FirstWakeup;
	}
	_flags = flags | Flag_Active;
}

void BidcosPeerContext::ResetFlags( int flags )
{
	_flags &= ~flags;
}

void BidcosPeerContext::SetFlags( int flags )
{
	_flags |= flags;
}

bool BidcosPeerContext::IsActive()const
{
	return (_flags & Flag_Active) != 0;
}

void BidcosPeerContext::Deactivate()
{
	_flags = 0;
}

void BidcosPeerContext::SetAesChannelMask( uint64_t aesChannelMask )
{
	_aesChannelMask = aesChannelMask;
}

uint64_t BidcosPeerContext::GetAesChannelMask()const
{
	return _aesChannelMask;
}

bool BidcosPeerContext::NeedsAesForChannel( uint8_t channelIndex )
{
	return (_aesChannelMask & (uint64_t(1)<<channelIndex)) != 0;
}

int BidcosPeerContext::GetFlags()const
{
	return _flags;
}

uint8_t BidcosPeerContext::GetAesKeyIndex()const
{
	return _aesKeyIndex;
}

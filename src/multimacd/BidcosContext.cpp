/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BidcosContext.h"
#include "Sysutils.h"
#include <Logger.h>

BidcosContext::BidcosContext(void)
{
	_numberOfActivePeers = 0;
	_appVersion = 0;
	_blVersion = 0;
	_defaultRfAddress = 0;
	_rfAddress = 0;
	_timeInfoUtcOffset = 127;
	_timeInfoClockOffset = 0;
}


BidcosContext::~BidcosContext(void)
{
}

void BidcosContext::SetRfAddress( uint32_t rfAddress )
{
	_rfAddress = rfAddress;
}

uint32_t BidcosContext::GetRfAddress()const
{
	return _rfAddress;
}

void BidcosContext::SetAesKey( BidcosAesKey::KeyId type, const BidcosAesKey& key )
{
	if( type >= BidcosAesKey::KeyId_Current && type <= BidcosAesKey::KeyId_Temp )
	{
		_aesKeys[ (int)type ] = key;
	}
}

const BidcosAesKey& BidcosContext::GetAesKeyByType( BidcosAesKey::KeyId type )const
{
	if( type >= BidcosAesKey::KeyId_Current && type <= BidcosAesKey::KeyId_Temp )
	{
		return _aesKeys[ (int)type ];
	}
	return _aesKeys[1];
}

const BidcosAesKey& BidcosContext::GetAesKeyByIndex( uint8_t index )const
{
	for( int i=0; i<4; i++ )
	{
	if( index == _aesKeys[i].GetIndex() )
	{
		return _aesKeys[i];
	}
	}
	return _aesKeys[BidcosAesKey::KeyId_Temp];
}

bool BidcosContext::IsKnownAesKeyIndex(uint8_t index) const {
	return (index == _aesKeys[BidcosAesKey::KeyId_Default].GetIndex() ||
			index == _aesKeys[BidcosAesKey::KeyId_Current].GetIndex()  ||
			index == _aesKeys[BidcosAesKey::KeyId_Previous].GetIndex());
}

bool BidcosContext::PeerAdd( uint32_t peerAddress, uint8_t aesKeyIndex, bool requestWakeup, bool lazyConfig, uint16_t* numberOfPeers, uint64_t* authChannels )
{
	int flags = (requestWakeup ? BidcosPeerContext::Flag_NeedsWakeup : 0) | (lazyConfig ? BidcosPeerContext::Flag_LazyConfig : 0);
	PeerMap::iterator it = _mapPeers.find( peerAddress );
	if( it != _mapPeers.end() )
	{
		if( !it->second.IsActive() )
		{
			_numberOfActivePeers++;
		}
		it->second.Update( aesKeyIndex, flags );
		*authChannels = it->second.GetAesChannelMask();
	}else{
		_mapPeers[peerAddress].Init( peerAddress, aesKeyIndex, flags );
		_numberOfActivePeers++;
		*authChannels = 0;
	}
	*numberOfPeers = _numberOfActivePeers;
	return true;
}

bool BidcosContext::PeerRemove( uint32_t peerAddress, uint16_t* numberOfPeers )
{
	PeerMap::iterator it = _mapPeers.find( peerAddress );
	if( it != _mapPeers.end() )
	{
		if( it->second.IsActive() )
		{
			_numberOfActivePeers--;
		}
		it->second.Deactivate();
	}
	*numberOfPeers = _numberOfActivePeers;
	return true;
}

bool BidcosContext::PeerSetAuthChannels( uint32_t peerAddress, uint64_t authChannels, uint64_t mask )
{
	PeerMap::iterator it = _mapPeers.find( peerAddress );
	if( it != _mapPeers.end() )
	{
		if( it->second.IsActive() )
		{
			uint64_t activeChannels = it->second.GetAesChannelMask();
			activeChannels = (activeChannels & (~mask)) | (authChannels & mask);
			it->second.SetAesChannelMask( activeChannels );
			return true;
		}
	}
	return false;
}

bool BidcosContext::PeerSetAesKeyIndex( uint32_t peerAddress, uint8_t aesKeyIndex )
{
	PeerMap::iterator it = _mapPeers.find( peerAddress );
	if( it != _mapPeers.end() )
	{
		if( it->second.IsActive() )
		{
			it->second.Update( aesKeyIndex, it->second.GetFlags() );
			return true;
		}
	}
	return false;
}

bool BidcosContext::GetPeerContext( uint32_t peerAddress, BidcosPeerContext* context )const
{
	PeerMap::const_iterator it = _mapPeers.find( peerAddress );
	if( it != _mapPeers.end() )
	{
		if( it->second.IsActive() )
		{
			*context = it->second;
			return true;
		}
	}
	return false;
}

bool BidcosContext::GetNextPeerContext( uint32_t peerAddress, BidcosPeerContext* context )const
{
	PeerMap::const_iterator it = _mapPeers.upper_bound( peerAddress );
	while( it != _mapPeers.end() )
	{
		if( it->second.IsActive() )
		{
			*context = it->second;
			return true;
		}
		it = _mapPeers.upper_bound( it->first );
	}
	return false;
}

uint16_t BidcosContext::GetNumberOfPeers()const
{
	return _numberOfActivePeers;
}

void BidcosContext::SetTimeInfo( int8_t utcOffset, uint32_t seconds )
{
	_timeInfoUtcOffset = utcOffset;
	_timeInfoClockOffset = seconds - TimeBaseOffset2k - Sysutils::GetMonotonicTimeSeconds();
}

bool BidcosContext::GetTimeInfoPayload( BinaryData& payload )
{
	if( _timeInfoUtcOffset <= 48 )
	{
		uint32_t seconds = Sysutils::GetMonotonicTimeSeconds() + _timeInfoClockOffset;
		payload.SetUInt8Value( 0, 2 );
		payload.SetUInt8Value( 1, _timeInfoUtcOffset & 0x3f );
		payload.SetUInt32Value( 2, seconds );
		return true;
	}else{
		payload.SetUInt8Value( 0, 6 );
		payload.SetUInt32Value( 2, 0 );
		return false;
	}
}

bool BidcosContext::PeerSetFlags( uint32_t peerAddress, int flags )
{
	PeerMap::iterator it = _mapPeers.find( peerAddress );
	if( it != _mapPeers.end() )
	{
		it->second.SetFlags( flags );
		return true;
	}
	return false;
}

bool BidcosContext::PeerResetFlags( uint32_t peerAddress, int flags )
{
	PeerMap::iterator it = _mapPeers.find( peerAddress );
	if( it != _mapPeers.end() )
	{
		it->second.ResetFlags( flags );
		return true;
	}
	return false;
}

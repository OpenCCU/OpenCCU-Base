/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include <stdint.h>
#include <map>
#include <string>
#include "BidcosAesKey.h"
#include "BidcosPeerContext.h"
#include "BinaryData.h"

class BidcosContext
{
public:
	BidcosContext(void);
	~BidcosContext(void);
	void SetRfAddress( uint32_t rfAddress );
	uint32_t GetRfAddress()const;

	void SetAesKey( BidcosAesKey::KeyId type, const BidcosAesKey& key );
	const BidcosAesKey& GetAesKeyByType( BidcosAesKey::KeyId type )const;
	const BidcosAesKey& GetAesKeyByIndex( uint8_t index )const;
	bool IsKnownAesKeyIndex(uint8_t index) const;

	bool PeerAdd( uint32_t peerAddress, uint8_t aesKeyIndex, bool requestWakeup, bool lazyConfig, uint16_t* numberOfPeers, uint64_t* authChannels );
	bool PeerRemove( uint32_t peerAddress, uint16_t* numberOfPeers );
	bool PeerSetAuthChannels( uint32_t peerAddress, uint64_t authChannels, uint64_t mask );
	bool PeerSetFlags( uint32_t peerAddress, int flags );
	bool PeerResetFlags( uint32_t peerAddress, int flags );

	bool PeerSetAesKeyIndex( uint32_t peerAddress, uint8_t aesKeyIndex );
	bool GetPeerContext( uint32_t peerAddress, BidcosPeerContext* context )const;
	bool GetNextPeerContext( uint32_t peerAddress, BidcosPeerContext* context )const;
	uint16_t GetNumberOfPeers()const;
	void SetTimeInfo( int8_t utcOffset, uint32_t seconds );
	bool GetTimeInfoPayload( BinaryData& payload );
private:
	enum 
	{
		TimeBaseOffset2k = 946684800
	};
	std::string _serialNumber;
	uint32_t _appVersion;
	uint32_t _blVersion;
	uint32_t _defaultRfAddress;
	uint32_t _rfAddress;
	BidcosAesKey _aesKeys[4];
	typedef std::map<uint32_t, BidcosPeerContext> PeerMap;
	PeerMap _mapPeers;
	uint16_t _numberOfActivePeers;
	int8_t _timeInfoUtcOffset;
	uint32_t _timeInfoClockOffset;
};


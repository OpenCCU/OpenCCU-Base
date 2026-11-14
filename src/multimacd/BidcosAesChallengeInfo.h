/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "BidcosTelegram.h"

class BidcosContext;

class BidcosAesChallengeInfo
{
public:
	BidcosAesChallengeInfo(const BidcosTelegram& originalTelegram, uint8_t* challenge, uint8_t keyIndex, int rssi);
	BidcosAesChallengeInfo(const BidcosTelegram& originalTelegram);
	~BidcosAesChallengeInfo(void);
	bool CheckSolution( const BidcosContext& bidcosContext, const BidcosTelegram& solutionTelegram );
	bool CheckAck( const BidcosTelegram& ackTelegram );
	uint32_t GetAckTailData();
	uint32_t GetNakTailData();
	const BidcosTelegram& GetOriginalTelegram()const;
	bool IsExpired()const;
	BidcosTelegram CalculateSolution(const BidcosContext& bidcosContext, const BidcosTelegram& challengeTelegram, const uint8_t* random);
	uint8_t GetKeyIndex()const;

	void SetRssi( int rssi );
	int GetRssi()const;

private:

	BidcosTelegram _originalTelegram;
	uint8_t _challenge[6];
	uint8_t _keyIndex;
	uint32_t _ackTailData;
	uint32_t _nakTailData;
	uint32_t _expirationTime;
	int _rssi;
};


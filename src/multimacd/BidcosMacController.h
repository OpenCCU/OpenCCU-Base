/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#define __STDC_LIMIT_MACROS
#include <stdint.h>
#include <map>
#include "BidcosTelegram.h"
#include "BidcosAesKey.h"
#include "SerialFrame/HmLegacyFrameBidcos.h"

/**
 *
 */
class LowLevelMacFrameRxTelegram;
class LowLevelMacFrameResponse;
class LowLevelMacFrame;
class SubsystemBidcos;
class BidcosContext;
class BidcosAesChallengeInfo;

class BidcosMacController
{
public:
	enum TxRate
	{
		TxRate_10k,
		TxRate_100k,
	};
	BidcosMacController(SubsystemBidcos& subsystem);
	~BidcosMacController(void);
	bool SendBidcosTelegram( const HmLegacyFrameBidcosTxTelegram& bidcosTxTelegramFrame );
	void OnLowLevelMacFrame( LowLevelMacFrame& lowLevelMacFrame );
	void OnTimeInfoChanged();
	void OnCyclicCall();
	void SetCsmaCaEnabled( bool enabled );
	bool GetCsmaCaEnabled()const;
	void SetTxRate( TxRate txRate );
	TxRate GetTxRate()const;
	uint32_t GetSleepTime();
private:
	enum AckAction
	{
		AckAction_NotForUs,
		AckAction_NotBidirectional,
		AckAction_Ack,
		AckAction_AesChallenge,
		AckAction_AesAck,
		AckAction_Wokenup,
		AckAction_Wakeup = 0x100,
		AckAction_CallCcu = 0x200,
		AckActionMask = 0xff,
	};

	void OnLowLevelMacRx( LowLevelMacFrameRxTelegram& lowLevelMacTelegram );
	void OnLowLevelMacResponse( LowLevelMacFrameResponse& lowLevelMacResponse );
	void OnRxAesChallenge( const BidcosTelegram& challengeTelegram, uint32_t receiveTime );
	void OnRxAesAck( const BidcosTelegram& ackTelegram, int rssi );
	void OnAckTimeout();
	void OnAesAckTimeout();
	void OnAesSolutionTimeout();
	void RememberAesChallenge( BidcosAesChallengeInfo* challengeInfo );
	bool TransformBeforeSend( BidcosTelegram& telegram );

	int GetAckActionForIncomingTelegram(const BidcosTelegram& telegram, uint8_t* keyIndex);

	bool GetRandomNumber(uint8_t* buffer, size_t size);

	void SendPlainAck( const BidcosTelegram& bidcosTelegram, uint32_t frameStartTime, bool wakeup );
	void SendAckWithPayload( const BidcosTelegram& bidcosTelegram, uint32_t frameStartTime, bool wakeup, const BinaryData& payload );
	void SendTimeInfoResponse( const BidcosTelegram& bidcosTelegram, uint32_t frameStartTime );

	void SendAesChallenge( const BidcosTelegram& bidcosTelegram, uint32_t frameStartTime, uint8_t keyIndex, int rssi, bool wakeup );
	bool CheckAesSolutionAndSendAesAck( const BidcosTelegram& solutionTelegram, uint32_t frameStartTime, bool wakeup );
	void SendCallCcuFrame( const BidcosTelegram& bidcosTelegram, uint32_t frameStartTime );
	void SendWakeupFrame( const BidcosTelegram& bidcosTelegram, uint32_t frameStartTime );

	void CleanupAesChallengeInfoMap();

	void ResendAutoTxFrame();

	void SendCyclicTimeInfo();

	enum
	{
		CoproTimeout = 5000,
		AckTimeoutRegular = 250,
		AckTimeoutUpdate = 1500,
		AckDelayRegular = 125,
		AckDelayRepeated = 80,
		AckDelayCallCcu = 90,
		TxBusyTimeRegular = 250,
		TxBusyTimeRepeated = 205,
		CcaNextTryDelay = 16,
		BidcosPreambleBits = 32,
		BidcosSyncwordSize = 32,
		BidcosLengthBits = 8,
		BidcosCrcBits = 16,
		BidcosBitRate = 10000,
		MaxNumberOfRepetitions = 3,
		MaxSendAttemptsFastRetry = 3,
		MaxSendAttemptsSlowRetry = 32,
		CoproBusySlowRetryDelay = 16,
		MaxTripleBurstCounter = 3,
		TripleBurstSlotLengthMs = 350,
		CyclicTimeInfoInterval = 6 * 3600 * 1000,
		KeyExchangeCheckNumber = 0x7E296FA5,
	};

	enum TxState
	{
		TxState_Idle,
		TxState_WaitTxDelay,
		TxState_WaitCoproResponse,
		TxState_WaitAck,
		TxState_WaitCoproResponseAesAck,
		TxState_WaitAesAck,
		TxState_WaitCoproResponseAesSolution,
		TxState_WaitAesSolution,
		TxState_Interrupted,
		TxState_CoproBusy,
	};

	enum AutoTxState
	{
		AutoTxState_Idle,
		AutoTxState_Ack,
		AutoTxState_AesChallenge,
		AutoTxState_AesAck,
		AutoTxState_CallCcu,
		AutoTxState_Wakeup,
		AutoTxState_TimeInfo
	};

	void NextSendAttempt();
	uint16_t CalculateFrameStartTime( LowLevelMacFrameRxTelegram& frame );
	std::string printAutoTxState(AutoTxState state);

	BidcosTelegram _curTxTelegram;
	HmLegacyFrameBidcosTxTelegram::BurstMode _curTxBurstMode;
	BidcosTelegram _curAutoTxTelegram;

	int _sendAttemptCount;
	int _repetitionCount;

	AutoTxState _autoTxState;
	int _autoTxLowLevelMacOptions;

	SubsystemBidcos& _subsystem;
	BidcosContext& _bidcosContext;

	uint32_t _txTimeoutTimeRegular;
	uint32_t _autoTxTimeoutTime;
	uint32_t _txTimeoutTimeCyclicTimeInfo;

	uint32_t _autoTxFrameId;
	uint32_t _regularFrameId;
	uint32_t _tripleBurstFirstSlotStartTimeHost;
	uint16_t _tripleBurstFirstSlotStartTimeCopro;
	int _tripleBurstCounter;

	BidcosAesChallengeInfo* _txAesChallengeInfo;
	uint8_t _curTxSequenceCounter;

	TxState _txState;

	int _urandomFd;

	bool _csmaCaEnabled;
	TxRate _txRate;


	typedef std::map<uint32_t, BidcosAesChallengeInfo*> MapPeerAddressToAesChallengeInfo;
	MapPeerAddressToAesChallengeInfo _mapPeerAddressToAesChallengeInfo;

	/*To remember that we got an aes-ack timeout (OnAesAckTimeout).
	 * Some devices do not respond to send repetitions after failed authentication. This breaks reporting unknown key to rfd because NextSendAttempt
	 * overwrites autoTxState and txState so that we do not know about failed authentication anymore...
	 * */
	bool _waitAesAckTimedOut;

};


/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BidcosMacController.h"
#include "SubsystemBidcos.h"
#include "BidcosTelegram.h"
#include "BidcosAesChallengeInfo.h"
#include "SerialFrame/LowLevelMacFrame.h"
#include "Sysutils.h"
#include "Logger.h"
#include <string.h>
#include <stdlib.h>
#include <openssl/evp.h>

#ifndef WIN32
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <errno.h>
#endif

#undef min
#undef max

BidcosMacController::BidcosMacController(SubsystemBidcos& subsystem)
	: _subsystem( subsystem ), _bidcosContext( subsystem._bidcosContext )
{
	_sendAttemptCount = 0;
	_repetitionCount = 0;
	_autoTxFrameId = 0;
	_regularFrameId = 0;
	_curTxSequenceCounter = 0;
	_txTimeoutTimeRegular = Sysutils::GetMonotonicTime();
	_autoTxTimeoutTime = Sysutils::GetMonotonicTime();
	_txTimeoutTimeCyclicTimeInfo = Sysutils::GetMonotonicTime() + 360000; //first timeinfo after 6 minutes
	_tripleBurstFirstSlotStartTimeHost = Sysutils::GetMonotonicTime();
	_tripleBurstFirstSlotStartTimeCopro = 0;
	_tripleBurstCounter = 0;
	_curTxBurstMode = HmLegacyFrameBidcosTxTelegram::BurstMode_None;
	_txAesChallengeInfo = 0;

	_txState = TxState_Idle;
	_autoTxState = AutoTxState_Idle;
	_autoTxLowLevelMacOptions = 0;
	_csmaCaEnabled = true;
	_txRate = TxRate_10k;
	_urandomFd = -1;
	_waitAesAckTimedOut = false;
}


BidcosMacController::~BidcosMacController(void)
{
	while( _mapPeerAddressToAesChallengeInfo.size() )
	{
		delete _mapPeerAddressToAesChallengeInfo.begin()->second;
		_mapPeerAddressToAesChallengeInfo.erase( _mapPeerAddressToAesChallengeInfo.begin() );
	}
	if( _txAesChallengeInfo )
	{
		delete _txAesChallengeInfo;
	}
}

bool BidcosMacController::TransformBeforeSend( BidcosTelegram& telegram )
{
	if(telegram.GetFrameType() == BidcosTelegram::FrameType_AesContainer)
	{
		BinaryData payload = telegram.GetPayload();
		uint8_t containerType = payload.GetUInt8Value(0);
		if( containerType == BidcosTelegram::AesContainerType_KeyExchange )
		{
			BidcosAesKey curAesKey = _bidcosContext.GetAesKeyByType( BidcosAesKey::KeyId_Current );
			uint8_t encryptionKeyIndex = payload.GetUInt8Value( 1 );
			uint8_t keyPart = encryptionKeyIndex & 0x01;
			encryptionKeyIndex >>= 1;
			payload.SetUInt8Value( 1, (curAesKey.GetIndex() << 1) | keyPart );
			payload.SetRange( 2, curAesKey.GetBytes().GetRange( 8 * keyPart, 8 ) );
			uint16_t randomNumber;
			GetRandomNumber( (uint8_t*)&randomNumber, sizeof( randomNumber ) );
			payload.SetUInt16Value( 10, randomNumber );
			payload.SetUInt32Value( 12, KeyExchangeCheckNumber );
			LOG( Logger::LOG_DEBUG, "KeyExchange payload = %s", payload.ToString().c_str() );

			uint8_t* key = _bidcosContext.GetAesKeyByIndex( encryptionKeyIndex ).GetSessionKey( BinaryData() );
			EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
			EVP_EncryptInit_ex( ctx, EVP_aes_128_ecb(), NULL, key, NULL);
			// raw single-block ECB, no PKCS7 padding
			EVP_CIPHER_CTX_set_padding( ctx, 0 );
			
			uint8_t buffer[16];
			payload.GetBinaryData( buffer, 0, 16 );
			int size;
			EVP_EncryptUpdate( ctx, buffer, &size, buffer, 16 );
			delete[] key;
			EVP_CIPHER_CTX_free( ctx );

			telegram.SetPayload( BinaryData( buffer, 16 ) );
			return true;

		}
	}
	return false;
}

bool BidcosMacController::SendBidcosTelegram( const HmLegacyFrameBidcosTxTelegram& bidcosTxTelegramFrame )
{
	if( _txState != TxState_Idle )
	{
		return false;
	}
	_curTxTelegram = BidcosTelegram( bidcosTxTelegramFrame.GetPayload() );
	TransformBeforeSend( _curTxTelegram );
	_sendAttemptCount = 0;
	_repetitionCount = 0;
	_curTxBurstMode = bidcosTxTelegramFrame.GetBurstMode();
	_curTxSequenceCounter = bidcosTxTelegramFrame.GetSequenceCounter();
	_tripleBurstCounter = 0;
	_waitAesAckTimedOut = false;

	if( bidcosTxTelegramFrame.GetSendDelay() )
	{
		_txTimeoutTimeRegular = Sysutils::GetMonotonicTime() + bidcosTxTelegramFrame.GetSendDelay();
		_txState = TxState_WaitTxDelay;
		LOG( Logger::LOG_DEBUG, "_txState = TxState_WaitTxDelay" );
	} else if( _autoTxState == AutoTxState_Idle )
	{
		NextSendAttempt();
	}else{
		_txTimeoutTimeRegular = Sysutils::GetMonotonicTime() + CoproTimeout;
		_txState = TxState_Interrupted;
		LOG( Logger::LOG_DEBUG, "BidcosMacController::SendBidcosTelegram(): _txState = TxState_Interrupted. _autoTxState = %s",printAutoTxState(_autoTxState).c_str() );
	}
	_bidcosContext.PeerResetFlags( _curTxTelegram.GetReceiverAddress(), BidcosPeerContext::Flag_WakeupSent );
	return true;
}

void BidcosMacController::NextSendAttempt()
{
	int macOptions = (_txRate == TxRate_10k) ? LowLevelMacFrame::Option_Band_868_10k : LowLevelMacFrame::Option_Band_868_100k;
	if( _curTxTelegram.GetFlags() & BidcosTelegram::Flag_Burst )
	{
		macOptions |= LowLevelMacFrame::Option_Burst;
	}
	if( !_csmaCaEnabled )
	{
		macOptions |= LowLevelMacFrame::Option_CcaOff;
	}

	uint16_t frameStartTime = LowLevelMacFrame::Timestamp_Relative;//ASAP

	if( (_curTxBurstMode == HmLegacyFrameBidcosTxTelegram::BurstMode_Triple) && _tripleBurstCounter && (_txState == TxState_WaitAck) )
	{
		//sending subsequent triple burst slots
		uint32_t now = Sysutils::GetMonotonicTime();
		int32_t timeElapsed = int32_t( now - _tripleBurstFirstSlotStartTimeHost );
		int32_t slotSpacingMs = TripleBurstSlotLengthMs * ((_curTxTelegram.GetFlags() & BidcosTelegram::Flag_RepeatEnable) ? 2 : 1);
		int currentSlot = (timeElapsed + slotSpacingMs / 2) / slotSpacingMs;
		uint16_t sendTime = _tripleBurstFirstSlotStartTimeCopro + slotSpacingMs * currentSlot;
		LOG( Logger::LOG_DEBUG, "3burst: Trial %d, slot %d, spacing %dms, @%dms", _tripleBurstCounter + 1, currentSlot, slotSpacingMs, sendTime );
		frameStartTime = (_tripleBurstFirstSlotStartTimeCopro + slotSpacingMs * currentSlot) & ~LowLevelMacFrame::Timestamp_Relative;
	} else {
		//sending regular telegram or first triple burst slot
		_tripleBurstFirstSlotStartTimeHost = Sysutils::GetMonotonicTime();
	}

	LowLevelMacFrame* frame;

	if( (frameStartTime == LowLevelMacFrame::Timestamp_Relative) && (_txRate == TxRate_100k) && (_curTxTelegram.GetFlags() & BidcosTelegram::Flag_BiDi)  )
	{
		LowLevelMacFrameTxWithRxModeSet* txFrame = new LowLevelMacFrameTxWithRxModeSet();
		frame = txFrame;
		txFrame->SetOptions( macOptions );
		if( _curTxTelegram.GetFrameType() == BidcosTelegram::FrameType_Update )
		{
			txFrame->SetRxEndTimeRelative( AckTimeoutUpdate + 500 );
		}else{
			txFrame->SetRxEndTimeRelative( AckTimeoutRegular + 500 );
		}
		txFrame->SetPayload( _curTxTelegram.GetRawData() );
		//no repetitions for update frames. Repetitions will be handled by rfd.
		switch( _curTxTelegram.GetFrameType() )
		{
		case BidcosTelegram::FrameType_Update:
		case BidcosTelegram::FrameType_TrxRegister:
			_repetitionCount = MaxNumberOfRepetitions;
			break;
		default:
			break;
		}
	}else{
		LowLevelMacFrameTx* txFrame = new LowLevelMacFrameTx();
		frame = txFrame;
		txFrame->SetOptions( macOptions );
		txFrame->SetFrameStartTime( frameStartTime );
		txFrame->SetPayload( _curTxTelegram.GetRawData() );
	}

	_regularFrameId = frame->GetId();

	_subsystem.SendFrameDownstream( frame );

	_txTimeoutTimeRegular = Sysutils::GetMonotonicTime() + CoproTimeout;
	_txState = TxState_WaitCoproResponse;
	LOG( Logger::LOG_DEBUG, "_txState = TxState_WaitCoproResponse" );

	_sendAttemptCount++;

}

void BidcosMacController::OnLowLevelMacFrame( LowLevelMacFrame& lowLevelMacFrame )
{
	try
	{
		switch(lowLevelMacFrame.GetCommand())
		{
		case LowLevelMacFrame::FrameType_RxTelegram:
			OnLowLevelMacRx( dynamic_cast<LowLevelMacFrameRxTelegram&>(lowLevelMacFrame) );
			break;
		case LowLevelMacFrame::FrameType_Response:
			OnLowLevelMacResponse( dynamic_cast<LowLevelMacFrameResponse&>(lowLevelMacFrame) );
			break;
		}
	}
	catch( std::bad_cast& ex )
	{
		LOG(Logger::LOG_DEBUG, "Bad cast: %s", ex.what() ); 
		LOG(Logger::LOG_DEBUG, "LowLevelMacFrame unknown: %s", lowLevelMacFrame.ToString().c_str());
	}
}

void BidcosMacController::OnRxAesAck( const BidcosTelegram& ackTelegram, int rssi )
{
	LOG( Logger::LOG_INFO, "RX AES ACK: %s", ackTelegram.ToString().c_str() );
	bool valid = false;
	uint8_t keyIndex = 0;
	if( _txAesChallengeInfo )
	{
		valid = _txAesChallengeInfo->CheckAck( ackTelegram );
		keyIndex = _txAesChallengeInfo->GetKeyIndex();
		delete _txAesChallengeInfo;
		_txAesChallengeInfo = NULL;
	}

	HmLegacyFrameBidcosResponse* response = 0;
	if( valid )
	{
		LOG( Logger::LOG_INFO, "AES ACK received after %d repetitions: %s", _repetitionCount, _curTxTelegram.ToString().c_str() );
		response = new HmLegacyFrameBidcosResponse(HmLegacyFrameBidcosResponse::ResponseCode_SendOkWithAuthOk);
		BinaryData rawTelegramData = ackTelegram.GetRawData();
		rawTelegramData.resize( rawTelegramData.size() - 4 );
		response->SetResponseTelegram( rawTelegramData );
		response->SetRssi( rssi );	
	} else {
		keyIndex = _bidcosContext.IsKnownAesKeyIndex(keyIndex) ? keyIndex : (uint8_t)0xFE;
		LOG( Logger::LOG_INFO, "AES invalid ACK received after %d repetitions: %s", _repetitionCount, _curTxTelegram.ToString().c_str() );
		response = new HmLegacyFrameBidcosResponse( HmLegacyFrameBidcosResponse::ResponseCode_SendFailedAuth );
	}
	if(response) {
		response->SetSequenceCounter( _curTxSequenceCounter );
		response->SetKeyIndex( keyIndex );
		_subsystem.SendFrameUpstream( response );
	}
	_waitAesAckTimedOut = false;
	_txState = TxState_Idle;
	LOG( Logger::LOG_DEBUG, "_txState = TxState_Idle" );
}

void BidcosMacController::OnRxAesChallenge( const BidcosTelegram& challengeTelegram, uint32_t receiveTime )
{
	LOG( Logger::LOG_INFO, "RX AES challenge: %s", challengeTelegram.ToString().c_str() );
	const uint32_t startTime = Sysutils::GetMonotonicTime();
	if( _txAesChallengeInfo )
	{
		delete _txAesChallengeInfo;
	}
	_txAesChallengeInfo = new BidcosAesChallengeInfo( _curTxTelegram );

	uint8_t randomNumber[6];
	if( !GetRandomNumber( randomNumber, sizeof(randomNumber) ) )
	{
		return;
	}

	BidcosTelegram aesSolution = _txAesChallengeInfo->CalculateSolution( _bidcosContext, challengeTelegram, randomNumber );
	const uint32_t endTime = Sysutils::GetMonotonicTime();
	LOG(Logger::LOG_DEBUG, "AES Solution calculation needed %u ms.", (endTime - startTime));
	_curAutoTxTelegram = aesSolution;

	LowLevelMacFrameTx* lowLevelMacFrame = new LowLevelMacFrameTx();

	_autoTxLowLevelMacOptions = LowLevelMacFrame::Option_Band_868_10k | LowLevelMacFrame::Option_CcaOff;

	lowLevelMacFrame->SetOptions( _autoTxLowLevelMacOptions );
	lowLevelMacFrame->SetFrameStartTimeAbsolute( receiveTime + ((challengeTelegram.GetFlags() & BidcosTelegram::Flag_Repeated ) ? AckDelayRepeated : AckDelayRegular ));

	lowLevelMacFrame->SetPayload( aesSolution.GetRawData() );

	_regularFrameId = lowLevelMacFrame->GetId();
	_autoTxFrameId = _regularFrameId;

	_subsystem.SendFrameDownstream( lowLevelMacFrame );

	_txState = TxState_WaitCoproResponseAesAck;
	LOG( Logger::LOG_DEBUG, "_txState = TxState_WaitCoproResponseAesAck" );
	_autoTxState = AutoTxState_AesAck;
	_txTimeoutTimeRegular = Sysutils::GetMonotonicTime() + AckTimeoutRegular;
	_autoTxTimeoutTime = _txTimeoutTimeRegular;
}

int BidcosMacController::GetAckActionForIncomingTelegram(const BidcosTelegram& telegram, uint8_t* keyIndex)
{
	uint32_t ownAddress = _bidcosContext.GetRfAddress();
	uint32_t senderAddress = telegram.GetSenderAddress();

	BidcosPeerContext peerContext;
	if( !_bidcosContext.GetPeerContext( senderAddress, &peerContext ) )
	{
		LOG( Logger::LOG_DEBUG, "GetAckActionForIncomingTelegram(): Unknown peer, AckAction_NotForUs" );
		return (telegram.GetFrameType() == BidcosTelegram::FrameType_Ack) ? AckAction_NotBidirectional : AckAction_NotForUs;
	}

	int peerFlags = peerContext.GetFlags();
	_bidcosContext.PeerResetFlags( senderAddress, BidcosPeerContext::Flag_WakeupSent );

	if( telegram.GetFrameType() == BidcosTelegram::FrameType_Ack )
	{
		if( (telegram.GetReceiverAddress() == ownAddress) && (peerFlags & BidcosPeerContext::Flag_WakeupSent) )
		{
			return AckAction_Wokenup;
		}
		return AckAction_NotBidirectional;
	}

	int ackAction = AckAction_NotForUs;
	if( telegram.GetReceiverAddress() == _bidcosContext.GetRfAddress() )
	{
		if( !(telegram.GetFlags() & BidcosTelegram::Flag_BiDi) )
		{
			if(telegram.GetFrameType() == BidcosTelegram::FrameType_Sysinfo)
			{//TWIST-1171: Some devices do not set bidi bit on SysInfo. This caused a CallCCU reply
					ackAction = AckAction_Ack;
			}
			else {
				ackAction = AckAction_NotBidirectional;
			}
		}else{
			ackAction = AckAction_Ack;
			uint64_t aesChannels = peerContext.GetAesChannelMask();
			if(aesChannels)
			{
				*keyIndex = peerContext.GetAesKeyIndex();
				switch(telegram.GetFrameType()){
				case BidcosTelegram::FrameType_SwitchConditional:
				case BidcosTelegram::FrameType_SwitchLevel:
				case BidcosTelegram::FrameType_SwitchUnconditional:
					{
						uint8_t channel = telegram.GetPayload().GetUInt8Value(0) & 0x3f;
						LOG( Logger::LOG_DEBUG, "GetAckActionForIncomingTelegram() channel=%d", channel );
						if( ((aesChannels >> channel) & 0x01) != 0 )
						{
							ackAction = AckAction_AesChallenge;
						}
					}
					break;
				case BidcosTelegram::FrameType_Info:
					if( telegram.GetPayload().GetUInt8Value(0) == 6 )
					{
						//Info status
						uint8_t channel = telegram.GetPayload().GetUInt8Value(1) & 0x3f;
						LOG( Logger::LOG_DEBUG, "GetAckActionForIncomingTelegram() channel=%d", channel );
						if( (channel == 0) || (((aesChannels >> channel) & 0x01) != 0) )
						{
							ackAction = AckAction_AesChallenge;
						}
					}else{
						ackAction = AckAction_AesChallenge;
					}
					break;
				case BidcosTelegram::FrameType_AesSolution:
					ackAction = AckAction_AesAck;
					break;
				default:
					break;
				}
			}
		}
	}
	bool wakeMeUp = (telegram.GetFlags() & BidcosTelegram::Flag_WakeMeUp) != 0;
	if( peerFlags & BidcosPeerContext::Flag_WakeMeUp )
	{
		_bidcosContext.PeerResetFlags( telegram.GetSenderAddress(), BidcosPeerContext::Flag_WakeMeUp );
		wakeMeUp = true;
	}

	switch( ackAction )
	{
	case AckAction_NotForUs:
		//telegram is unidirectional or bidirectional and for a different receiver
		if( wakeMeUp && ( (peerFlags & (BidcosPeerContext::Flag_NeedsWakeup | BidcosPeerContext::Flag_LazyConfig)) == BidcosPeerContext::Flag_NeedsWakeup) )
		{
			if( peerFlags & BidcosPeerContext::Flag_FirstWakeup )
			{
				_bidcosContext.PeerResetFlags( telegram.GetSenderAddress(), BidcosPeerContext::Flag_FirstWakeup );
				ackAction |= AckAction_Wakeup;
			}else if( (!(telegram.GetFlags() & BidcosTelegram::Flag_BiDi)) && ((rand() & 0xff) < 205))
			{
				ackAction |= AckAction_Wakeup;
			}
		}else if( peerFlags & BidcosPeerContext::Flag_LazyConfig )
		{
			ackAction |= AckAction_CallCcu;
		}
		break;
	case AckAction_NotBidirectional:
		//telegram is for us but is marked unidirectional
		if( wakeMeUp && ( (peerFlags & (BidcosPeerContext::Flag_NeedsWakeup | BidcosPeerContext::Flag_LazyConfig)) == BidcosPeerContext::Flag_NeedsWakeup ) )
		{
			if( peerFlags & BidcosPeerContext::Flag_FirstWakeup )
			{
				_bidcosContext.PeerResetFlags( telegram.GetSenderAddress(), BidcosPeerContext::Flag_FirstWakeup );
				ackAction |= AckAction_Wakeup;
			}else if( (rand() & 0xff) < 205)
			{
				ackAction |= AckAction_Wakeup;
			}
		}else if( peerFlags & BidcosPeerContext::Flag_LazyConfig )
		{
			ackAction |= AckAction_CallCcu;
		}
		break;
	case AckAction_AesChallenge:
		_bidcosContext.PeerSetFlags( telegram.GetSenderAddress(), BidcosPeerContext::Flag_WakeMeUp );
		//fallthrough
	case AckAction_AesAck:
	case AckAction_Ack:
		if( peerFlags & BidcosPeerContext::Flag_NeedsWakeup )
		{
			ackAction |= AckAction_Wakeup;
		}
		break;
	}

	const char* ACTION_NAMES[] = {
		"AckAction_NotForUs",
		"AckAction_NotBidirectional",
		"AckAction_Ack",
		"AckAction_AesChallenge",
		"AckAction_AesAck",
		"AckAction_Wokenup"
	};

	const char* WAKEUP_NAMES[] = {
		"None",
		"Wakeup",
		"CallCcu",
	};

	LOG( Logger::LOG_DEBUG, "GetAckActionForIncomingTelegram() ackAction=%s, wakeup=%s", ACTION_NAMES[ackAction&AckActionMask], WAKEUP_NAMES[ackAction >> 8] );
	return ackAction;
}

void BidcosMacController::OnLowLevelMacRx( LowLevelMacFrameRxTelegram& lowLevelMacTelegram )
{
	bool sendUpstream = false;
	bool performAckAction = true;
	BidcosTelegram bidcosTelegram( lowLevelMacTelegram.GetPayload() );

	LOG( Logger::LOG_DEBUG, "Bidcos RX: %s", bidcosTelegram.ToString().c_str());

	BidcosTelegram::FrameType frameType = bidcosTelegram.GetFrameType();
	uint32_t receiverAddress = bidcosTelegram.GetReceiverAddress();
	uint32_t ownAddress = _bidcosContext.GetRfAddress();
	sendUpstream = true;
	_autoTxTimeoutTime = Sysutils::GetMonotonicTime() + CoproTimeout;

	uint8_t aesKeyIndex;
	int ackAction = GetAckActionForIncomingTelegram( bidcosTelegram, &aesKeyIndex );


	if( _txState == TxState_WaitAck )
	{
		//if( ((frameType == BidcosTelegram::FrameType_Ack) || (frameType == BidcosTelegram::FrameType_Info)) && (receiverAddress == ownAddress ) )
		if((receiverAddress == ownAddress ) ) //TWIST-1263 Some device answer with other frame types than ack or info to status request
		{
			if( (bidcosTelegram.GetSenderAddress() == _curTxTelegram.GetReceiverAddress()) && (bidcosTelegram.GetTelegramCounter() == _curTxTelegram.GetTelegramCounter()) )
			{
				if( (frameType == BidcosTelegram::FrameType_Ack) && (bidcosTelegram.GetAckCode() == BidcosTelegram::AckCode_AckWithAesChallenge) )
				{
					OnRxAesChallenge( bidcosTelegram, CalculateFrameStartTime(lowLevelMacTelegram ) );
					sendUpstream = false;
				}else{
					if( (ackAction & AckActionMask) != AckAction_AesChallenge )
					{
						LOG( Logger::LOG_INFO, "ACK received after %d repetitions: %s", _repetitionCount, bidcosTelegram.ToString().c_str() );
						HmLegacyFrameBidcosResponse* response = new HmLegacyFrameBidcosResponse(HmLegacyFrameBidcosResponse::ResponseCode_SendOkAndAckReceived);
						response->SetSequenceCounter( _curTxSequenceCounter );
						response->SetResponseTelegram( lowLevelMacTelegram.GetPayload() );
						response->SetRssi( lowLevelMacTelegram.GetRssi() );
						_subsystem.SendFrameUpstream( response );
					}
					//FIXME Maybe check if frametype Ack && _waitAesAckTimedOut==true and then call OnAesAckTimeout (For the case that devices reply a repeated telegram with Ack instead of Ack_AES on repetition.)
					else{
						LOG( Logger::LOG_INFO, "ACK received after %d repetitions, starting AES cycle: %s", _repetitionCount, bidcosTelegram.ToString().c_str() );
					}

					_txState = TxState_Idle;
					LOG( Logger::LOG_DEBUG, "_txState = TxState_Idle" );
					sendUpstream = false;
					if( _txRate == TxRate_100k )
					{
						_txState = TxState_WaitCoproResponse;
						LOG( Logger::LOG_DEBUG, "_txState = TxState_WaitCoproResponse" );
						LowLevelMacFrameRxModeDefault* unlockRxModeFrame = new LowLevelMacFrameRxModeDefault();
						_regularFrameId = unlockRxModeFrame->GetId();
						_curTxTelegram = BidcosTelegram();//reset _curTxTelegram and _curTxSequenceCounter, sonce communication has been finished and without it leads into false positive timeout
						_curTxSequenceCounter = 0;
						_txTimeoutTimeRegular = Sysutils::GetMonotonicTime() + AckTimeoutRegular;
						_subsystem.SendFrameDownstream( unlockRxModeFrame );
					}
				}
			}
		}
	}else if( _txState == TxState_WaitAesAck )
	{
		if( (bidcosTelegram.GetSenderAddress() == _curTxTelegram.GetReceiverAddress()) && (bidcosTelegram.GetTelegramCounter() == _curTxTelegram.GetTelegramCounter()) && (receiverAddress == ownAddress ) )
		{
			OnRxAesAck( bidcosTelegram, lowLevelMacTelegram.GetRssi() );
			sendUpstream = false;
		}
	}
	else if( _txState == TxState_WaitAesSolution )
	{
		if( (bidcosTelegram.GetSenderAddress() == _curTxTelegram.GetReceiverAddress()) && (bidcosTelegram.GetTelegramCounter() == _curTxTelegram.GetTelegramCounter()) && (receiverAddress == ownAddress ) ) {
			if(bidcosTelegram.GetFrameType() != BidcosTelegram::FrameType_AesSolution &&
					(bidcosTelegram.GetFlags() & BidcosTelegram::Flag_BiDi) ) {//if for us, and bidi, but not a solution, ignore message.
				sendUpstream = false;
				performAckAction = false;
			}
		}
	}

	int upstreamFlags = HmLegacyFrameBidcosRxTelegram::Flag_AuthNotNecessary;

	if(performAckAction) {
		if(_autoTxState == AutoTxState_Idle )
		{
			switch( ackAction & AckActionMask )
			{
			case AckAction_Ack:
				SendPlainAck( bidcosTelegram, CalculateFrameStartTime(lowLevelMacTelegram ), (ackAction & AckAction_Wakeup) != 0 );
				upstreamFlags |= (ackAction & AckAction_Wakeup) ? HmLegacyFrameBidcosRxTelegram::Flag_Wokenup : 0;
				break;
			case AckAction_AesChallenge:
				SendAesChallenge( bidcosTelegram, CalculateFrameStartTime(lowLevelMacTelegram ), aesKeyIndex, lowLevelMacTelegram.GetRssi(), (ackAction & AckAction_Wakeup) != 0 );
				sendUpstream = false;
				upstreamFlags = HmLegacyFrameBidcosRxTelegram::Flag_AuthFailed;
				break;
			case AckAction_AesAck:
				CheckAesSolutionAndSendAesAck( bidcosTelegram, CalculateFrameStartTime(lowLevelMacTelegram ), (ackAction & AckAction_Wakeup) != 0 );
				sendUpstream = false;
				break;
			case AckAction_NotBidirectional:
			case AckAction_NotForUs:
				upstreamFlags = HmLegacyFrameBidcosRxTelegram::Flag_AuthNone;
				if( ackAction & AckAction_Wakeup )
				{
					SendWakeupFrame( bidcosTelegram, CalculateFrameStartTime(lowLevelMacTelegram ) );
				}else if( ackAction & AckAction_CallCcu )
				{
					SendCallCcuFrame( bidcosTelegram, CalculateFrameStartTime(lowLevelMacTelegram ) );
				}
				break;
			case AckAction_Wokenup:
				upstreamFlags |= HmLegacyFrameBidcosRxTelegram::Flag_Wokenup;
				break;
			}
		}
		else {
			//set upstream flags if AutoTx is not possible
			upstreamFlags = HmLegacyFrameBidcosRxTelegram::Flag_AuthNotNecessary;
			switch (ackAction & AckActionMask) {
			case AckAction_Ack:
				break;
			case AckAction_AesChallenge:
				//we can't send an AES Challenge -> Authentication failed
				upstreamFlags = HmLegacyFrameBidcosRxTelegram::Flag_AuthFailed;
				break;
			case AckAction_AesAck:
				break;
			case AckAction_NotBidirectional:
			case AckAction_NotForUs:
				upstreamFlags = HmLegacyFrameBidcosRxTelegram::Flag_AuthNone;
				break;
			case AckAction_Wokenup:
				//do note set the wokeup flag because we can't send the ACK
				break;
			}
		}
	}

	if( sendUpstream )
	{
		HmLegacyFrameBidcosRxTelegram* bidcosRxTelegram = new HmLegacyFrameBidcosRxTelegram();
		bidcosRxTelegram->SetPayload( lowLevelMacTelegram.GetPayload() );
		bidcosRxTelegram->SetRssi( lowLevelMacTelegram.GetRssi() );
		bidcosRxTelegram->SetFlags( upstreamFlags );
		_subsystem.SendFrameUpstream( bidcosRxTelegram );
	}
}

bool BidcosMacController::CheckAesSolutionAndSendAesAck( const BidcosTelegram& solutionTelegram, uint32_t frameStartTime, bool wakeup )
{
	_txState = TxState_Idle;
	LOG(Logger::LOG_DEBUG, "_txState = TxState_Idle");

	int upstreamFlags = HmLegacyFrameBidcosRxTelegram::Flag_AuthNone;

	BidcosAesChallengeInfo* challengeInfo = NULL;
	uint32_t peerAddress = solutionTelegram.GetSenderAddress();
	MapPeerAddressToAesChallengeInfo::iterator it = _mapPeerAddressToAesChallengeInfo.find( peerAddress );
	if( it != _mapPeerAddressToAesChallengeInfo.end() )
	{
		challengeInfo = it->second;
		_mapPeerAddressToAesChallengeInfo.erase( it );
	}else{
		LOG( Logger::LOG_INFO, "Unexpected AES solution: %s", solutionTelegram.ToString().c_str() );
		return false;
	}
	bool authSuccess = true;
	if( !challengeInfo->CheckSolution( _bidcosContext, solutionTelegram ) )
	{
		upstreamFlags = HmLegacyFrameBidcosRxTelegram::Flag_AuthFailed;
		authSuccess = false;
	} else {
		upstreamFlags = HmLegacyFrameBidcosRxTelegram::Flag_AuthSuccessful;
	}
	if( wakeup )
	{
		upstreamFlags |= HmLegacyFrameBidcosRxTelegram::Flag_Wokenup;
	}
	BinaryData ackPayload( uint8_t(0) );
	ackPayload.SetUInt32Value( 1, challengeInfo->GetAckTailData() );
	SendAckWithPayload( challengeInfo->GetOriginalTelegram(), frameStartTime, wakeup, ackPayload );


	const BidcosTelegram& originalTelegram = challengeInfo->GetOriginalTelegram();
	if(_curTxTelegram.GetTelegramCounter() == originalTelegram.GetTelegramCounter()) {//is response of open request?
		LOG(Logger::LOG_DEBUG, "Send originalTelegram as response");
		HmLegacyFrameBidcosResponse* response = 0;
		if(authSuccess) {
			response = new HmLegacyFrameBidcosResponse( HmLegacyFrameBidcosResponse::ResponseCode_SendOkWithAuthOk );
			response->SetRssi(challengeInfo->GetRssi());
			BinaryData data = originalTelegram.GetRawData();
			response->SetResponseTelegram(data);
		}
		else {
			response = new HmLegacyFrameBidcosResponse( HmLegacyFrameBidcosResponse::ResponseCode_SendFailedAuth );
		}
		if(response) {
			response->SetSequenceCounter( _curTxSequenceCounter );
			response->SetKeyIndex( (authSuccess || _bidcosContext.IsKnownAesKeyIndex(challengeInfo->GetKeyIndex())) ? challengeInfo->GetKeyIndex(): (uint8_t)0xFE );
			_subsystem.SendFrameUpstream( response );
		}
	}
	else {
		HmLegacyFrameBidcosRxTelegram* bidcosRxTelegram = new HmLegacyFrameBidcosRxTelegram();
		bidcosRxTelegram->SetPayload( challengeInfo->GetOriginalTelegram().GetRawData() );
		bidcosRxTelegram->SetRssi( challengeInfo->GetRssi() );
		bidcosRxTelegram->SetFlags( upstreamFlags );
		bidcosRxTelegram->SetKeyIndex( (authSuccess || _bidcosContext.IsKnownAesKeyIndex(challengeInfo->GetKeyIndex())) ? challengeInfo->GetKeyIndex(): (uint8_t)0xFE );
		_subsystem.SendFrameUpstream( bidcosRxTelegram );
	}

	delete challengeInfo;
	return true;
}

void BidcosMacController::SendPlainAck( const BidcosTelegram& bidcosTelegram, uint32_t frameStartTime, bool wakeup )
{
	if( bidcosTelegram.GetFrameType() == BidcosTelegram::FrameType_TimeInfo )
	{
		BinaryData timeInfoPayload;
		_bidcosContext.GetTimeInfoPayload(timeInfoPayload);
		SendAckWithPayload( bidcosTelegram, frameStartTime, wakeup, timeInfoPayload );
	}else{
		SendAckWithPayload( bidcosTelegram, frameStartTime, wakeup, BinaryData( uint8_t(0) ) );
	}
}

void BidcosMacController::SendAckWithPayload( const BidcosTelegram& bidcosTelegram, uint32_t frameStartTime, bool wakeup, const BinaryData& payload )
{
	int flags = bidcosTelegram.GetFlags();
	if( wakeup )
	{
		flags |= BidcosTelegram::Flag_Wakeup;
	}

	_curAutoTxTelegram = BidcosTelegram();
	_curAutoTxTelegram.SetTelegramCounter( bidcosTelegram.GetTelegramCounter() );
	_curAutoTxTelegram.SetFlags( flags & (BidcosTelegram::Flag_RepeatEnable | BidcosTelegram::Flag_Wakeup) );
	if( bidcosTelegram.GetFrameType() != BidcosTelegram::FrameType_TimeInfo )
	{
		_curAutoTxTelegram.SetFrameType( BidcosTelegram::FrameType_Ack );
	}else{
		_curAutoTxTelegram.SetFrameType( BidcosTelegram::FrameType_TimeInfo );
	}
	_curAutoTxTelegram.SetSenderAddress( bidcosTelegram.GetReceiverAddress() );
	_curAutoTxTelegram.SetReceiverAddress( bidcosTelegram.GetSenderAddress() );
	_curAutoTxTelegram.SetPayload( payload );

	LowLevelMacFrameTx* lowLevelMacAck = new LowLevelMacFrameTx();
	_autoTxLowLevelMacOptions = LowLevelMacFrame::Option_Band_868_10k | LowLevelMacFrame::Option_CcaOff;
	lowLevelMacAck->SetOptions( _autoTxLowLevelMacOptions );
	uint16_t ackStartTime = frameStartTime + ((flags & BidcosTelegram::Flag_Repeated ) ? AckDelayRepeated : AckDelayRegular);
	lowLevelMacAck->SetFrameStartTimeAbsolute( ackStartTime );
	lowLevelMacAck->SetPayload( _curAutoTxTelegram.GetRawData() );
	_autoTxFrameId = lowLevelMacAck->GetId();
	_subsystem.SendFrameDownstream( lowLevelMacAck );
	_autoTxState = AutoTxState_Ack;
	LOG( Logger::LOG_DEBUG, "AutoTx: %s", _curAutoTxTelegram.ToString().c_str());
}

void BidcosMacController::SendWakeupFrame( const BidcosTelegram& bidcosTelegram, uint32_t frameStartTime )
{
	int flags = bidcosTelegram.GetFlags() | BidcosTelegram::Flag_Wakeup | BidcosTelegram::Flag_BiDi;

	_curAutoTxTelegram = BidcosTelegram();
	_curAutoTxTelegram.SetTelegramCounter( rand() & 127 );
	_curAutoTxTelegram.SetFlags( flags & (BidcosTelegram::Flag_RepeatEnable | BidcosTelegram::Flag_Wakeup | BidcosTelegram::Flag_BiDi) );
	_curAutoTxTelegram.SetFrameType( BidcosTelegram::FrameType_Wakeup );
	_curAutoTxTelegram.SetSenderAddress( _bidcosContext.GetRfAddress() );
	_curAutoTxTelegram.SetReceiverAddress( bidcosTelegram.GetSenderAddress() );

	LowLevelMacFrameTx* lowLevelMacAck = new LowLevelMacFrameTx();
	_autoTxLowLevelMacOptions = LowLevelMacFrame::Option_Band_868_10k | LowLevelMacFrame::Option_CcaOff;
	lowLevelMacAck->SetOptions( _autoTxLowLevelMacOptions );
	uint16_t ackStartTime = frameStartTime + ((flags & BidcosTelegram::Flag_Repeated ) ? AckDelayRepeated : AckDelayRegular);
	lowLevelMacAck->SetFrameStartTimeAbsolute( ackStartTime );
	lowLevelMacAck->SetPayload( _curAutoTxTelegram.GetRawData() );
	_autoTxFrameId = lowLevelMacAck->GetId();
	_subsystem.SendFrameDownstream( lowLevelMacAck );
	_autoTxState = AutoTxState_Wakeup;
	_bidcosContext.PeerSetFlags( bidcosTelegram.GetSenderAddress(), BidcosPeerContext::Flag_WakeupSent );
	LOG( Logger::LOG_DEBUG, "AutoTx: %s", _curAutoTxTelegram.ToString().c_str());
}


void BidcosMacController::SendCallCcuFrame( const BidcosTelegram& bidcosTelegram, uint32_t frameStartTime )
{
	_curAutoTxTelegram = BidcosTelegram();
	_curAutoTxTelegram.SetTelegramCounter( rand() & 127 );
	_curAutoTxTelegram.SetFrameType( BidcosTelegram::FrameType_CallCcu );

	_curAutoTxTelegram.SetSenderAddress( _bidcosContext.GetRfAddress() );
	_curAutoTxTelegram.SetReceiverAddress( bidcosTelegram.GetSenderAddress() );

	LowLevelMacFrameTx* lowLevelMacAck = new LowLevelMacFrameTx();
	_autoTxLowLevelMacOptions = LowLevelMacFrame::Option_Band_868_10k | LowLevelMacFrame::Option_CcaOff;
	lowLevelMacAck->SetOptions( _autoTxLowLevelMacOptions );
	uint16_t ackStartTime = frameStartTime + AckDelayCallCcu;
	lowLevelMacAck->SetFrameStartTimeAbsolute( ackStartTime );
	lowLevelMacAck->SetPayload( _curAutoTxTelegram.GetRawData() );
	_autoTxFrameId = lowLevelMacAck->GetId();
	_subsystem.SendFrameDownstream( lowLevelMacAck );
	_autoTxState = AutoTxState_CallCcu;
	LOG( Logger::LOG_DEBUG, "AutoTx: %s", _curAutoTxTelegram.ToString().c_str());
}


void BidcosMacController::SendAesChallenge( const BidcosTelegram& bidcosTelegram, uint32_t frameStartTime, uint8_t keyIndex, int rssi, bool wakeup )
{
	LOG(Logger::LOG_DEBUG, "BidcosMacController::SendAESChallenge()");
	int flags = bidcosTelegram.GetFlags();

	_curAutoTxTelegram = BidcosTelegram();
	_curAutoTxTelegram.SetTelegramCounter( bidcosTelegram.GetTelegramCounter() );
	_curAutoTxTelegram.SetFlags( BidcosTelegram::Flag_RepeatEnable | BidcosTelegram::Flag_BiDi | (wakeup ? BidcosTelegram::Flag_Wakeup : 0)  );
	_curAutoTxTelegram.SetFrameType( BidcosTelegram::FrameType_Ack );
	_curAutoTxTelegram.SetSenderAddress( bidcosTelegram.GetReceiverAddress() );
	_curAutoTxTelegram.SetReceiverAddress( bidcosTelegram.GetSenderAddress() );
	uint8_t payload[8];
	GetRandomNumber( payload+1, 6 );
	payload[0] = 4;
	BidcosAesKey aesKey = _bidcosContext.GetAesKeyByIndex( keyIndex );
	payload[7] = aesKey.GetIndex() << 1;
	_curAutoTxTelegram.SetPayload( BinaryData( payload, 8 ) );

	LowLevelMacFrameTx* lowLevelMacAck = new LowLevelMacFrameTx();
	_autoTxLowLevelMacOptions = LowLevelMacFrame::Option_Band_868_10k | LowLevelMacFrame::Option_CcaOff;
	lowLevelMacAck->SetOptions( _autoTxLowLevelMacOptions );
	uint16_t ackStartTime = frameStartTime + ((flags & BidcosTelegram::Flag_Repeated ) ? AckDelayRepeated : AckDelayRegular);
	lowLevelMacAck->SetFrameStartTimeAbsolute( ackStartTime );
	lowLevelMacAck->SetPayload( _curAutoTxTelegram.GetRawData() );
	_autoTxFrameId = lowLevelMacAck->GetId();
	_txTimeoutTimeRegular = Sysutils::GetMonotonicTime() + AckTimeoutRegular;
	_subsystem.SendFrameDownstream( lowLevelMacAck );
	_autoTxState = AutoTxState_AesChallenge;
	_txState = TxState_WaitCoproResponseAesSolution;
	LOG(Logger::LOG_DEBUG, "BidcosMacController::SendAESChallenge() - autoTxState=AutoTxState_AesChallenge, txState=TxState_WaitCoproResponseAesSolution");
	RememberAesChallenge( new BidcosAesChallengeInfo( bidcosTelegram, payload+1, aesKey.GetIndex(), rssi ));
	LOG( Logger::LOG_DEBUG, "AutoTx: %s", _curAutoTxTelegram.ToString().c_str());
}

void BidcosMacController::ResendAutoTxFrame()
{
	LOG(Logger::LOG_DEBUG, "ResendAutoTxFrame");
	LowLevelMacFrameTx* lowLevelMacAck = new LowLevelMacFrameTx();
	lowLevelMacAck->SetOptions( _autoTxLowLevelMacOptions );
	lowLevelMacAck->SetFrameStartTimeRelative( 0 );
	lowLevelMacAck->SetPayload( _curAutoTxTelegram.GetRawData() );
	_autoTxFrameId = lowLevelMacAck->GetId();
	_sendAttemptCount++;
	_subsystem.SendFrameDownstream( lowLevelMacAck );
}


void BidcosMacController::SendCyclicTimeInfo()
{
	BinaryData payload;
	if( _bidcosContext.GetTimeInfoPayload( payload ) )
	{
		_curAutoTxTelegram = BidcosTelegram();
		_curAutoTxTelegram.SetSenderAddress( _bidcosContext.GetRfAddress() );
		_curAutoTxTelegram.SetPayload( payload );
		_curAutoTxTelegram.SetFrameType( BidcosTelegram::FrameType_TimeInfo );
		_curAutoTxTelegram.SetTelegramCounter( rand() % 255 );
		_curAutoTxTelegram.SetFlags( BidcosTelegram::Flag_Broadcast | BidcosTelegram::Flag_Burst | BidcosTelegram::Flag_RepeatEnable );

		LowLevelMacFrameTx* macFrame = new LowLevelMacFrameTx();
		_autoTxLowLevelMacOptions = LowLevelMacFrame::Option_Band_868_10k | LowLevelMacFrame::Option_Burst | (_csmaCaEnabled ? 0 : LowLevelMacFrame::Option_CcaOff);
		macFrame->SetOptions( _autoTxLowLevelMacOptions );
		macFrame->SetFrameStartTimeRelative( 0 );
		macFrame->SetPayload( _curAutoTxTelegram.GetRawData() );
		_autoTxFrameId = macFrame->GetId();
		_subsystem.SendFrameDownstream( macFrame );
	}
}

void BidcosMacController::RememberAesChallenge( BidcosAesChallengeInfo* challengeInfo )
{
	uint32_t peerAddress = challengeInfo->GetOriginalTelegram().GetSenderAddress();
	MapPeerAddressToAesChallengeInfo::iterator it = _mapPeerAddressToAesChallengeInfo.find( peerAddress );
	if( it != _mapPeerAddressToAesChallengeInfo.end() )
	{
		delete it->second;
	}
	_mapPeerAddressToAesChallengeInfo[peerAddress] = challengeInfo;
}

void BidcosMacController::OnLowLevelMacResponse( LowLevelMacFrameResponse& lowLevelMacResponse )
{
	if( _autoTxState != AutoTxState_Idle )
	{
		if (lowLevelMacResponse.GetId() == _autoTxFrameId)
		{
			if( lowLevelMacResponse.GetResponseCode() == LowLevelMacFrameResponse::ResponseCode_Ack )
			{
				_autoTxState = AutoTxState_Idle;
			}else{
				if( _sendAttemptCount < MaxSendAttemptsFastRetry )
				{
					LOG( Logger::LOG_DEBUG, "Coprocessor is busy. Response retry attempt %d", _sendAttemptCount );
					ResendAutoTxFrame();
				}else{
					LOG( Logger::LOG_WARNING, "Coprocessor is busy. Max response retry attempts reached." );
					_autoTxState = AutoTxState_Idle;
				}
			}
			if( _autoTxState == AutoTxState_Idle )
			{
				if( _txState == TxState_Interrupted )
				{
					_sendAttemptCount = 0;
					NextSendAttempt();
					return;
				}
			}
		}
		else
		{
			LOG( Logger::LOG_ERROR, "Response ID do not match ");
		}
	}

	if( (_txState != TxState_Idle) && (_txState != TxState_Interrupted) && ((lowLevelMacResponse.GetId() == _regularFrameId) ||  (lowLevelMacResponse.GetId() == _autoTxFrameId)))
	{
		if( lowLevelMacResponse.GetResponseCode() == LowLevelMacFrameResponse::ResponseCode_Ack )
		{
			_sendAttemptCount = 0;
			if( _txState == TxState_WaitCoproResponseAesAck )
			{
				_txState = TxState_WaitAesAck;
				LOG( Logger::LOG_DEBUG, "_txState = TxState_WaitAesAck" );
				_txTimeoutTimeRegular = Sysutils::GetMonotonicTime() + AckTimeoutRegular;
			}
			else if(_txState == TxState_WaitCoproResponseAesSolution) {
				_txState = TxState_WaitAesSolution;
				LOG( Logger::LOG_DEBUG, "_txState = TxState_WaitAesSolution" );
				_txTimeoutTimeRegular = Sysutils::GetMonotonicTime() + AckTimeoutRegular;
			}
			else{
				if( _curTxTelegram.GetFlags() & BidcosTelegram::Flag_BiDi )
				{
					_txState = TxState_WaitAck;
					LOG( Logger::LOG_DEBUG, "_txState = TxState_WaitAck" );
					if( (_txRate == TxRate_100k) && (_curTxTelegram.GetFrameType() == BidcosTelegram::FrameType_Update) )
					{
						_txTimeoutTimeRegular = Sysutils::GetMonotonicTime() + AckTimeoutUpdate;
					}else{
						_txTimeoutTimeRegular = Sysutils::GetMonotonicTime() + AckTimeoutRegular;
					}
					if( (_curTxBurstMode == HmLegacyFrameBidcosTxTelegram::BurstMode_Triple) && (_tripleBurstCounter == 0) )
					{
						_tripleBurstFirstSlotStartTimeCopro = lowLevelMacResponse.GetPayload().GetUInt16Value( 0 );
						LOG( Logger::LOG_DEBUG, "3burst: Trial 1 sent @%dms", _tripleBurstFirstSlotStartTimeCopro );
					}
				}else{
					_txState = TxState_Idle;
					LOG( Logger::LOG_DEBUG, "_txState = TxState_Idle" );
					HmLegacyFrameBidcosResponse* response = new HmLegacyFrameBidcosResponse( HmLegacyFrameBidcosResponse::ResponseCode_SendOk );
					response->SetSequenceCounter( _curTxSequenceCounter );

					_subsystem.SendFrameUpstream( response );
				}
			}
		} else if( _txState != TxState_WaitCoproResponseAesAck && _txState != TxState_WaitCoproResponseAesSolution) {
			if( _autoTxState != AutoTxState_Idle )
			{
				_txState = TxState_Interrupted;
				LOG( Logger::LOG_DEBUG, "BidcosMacController::OnLowLevelMacResponse(): _txState = TxState_Interrupted. _autoTxState = %s",printAutoTxState(_autoTxState).c_str() );
				_txTimeoutTimeRegular = Sysutils::GetMonotonicTime() + CoproTimeout;
			} else {
				if( _sendAttemptCount < MaxSendAttemptsFastRetry )
				{
					LOG( Logger::LOG_DEBUG, "Coprocessor is busy. Fast retry attempt %d", _sendAttemptCount );
					NextSendAttempt();
				}else if( (!_tripleBurstCounter) && (_sendAttemptCount < MaxSendAttemptsFastRetry + MaxSendAttemptsSlowRetry) ){
					_txState = TxState_CoproBusy;
					LOG( Logger::LOG_DEBUG, "_txState = TxState_CoproBusy" );
					_txTimeoutTimeRegular = Sysutils::GetMonotonicTime() + CoproBusySlowRetryDelay;
				}else if( (_tripleBurstCounter > 0) && (_tripleBurstCounter < MaxTripleBurstCounter) ){
					LOG( Logger::LOG_DEBUG, "Max send attempts reached for current triple burst slot. Trying next slot." );
					_sendAttemptCount = 0;
					NextSendAttempt();
				}else{
					LOG( Logger::LOG_DEBUG, "Max send attempts reached. Aborting send." );

					HmLegacyFrameBidcosResponse* response = 0;
					switch( lowLevelMacResponse.GetResponseCode() )
					{
					case LowLevelMacFrameResponse::ResponseCode_Busy:
						response = new HmLegacyFrameBidcosResponse( HmLegacyFrameBidcosResponse::ResponseCode_Busy );
						break;
					case LowLevelMacFrameResponse::ResponseCode_InvalidInput:
						response = new HmLegacyFrameBidcosResponse( HmLegacyFrameBidcosResponse::ResponseCode_InputError );
						break;
					case LowLevelMacFrameResponse::ResponseCode_Abort:
					case LowLevelMacFrameResponse::ResponseCode_CcaFailed:
					case LowLevelMacFrameResponse::ResponseCode_WrongFreqBitrate:
					case LowLevelMacFrameResponse::ResponseCode_BusyTrx:
						response = new HmLegacyFrameBidcosResponse( HmLegacyFrameBidcosResponse::ResponseCode_SendFailedCsmaCa );
						break;
					case LowLevelMacFrameResponse::ResponseCode_DutyCycleFull:
						response = new HmLegacyFrameBidcosResponse( HmLegacyFrameBidcosResponse::ResponseCode_SendFailedDutyCycle );
						break;
					case LowLevelMacFrameResponse::ResponseCode_Error:
					default:
						response = new HmLegacyFrameBidcosResponse( HmLegacyFrameBidcosResponse::ResponseCode_Error );
						break;
					}
					if(response) {
						response->SetSequenceCounter( _curTxSequenceCounter );
						_subsystem.SendFrameUpstream( response );
					}

					if( (_txRate == TxRate_100k) && (_curTxTelegram.GetFlags() & BidcosTelegram::Flag_BiDi) ) {
						_txState = TxState_WaitCoproResponse;
						LOG( Logger::LOG_DEBUG, "_txState = TxState_WaitCoproResponse" );
						LowLevelMacFrameRxModeDefault* unlockRxModeFrame = new LowLevelMacFrameRxModeDefault();
						_regularFrameId = unlockRxModeFrame->GetId();
						_txTimeoutTimeRegular = Sysutils::GetMonotonicTime() + AckTimeoutRegular;
						_subsystem.SendFrameDownstream( unlockRxModeFrame );
					}
					else {
						_txState = TxState_Idle;
						LOG( Logger::LOG_DEBUG, "_txState = TxState_Idle" );
					}
				}
			}
		}
	}
}

void BidcosMacController::OnAckTimeout()
{
	if(_waitAesAckTimedOut) {
		OnAesAckTimeout();
	}
	LOG( Logger::LOG_DEBUG, "No ACK for Bidcos TX: %s", _curTxTelegram.ToString().c_str());
	if( _repetitionCount >= MaxNumberOfRepetitions )
	{
		LOG( Logger::LOG_INFO, "No ACK received after %d repetitions: %s", _repetitionCount, _curTxTelegram.ToString().c_str() );
		HmLegacyFrameBidcosResponse* response = new HmLegacyFrameBidcosResponse(HmLegacyFrameBidcosResponse::ResponseCode_SendFailed);
		response->SetSequenceCounter( _curTxSequenceCounter );

		_subsystem.SendFrameUpstream( response );

		_txState = TxState_Idle;
		LOG( Logger::LOG_DEBUG, "_txState = TxState_Idle" );
		return;
	}

	if( (_curTxBurstMode == HmLegacyFrameBidcosTxTelegram::BurstMode_Triple) && (_tripleBurstCounter < MaxTripleBurstCounter) )
	{
		LOG( Logger::LOG_DEBUG, "No ACK for current triple burst slot. Trying next slot." );
		_tripleBurstCounter++;
		_sendAttemptCount = 0;
	}else{
		_tripleBurstCounter = 0;
		_repetitionCount++;
	}
	NextSendAttempt();
}

void BidcosMacController::OnAesAckTimeout()
{
	
	_waitAesAckTimedOut = true;
	if(_curTxTelegram.GetTelegramCounter() == _curAutoTxTelegram.GetTelegramCounter()  && //(this may always be true because Aes Ack Timeout is only possible if device requested AES and that's only possible if mmd initiated the communication.)
			_curTxTelegram.GetReceiverAddress() == _curAutoTxTelegram.GetReceiverAddress()) {//SPHM-481: check if timeout corresponds to curTxTelegram
		LOG( Logger::LOG_DEBUG, "No AES ACK for Bidcos TX: %s", _curTxTelegram.ToString().c_str());
		if(_repetitionCount >= MaxNumberOfRepetitions) 
		{
			LOG( Logger::LOG_INFO, "No AES ACK received after %d repetitions: %s", _repetitionCount, _curTxTelegram.ToString().c_str() );

			HmLegacyFrameBidcosResponse* response = new HmLegacyFrameBidcosResponse(HmLegacyFrameBidcosResponse::ResponseCode_SendFailedAuth);
			response->SetSequenceCounter( _curTxSequenceCounter );

			if( _txAesChallengeInfo )
			{
				const uint8_t usedKeyIndex = _txAesChallengeInfo->GetKeyIndex();
				response->SetKeyIndex( _bidcosContext.IsKnownAesKeyIndex(usedKeyIndex) ?  usedKeyIndex : (uint8_t)0xFE);
				delete _txAesChallengeInfo;
				_txAesChallengeInfo = NULL;
			}

			_subsystem.SendFrameUpstream( response );

			_txState = TxState_Idle;
			LOG( Logger::LOG_DEBUG, "_txState = TxState_Idle" );
			return;
		}
		LOG( Logger::LOG_INFO, "No AES ACK received, repeating: %s", _curTxTelegram.ToString().c_str() );
		_repetitionCount++;
		NextSendAttempt();
	}
}

void BidcosMacController::OnAesSolutionTimeout()
{
	//check if there is a curTxTelegram  with matching counters to timed out solution
	if(_curTxTelegram.GetTelegramCounter() == _curAutoTxTelegram.GetTelegramCounter() &&
		_curTxTelegram.GetReceiverAddress() == _curAutoTxTelegram.GetReceiverAddress() ) { //SPHM-481 MMD repeats curTxTelegram triggered by SolutionTimout caused by another device
		//rfd/mmd initiated communication
		// -> repeat curTxTelegram depending on number of repetitions which may already performed
		if(_repetitionCount >= MaxNumberOfRepetitions) {//max repetitions reached -> abort and tell rfd about failure
			LOG(Logger::LOG_INFO, "No aes solution received (repetitions %d) for %s.", _repetitionCount, _curTxTelegram.ToString().c_str() );
			if(_txAesChallengeInfo != NULL) {
				delete _txAesChallengeInfo;
				_txAesChallengeInfo = NULL;
			}

			HmLegacyFrameBidcosResponse* response = new HmLegacyFrameBidcosResponse(HmLegacyFrameBidcosResponse::ResponseCode_SendFailed);//FIXME code okay?
			response->SetSequenceCounter( _curTxSequenceCounter );
			_subsystem.SendFrameUpstream( response );
			_txState = TxState_Idle;
			_autoTxState = AutoTxState_Idle;
			LOG( Logger::LOG_DEBUG, "_txState = TxState_Idle" );
			LOG( Logger::LOG_DEBUG, "_autoTxState = AutoTxState_Idle" );
		}
		else {
			LOG(Logger::LOG_INFO, "No AES solution received, repeating: %s", _curTxTelegram.ToString().c_str() );
			_repetitionCount++;
			NextSendAttempt();
		}
	}
	else {//device initiated communication (INFO telegram (without request from rfd))
		// -> just ignore it because the device is the initiator of the communication and has to repeat it.
		LOG(Logger::LOG_DEBUG, "No aes solution received. Repetition not possible because communication has been initiated by device.");
		_txState = TxState_Idle;
		_autoTxState = AutoTxState_Idle;
		LOG( Logger::LOG_DEBUG, "_txState = TxState_Idle" );
		LOG( Logger::LOG_DEBUG, "_autoTxState = AutoTxState_Idle" );
	}


}

void BidcosMacController::OnCyclicCall()
{
	uint32_t now = Sysutils::GetMonotonicTime();

	if( _autoTxState != AutoTxState_Idle )
	{
		if( int32_t( now - _autoTxTimeoutTime ) >= 0 )
		{
			LOG( Logger::LOG_ERROR, "Copro timeout on response send. _autoTxState = %s",printAutoTxState(_autoTxState).c_str() );
			_autoTxState = AutoTxState_Idle;
		}
	}

	if( int32_t( now - _txTimeoutTimeCyclicTimeInfo ) >= 0 )
	{
		if( _autoTxState == AutoTxState_Idle )
		{
			SendCyclicTimeInfo();
			_txTimeoutTimeCyclicTimeInfo = now + CyclicTimeInfoInterval + rand() % 300000;
		}else{
			_txTimeoutTimeCyclicTimeInfo += 500;
		}
	}


	switch( _txState )
	{
	case TxState_Idle:
		if( _autoTxState == AutoTxState_Idle )
		{
			CleanupAesChallengeInfoMap();
		}
		break;
	case TxState_Interrupted:
		if( _autoTxState == AutoTxState_Idle )
		{
			NextSendAttempt();
		} else if( int32_t( now - _txTimeoutTimeRegular ) >= 0 )
		{
			LOG( Logger::LOG_ERROR, "Copro timeout on regular interrupted. _autoTxState = %s",printAutoTxState(_autoTxState).c_str() );
			_txState = TxState_Idle;
			LOG( Logger::LOG_DEBUG, "_txState = TxState_Idle" );
		}
		break;
	case TxState_WaitAck:
		if( int32_t( now - _txTimeoutTimeRegular ) >= 0 )
		{
			OnAckTimeout();
		}
		break;
	case TxState_WaitAesAck:
		if( int32_t( now - _txTimeoutTimeRegular ) >= 0 )
		{
			OnAesAckTimeout();
		}
		break;
	case TxState_WaitAesSolution:
		if( int32_t( now - _txTimeoutTimeRegular ) >= 0 )
		{
			OnAesSolutionTimeout();
		}
		break;
	case TxState_WaitCoproResponse:
		if( int32_t( now - _txTimeoutTimeRegular ) >= 0 )
		{
			LOG( Logger::LOG_ERROR, "Copro timeout on regular send. _autoTxState = %s",printAutoTxState(_autoTxState).c_str() );
			_txState = TxState_Idle;
			LOG( Logger::LOG_DEBUG, "_txState = TxState_Idle" );
		}
		break;
	case TxState_WaitCoproResponseAesAck:
		if( int32_t( now - _txTimeoutTimeRegular ) >= 0 )
		{
			LOG( Logger::LOG_ERROR, "Copro timeout on AES send" );
			delete _txAesChallengeInfo;
			_txAesChallengeInfo = NULL;
			_txState = TxState_Idle;
			LOG( Logger::LOG_DEBUG, "_txState = TxState_Idle" );
			if(_autoTxState == AutoTxState_AesAck) {
				_autoTxState = AutoTxState_Idle;
				LOG(Logger::LOG_DEBUG, "_autoTxState = autoTxState_Idle");
			}

		}
		break;
	case TxState_WaitCoproResponseAesSolution:
		if( int32_t( now - _txTimeoutTimeRegular ) >= 0 )
		{
			LOG( Logger::LOG_ERROR, "Copro timeout on ACK_AESChallenge send" );
			_txState = TxState_Idle;
			LOG( Logger::LOG_DEBUG, "_txState = TxState_Idle" );
			if(_autoTxState == AutoTxState_AesChallenge) {
				_autoTxState = AutoTxState_Idle;
				LOG(Logger::LOG_DEBUG, "_autoTxState=AutoTxState_Idle");
			}
		}
		break;
	case TxState_WaitTxDelay:
		if( _autoTxState == AutoTxState_Idle )
		{
			NextSendAttempt();
		}else{
			_txState = TxState_Interrupted;
			_txTimeoutTimeRegular = now + CoproTimeout;
			LOG( Logger::LOG_DEBUG, "BidcosMacController::OnCyclicCall()): _txState = TxState_Interrupted. _autoTxState = %s old tx_state = TxState_WaitTxDelay",printAutoTxState(_autoTxState).c_str() );
		}
		break;
	case TxState_CoproBusy:
		if( _autoTxState == AutoTxState_Idle )
		{
			NextSendAttempt();
		} else if( int32_t( now - _txTimeoutTimeRegular ) >= 0 )
		{
			_txState = TxState_Interrupted;
			_txTimeoutTimeRegular = now + CoproTimeout;
			LOG( Logger::LOG_DEBUG, "BidcosMacController::OnCyclicCall()): _txState = TxState_Interrupted. _autoTxState = %s old tx_state = TxState_CoproBusy",printAutoTxState(_autoTxState).c_str() );
		}
		break;
	}
}
std::string BidcosMacController::printAutoTxState(AutoTxState state)
{
	std::string retVal;
	switch(state)
	{
	case AutoTxState_Idle:
		retVal = "AutoTxState_Idle";
		break;
	case AutoTxState_Ack:
		retVal = "AutoTxState_Ack";
				break;
	case AutoTxState_AesChallenge:
		retVal = "AutoTxState_AesChallenge";
				break;
	case AutoTxState_AesAck:
		retVal = "AutoTxState_AesAck";
				break;
	case AutoTxState_CallCcu:
		retVal = "AutoTxState_CallCcu";
				break;
	case AutoTxState_Wakeup:
		retVal = "AutoTxState_Wakeup";
				break;
	case AutoTxState_TimeInfo:
		retVal = "AutoTxState_TimeInfo";
				break;
	default:
		retVal = "unknown";
				break;
	}
	return retVal;
}
uint32_t BidcosMacController::GetSleepTime()
{
	uint32_t now = Sysutils::GetMonotonicTime();
	int32_t sleepTime = INT32_MAX;

	if( _autoTxState != AutoTxState_Idle )
	{
		sleepTime = _autoTxTimeoutTime - now;
	}

	if( _txState != TxState_Idle )
	{
		sleepTime = std::min( sleepTime, int32_t(_txTimeoutTimeRegular - now) );
	}

	if( sleepTime < INT32_MAX )
	{
		//LOG( Logger::LOG_DEBUG, "SleepTime=%d", sleepTime );
	}

	if( _autoTxState == AutoTxState_Idle )
	{
		sleepTime = std::min( sleepTime, int32_t(_txTimeoutTimeCyclicTimeInfo - now) );
	}

	return std::max( sleepTime, 0 );
}

uint16_t BidcosMacController::CalculateFrameStartTime( LowLevelMacFrameRxTelegram& frame )
{
	uint16_t frameEndTime = frame.GetFrameEndTime();
	uint32_t bitCount = frame.GetPayload().size() * 8 + BidcosLengthBits + BidcosPreambleBits + BidcosSyncwordSize + BidcosCrcBits;
	return frameEndTime - bitCount * 1000 / BidcosBitRate;
}

void BidcosMacController::CleanupAesChallengeInfoMap()
{
	for( MapPeerAddressToAesChallengeInfo::iterator it = _mapPeerAddressToAesChallengeInfo.begin(); it != _mapPeerAddressToAesChallengeInfo.end(); it++ )
	{
		if( it->second->IsExpired() )
		{
			delete it->second;
			_mapPeerAddressToAesChallengeInfo.erase( it );
			break;
		}
	}
}

void BidcosMacController::OnTimeInfoChanged()
{
	_txTimeoutTimeCyclicTimeInfo = Sysutils::GetMonotonicTime() + 2000 + rand() % 10000;
}

void BidcosMacController::SetCsmaCaEnabled( bool enabled )
{
	_csmaCaEnabled = enabled;
}

bool BidcosMacController::GetCsmaCaEnabled()const
{
	return _csmaCaEnabled;
}

void BidcosMacController::SetTxRate( TxRate txRate )
{
	_txRate = txRate;
}

BidcosMacController::TxRate BidcosMacController::GetTxRate()const
{
	return _txRate;
}


bool BidcosMacController::GetRandomNumber( uint8_t* buffer, size_t size )
{
#ifdef WIN32
	return false;
#else
	if( _urandomFd < 0 )
	{
		_urandomFd = open( "/dev/urandom", O_RDONLY );
		if( _urandomFd < 0 )
		{
			char buffer[256];
			char *err = strerror_r( errno, buffer, sizeof( buffer ) );
			LOG( Logger::LOG_ERROR, "Could not open /dev/urandom: %s", err );
			return false;
		}
	}

	int count = read( _urandomFd, buffer, size );
	if( count != int(size) )
	{
		if( count < 0 )
		{
			char buffer[256];
			char *err = strerror_r( errno, buffer, sizeof( buffer ) );
			LOG( Logger::LOG_ERROR, "Error reading from /dev/urandom: %s", err );
		}else{
			LOG( Logger::LOG_ERROR, "/dev/urandom short read: %d", count );
		}
		return false;
	}
	return true;

#endif

}


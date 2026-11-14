/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "BidcosAesChallengeInfo.h"
#include "BidcosContext.h"
#include "Sysutils.h"
#include <string.h>
#include <Logger.h>
#include <openssl/evp.h>

BidcosAesChallengeInfo::BidcosAesChallengeInfo(const BidcosTelegram& originalTelegram, uint8_t* challenge, uint8_t keyIndex, int rssi )
{
	_originalTelegram = originalTelegram;
//	_originalTelegram.SetFlags( _originalTelegram.GetFlags() & ~BidcosTelegram::Flag_Repeated );
	_keyIndex = keyIndex;
	memcpy( _challenge, challenge, sizeof( _challenge ) );
	_ackTailData = 0;
	_nakTailData = 0;
	_expirationTime = Sysutils::GetMonotonicTime() + 2000;
	_rssi = rssi;
}

BidcosAesChallengeInfo::BidcosAesChallengeInfo(const BidcosTelegram& originalTelegram)
{
	_originalTelegram = originalTelegram;
//	_originalTelegram.SetFlags( _originalTelegram.GetFlags() & ~BidcosTelegram::Flag_Repeated );
	_keyIndex = 0;
	memset( _challenge, 0, sizeof( _challenge ) );
	_ackTailData = 0;
	_nakTailData = 0;
	_expirationTime = Sysutils::GetMonotonicTime() + 2000;
	_rssi = 0;
}

BidcosAesChallengeInfo::~BidcosAesChallengeInfo(void)
{
}

bool BidcosAesChallengeInfo::CheckSolution( const BidcosContext& bidcosContext, const BidcosTelegram& solutionTelegram )
{
	if( IsExpired() )
	{
		return false;
	}
	BinaryData telegramData = _originalTelegram.GetRawData();

	//LOG( Logger::LOG_DEBUG, "originalTelegram=%s", _originalTelegram.ToString().c_str() );
	//LOG( Logger::LOG_DEBUG, "originalTelegramRaw=%s", telegramData.ToString().c_str() );
	//LOG( Logger::LOG_DEBUG, "sloutionTelegram=%s", solutionTelegram.ToString().c_str() );

	uint8_t secondBlock[16];
	solutionTelegram.GetPayload().GetBinaryData( secondBlock, 0, 16 );

	//LOG( Logger::LOG_DEBUG, "challenge=%s", BinaryData( _challenge, 6).ToString().c_str() );

	//LOG( Logger::LOG_DEBUG, "secondBlock=%s", BinaryData( secondBlock, 16).ToString().c_str() );

	uint8_t* key = bidcosContext.GetAesKeyByIndex( _keyIndex ).GetSessionKey( BinaryData(_challenge, 6) );
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	EVP_DecryptInit_ex( ctx, EVP_aes_128_ecb(), NULL, key, NULL);
	// The BidCoS AES signature is a raw single-block ECB operation (no PKCS7
	// padding). With OpenSSL's default padding enabled, EVP_DecryptUpdate holds
	// the final block back for padding removal and emits 0 bytes for a lone
	// 16-byte block, leaving firstBlock uninitialised -> "AES solution not
	// accepted". Disable padding so the block is decrypted in place.
	EVP_CIPHER_CTX_set_padding( ctx, 0 );

	uint8_t firstBlock[16];
	//dec->ProcessData( secondBlock, firstBlock );
	int size = 0;
	EVP_DecryptUpdate( ctx, firstBlock, &size, secondBlock, 16 );

	for( size_t i=0; i<16 && i+10 < telegramData.size(); i++ )
	{
		firstBlock[i] ^= telegramData[i + 10];
	}

	//LOG( Logger::LOG_DEBUG, "firstBlock=%s", BinaryData( firstBlock, 16).ToString().c_str() );

	uint8_t plaintext[16];

	EVP_DecryptUpdate( ctx, plaintext, &size, firstBlock, 16 );

	EVP_CIPHER_CTX_free( ctx );
	delete[] key;


	//LOG( Logger::LOG_DEBUG, "plaintext=%s", BinaryData( plaintext, 16).ToString().c_str() );

	BinaryData telegramHeader = telegramData.GetRange(0, 10);
	//ignore repeated flag on check
	telegramHeader[1] &= ~BidcosTelegram::Flag_Repeated;

	if( BinaryData( plaintext + 6, 10 ) == telegramHeader )
	{
		LOG( Logger::LOG_DEBUG, "AES solution accepted" );
		BinaryData tailData( firstBlock, 8 );
		_ackTailData = tailData.GetUInt32Value( 0 );
		_nakTailData = tailData.GetUInt32Value( 4 );
		return true;
	} else {
		LOG( Logger::LOG_DEBUG, "AES solution not accepted" );
		return false;
	}
}

uint32_t BidcosAesChallengeInfo::GetAckTailData()
{
	return _ackTailData;
}

uint32_t BidcosAesChallengeInfo::GetNakTailData()
{
	return _nakTailData;
}

const BidcosTelegram& BidcosAesChallengeInfo::GetOriginalTelegram()const
{
	return _originalTelegram;
}

bool BidcosAesChallengeInfo::IsExpired()const
{
	return int32_t( Sysutils::GetMonotonicTime() - _expirationTime ) >= 0;
}

BidcosTelegram BidcosAesChallengeInfo::CalculateSolution(const BidcosContext& bidcosContext, const BidcosTelegram& challengeTelegram, const uint8_t* random)
{

	BinaryData payload = challengeTelegram.GetPayload();

	payload.GetBinaryData( _challenge, 1, 6 );
	_keyIndex = payload.GetUInt8Value( 7 ) >> 1;

	LOG( Logger::LOG_DEBUG, "challenge=%s, keyIndx=%d", BinaryData( _challenge, 6).ToString().c_str(), _keyIndex );

	uint8_t* key = bidcosContext.GetAesKeyByIndex( _keyIndex ).GetSessionKey( BinaryData( _challenge, 6) );
	EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
	EVP_EncryptInit_ex( ctx, EVP_aes_128_ecb(), NULL, key, NULL);
	// raw single-block ECB, no PKCS7 padding (see CheckSolution)
	EVP_CIPHER_CTX_set_padding( ctx, 0 );

	uint8_t buffer[16];

	memcpy( buffer, random, 6 );

	//LOG( Logger::LOG_DEBUG, "random=%s", BinaryData( random, 6).ToString().c_str() );

	BinaryData telegramData = _originalTelegram.GetRawData();
	telegramData.GetBinaryData( buffer + 6, 0, 10 );

	//LOG( Logger::LOG_DEBUG, "plaintext=%s", BinaryData( buffer, 16).ToString().c_str() );

	uint8_t firstBlock[16];
	int size = 0;
	EVP_EncryptUpdate( ctx, firstBlock, &size, buffer, 16 );

	//LOG( Logger::LOG_DEBUG, "firstBlock=%s", BinaryData( firstBlock, 16).ToString().c_str() );

	telegramData.GetBinaryData( buffer, 10, 16 );
	for( int i=0; i<16; i++ )
	{
		buffer[i] ^= firstBlock[i];
	}

	//LOG( Logger::LOG_DEBUG, "intermediate=%s", BinaryData( buffer, 16).ToString().c_str() );

	uint8_t secondBlock[16];
	EVP_EncryptUpdate( ctx, secondBlock, &size, buffer, 16 );

	EVP_CIPHER_CTX_free( ctx );
	delete[] key;


	//LOG( Logger::LOG_DEBUG, "secondBlock=%s", BinaryData( secondBlock, 16).ToString().c_str() );

	BidcosTelegram aesSolution;
	aesSolution.SetTelegramCounter( _originalTelegram.GetTelegramCounter() );
	aesSolution.SetReceiverAddress( _originalTelegram.GetReceiverAddress() );
	aesSolution.SetSenderAddress( _originalTelegram.GetSenderAddress() );
	aesSolution.SetFrameType( BidcosTelegram::FrameType_AesSolution );
	aesSolution.SetFlags( _originalTelegram.GetFlags() & ~(BidcosTelegram::Flag_Burst | BidcosTelegram::Flag_Broadcast) );
	aesSolution.SetPayload( BinaryData( secondBlock, 16 ) );

	BinaryData expectedAckData( firstBlock, 8 );
	_ackTailData = expectedAckData.GetUInt32Value( 0 );
	_nakTailData = expectedAckData.GetUInt32Value( 4 );

	return aesSolution;

}

bool BidcosAesChallengeInfo::CheckAck( const BidcosTelegram& ackTelegram )
{
	if( ackTelegram.GetPayload().size() >= 4 )
	{
		uint32_t tailData = ackTelegram.GetPayload().GetUInt32Value( ackTelegram.GetPayload().size() - 4 );
		if( ackTelegram.GetAckCode() < BidcosTelegram::AckCode_Nak )
		{
			return tailData == _ackTailData;
		} else {
			return tailData == _nakTailData;
		}
	}
	return false;
}

uint8_t BidcosAesChallengeInfo::GetKeyIndex()const
{
	return _keyIndex;
}

void BidcosAesChallengeInfo::SetRssi( int rssi )
{
	_rssi = rssi;
}

int BidcosAesChallengeInfo::GetRssi()const
{
	return _rssi;
}

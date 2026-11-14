/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include "HmLegacyFrameBidcos.h"
#include "../../BidcosAesKey.h"
class HmLegacyFrameBidcosResponse :
	public HmLegacyFrameBidcos
{
public:
	enum ResponseCode
	{
		ResponseCode_Error,
		ResponseCode_Ok,
		ResponseCode_SendOk,
		ResponseCode_SendOkAndAckReceived,
		ResponseCode_SendFailed,
		ResponseCode_SendFailedDutyCycle,
		ResponseCode_SendFailedCsmaCa,
		ResponseCode_OkWithData,
		ResponseCode_Busy,
		ResponseCode_TransceiverBusy,
		ResponseCode_NotInitialized,
		ResponseCode_InputError,
		ResponseCode_SendOkWithAuthOk,
		ResponseCode_SendFailedAuth
	};

	//! default constructor. Don't use! Needed for dynamic
	HmLegacyFrameBidcosResponse();
	HmLegacyFrameBidcosResponse(const ResponseCode responseCode);
	virtual ~HmLegacyFrameBidcosResponse(void);
	ResponseCode GetResponseCode()const;
	void SetResponseCode( ResponseCode responseCode );

	//ResponseCode_SendOkAndAckReceived, ResponseCode_SendOkWithAuthOk, ResponseCode_SendFailedAuth
	uint8_t GetKeyIndex()const;
	void SetKeyIndex( uint8_t keyIndex );

	//ResponseCode_OkWithData for FrameType_PeerGetAesKeyId
	//uint8_t GetPeerKeyIndex()const;
	void SetPeerKeyIndex( uint8_t keyIndex );

	//ResponseCode_SendOkAndAckReceived, ResponseCode_SendOkWithAuthOk
	int GetRssi()const;
	void SetRssi( int rssi );

	//ResponseCode_SendOkAndAckReceived, ResponseCode_SendOkWithAuthOk
	BinaryData GetResponseTelegram()const;
	void SetResponseTelegram( const BinaryData& telegramData );

	//ResponseCode_OkWithData
	uint8_t GetDataChunkIndex()const;
	void SetDataChunkIndex( uint8_t keyIndex );

	//ResponseCode_OkWithData
	uint8_t GetDataChunkCount()const;
	void SetDataChunkCount( uint8_t keyIndex );

	//ResponseCode_OkWithData
	BinaryData GetResponseData()const;
	void SetResponseData( const BinaryData& responseData );

	//ResponseCode_OkWithData for FrameType_GetRfAddress, FrameType_GetDefaultRfAddress, FrameType_PeerGetAesKeyId
	uint32_t GetRfAddress()const;
	void SetRfAddress( uint32_t rfAddress );

	//ResponseCode_OkWithData for FrameType_PeerAdd, FrameType_PeerRemove
	uint16_t GetNumberOfPeers()const;
	void SetNumberOfPeers( uint16_t numberOfPeers );

	//ResponseCode_OkWithData for FrameType_PeerAdd
	uint64_t GetAuthChannels()const;
	void SetAuthChannels( uint64_t authChannels );
	bool isResponseFrame();

	virtual std::string ToString()const;

protected:
	ResponseCode _responseCode;

	void LogIllegalGetOperation(const char* msg) const;
	void LogIllegalSetOperation(const char* msg) const;

};


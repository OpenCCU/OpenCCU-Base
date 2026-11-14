/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HmLegacyFrameBidcosResponse.h"
#include <stdio.h>
#include <Logger.h>

#define LOG_ILLEGAL_GET_OPERATION_PREFIX "HmLegacyFrameBidcosResponse - Illegal Get-Operation: "
#define LOG_ILLEGAL_SET_OPERATION_PREFIX "HmLegacyFrameBidcosResponse - Illegal Set-Operation: "

/// @cond ---- Exclude from Doxygen build ----
static Dynamic::StaticMachine<HmLegacyFrame, HmLegacyFrameBidcosResponse, int> m((HmLegacyFrame::SubsystemType_Bidcos<<8)|HmLegacyFrameBidcos::FrameType_Response);
/// @endcond

HmLegacyFrameBidcosResponse::HmLegacyFrameBidcosResponse(ResponseCode responseCode)
: HmLegacyFrameBidcos( FrameType_Response )
,_responseCode(responseCode)
{
	Data().SetUInt8Value(0, (uint8_t)responseCode );
	switch(responseCode) {
		case ResponseCode_OkWithData:
			if(GetDataChunkCount() == 0) {
				SetDataChunkIndex(0);//Feature not used yet (by rfd)
				SetDataChunkCount(1);
			}
			break;
		default:
			break;
	}
}

//! Default constructor. Don't use! Needed for dynamic frame creation
HmLegacyFrameBidcosResponse::HmLegacyFrameBidcosResponse()
: HmLegacyFrameBidcos( FrameType_Response )
{
}


HmLegacyFrameBidcosResponse::~HmLegacyFrameBidcosResponse(void)
{
}


HmLegacyFrameBidcosResponse::ResponseCode HmLegacyFrameBidcosResponse::GetResponseCode()const
{
	return _responseCode;
}

/*void HmLegacyFrameBidcosResponse::SetResponseCode( ResponseCode responseCode )
{
	Data().SetUInt8Value( 0, (uint8_t)responseCode );
	if( responseCode == ResponseCode_OkWithData && GetDataChunkCount() == 0 )
	{
		SetDataChunkCount( 1 );
	}
}*/


//ResponseCode_SendOkAndAckReceived, ResponseCode_SendOkWithAuthOk, ResponseCode_SendFailedAuth
uint8_t HmLegacyFrameBidcosResponse::GetKeyIndex()const
{
	switch (_responseCode) {
		case ResponseCode_SendOkAndAckReceived:
		case ResponseCode_SendOkWithAuthOk:
		case ResponseCode_SendFailedAuth:
			return Data().GetUInt8Value( 1 );
		default:
			LogIllegalGetOperation("GetKeyIndex");
			return 0;
	}
}


void HmLegacyFrameBidcosResponse::SetKeyIndex( uint8_t keyIndex )
{
	switch (_responseCode) {
		case ResponseCode_SendOkAndAckReceived:
		case ResponseCode_SendOkWithAuthOk:
		case ResponseCode_SendFailedAuth:
			Data().SetUInt8Value( 1, keyIndex );
			break;
		default:
			LogIllegalSetOperation("GetKeyIndex");
			break;
	}
}

//ResponseCode_OkWithData for FrameType_PeerGetAesKeyId
/*uint8_t HmLegacyFrameBidcosResponse::GetPeerKeyIndex()const
{
	return BidcosAesKey::KeyId(Data().GetUInt8Value( 6 ));
}
*/


//ResponseCode_OkWithData for FrameType_PeerGetAesKeyId
void HmLegacyFrameBidcosResponse::SetPeerKeyIndex( uint8_t keyIndex )
{
	if(_responseCode == ResponseCode_OkWithData ) {
		Data().SetUInt8Value( 6, keyIndex );
	}
}


//ResponseCode_SendOkAndAckReceived, ResponseCode_SendOkWithAuthOk
int HmLegacyFrameBidcosResponse::GetRssi()const
{
	switch(_responseCode) {
		case ResponseCode_SendOkAndAckReceived:
		case ResponseCode_SendOkWithAuthOk:
			return Data().GetUInt8Value( 2 );
		default:
			LogIllegalGetOperation("GetRssi");
			return 0;
	}
}


void HmLegacyFrameBidcosResponse::SetRssi( int rssi )
{
	switch(_responseCode) {
		case ResponseCode_SendOkAndAckReceived:
		case ResponseCode_SendOkWithAuthOk:
			Data().SetUInt8Value( 2, rssi );
			break;
		default:
			LogIllegalSetOperation("SetRssi");
			break;
	}
}


//ResponseCode_SendOkAndAckReceived, ResponseCode_SendOkWithAuthOk
BinaryData HmLegacyFrameBidcosResponse::GetResponseTelegram()const
{
	switch(_responseCode) {
		case ResponseCode_SendOkAndAckReceived:
		case ResponseCode_SendOkWithAuthOk:
			if( Data().size() > 3 )
			{
				return Data().GetRange( 3, Data().size()-3);
			}
			break;
		case ResponseCode_OkWithData://convenience
			return GetResponseData();
		default:
			LogIllegalGetOperation("GetResponseTelegram");
	}
	return BinaryData();
}


void HmLegacyFrameBidcosResponse::SetResponseTelegram( const BinaryData& telegramData )
{
	switch(_responseCode) {
		case ResponseCode_SendOkAndAckReceived:
		case ResponseCode_SendOkWithAuthOk:
			Data().SetRange( 3, telegramData );
			break;
		case ResponseCode_OkWithData://convenience
			SetResponseData(telegramData);
			break;
		default:
			LogIllegalSetOperation("SetResponseTelegram");
			break;
	}
}


//ResponseCode_OkWithData
uint8_t HmLegacyFrameBidcosResponse::GetDataChunkIndex()const
{
	if(_responseCode == ResponseCode_OkWithData) {
		return Data().GetUInt8Value( 1 );
	}
	else {
		LogIllegalGetOperation("GetDataChunkIndex");
		return 0;
	}
}


void HmLegacyFrameBidcosResponse::SetDataChunkIndex( uint8_t chunkIndex )
{
	if(_responseCode == ResponseCode_OkWithData) {
		Data().SetUInt8Value( 1, chunkIndex );
	}
	else {
		LogIllegalSetOperation("SetDataChunkIndex");
	}
}


//ResponseCode_OkWithData
uint8_t HmLegacyFrameBidcosResponse::GetDataChunkCount()const
{
	if(_responseCode == ResponseCode_OkWithData) {
		return Data().GetUInt8Value( 2 );
	}
	else {
		LogIllegalGetOperation("GetDataChunkCount");
		return 0;
	}

}


void HmLegacyFrameBidcosResponse::SetDataChunkCount( uint8_t chunkCount )
{
	if(_responseCode == ResponseCode_OkWithData) {
		Data().SetUInt8Value( 2, chunkCount );
	}
	else {
		LogIllegalSetOperation("SetDataChunkCount");
	}
}


//ResponseCode_OkWithData
BinaryData HmLegacyFrameBidcosResponse::GetResponseData()const
{
	switch(_responseCode) {
		case ResponseCode_OkWithData:
			if( Data().size() > 3 )
			{
				return Data().GetRange( 3, Data().size()-3);
			}
			break;
		case ResponseCode_SendOkAndAckReceived://convenience
		case ResponseCode_SendOkWithAuthOk:
			return GetResponseTelegram();
		default:
			LogIllegalGetOperation("GetResponseData");

	}
	return BinaryData();
}


void HmLegacyFrameBidcosResponse::SetResponseData( const BinaryData& responseData )
{
	switch(_responseCode) {
		case ResponseCode_OkWithData:
			Data().SetRange( 3, responseData );
			break;
		case ResponseCode_SendOkAndAckReceived://convenience
		case ResponseCode_SendOkWithAuthOk:
			SetResponseTelegram(responseData);
			break;
		default:
			LogIllegalSetOperation("SetResponseData");
			break;
	}
}

//ResponseCode_OkWithData for FrameType_GetRfAddress, FrameType_GetDefaultRfAddress, FrameType_PeerGetAesKeyId
uint32_t HmLegacyFrameBidcosResponse::GetRfAddress()const
{
	if(_responseCode != ResponseCode_OkWithData) {
		LogIllegalGetOperation("GetRfAddress");
	}
	return Data().GetUInt24Value( 3 );
}

void HmLegacyFrameBidcosResponse::SetRfAddress( uint32_t rfAddress )
{
	if(_responseCode != ResponseCode_OkWithData) {
		LogIllegalSetOperation("SetRfAddress");
	}
	Data().SetUInt24Value( 3, rfAddress );
}

//ResponseCode_OkWithData for FrameType_PeerAdd, FrameType_PeerRemove
uint16_t HmLegacyFrameBidcosResponse::GetNumberOfPeers()const
{
	if(_responseCode != ResponseCode_OkWithData) {
		LogIllegalGetOperation("GetNumberOfPeers");
	}
	return Data().GetUInt16Value( 3 );
}

void HmLegacyFrameBidcosResponse::SetNumberOfPeers( uint16_t numberOfPeers )
{
	if(_responseCode != ResponseCode_OkWithData) {
		LogIllegalSetOperation("SetNumerOfPeers");
	}
	Data().SetUInt16Value( 3, numberOfPeers );
}

//ResponseCode_OkWithData for FrameType_PeerAdd
uint64_t HmLegacyFrameBidcosResponse::GetAuthChannels()const
{
	if(_responseCode != ResponseCode_OkWithData) {
		LogIllegalGetOperation("GetAuthChannels");
	}
	uint64_t channels = Data().GetUInt32Value( 5 );
	channels <<= 32;
	channels |= Data().GetUInt32Value( 9 );
	return channels;

}

void HmLegacyFrameBidcosResponse::SetAuthChannels( uint64_t authChannels )
{
	if(_responseCode != ResponseCode_OkWithData) {
		LogIllegalSetOperation("SetAuthChannels");
	}
	Data().SetUInt32Value( 5, authChannels >> 32 );
	Data().SetUInt32Value( 9, authChannels & 0xffffffff );
}

bool HmLegacyFrameBidcosResponse::isResponseFrame()
{
	return true;
}
std::string HmLegacyFrameBidcosResponse::ToString()const
{
	char buffer[8];
	snprintf( buffer, sizeof(buffer), "#%d", GetSequenceCounter() );
	std::string s = buffer;
	s += " HmBidcos Response ";
	s += Data().ToString();
	return s;
}

void HmLegacyFrameBidcosResponse::LogIllegalGetOperation(const char* msg) const
{
	if(msg) {
		LOG(Logger::LOG_DEBUG, "%s%s", LOG_ILLEGAL_GET_OPERATION_PREFIX, msg);
	}
	else {
		LOG(Logger::LOG_DEBUG, "%s" , LOG_ILLEGAL_GET_OPERATION_PREFIX);
	}
}

void HmLegacyFrameBidcosResponse::LogIllegalSetOperation(const char* msg) const
{
	if(msg) {
		LOG(Logger::LOG_DEBUG, "%s%s", LOG_ILLEGAL_SET_OPERATION_PREFIX, msg);
	}
	else {
		LOG(Logger::LOG_DEBUG, "%s" , LOG_ILLEGAL_SET_OPERATION_PREFIX);
	}
}


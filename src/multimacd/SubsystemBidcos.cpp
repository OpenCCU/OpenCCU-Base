/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "SubsystemBidcos.h"
#include "Enums.h"
#include "SerialFrame/HmLegacyFrameSystem.h"
#include "SerialFrame/LowLevelMacFrame.h"
#include "BinaryData.h"
#include "BidcosContext.h"
#include "BidcosPeerContext.h"
#include "BidcosTelegram.h"
#include "Sysutils.h"
#include <string.h>
#include <stdlib.h>

#include <Logger.h>


const uint8_t SubsystemBidcos::DUTY_CYCLE_EVENT_THRESHOLDS[] = { uint8_t(40), uint8_t(65), uint8_t(80), uint8_t(90), uint8_t(95), uint8_t(98), uint8_t(99), uint8_t(100), uint8_t(0) };

SubsystemBidcos::SubsystemBidcos(void) : Subsystem( MacSubsystemType_Bidcos, "Bidcos" ), _bidcosMacController( *this )
{
	_rxBufLength = 0;
	_dutyCycle = 0;
}


SubsystemBidcos::~SubsystemBidcos(void)
{
}

bool SubsystemBidcos::CheckIfResponsibleForUpstreamFrame( const SerialFrame* frame )
{
	//LOG( Logger::LOG_DEBUG, "SubsystemBidcos::CheckIfResponsibleForUpstreamFrame(%s)", frame->ToString().c_str());
	switch( frame->GetSubsystem() )
	{
	case SerialFrame::FrameSubsystemType_LowLevelMac:
		return true;
	case SerialFrame::FrameSubsystemType_Internal:
		return true;
	default:
		return false;
	}
}

std::size_t SubsystemBidcos::ConsumeDownstreamBytes( uint8_t* buf, std::size_t len )
{
	//LOG( Logger::LOG_WARNING, "SubsystemBidcos::ConsumeDownstreamBytes(%s)", BinaryData( (const uint8_t*)buf, len ).ToString().c_str());
	if( len > sizeof( _rxBuf ) - _rxBufLength )
	{
		len = sizeof( _rxBuf ) - _rxBufLength;
	}
	memmove( _rxBuf + _rxBufLength, buf, len );
	_rxBufLength += len;

	while( _rxBufLength )
	{
		size_t numEaten = 0;
		SerialFrame* frame = HmLegacyFrame::Create( _rxBuf, _rxBufLength, &numEaten );
		if( numEaten )
		{
			_rxBufLength -= numEaten;
			memmove( _rxBuf, _rxBuf + numEaten, _rxBufLength );
		}
		if( !frame )
		{
			LOG( Logger::LOG_WARNING, "Unable to parse frame: %s", BinaryData( (const uint8_t*)buf, len ).ToString().c_str());
			break;
		}
		LOG( Logger::LOG_DEBUG, "A>: %s", frame->ToString().c_str() );
		OnDownstreamFrame( frame );
	}
	return len;
}

bool SubsystemBidcos::OnUpstreamConnect()
{
	if( _internalIdentifyFrame.GetApplicationVersion() )
	{
		HmLegacyFrameSystemIdentify* identifyFrame = new HmLegacyFrameSystemIdentify();
		identifyFrame->SetIdentification( IDENTIFICATION_BIDCOS_APP );
		SendFrameUpstream( identifyFrame );
	}
	return true;
}

bool SubsystemBidcos::ProcessUpstreamFrame( SerialFrame* frame )
{
	try
	{
		switch( frame->GetSubsystem() )
		{
		case SerialFrame::FrameSubsystemType_LowLevelMac:
			{
				LowLevelMacFrame& llMacFrame = dynamic_cast<LowLevelMacFrame&>(*frame);
				_bidcosMacController.OnLowLevelMacFrame( llMacFrame );
			}
			break;
		case SerialFrame::FrameSubsystemType_Internal:
			{
				InternalFrame& internalFrame = dynamic_cast<InternalFrame&>(*frame);
				ProcessUpstreamFrameInternal( internalFrame );
			}
			break;
		}
	}
	catch( std::bad_cast& ex )
	{
		LOG( Logger::LOG_DEBUG, "Bad cast: %s", ex.what() ); 
		if(frame != NULL) {
			LOG(Logger::LOG_DEBUG, "Upstream serial frame with following data unknown: %s", frame->ToString().c_str());
		}
	}
	delete frame;

	return true;
}

bool SubsystemBidcos::ProcessDownstreamSystemFrame( HmLegacyFrameSystem* frame )
{
	try
	{
		//TODO: FW: request coprocessor if the data not valid
		switch( frame->GetFrameType() )
		{
		case HmLegacyFrameSystem::FrameType_StartBootloader:
			{
				HmLegacyFrameSystemResponse* response = new HmLegacyFrameSystemResponse();
				response->SetSequenceCounter( frame->GetSequenceCounter() );
				response->SetResponseCode( HmLegacyFrameSystemResponse::ResponseCode_Ok );
				SendFrameUpstream( response );
				HmLegacyFrameSystemIdentify* identify = new HmLegacyFrameSystemIdentify();
				identify->SetIdentification( IDENTIFICATION_BIDCOS_APP );
				SendFrameUpstream( identify );
			}
			break;
		case HmLegacyFrameSystem::FrameType_VersionRequest:
			if( _internalIdentifyFrame.GetApplicationVersion() )
			{
				HmLegacyFrameSystemResponse* response = new HmLegacyFrameSystemResponse();
				response->SetSequenceCounter( frame->GetSequenceCounter() );
				response->SetResponseCode( HmLegacyFrameSystemResponse::ResponseCode_OkWithData );
				response->SetBootloaderVersion( _internalIdentifyFrame.GetBootloaderVersion() );
				response->SetApplicationVersion( _internalIdentifyFrame.GetApplicationVersion() );
				SendFrameUpstream( response );
			}
			break;
		case HmLegacyFrameSystem::FrameType_GetSerialNumber:
			if( _internalIdentifyFrame.GetApplicationVersion() )
			{
				HmLegacyFrameSystemResponse* response = new HmLegacyFrameSystemResponse();
				response->SetSequenceCounter( frame->GetSequenceCounter() );
				response->SetResponseCode( HmLegacyFrameSystemResponse::ResponseCode_OkWithData );
				response->SetSerialNumber( _internalIdentifyFrame.GetSerialNumber() );
				SendFrameUpstream( response );
			}
			break;
		case HmLegacyFrameSystem::FrameType_DutyCycleRequest:
			{
				HmLegacyFrameSystemResponse* response = new HmLegacyFrameSystemResponse();
				response->SetSequenceCounter( frame->GetSequenceCounter() );
				response->SetResponseCode( HmLegacyFrameSystemResponse::ResponseCode_OkWithData );
				response->SetDutyCycle( _dutyCycle );
				SendFrameUpstream( response );
			}
			break;
		case HmLegacyFrameSystem::FrameType_CsmaCaControl:
			{
				HmLegacyFrameSystemCsmaCaControl& csmaCaControlFrame = dynamic_cast<HmLegacyFrameSystemCsmaCaControl&>(*frame);
				bool enabled = csmaCaControlFrame.GetActiveFlag();
				_bidcosMacController.SetCsmaCaEnabled( enabled );
				LOG( Logger::LOG_DEBUG, "CSMA/CD %s", enabled ? "on" : "off" );
				HmLegacyFrameSystemResponse* response = new HmLegacyFrameSystemResponse();
				response->SetSequenceCounter( frame->GetSequenceCounter() );
				response->SetResponseCode( HmLegacyFrameSystemResponse::ResponseCode_Ok );
				SendFrameUpstream( response );
			}
			break;
		case HmLegacyFrameSystem::FrameType_DutyCycleControl:
			{
				HmLegacyFrameSystemDutyCycleControl& dutyCycleControlFrame = dynamic_cast<HmLegacyFrameSystemDutyCycleControl&>(*frame);
				bool enabled = dutyCycleControlFrame.GetActiveFlag();
				LOG( Logger::LOG_DEBUG, "DC check %s", enabled ? "on" : "off" );
				HmLegacyFrameSystemResponse* response = new HmLegacyFrameSystemResponse();
				response->SetSequenceCounter( frame->GetSequenceCounter() );
				response->SetResponseCode( HmLegacyFrameSystemResponse::ResponseCode_Ok );
				SendFrameUpstream( response );
			}
			break;
		case HmLegacyFrameSystem::FrameType_SetTime:
			{
				HmLegacyFrameSystemSetTime& setTimeFrame = dynamic_cast<HmLegacyFrameSystemSetTime&>(*frame);
				int8_t timezone = setTimeFrame.GetTimezone();
				LOG( Logger::LOG_DEBUG, "Time %d %s%02d:%02d", setTimeFrame.GetSeconds(), timezone < 0 ? "-" : "+", abs(timezone / 2), abs(timezone * 30) % 60 );
				HmLegacyFrameSystemResponse* response = new HmLegacyFrameSystemResponse();
				response->SetSequenceCounter( frame->GetSequenceCounter() );
				response->SetResponseCode( HmLegacyFrameSystemResponse::ResponseCode_Ok );
				SendFrameUpstream( response );
				_bidcosContext.SetTimeInfo( timezone, setTimeFrame.GetSeconds() );
				_bidcosMacController.OnTimeInfoChanged();
			}
			break;
		case HmLegacyFrameSystem::FrameType_SetTxRate10k:
			{
				_bidcosMacController.SetTxRate( BidcosMacController::TxRate_10k );
				HmLegacyFrameSystemResponse* response = new HmLegacyFrameSystemResponse();
				response->SetSequenceCounter( frame->GetSequenceCounter() );
				response->SetResponseCode( HmLegacyFrameSystemResponse::ResponseCode_Ok );
				SendFrameUpstream( response );
			}
			break;
		case HmLegacyFrameSystem::FrameType_SetTxRate100k:
			{
				_bidcosMacController.SetTxRate( BidcosMacController::TxRate_100k );
				HmLegacyFrameSystemResponse* response = new HmLegacyFrameSystemResponse();
				response->SetSequenceCounter( frame->GetSequenceCounter() );
				response->SetResponseCode( HmLegacyFrameSystemResponse::ResponseCode_Ok );
				SendFrameUpstream( response );
			}
			break;
		case HmLegacyFrameSystem::FrameType_Identify:
			{
				HmLegacyFrameSystemResponse* response = new HmLegacyFrameSystemResponse();
				response->SetSequenceCounter( frame->GetSequenceCounter() );
				response->SetResponseCode( HmLegacyFrameSystemResponse::ResponseCode_OkWithData );
				response->SetIdentification( IDENTIFICATION_BIDCOS_APP );
				SendFrameUpstream( response );
			}
			break;
		default:
			break;
		}
	}
	catch( std::bad_cast& ex )
	{
		LOG( Logger::LOG_DEBUG, "Bad cast: %s", ex.what() ); 
		if(frame != NULL) {
			LOG(Logger::LOG_DEBUG, "HmLegacyFrameSystem unknown: %s", frame->ToString().c_str());
		}
	}
	delete frame;
	//SendFrameDownstream( frame );
	return true;

}

bool SubsystemBidcos::ProcessTxTelegramFrame( HmLegacyFrameBidcosTxTelegram& frame )
{
	if( _txQueue.size() < MaxTxQueueSize )
	{
		_txQueue.push( &frame );
	}else{
		HmLegacyFrameBidcosResponse* nackResponse = new HmLegacyFrameBidcosResponse(HmLegacyFrameBidcosResponse::ResponseCode_Busy);
		nackResponse->SetSequenceCounter( frame.GetSequenceCounter() );

		SendFrameUpstream( nackResponse );

		delete &frame;
	}
	return true;
}

void SubsystemBidcos::ProcessTxQueue()
{
	if( _txQueue.size() )
	{
		LOG( Logger::LOG_DEBUG, "SubsystemBidcos::ProcessTxQueue() size=%u", _txQueue.size());
		HmLegacyFrameBidcosTxTelegram* frame = _txQueue.front();
		if( _bidcosMacController.SendBidcosTelegram( *frame ) )
		{
			//frame was processed -> remove it from the queue and delete it
			_txQueue.pop();
			delete frame;
		}
	}
}


bool SubsystemBidcos::ProcessSetAesKeyFrame( HmLegacyFrameBidcosSetAesKey& frame )
{
	BidcosAesKey::KeyId keyId;
	switch( frame.GetFrameType() )
	{
	case HmLegacyFrameBidcos::FrameType_SetAesKey:
		keyId = BidcosAesKey::KeyId_Current;
		break;
	case HmLegacyFrameBidcos::FrameType_SetPreviousAesKey:
		keyId = BidcosAesKey::KeyId_Previous;
		break;
	case HmLegacyFrameBidcos::FrameType_SetTempAesKey:
		keyId = BidcosAesKey::KeyId_Temp;
		break;
	default:
		keyId = BidcosAesKey::KeyId_Default;
		break;
	}

	HmLegacyFrameBidcosResponse* response;

	if( keyId != BidcosAesKey::KeyId_Default )
	{
		response = new HmLegacyFrameBidcosResponse( HmLegacyFrameBidcosResponse::ResponseCode_Ok );
		BidcosAesKey key( frame.GetKeyIndex(), frame.GetKeyData()  );
		_bidcosContext.SetAesKey( keyId, key );
	}
	else{
		response = new HmLegacyFrameBidcosResponse( HmLegacyFrameBidcosResponse::ResponseCode_InputError );
	}
	if(response) {
		response->SetSequenceCounter( frame.GetSequenceCounter() );
		SendFrameUpstream( response );
	}

	return keyId != BidcosAesKey::KeyId_Default;
}

bool SubsystemBidcos::ProcessSetRfAddressFrame( HmLegacyFrameBidcosSetRfAddress& frame )
{
	_bidcosContext.SetRfAddress( frame.GetRfAddress() );
	HmLegacyFrameBidcosResponse* response = new HmLegacyFrameBidcosResponse(HmLegacyFrameBidcosResponse::ResponseCode_Ok );
	response->SetSequenceCounter( frame.GetSequenceCounter() );
	SendFrameUpstream( response );
	return true;
}

bool SubsystemBidcos::ProcessGetRfAddressFrame( HmLegacyFrameBidcosGetRfAddress& frame )
{
	HmLegacyFrameBidcosResponse* response = new HmLegacyFrameBidcosResponse(HmLegacyFrameBidcosResponse::ResponseCode_OkWithData);
	response->SetSequenceCounter( frame.GetSequenceCounter() );
	response->SetRfAddress( _bidcosContext.GetRfAddress() );
	SendFrameUpstream( response );
	return true;
}

bool SubsystemBidcos::ProcessGetDefaultRfAddressFrame( HmLegacyFrameBidcosGetDefaultRfAddress& frame )
{
	if( _internalIdentifyFrame.GetApplicationVersion() )
	{
		HmLegacyFrameBidcosResponse* response = new HmLegacyFrameBidcosResponse(HmLegacyFrameBidcosResponse::ResponseCode_OkWithData);
		response->SetSequenceCounter( frame.GetSequenceCounter() );
		response->SetRfAddress( _internalIdentifyFrame.GetDefaultRfAddress() );
		SendFrameUpstream( response );
	}
	return true;
}

bool SubsystemBidcos::ProcessPeerAddFrame( HmLegacyFrameBidcosPeerAdd& frame )
{
	uint16_t numberOfPeers = 0;
	uint64_t authChannels = 0;
	_bidcosContext.PeerAdd( frame.GetRfAddress(), frame.GetKeyIndex(), frame.GetNeedsWakeupFlag(), frame.GetLazyConfigFlag(), &numberOfPeers, &authChannels );
	HmLegacyFrameBidcosResponse* response = new HmLegacyFrameBidcosResponse( HmLegacyFrameBidcosResponse::ResponseCode_OkWithData );
	response->SetSequenceCounter( frame.GetSequenceCounter() );
	response->SetDataChunkCount( 1 );
	response->SetNumberOfPeers( numberOfPeers );
	//invert authChannels according to specification
	response->SetAuthChannels( ~authChannels );
	SendFrameUpstream( response );
	return true;
}

bool SubsystemBidcos::ProcessPeerRemoveFrame( HmLegacyFrameBidcosPeerRemove& frame )
{
	uint16_t numberOfPeers = 0;
	_bidcosContext.PeerRemove( frame.GetRfAddress(), &numberOfPeers );
	HmLegacyFrameBidcosResponse* response = new HmLegacyFrameBidcosResponse(HmLegacyFrameBidcosResponse::ResponseCode_OkWithData);
	response->SetSequenceCounter( frame.GetSequenceCounter() );
	response->SetDataChunkCount( 1 );
	response->SetNumberOfPeers( numberOfPeers );
	SendFrameUpstream( response );
	return true;
}

bool SubsystemBidcos::ProcessPeerActivateAuthFrame( HmLegacyFrameBidcosPeerActivateAuth& frame )
{
	uint64_t channels = frame.GetChannels();
	_bidcosContext.PeerSetAuthChannels( frame.GetRfAddress(), channels, channels );
	HmLegacyFrameBidcosResponse* response = new HmLegacyFrameBidcosResponse( HmLegacyFrameBidcosResponse::ResponseCode_Ok );
	response->SetSequenceCounter( frame.GetSequenceCounter() );
	SendFrameUpstream( response );
	return true;
}

bool SubsystemBidcos::ProcessPeerDeactivateAuthFrame( HmLegacyFrameBidcosPeerDeactivateAuth& frame )
{
	uint64_t channels = frame.GetChannels();
	_bidcosContext.PeerSetAuthChannels( frame.GetRfAddress(), ~channels, channels );
	HmLegacyFrameBidcosResponse* response = new HmLegacyFrameBidcosResponse( HmLegacyFrameBidcosResponse::ResponseCode_Ok );
	response->SetSequenceCounter( frame.GetSequenceCounter() );
	SendFrameUpstream( response );
	return true;
}

bool SubsystemBidcos::ProcessPeerSetAesKeyIdFrame( HmLegacyFrameBidcosPeerSetAesKeyId& frame )
{
	_bidcosContext.PeerSetAesKeyIndex( frame.GetRfAddress(), frame.GetKeyIndex() );
	HmLegacyFrameBidcosResponse* response = new HmLegacyFrameBidcosResponse(HmLegacyFrameBidcosResponse::ResponseCode_Ok);
	response->SetSequenceCounter( frame.GetSequenceCounter() );
	SendFrameUpstream( response );
	return true;
}

bool SubsystemBidcos::ProcessPeerGetAesKeyIdFrame( HmLegacyFrameBidcosPeerGetAesKeyId& frame )
{
	HmLegacyFrameBidcosResponse* response = 0;


	BidcosPeerContext peerContext;
	if( _bidcosContext.GetPeerContext( frame.GetRfAddress(), &peerContext ) )
	{
		response = new HmLegacyFrameBidcosResponse(HmLegacyFrameBidcosResponse::ResponseCode_OkWithData);
		response->SetDataChunkCount( 1 );
		response->SetRfAddress( frame.GetRfAddress() );
		response->SetPeerKeyIndex( peerContext.GetAesKeyIndex() );
	}else{
		response = new HmLegacyFrameBidcosResponse( HmLegacyFrameBidcosResponse::ResponseCode_Error );
	}
	if(response) {
		response->SetSequenceCounter( frame.GetSequenceCounter() );
		SendFrameUpstream( response );
	}
	return true;
}


bool SubsystemBidcos::ProcessGetPeersFrame( HmLegacyFrameBidcosGetPeers& frame )
{
	uint16_t numberOfPeers = _bidcosContext.GetNumberOfPeers();
	uint8_t numberOfFrames = (numberOfPeers + 4) / 5;
	uint8_t frameIndex=0;
	uint16_t peerIndex = 0;
	BidcosPeerContext peerContext;
	while(frameIndex<numberOfFrames)
	{
		BinaryData dataChunk;
		uint8_t peerStartIndex = 0;
		uint8_t chunkPeerCount = 0;
		while( peerIndex < numberOfPeers && chunkPeerCount < 5 )
		{
			if( _bidcosContext.GetNextPeerContext( peerContext.GetRfAddress(), &peerContext ) )
			{
				dataChunk.SetUInt24Value( peerStartIndex, peerContext.GetRfAddress() );
				dataChunk.SetUInt32Value( peerStartIndex + 3, peerContext.GetAesChannelMask() >> 32 );
				dataChunk.SetUInt32Value( peerStartIndex + 7, peerContext.GetAesChannelMask() & 0xffffffff );
				dataChunk.SetUInt8Value( peerStartIndex + 11, peerContext.GetAesKeyIndex() );
				peerStartIndex += 12;
				peerIndex++;
				chunkPeerCount++;
			}else{
				break;
			}
		}
		if( chunkPeerCount )
		{
			HmLegacyFrameBidcosResponse* response = new HmLegacyFrameBidcosResponse(HmLegacyFrameBidcosResponse::ResponseCode_OkWithData);
			response->SetSequenceCounter( frame.GetSequenceCounter() );
			response->SetDataChunkCount( numberOfFrames );
			response->SetDataChunkIndex( frameIndex );
			response->SetResponseData( dataChunk );
			SendFrameUpstream( response );
		}else{
			return false;
		}
		frameIndex++;
	}
	return true;
}



bool SubsystemBidcos::ProcessDownstreamHmLegacyFrame( HmLegacyFrameBidcos* frame )
{
	try{
		switch( frame->GetFrameType() )
		{
		case HmLegacyFrameBidcos::FrameType_SetAesKey:
		case HmLegacyFrameBidcos::FrameType_SetPreviousAesKey:
		case HmLegacyFrameBidcos::FrameType_SetTempAesKey:
			ProcessSetAesKeyFrame( dynamic_cast<HmLegacyFrameBidcosSetAesKey&>(*frame) );
			break;
		case HmLegacyFrameBidcos::FrameType_SetRfAddress:
			ProcessSetRfAddressFrame( dynamic_cast<HmLegacyFrameBidcosSetRfAddress&>(*frame) );
			break;
		case HmLegacyFrameBidcos::FrameType_GetRfAddress:
			ProcessGetRfAddressFrame( dynamic_cast<HmLegacyFrameBidcosGetRfAddress&>(*frame) );
			break;
		case HmLegacyFrameBidcos::FrameType_GetDefaultRfAddress:
			ProcessGetDefaultRfAddressFrame( dynamic_cast<HmLegacyFrameBidcosGetDefaultRfAddress&>(*frame) );
			break;
		case HmLegacyFrameBidcos::FrameType_PeerAdd:
			ProcessPeerAddFrame( dynamic_cast<HmLegacyFrameBidcosPeerAdd&>(*frame) );
			break;
		case HmLegacyFrameBidcos::FrameType_PeerRemove:
			ProcessPeerRemoveFrame( dynamic_cast<HmLegacyFrameBidcosPeerRemove&>(*frame) );
			break;
		case HmLegacyFrameBidcos::FrameType_PeerActivateAuth:
			ProcessPeerActivateAuthFrame( dynamic_cast<HmLegacyFrameBidcosPeerActivateAuth&>(*frame) );
			break;
		case HmLegacyFrameBidcos::FrameType_PeerDeactivateAuth:
			ProcessPeerDeactivateAuthFrame( dynamic_cast<HmLegacyFrameBidcosPeerDeactivateAuth&>(*frame) );
			break;
		case HmLegacyFrameBidcos::FrameType_PeerSetAesKeyId:
			ProcessPeerSetAesKeyIdFrame( dynamic_cast<HmLegacyFrameBidcosPeerSetAesKeyId&>(*frame) );
			break;
		case HmLegacyFrameBidcos::FrameType_PeerGetAesKeyId:
			ProcessPeerSetAesKeyIdFrame( dynamic_cast<HmLegacyFrameBidcosPeerSetAesKeyId&>(*frame) );
			break;
		case HmLegacyFrameBidcos::FrameType_GetPeers:
			ProcessGetPeersFrame( dynamic_cast<HmLegacyFrameBidcosGetPeers&>(*frame) );
			break;
		case HmLegacyFrameBidcos::FrameType_TxTelegram:
			return ProcessTxTelegramFrame( dynamic_cast<HmLegacyFrameBidcosTxTelegram&>(*frame) );
		default:
			break;
		}
	}
	catch( std::bad_cast& ex )
	{
		LOG( Logger::LOG_DEBUG, "Bad cast: %s", ex.what() );
		if(frame != NULL) {
			LOG(Logger::LOG_DEBUG, "HmLegacyFrameBidcos unknown: %s", frame->ToString().c_str());
		} 
		
	}
	delete frame;
	//SendFrameDownstream( frame );
	return true;

}

bool SubsystemBidcos::ProcessDownstreamFrame( SerialFrame* frame )
{

	HmLegacyFrameSystem* systemFrame = dynamic_cast<HmLegacyFrameSystem*>(frame);
	if( systemFrame )
	{
		return ProcessDownstreamSystemFrame( systemFrame );
	}

	HmLegacyFrameBidcos* bidcosFrame = dynamic_cast<HmLegacyFrameBidcos*>(frame);
	if( bidcosFrame )
	{
		return ProcessDownstreamHmLegacyFrame( bidcosFrame );
	}

	return false;

}

void SubsystemBidcos::ProcessUpstreamFrameInternal( const InternalFrame& internalFrame )
{
	try
	{
		HmLegacyFrameSystemIdentify* hmIdentify;
		switch( internalFrame.GetFrameType() )
		{
		case InternalFrame::FrameType_Identify:
			_internalIdentifyFrame = dynamic_cast<const InternalFrameIdentify&>(internalFrame);
			hmIdentify = new HmLegacyFrameSystemIdentify();
			hmIdentify->SetIdentification( _internalIdentifyFrame.GetIdentificationBidcos() );
			//LOG(Logger::LOG_DEBUG, "SubsystemBidcos handling internal identify frame: %s", _internalIdentifyFrame.ToString().c_str());		
			//LOG(Logger::LOG_DEBUG, "SubsystemBidcos upstreaming identify frame: %s", hmIdentify->ToString().c_str());		
			SendFrameUpstream(hmIdentify);
			break;
		case InternalFrame::FrameType_DutyCycle:
			{
				uint8_t dutyCycle = dynamic_cast<const InternalFrameDutyCycle&>(internalFrame).GetDutyCylce();
				if( CheckDutyCycleEventThreshold( _dutyCycle, dutyCycle ) )
				{
					HmLegacyFrameSystemDutyCycleEvent* dutyCycleEventFrame = new HmLegacyFrameSystemDutyCycleEvent();
					dutyCycleEventFrame->SetDutyCycle( dutyCycle );
					SendFrameUpstream( dutyCycleEventFrame );
				}
				_dutyCycle = dutyCycle;
			}
			break;
		default:
			break;
		}
	}
	catch( std::bad_cast& ex )
	{
		LOG( Logger::LOG_DEBUG, "Bad cast: %s", ex.what() ); 
		LOG(Logger::LOG_DEBUG, "InternalFrame unknown: %s", internalFrame.ToString().c_str());
	}

}

bool SubsystemBidcos::CheckDutyCycleEventThreshold( uint8_t dutyCycleOld, uint8_t dutyCycleNew )
{
	bool result = false;
	for( int i=0; DUTY_CYCLE_EVENT_THRESHOLDS[i]; i++ )
	{
		uint8_t threshold = DUTY_CYCLE_EVENT_THRESHOLDS[i] * 2;
		if( dutyCycleOld < threshold && dutyCycleNew >= threshold )
		{
			result = true;
			break;
		}
		if( dutyCycleOld >= threshold && dutyCycleNew < threshold )
		{
			result = true;
			break;
		}
	}
	LOG( Logger::LOG_DEBUG, "SubsystemBidcos::CheckDutyCycleEventThreshold( %.1f, %.1f ) = %d", double(dutyCycleOld)/2, double(dutyCycleNew)/2, result );
	return result;
}

uint32_t SubsystemBidcos::CanSleep()
{
	ProcessTxQueue();
	while( true )
	{
		uint32_t sleepTime = _bidcosMacController.GetSleepTime();
		if( sleepTime > 0 )
		{
			return sleepTime;
		}
		_bidcosMacController.OnCyclicCall();
	};
}

void SubsystemBidcos::OnWorkerThreadStarted()
{
	Sysutils::ThreadSetSchedulingPriority( 5 );
}

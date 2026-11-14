/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "SubsystemHmIp.h"
#include "SerialFrame/SerialFrame.h"
#include "Sysutils.h"
#include <string.h>
#include <Logger.h>


SubsystemHmIp::SubsystemHmIp(void) : Subsystem( MacSubsystemType_HmIp, "HmIP" )
{
	_rxBufLength = 0;
	_bootloaderMode = true;
	_dutyCycle = 0;
}

SubsystemHmIp::~SubsystemHmIp(void)
{
}

bool SubsystemHmIp::CheckIfResponsibleForUpstreamFrame( const SerialFrame* frame )
{
	switch( frame->GetSubsystem() )
	{
	case SerialFrame::FrameSubsystemType_Bootloader:
	case SerialFrame::FrameSubsystemType_Common:
	case SerialFrame::FrameSubsystemType_HmIpStack:
	case SerialFrame::FrameSubsystemType_TrxAdapter:
	case SerialFrame::FrameSubsystemType_Internal:
	case SerialFrame::FrameSubsystemType_Router:
	case SerialFrame::FrameSubsystemType_Backbone:
		return true;
	default:
		return false;
	}
}

bool SubsystemHmIp::OnUpstreamConnect()
{
	_bootloaderMode = false;
#if 0
	if( _internalIdentifyFrame.GetApplicationVersion() )
	{
		CommonCommandFrameIdentify* identify = new CommonCommandFrameIdentify();
		identify->SetIdentification( _bootloaderMode ? IDENTIFY_BL: IDENTIFY_APP );
		SendFrameUpstream( identify );
	}
#endif
	return true;
}


bool SubsystemHmIp::ProcessUpstreamFrame( SerialFrame* frame )
{
	try
	{
		switch( frame->GetSubsystem() )
		{
		case SerialFrame::FrameSubsystemType_Internal:
			{
				InternalFrame& internalFrame = dynamic_cast<InternalFrame&>(*frame);
				ProcessUpstreamFrameInternal( internalFrame );
			}
			break;
		default:
			SendFrameUpstream( frame );
			return true;
		}
	}
	catch( std::bad_cast& ex )
	{
		LOG( Logger::LOG_DEBUG, "Bad cast: %s", ex.what() );
		if(frame != NULL) {
			LOG(Logger::LOG_DEBUG, "HmIP upstream frame unknown: %s", frame->ToString().c_str());
		}
	}
	delete frame;
	return true;
}

void SubsystemHmIp::ProcessUpstreamFrameInternal( const InternalFrame& internalFrame )
{
	try
	{
		CommonCommandFrameIdentify* ccframeIdentify;
		switch( internalFrame.GetFrameType() )
		{
		case InternalFrame::FrameType_Identify:
				_internalIdentifyFrame = dynamic_cast<const InternalFrameIdentify&>( internalFrame );
				ccframeIdentify = new CommonCommandFrameIdentify();
				ccframeIdentify->SetIdentification(_internalIdentifyFrame.GetIdentificationHmIP());
				//LOG(Logger::LOG_DEBUG, "SubsystemHmIP handling internal identify: %s", _internalIdentifyFrame.ToString().c_str());
				//LOG(Logger::LOG_DEBUG, "SubsystemHmIP upstreaming identify: %s", ccframeIdentify->ToString().c_str());
				SendFrameUpstream( ccframeIdentify );
			break;
		case InternalFrame::FrameType_DutyCycle:
			_dutyCycle = dynamic_cast<const InternalFrameDutyCycle&>(internalFrame).GetDutyCylce();
			break;
		default:
			break;
		}
	}
	catch( std::bad_cast& ex )
	{
		LOG( Logger::LOG_DEBUG, "Bad cast: %s", ex.what() );
		LOG(Logger::LOG_DEBUG, "InternalFrame (upstream) unknown: %s", internalFrame.ToString().c_str());
	}
}

bool SubsystemHmIp::ProcessDownstreamFrame( SerialFrame* frame )
{
	try
	{
		switch( frame->GetSubsystem() )
		{
		case SerialFrame::FrameSubsystemType_Bootloader:
			return false;
		case SerialFrame::FrameSubsystemType_Common:
			{
				CommonCommandFrame& commonCommandFrame = dynamic_cast<CommonCommandFrame&>(*frame);
				ProcessDownstreamFrameCommon( commonCommandFrame );
			}
			break;
		case SerialFrame::FrameSubsystemType_HmIpStack:
			SendFrameDownstream( frame );
			return true;
		case SerialFrame::FrameSubsystemType_TrxAdapter:
			{
				TrxAdapterFrame* trxAdapterFrame = dynamic_cast<TrxAdapterFrame*>(frame);
				if( (!trxAdapterFrame) || !ProcessDownstreamFrameTrxAdapter( *trxAdapterFrame ) )
				{
					SendFrameDownstream( frame );
					return true;
				}
			}
			break;
		case SerialFrame::FrameSubsystemType_Router:
			SendFrameDownstream( frame );
			return true;
		case SerialFrame::FrameSubsystemType_Backbone:
			SendFrameDownstream(frame);
			return true;
		default:
			return false;
		}
		delete frame;
		return true;
	}
	catch( std::bad_cast& ex )
	{
		LOG( Logger::LOG_DEBUG, "Bad cast: %s", ex.what() ); 
		if(frame != NULL) {
			LOG(Logger::LOG_DEBUG, "Frame (hmip downstream) unknown: %s", frame->ToString().c_str());
		}
	}
	return false;
}

bool SubsystemHmIp::ProcessDownstreamFrameCommon( const CommonCommandFrame& frame )
{
	if( !_internalIdentifyFrame.GetApplicationVersion() )
	{
		return false;
	}

	switch (frame.GetCommand())
	{
	case CommonCommandFrame::FrameType_StartApplication:
		{
			_bootloaderMode = false;
			CommonCommandFrameResponse* response = new CommonCommandFrameResponse();
			response->SetSequenceCounter( frame.GetSequenceCounter() );
			response->SetResponseCode( CommonCommandFrameResponse::ResponseCode_Ack );	
			SendFrameUpstream( response );
			CommonCommandFrameIdentify* identify = new CommonCommandFrameIdentify();
			identify->SetIdentification( IDENTIFICATION_HMIP_APP );
			SendFrameUpstream( identify );
		}
		break;
	case CommonCommandFrame::FrameType_StartBootloader:
		{
			_bootloaderMode = true;
			CommonCommandFrameResponse* response = new CommonCommandFrameResponse();
			response->SetSequenceCounter( frame.GetSequenceCounter() );
			response->SetResponseCode( CommonCommandFrameResponse::ResponseCode_Ack );	
			SendFrameUpstream( response );
			CommonCommandFrameIdentify* identify = new CommonCommandFrameIdentify();
			identify->SetIdentification( IDENTIFICATION_HMIP_BOOTLOADER );
			SendFrameUpstream( identify );
		}
		break;
	case CommonCommandFrame::FrameType_IdentifyRequest:
		{
			CommonCommandFrameResponse* response = new CommonCommandFrameResponse();
			response->SetSequenceCounter( frame.GetSequenceCounter() );
			response->SetResponseCode( CommonCommandFrameResponse::ResponseCode_Ack );	
			response->SetPayload( BinaryData( _bootloaderMode ? IDENTIFICATION_HMIP_BOOTLOADER.c_str() : IDENTIFICATION_HMIP_APP.c_str() ) );
			SendFrameUpstream( response );
		}
		break;
	case CommonCommandFrame::FrameType_GetSGTIN:
		{
			CommonCommandFrameResponse* response = new CommonCommandFrameResponse();
			response->SetSequenceCounter( frame.GetSequenceCounter() );
			response->SetResponseCode( CommonCommandFrameResponse::ResponseCode_Ack );
			response->SetPayload( _internalIdentifyFrame.GetSgtin() );
			SendFrameUpstream( response );
		}
		break;
	default:
		break;
	}
	return true;
}

bool SubsystemHmIp::ProcessDownstreamFrameTrxAdapter( const TrxAdapterFrame& frame )
{
	if( !_internalIdentifyFrame.GetApplicationVersion() )
	{
		return false;
	}

	switch (frame.GetCommand())
	{
	case TrxAdapterFrame::FrameType_GetVersion:
		{
			TrxAdapterFrameResponse* response = new TrxAdapterFrameResponse();
			response->SetSequenceCounter( frame.GetSequenceCounter() );
			response->SetResponseCode( TrxAdapterFrameResponse::ResponseCode_Ack );	
			response->SetApplicationVersion( _internalIdentifyFrame.GetApplicationVersion() );
			response->SetBootloaderVersion( _internalIdentifyFrame.GetBootloaderVersion() );
			response->SetHmosVersion( _internalIdentifyFrame.GetHmosVersion() );
			SendFrameUpstream( response );
		}
		break;
	case TrxAdapterFrame::FrameType_GetDutyCycle:
		{
			TrxAdapterFrameResponse* response = new TrxAdapterFrameResponse();
			response->SetSequenceCounter( frame.GetSequenceCounter() );
			response->SetResponseCode( TrxAdapterFrameResponse::ResponseCode_Ack );	
			response->SetDutyCycle( _dutyCycle );
			SendFrameUpstream( response );
		}
		break;
	default:
		return false;
	}
	return true;
}


std::size_t SubsystemHmIp::ConsumeDownstreamBytes( uint8_t* buf, std::size_t len )
{
	if( len > sizeof( _rxBuf ) - _rxBufLength )
	{
		len = sizeof( _rxBuf ) - _rxBufLength;
	}
	memmove( _rxBuf + _rxBufLength, buf, len );
	_rxBufLength += len;

	while( _rxBufLength )
	{
		size_t numEaten = 0;
		SerialFrame* frame = SerialFrame::Create( _rxBuf, _rxBufLength, &numEaten );
		if( !frame )
		{
			LOG( Logger::LOG_DEBUG, "Error creating frame from %s", BinaryData( _rxBuf, _rxBufLength ).ToString().c_str());
		}
		if( numEaten )
		{
			_rxBufLength -= numEaten;
			memmove( _rxBuf, _rxBuf + numEaten, _rxBufLength );
		}
		if( !frame )
		{
			break;
		}
		LOG( Logger::LOG_DEBUG, "A>: %s", frame->ToString().c_str() );
		OnDownstreamFrame( frame );
	}
	return len;
}

void SubsystemHmIp::OnWorkerThreadStarted()
{
	Sysutils::ThreadSetSchedulingPriority( 10 );
}

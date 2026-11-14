/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "MacController.h"
#include "MultimacManager.h"
#include "SerialFrame/CommonCommandFrame.h"
#include "SerialFrame/LowLevelMacFrame.h"
#include "SerialFrame/TrxAdapterFrame.h"
#include "SerialFrame/HmLegacyFrameSystem.h"
#include "SerialFrame/HmIpStackFrame.h"
#include "SerialFrame/RouterFrame.h"
#include "Enums.h"
#include <Logger.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <typeinfo>
#include "Sysutils.h"
#ifndef WIN32
#include <sys/syscall.h>
#else
#include <io.h>
#endif

#ifdef DBG_MAPOPENFRAMES
  void printMapOpenFrames();
#endif

#define INIT_ERROR_RETRIES 2

MacController::MacController(void)
{
	_fd = -1;
	_exit = false;
	_frameSink = NULL;
	_nextDownstreamSequenceCounter = 0;
	_coproState = CoproState_Unknown;
	_nextTrxSubsystemTimer = 0;
	_initSequenceTimer = 0;
	_nextDutyCyclePoll = 0;
	_trxSubsystemState = TrxSubsystemState_Idle;
	_lastRequestFrameId = -1;
	_initSendErrCnt = 0;
	_initializationDone = false;
}


MacController::~MacController(void)
{
	while( _mapOpenFrames.size() )
	{
		delete _mapOpenFrames.begin()->second;
		_mapOpenFrames.erase( _mapOpenFrames.begin() );
	}
}

bool MacController::Start( const std::string& device )
{
	if( _thread.joinable() )
	{
		LOG( Logger::LOG_WARNING, "MacController thread already running" ); 
		return false;
	}
	_fd = OpenPort( device.c_str() );
	if( _fd < 0 )
	{
		return false;
	}
	_exit = false;
	_thread = tthread::thread( &sThreadFunction, this );
	return true;
}

bool MacController::Stop()
{
	if( _thread.joinable() )
	{
		_exit = true;
		_incomingQueue.Signal();
		_thread.join();
		return true;
	}
	return false;
}

void MacController::SetUpstreamFrameSink( IUpstreamSerialFrameSink& frameSink )
{
	_frameSink = &frameSink;
}

void MacController::OnDownstreamFrame( SerialFrame* frame )
{
	LOG( Logger::LOG_DEBUG, "MacController::OnDownstreamFrame(%s)", frame->ToString().c_str() ); 
	_incomingQueue.Add( new MacControllerQueueItem( frame, MacControllerQueueItem::Direction_Downstream ) );
}

void MacController::OnUpstreamFrame( SerialFrame* frame )
{
	_incomingQueue.Add( new MacControllerQueueItem( frame, MacControllerQueueItem::Direction_Upstream ) );
}


int MacController::OpenPort( const char* device )
{
#ifndef WIN32
	int fd = open (device, O_WRONLY);
	if (fd <= 0)
	{
		LOG( Logger::LOG_WARNING, "MacController could not open port %s: %s", device, strerror( errno ) ); 
		return -1;
	}
	return fd;
#else
	return 0;
#endif
}

MacController::SendResult MacController::SendInitSequenceFrameToCoprocessor( SerialFrame* frame )
{
	_initSequenceTimer = Sysutils::GetMonotonicTime() + InitTimeout;
	return SendFrameToCoprocessor( frame );
}

MacController::SendResult MacController::SendFrameToCoprocessor( SerialFrame* frame )
{
	frame->SetSequenceCounter( GetNextDownstreamSequenceCounter() );

	LOG( Logger::LOG_DEBUG, "C<: %s", frame->ToString().c_str() ); 

	size_t bufLength = frame->GetRawData( _txBuffer, sizeof( _txBuffer ) );
#ifndef WIN32
	if( bufLength )
	{
		BinaryData binData( (uint8_t*)_txBuffer, bufLength );
		LOG( Logger::LOG_DEBUG, "C< @%u: bin:%s", Sysutils::GetMonotonicTime(), binData.ToString().c_str());

		int count = write( _fd, _txBuffer, bufLength );
		if( count < 0 )
		{
			LOG( Logger::LOG_WARNING, "MacController write error: %s", strerror( errno ) ); 
			return SendResult_Failed;
		}
		if( count < (int)bufLength )
		{
			LOG( Logger::LOG_INFO, "MacController send interrupted after %d bytes", strerror( errno ) ); 
			return SendResult_TryAgain;
		}
		//LOG( Logger::LOG_WARNING, "MacController frame sent: %s", frame->ToString().c_str() ); 
	}
#endif
	//LOG(Logger::LOG_DEBUG, "MacController::SendFrameToCoprocessor: Search and may replace open frame");

#ifdef DBG_MAPOPENFRAMES
	printMapOpenFrames();
#endif

	MapOpenFrames::iterator it = _mapOpenFrames.find( ((uint16_t)frame->GetSubsystem()) << 8 | frame->GetSequenceCounter());
	if( it != _mapOpenFrames.end() )
	{
		delete it->second;
	}

#ifdef DBG_MAPOPENFRAMES
	uint16_t foo = ((uint16_t)frame->GetSubsystem());
	uint16_t bar = ((uint16_t)frame->GetSubsystem() << 8);
	uint16_t buz = ((uint16_t)frame->GetSubsystem()) << 8 | frame->GetSequenceCounter();
	LOG(Logger::LOG_DEBUG, "Placing request int mapOpenFrames with: subsys=%u, shifted-subsys=%u, shifted-subsys-plus-seqcnt=%u", (unsigned int)foo, (unsigned int)bar, (unsigned int)buz);
#endif

	_mapOpenFrames[((uint16_t)frame->GetSubsystem()) << 8 | frame->GetSequenceCounter()] = frame;

#ifdef DBG_MAPOPENFRAMES
	printMapOpenFrames();
#endif
	return SendResult_Success;
}

void  MacController::HandleInitFrameFromCoprocessor( SerialFrame* frame )
{
	BinaryData payload;
	std::string identify = "";
	bool forwardIdentifyUpstream = false;

	try{
		if( frame->CheckType( SerialFrame::FrameSubsystemType_Common, CommonCommandFrame::FrameType_Identify ) )
		{
			CommonCommandFrameIdentify& identifyFrame = dynamic_cast<CommonCommandFrameIdentify&>(*frame);
			payload.SetStringValue( 0, identifyFrame.GetIdentification() );
			forwardIdentifyUpstream = true;
		}else if( frame->CheckType( SerialFrame::FrameSubsystemType_Common, CommonCommandFrame::FrameType_Response ) )
		{
			CommonCommandFrameResponse& responseFrame = dynamic_cast<CommonCommandFrameResponse&>(*frame);
			if( responseFrame.IsAck() )
			{
				payload = responseFrame.GetPayload();
			}
		}else if( frame->CheckType( SerialFrame::FrameSubsystemType_LowLevelMac, LowLevelMacFrame::FrameType_Response ) )
		{
			LowLevelMacFrameResponse& responseFrame = dynamic_cast<LowLevelMacFrameResponse&>(*frame);
			if( responseFrame.IsAck() )
			{
				payload = responseFrame.GetPayload();
			}
		}else if( frame->CheckType( SerialFrame::FrameSubsystemType_TrxAdapter, TrxAdapterFrame::FrameType_Response ) )
		{
			TrxAdapterFrameResponse& responseFrame = dynamic_cast<TrxAdapterFrameResponse&>(*frame);
			if( responseFrame.IsAck() )
			{
				payload = responseFrame.GetPayload();
			}
		}else if( frame->CheckType( HmLegacyFrame::SubsystemType_System, HmLegacyFrameSystem::FrameType_Response ) )
		{
			const BinaryData& data = frame->Data();
			uint8_t status = data.GetUInt8Value(0);
			if( status == HmLegacyFrameSystemResponse::ResponseCode_OkWithData )
			{
				payload = data.GetRange( 1, data.size() - 1);
			}else if( status == HmLegacyFrameSystemResponse::ResponseCode_InputError && _coproState == CoproState_Identify )
			{
				LOG( Logger::LOG_INFO, "Invalid input error on identify request. Checking for legacy bootloader." );
				SendInitSequenceFrameToCoprocessor( new HmLegacyFrameSystemIdentify() );
				_coproState = CoproState_IdentifyLegacyBootloader;
				return;
			}
		}else if( frame->CheckType( HmLegacyFrame::SubsystemType_System, HmLegacyFrameSystem::FrameType_Identify ) )
		{
			payload = frame->Data();
			forwardIdentifyUpstream = true;
		}
		//LOG( Logger::LOG_DEBUG, "Payload=%s", payload.ToString().c_str() );
		//if(identify)
		if( payload.GetStringValue( 0 ) == IDENTIFICATION_HMIP_BOOTLOADER /*"HMIP_TRX_Bl"*/ )
		{
			_coproState = CoproState_Identify;
		}
		if( payload.GetStringValue( 0 ) == IDENTIFICATION_BIDCOS_BOOTLOADER /*"Co_CPU_BL"*/ )
		{
			_coproState = CoproState_Identify;
		}


		switch( _coproState )
		{
		case CoproState_Unknown:
		case CoproState_Bootloader:
		case CoproState_Identify:
		case CoproState_IdentifyLegacyBootloader:
		case CoproState_StartApplication:
			if( payload.GetStringValue( 0 ) == IDENTIFICATION_HMIP_BOOTLOADER /*"HMIP_TRX_Bl"*/ )
			{
				LOG( Logger::LOG_INFO, "Copro Bootloader detected. Starting application." );
				SendInitSequenceFrameToCoprocessor(new CommonCommandFrameStartApplication());
				_internalIdentifyFrame.SetIdentification( IDENTIFICATION_INTERNAL_APP );
				_coproState = CoproState_StartApplication;
			}else if( payload.GetStringValue( 0 ) == IDENTIFICATION_DUAL_APP /*"DualCoPro_App"*/ )
			{
				LOG( Logger::LOG_INFO, "Copro application running." );
				TrxAdapterFrameGetVersion* getVersionFrame = new TrxAdapterFrameGetVersion();
				_lastRequestFrameId = getVersionFrame->GetId();
				SendInitSequenceFrameToCoprocessor(getVersionFrame);
				_internalIdentifyFrame.SetIdentification( IDENTIFICATION_INTERNAL_APP );
				_coproState = CoproState_GetVersion;
			}else if( payload.GetStringValue( 0 ) == IDENTIFICATION_BIDCOS_BOOTLOADER /*"Co_CPU_BL"*/ )
			{
				LOG( Logger::LOG_INFO, "Legacy Bootloader detected. Starting application." );
				SendInitSequenceFrameToCoprocessor(new HmLegacyFrameSystemStartBootloader());
				_internalIdentifyFrame.SetIdentification( IDENTIFICATION_INTERNAL_BOOTLOADER );
				_coproState = CoproState_StartApplication;
			}else if(payload.GetStringValue( 0 ) == IDENTIFICATION_HMIP_APP /*"HMIP_TRX_App"*/)
			{
				LOG( Logger::LOG_ERROR, "HomeMatic IP Coprocessor detected!!! Please install DualCoPro Firmware " );
				_coproState = CoproState_WaitExit;
				MultimacManager::Instance().Exit();
			}
			else if(payload.GetStringValue( 0 ) == IDENTIFICATION_BIDCOS_APP /*"Co_CPU_App"*/)
			{
				LOG( Logger::LOG_ERROR, "HomeMatic Coprocessor detected!!! Please install DualCoPro Firmware " );
				_coproState = CoproState_WaitExit;
				MultimacManager::Instance().Exit();
			}

			break;
		case CoproState_GetVersion:
			if( _lastRequestFrameId == frame->GetId() )
			{
				if(!payload.empty()) {
					_initSendErrCnt = 0;
					LOG( Logger::LOG_INFO, "Vapp=%06X Vbl=%06X Vhmos=%06X", payload.GetUInt24Value(0), payload.GetUInt24Value(3), payload.GetUInt24Value(6) );
					_internalIdentifyFrame.SetApplicationVersion( payload.GetUInt24Value(0) );
					_internalIdentifyFrame.SetBootloaderVersion( payload.GetUInt24Value(3) );
					_internalIdentifyFrame.SetHmosVersion( payload.GetUInt24Value(6) );
				}
				else if(_initSendErrCnt < INIT_ERROR_RETRIES) {
					LOG(Logger::LOG_DEBUG,"GetVersion failed. Retrying.");
					_initSendErrCnt++;
					TrxAdapterFrameGetVersion* getVersionFrame = new TrxAdapterFrameGetVersion();
					_lastRequestFrameId = getVersionFrame->GetId();
					SendInitSequenceFrameToCoprocessor(getVersionFrame);
					break;
				}
				else {
					_initSendErrCnt = 0;
					LOG(Logger::LOG_WARNING, "GetVersion finally failed.");
				}
				CommonCommandFrameGetSgtin* getSgtinFrame = new CommonCommandFrameGetSgtin();
				_lastRequestFrameId = getSgtinFrame->GetId();
				SendInitSequenceFrameToCoprocessor(getSgtinFrame);
				_coproState = CoproState_GetSgtin;
			}
			break;
		case CoproState_GetSgtin:
			if( _lastRequestFrameId == frame->GetId() )
			{
				if(!payload.empty()) {
					_initSendErrCnt = 0;
					LOG( Logger::LOG_INFO, "SGTIN=%s", payload.ToString().c_str() );
					_internalIdentifyFrame.SetSgtin( payload );
				}
				else if(_initSendErrCnt < INIT_ERROR_RETRIES) {
					LOG(Logger::LOG_DEBUG,"GetSgtin failed. Retrying.");
					_initSendErrCnt++;
					CommonCommandFrameGetSgtin* getSgtinFrame = new CommonCommandFrameGetSgtin();
					_lastRequestFrameId = getSgtinFrame->GetId();
					SendInitSequenceFrameToCoprocessor(getSgtinFrame);
					break;
				}
				else {
					_initSendErrCnt = 0;
					LOG(Logger::LOG_WARNING, "GetSgtin finally failed.");
				}
				LowLevelMacFrameGetDefaultRfAddress* getDefaultRfAddressFrame = new LowLevelMacFrameGetDefaultRfAddress();
				_lastRequestFrameId = getDefaultRfAddressFrame->GetId();
				SendInitSequenceFrameToCoprocessor(getDefaultRfAddressFrame);
				_coproState = CoproState_GetRfAddress;
			}
			break;
		case CoproState_GetRfAddress:
			if( _lastRequestFrameId == frame->GetId() )
			{
				if(!payload.empty()) {
					_initSendErrCnt = 0;
					uint32_t rfAddress = payload.GetUInt24Value(0);
					_internalIdentifyFrame.SetDefaultRfAddress( rfAddress );
					LOG( Logger::LOG_INFO, "RF address=%u", rfAddress );
				}
				else if(_initSendErrCnt < INIT_ERROR_RETRIES) {
					LOG(Logger::LOG_DEBUG,"GetRfAddress failed. Retrying.");
					_initSendErrCnt++;
					LowLevelMacFrameGetDefaultRfAddress* getDefaultRfAddressFrame = new LowLevelMacFrameGetDefaultRfAddress();
					_lastRequestFrameId = getDefaultRfAddressFrame->GetId();
					SendInitSequenceFrameToCoprocessor(getDefaultRfAddressFrame);
					break;
				}
				else {
					_initSendErrCnt = 0;
					LOG(Logger::LOG_WARNING, "GetRfAdress finally failed.");
				}
				LowLevelMacFrameGetSerialNumber* getSerialNumberFrame = new LowLevelMacFrameGetSerialNumber();
				_lastRequestFrameId = getSerialNumberFrame->GetId();
				SendInitSequenceFrameToCoprocessor(getSerialNumberFrame);
				_coproState = CoproState_GetSerialNumber;
			}
			break;
		case CoproState_GetSerialNumber:
			if( _lastRequestFrameId == frame->GetId() )
			{
				if(!payload.empty()) {
					_initSendErrCnt = 0;
					std::string serialNumber = payload.GetStringValue( 0 );
					_internalIdentifyFrame.SetSerialNumber( payload.GetStringValue(0) );
					LOG( Logger::LOG_INFO, "Serial Number=%s", serialNumber.c_str() );
				}
				else if(_initSendErrCnt < INIT_ERROR_RETRIES) {
					LOG(Logger::LOG_DEBUG,"GetSerialNumber failed. Retrying.");
					_initSendErrCnt++;
					LowLevelMacFrameGetSerialNumber* getSerialNumberFrame = new LowLevelMacFrameGetSerialNumber();
					_lastRequestFrameId = getSerialNumberFrame->GetId();
					SendInitSequenceFrameToCoprocessor(getSerialNumberFrame);
					break;
				}
				else {
					_initSendErrCnt = 0;
					LOG(Logger::LOG_WARNING, "GetRfAdress finally failed.");
				}
				LowLevelMacFrameGetTimestamp* getTimestampFrame = new LowLevelMacFrameGetTimestamp();
				_lastRequestFrameId = getTimestampFrame->GetId();
				SendInitSequenceFrameToCoprocessor(getTimestampFrame);
				_coproState = CoproState_GetTimer;
			}
			break;
		case CoproState_GetTimer:
			if( _lastRequestFrameId == frame->GetId() )
			{
				uint16_t timer = payload.GetUInt16Value( 0 );//FIXME Check if GetTimer can be removed or must be set internally somewhere!
				LOG( Logger::LOG_INFO, "Timer=%5u", timer );
				_coproState = CoproState_Application;
				_trxSubsystemState = TrxSubsystemState_Idle;
				_nextTrxSubsystemTimer = Sysutils::GetMonotonicTime() + DutyCyclePollInterval;
				_nextDutyCyclePoll = _nextTrxSubsystemTimer;
				_trxSubsystemState = TrxSubsystemState_Idle;

				forwardIdentifyUpstream = true;
				_initializationDone = true;
			}
			break;
		case CoproState_WaitExit:
			break;
		default:
			break;
		}
		
		
		if(forwardIdentifyUpstream && _frameSink) {
			InternalFrameIdentify* identifyFrame = new InternalFrameIdentify();
			*identifyFrame = _internalIdentifyFrame;
			_frameSink->OnUpstreamFrame( identifyFrame );
			//LOG(Logger::LOG_DEBUG, "MacController. Sending identify frame upstream: %s", identifyFrame->ToString().c_str());			
		}
	}
	catch( std::bad_cast& ex )
	{
		LOG( Logger::LOG_DEBUG, "Bad cast: %s", ex.what() ); 
		if(frame != NULL) {
			LOG(Logger::LOG_DEBUG, "Serial frame with following data unknown: %s", frame->ToString().c_str());
		}
	}
}

void MacController::OnInitSequenceTimeout()
{
		switch( _coproState )
		{
		case CoproState_Identify:
			SendInitSequenceFrameToCoprocessor( new HmLegacyFrameSystemIdentify() );
			_coproState = CoproState_IdentifyLegacyBootloader;
			break;
		case CoproState_IdentifyLegacyBootloader:
			LOG( Logger::LOG_ERROR, "No Coprocessor detected!!! " );
			_coproState = CoproState_WaitExit;
			MultimacManager::Instance().Exit();
			break;
		default:
			//LOG( Logger::LOG_ERROR, "Copro identify timeout. Retrying" );
			//SendInitSequenceFrameToCoprocessor( new CommonCommandFrameIdentifyRequest() );
			//_coproState = CoproState_Identify;
			break;

		}
}

#ifdef DBG_MAPOPENFRAMES
void MacController::printMapOpenFrames() {
	MapOpenFrames::iterator it = _mapOpenFrames.begin();
	LOG(Logger::LOG_DEBUG, "_mapOpenFrames:");
	for( ; it != _mapOpenFrames.end() ; ++it) {
		if(it->second != NULL) {
			LOG(Logger::LOG_DEBUG, "%u : %s", ((int)it->first), it->second->ToString().c_str());
		}
	}
}
#endif

void  MacController::HandleFrameFromCoprocessor( SerialFrame* frame )
{
#ifdef DBG_MAPOPENFRAMES
	LOG(Logger::LOG_DEBUG, "MacController::HandleFrameFromCoprocessor");
	printMapOpenFrames();
#endif
	if(frame->isResponseFrame())
	{   //check open frame map just for response frames
		MapOpenFrames::iterator it = _mapOpenFrames.find( ((uint16_t)frame->GetSubsystem()) << 8 | frame->GetSequenceCounter());
		if( it != _mapOpenFrames.end() )
		{
			//LOG(Logger::LOG_DEBUG, "MacController::HandleFrameFromCoprocessor: Found open frame");
			frame->SetId( it->second->GetId() );
			frame->SetResponsibleSubsystem( it->second->GetResponsibleSubsystem() );
			delete it->second;
			_mapOpenFrames.erase( it );
		}
	}
	//LOG( Logger::LOG_DEBUG, "HandleFrameFromCoprocessor(%s)", frame->ToString().c_str() );
	bool delegated = false;
	if( (_coproState != CoproState_Application) || 
		frame->CheckType( SerialFrame::FrameSubsystemType_Common, CommonCommandFrame::FrameType_Identify ) ||
		frame->CheckType( HmLegacyFrameSystem::SubsystemType_System, HmLegacyFrameSystem::FrameType_Identify )
		)
	{
		HandleInitFrameFromCoprocessor( frame );
	}else if( frame->GetSubsystem() == SerialFrame::FrameSubsystemType_TrxAdapter )
	{
		delegated = HandleTrxSubsystemFrameFromCoprocessor( frame );
	}else{
		if( _frameSink )
		{
			_frameSink->OnUpstreamFrame( frame );
			delegated = true;
		}
	}
	if( !delegated )
	{
		delete frame;
	}
}

bool MacController::HandleTrxSubsystemFrameFromCoprocessor( SerialFrame* frame )
{
	bool handled = false;
	try
	{
		TrxAdapterFrame& trxAdapterFrame = dynamic_cast<TrxAdapterFrame&>(*frame);
		if( trxAdapterFrame.GetFrameType() == TrxAdapterFrame::FrameType_Response )
		{
			switch( _trxSubsystemState )
			{
			case TrxSubsystemState_WaitApplicationResponse:
				_trxSubsystemState = TrxSubsystemState_Idle;
				_nextTrxSubsystemTimer = _nextDutyCyclePoll;
				break;
			case TrxSubsystemState_WaitDutyCycleResponse:
				if( _frameSink )
				{
					uint8_t dutyCycle = dynamic_cast<TrxAdapterFrameResponse&>(trxAdapterFrame).GetDutyCycle();
					InternalFrameDutyCycle* dutyCycleFrame = new InternalFrameDutyCycle();
					dutyCycleFrame->SetDutyCycle( dutyCycle );
					_frameSink->OnUpstreamFrame( dutyCycleFrame );
				}
				_trxSubsystemState = TrxSubsystemState_Idle;
				_nextTrxSubsystemTimer = _nextDutyCyclePoll;
				handled = true;
				break;
			case TrxSubsystemState_Idle:
				break;
			}
		}
	}
	catch( std::bad_cast& ex )
	{
		LOG(Logger::LOG_DEBUG, "Bad cast: %s", ex.what() ); 
		if(frame != NULL) {
			LOG(Logger::LOG_DEBUG, "TrxSubsystemFrame unknown: %s", frame->ToString().c_str());
		}
	}
	if( (!handled) && _frameSink )
	{
		_frameSink->OnUpstreamFrame( frame );
		return true;
	}else{
		return false;
	}
}


void MacController::ThreadFunction()
{
#ifndef WIN32
	LOG( Logger::LOG_DEBUG, "MacController::ThreadFunction() started. Id=%d", syscall(SYS_gettid) ); 
#endif

	Sysutils::ThreadSetSchedulingPriority( 2 );

	_coproState = CoproState_Unknown;

	MacControllerQueueItem* queueItem = NULL;

	SerialFrame* pendingTrxSubsystemFrame = NULL;

	while( !_exit )
	{
		if( pendingTrxSubsystemFrame && _trxSubsystemState == TrxSubsystemState_Idle && _coproState == CoproState_Application )
		{
			SendResult sendResult = SendFrameToCoprocessor( pendingTrxSubsystemFrame );
			if( sendResult == SendResult_Failed )
			{
				//fatal write error. Exit loop.
				break;
			}
			if( sendResult == SendResult_TryAgain )
			{
				//send was interupted by a fast frame. Send again.
				continue;
			}
			pendingTrxSubsystemFrame = NULL;
			_trxSubsystemState = TrxSubsystemState_WaitApplicationResponse;
			_nextTrxSubsystemTimer = Sysutils::GetMonotonicTime() + TrxSubsystemResponseTimeout;
		}else if( queueItem )
		{
			if( queueItem->GetDirection() == MacControllerQueueItem::Direction_Downstream )
			{
				if( _coproState == CoproState_Application )
				{
					SerialFrame* frame = queueItem->GetFrame( false );
					if( frame->GetSubsystem() == SerialFrame::FrameSubsystemType_TrxAdapter)
					{
						if( pendingTrxSubsystemFrame )
						{
							delete pendingTrxSubsystemFrame;
						}
						pendingTrxSubsystemFrame = queueItem->GetFrame( true );
						delete queueItem;
						queueItem = NULL;
						continue;
					}
					SendResult sendResult = SendFrameToCoprocessor( frame );
					if( sendResult == SendResult_Failed )
					{
						//fatal write error. Exit loop.
						break;
					}
					if( sendResult == SendResult_TryAgain )
					{
						//send was interupted by a fast frame. Send again.
						continue;
					}
					//prevent deletion of frame on queueItem delete
					queueItem->GetFrame( true );
				}
			} else {
				HandleFrameFromCoprocessor( queueItem->GetFrame( true ) );
			}
			delete queueItem;
			queueItem = NULL;
		}else{
			if(_coproState == CoproState_Unknown)
			{
				_coproState = CoproState_Identify;
				SendInitSequenceFrameToCoprocessor( new CommonCommandFrameIdentifyRequest() );
			}
		}
		uint32_t timeout = 5000;
		if( _coproState == CoproState_Application )
		{
			uint32_t now = Sysutils::GetMonotonicTime();
			int32_t timeUntilNextTrxSubsystemTimer = int32_t(_nextTrxSubsystemTimer - now);
			if( timeUntilNextTrxSubsystemTimer <= 0 )
			{
				OnTrxSubsystemTimer();
			}else if( uint32_t(timeUntilNextTrxSubsystemTimer) < timeout )
			{
				timeout = timeUntilNextTrxSubsystemTimer;
			}
		}else if( _coproState != CoproState_Unknown ){
			uint32_t now = Sysutils::GetMonotonicTime();
			int32_t timeUntilInitTimeout = int32_t(_initSequenceTimer - now);
			if( timeUntilInitTimeout <= 0 )
			{
				OnInitSequenceTimeout();
			}else if( uint32_t(timeUntilInitTimeout) < timeout )
			{
				timeout = timeUntilInitTimeout;
			}
		}
		queueItem = _incomingQueue.GetNextElement(timeout, true);
	}
	if( queueItem )
	{
		delete queueItem;
	}
	if( pendingTrxSubsystemFrame )
	{
		delete pendingTrxSubsystemFrame;
	}
	LOG( Logger::LOG_DEBUG, "MacController::ThreadFunction() ended" ); 
}

uint8_t MacController::GetNextDownstreamSequenceCounter()
{
	return _nextDownstreamSequenceCounter++;
}

void MacController::OnTrxSubsystemTimer()
{
	switch( _trxSubsystemState )
	{
	case TrxSubsystemState_Idle:
		{
			TrxAdapterFrameGetDutyCycle* getDutyCycleFrame = new TrxAdapterFrameGetDutyCycle();
			_lastRequestFrameId = getDutyCycleFrame->GetId();
			SendFrameToCoprocessor(getDutyCycleFrame);
			_trxSubsystemState = TrxSubsystemState_WaitDutyCycleResponse;
			_nextTrxSubsystemTimer = Sysutils::GetMonotonicTime() + TrxSubsystemResponseTimeout;
			_nextDutyCyclePoll = Sysutils::GetMonotonicTime() + DutyCyclePollInterval;
		}
		break;
	case TrxSubsystemState_WaitApplicationResponse:
		//Timeout on request frame from application
		_trxSubsystemState = TrxSubsystemState_Idle;
		_nextTrxSubsystemTimer = _nextDutyCyclePoll;
		break;
	case TrxSubsystemState_WaitDutyCycleResponse:
		//Timeout on duty cycle request
		LOG( Logger::LOG_WARNING, "Duty cycle response timeout" ); 
		_trxSubsystemState = TrxSubsystemState_Idle;
		_nextTrxSubsystemTimer = _nextDutyCyclePoll;
		break;
	}
}

bool MacController::IsInitializationDone() const
{
	return _initializationDone;
}

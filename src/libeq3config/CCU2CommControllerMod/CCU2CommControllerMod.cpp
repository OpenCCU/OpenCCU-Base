/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "CCU2CommControllerMod.h"

#include <CCU2SerialFrameMod.h>
#include <CCU2SerialPortWrapperMod.h>

#include <utils.h>//for time_millis
#include <HM2UtilsMod.h>
#include <Logger.h>
#include <pthread.h>
#include <errno.h>

//#include <FileIOMod.h>

#ifndef WIN32
#include <unistd.h>
#include <stdio.h>
#endif

#define BUSY_RETRY_AMOUNT 2
#define BUSY_RETRY_WAIT_MICROSECONDS 250000 //100*1000�s -> 130 ms

//#define DUMP 1

using namespace HM2Mod;

const uint32_t CCU2CommControllerMod::defaultResponseTimeout = 3500;//2000;
const uint32_t CCU2CommControllerMod::shortResponseTimeout = 1000;

CCU2CommControllerMod::CCU2CommControllerMod()
: pPortWrapper(NULL)//Will be instantiated in inheriting classes
, interfaceState(IFSTATE_INACTIVE)
, coprocessorState(COPROCESSOR_STATE_UNDEFINED)
, startApp(false)
, receiveThread(0)
, startCoprocessorAppThread(0)
, coprocessorType(COPROCESSOR_TYPE_UNDEFINED)
{
	pthread_mutexattr_init(&mutexAttr);
	pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_ERRORCHECK);

	pthread_mutex_init( &mutexSystemCommandRequest, &mutexAttr );
	pthread_mutex_init( &mutexHmipCommonCommandRequest, &mutexAttr);
	pthread_mutex_init( &mutexHmipTrxAdapterCommandRequest,&mutexAttr);
	pthread_mutex_init( &mutexBidcosRequest, &mutexAttr );
	pthread_mutex_init( &mutexBidcosTelegramRequest, &mutexAttr );
	pthread_mutex_init( &mutexLowLevelMacRequest, &mutexAttr );

	pthread_cond_init( &waitConditionSystemCommandRequest, NULL);
	pthread_cond_init( &waitConditionHmipCommonCommandRequest,NULL);
	pthread_cond_init( &waitConditionHmipTrxAdapterCommandRequest,NULL);
	pthread_cond_init( &waitConditionBidcosRequest, NULL);
	pthread_cond_init( &waitConditionBidcosTelegramRequest, NULL);
	pthread_cond_init( &waitConditionLowLevelMacRequest, NULL);

	pthread_attr_init(&receiveThreadAttributes);
	pthread_attr_setstacksize(&receiveThreadAttributes, 512*1024);

	pthread_attr_init( &startCoprocessorAppThreadAttributes );
	pthread_attr_setstacksize(&startCoprocessorAppThreadAttributes, 512*1024);

	pthread_mutex_init( &mutexCoprocessorState, &mutexAttr);
}

CCU2CommControllerMod::~CCU2CommControllerMod(void)
{
	interfaceState = IFSTATE_INACTIVE;
	void* foo;
	if(receiveThread != 0) {
		pthread_cancel(receiveThread);
		pthread_join( receiveThread, &foo );
		receiveThread = 0;
	}
	if(startCoprocessorAppThread != 0) {
		pthread_cancel(startCoprocessorAppThread);
		pthread_join( startCoprocessorAppThread, &foo );
		startCoprocessorAppThread = 0;
	}

	if(pPortWrapper != NULL) {
		pPortWrapper->Disconnect();
		delete pPortWrapper;
		pPortWrapper = NULL;
	}
//	pthread_exit( &startCoprocessorAppThread );
//	pthread_exit( &receiveThread );

	pthread_attr_destroy( &receiveThreadAttributes );
	pthread_attr_destroy( &startCoprocessorAppThreadAttributes );

	pthread_mutex_destroy( &mutexSystemCommandRequest );
	pthread_mutex_destroy(&mutexHmipCommonCommandRequest);
	pthread_mutex_destroy(&mutexHmipTrxAdapterCommandRequest);
	pthread_mutex_destroy( &mutexBidcosRequest );
	pthread_mutex_destroy( &mutexBidcosTelegramRequest );
	pthread_mutex_destroy( &mutexCoprocessorState );
	pthread_mutex_destroy( &mutexLowLevelMacRequest );

	pthread_cond_destroy( &waitConditionBidcosRequest );
	pthread_cond_destroy( &waitConditionBidcosTelegramRequest );
	pthread_cond_destroy( &waitConditionSystemCommandRequest );
	pthread_cond_destroy( &waitConditionHmipCommonCommandRequest);
	pthread_cond_destroy( &waitConditionHmipTrxAdapterCommandRequest);
	pthread_cond_destroy( &waitConditionLowLevelMacRequest);

}



bool CCU2CommControllerMod::init(bool skipStartApp) {
	//Start receiver in intialization mode
	interfaceState = IFSTATE_INIT;
	pthread_create( &receiveThread, &receiveThreadAttributes, CCU2CommControllerMod::receiveThreadFunction, (void*)this);
	usleep(500000);
	//Send identify to check if coprocessor is in bootloader or in the app.
	bool done = performIdentify();
	if(!done) {
		return false;
	}

	if(skipStartApp) {
		return true;
	}
	else {
		done = false;
		pthread_mutex_lock(&mutexCoprocessorState);
		if( coprocessorState == COPROCESSOR_STATE_DUAL_COPRO_APP || coprocessorState == COPROCESSOR_STATE_APPLICATION ) {
			pthread_mutex_unlock(&mutexCoprocessorState);
			return true;
		}
		else {
			pthread_mutex_unlock(&mutexCoprocessorState);
			done = sendHmipCommonCommand(HMIP_COMMON_START_APPLICATION, "", NULL, true);
			if(!done) {
				done = sendSystemCommand(SYSTEMCMD_STARTBOOTLOADER, "", NULL);
				if(!done) {
					LOG(Logger::LOG_ERROR, "CCU2CommControllerMod::init(): Cannot start coprocessor application. Aborting.");
					return false;
				}
			}
			//Wait up to 5 seconds for coprocessor to get into app.
			int waitCounter = 0;
			while( (!(coprocessorState == COPROCESSOR_STATE_DUAL_COPRO_APP || coprocessorState == COPROCESSOR_STATE_APPLICATION)) &&
					(waitCounter != 10)) {
				usleep(500000); //500 ms
				waitCounter++;
			}
		}
		done = (coprocessorState == COPROCESSOR_STATE_DUAL_COPRO_APP || coprocessorState == COPROCESSOR_STATE_APPLICATION);

		//Read firmware version and tell BidcosInterface...
		/*if(done) {
			std::string blVersion;
			std::string fwVersion;
			readFirmwareVersion(blVersion, fwVersion);
			//LOG(Logger::LOG_INFO, "CCU2CommControllerMod::init(): Coprocessor Bootloader Version is: %s" , blVersion.c_str());
			//LOG(Logger::LOG_INFO, "CCU2CommControllerMod::init(): Coprocessor Firmware Version is: %s" , fwVersion.c_str());
			//Now tell the BidcosInterface
			//pBidcosRemoteInterfcace->publishFirmwareVersion(fwVersion);
		}
		//setCSMACAEnabled(csmacaEnabled);*/
		return done;
	}
}

bool CCU2CommControllerMod::performIdentify() {
	std::string response;
	bool done = sendHmipCommonCommand(HMIP_COMMON_GET_IDENTIFY, "", &response, true);
	if(!done) {
		//fallback to homematic identify
		done = sendSystemCommand(SYSTEMCMD_IDENTIFY, "", &response);
		if(!done) {
			LOG(Logger::LOG_ERROR, "CCU2CommControllerMod::performIdentify(): Unable to determine coprocessor state.");
			return false;
		}
	}
	updateCoprocessorState(response);
	return true;
}

bool CCU2CommControllerMod::isDeviceOpen() const {
	return pPortWrapper->IsConnected();
}

bool CCU2CommControllerMod::sendSystemCommand(const SystemCommand systemCommand, const std::string& cmdData, std::string* pResponseValue) {
	bool returnCode = false;
	//Create coprocessor command
	switch(systemCommand)
	{
	case SYSTEMCMD_STARTBOOTLOADER:
		LOG(Logger::LOG_DEBUG,"CCU2CommControllerMod::sendSystemCommand(): Start Application / Bootloader");
		break;
	}
	CCU2CoprocessorCommandMod copCmd(systemCommand, cmdData);
	checkedLock(&mutexSystemCommandRequest);

	//Perform send
	for(int i = 0; i <= BUSY_RETRY_AMOUNT; i++) {
		CCU2SerialFrameMod reqFrame;
		reqFrame.setPayload( copCmd.getCommandString() );
		std::string reqFrameStr = reqFrame.getFrameData();//(getCommandString increments sequence counter automatically)

		//Begin new request
		systemCommandRequestInfo.requestCmd = copCmd;
		systemCommandRequestInfo.responseStatus = RESPONSE_WAIT;
		//LOG(Logger::LOG_DEBUG,"CCU2CommControllerMod::sendSystemCommand(): Send command seqence");
		//Send to coprocessor
		int sent = pPortWrapper->SendData( reqFrameStr );
		if(reqFrameStr.size() != (unsigned int)sent) {
			pthread_mutex_unlock(&mutexSystemCommandRequest);
			return false;
		}
		uint32_t timeout = shortResponseTimeout;
		if(systemCommand == SYSTEMCMD_SEND_UPDATEFRAME) {//fix for hm-mod-uart rtte firmware flashing (erasing empty pages needs more time
			timeout = defaultResponseTimeout;
		}

		waitForCoProcessorResponse(timeout, systemCommandRequestInfo, &mutexSystemCommandRequest, &waitConditionSystemCommandRequest);

		//Handle response
		if(systemCommandRequestInfo.responseStatus == RESPONSE_OK) {//OK
			CCU2CoprocessorCommandMod response = systemCommandRequestInfo.responseCmd;
			if(pResponseValue != NULL) {
				*pResponseValue = response.getCommandData();
			}
			if(i > 0) {
				LOG(Logger::LOG_INFO, "(%s) CCU2CommControllerMod::sendSystemCommand(): Got response after retry number %d",interfaceSerial.c_str(),(i));
			}
			returnCode = true;
			break;
		}
		else if(systemCommandRequestInfo.responseStatus == RESPONSE_FAIL_BUSY)//FAIL
		{
			usleep(BUSY_RETRY_WAIT_MICROSECONDS);
			LOG(Logger::LOG_INFO, "(%s) CCU2CommControllerMod::sendSystemCommand(): Retrying to send request (coprocessor was busy). Retry number %d",interfaceSerial.c_str() ,(i+1));
			continue;
		}
		else {//FAIL state...
			LOG(Logger::LOG_INFO, "CCU2CommControllerMod::sendSystemCommand(): failed");
			break;
		}
	}
	//Finish request and unlock mutex
	systemCommandRequestInfo.responseStatus = RESPONSE_IDLE;
	pthread_mutex_unlock(&mutexSystemCommandRequest);
	return returnCode;
}


bool CCU2CommControllerMod::sendHmipCommonCommand(const HmipCommonCommand hmipCommonCommand, const std::string& cmdData, std::string* pResponseValue, bool shortTimeout /*=false*/)
{
	bool returnCode = false;
	//Create coprocessor command
	CCU2CoprocessorCommandMod copCmd(hmipCommonCommand, cmdData);
	checkedLock(&mutexHmipCommonCommandRequest);

	//Perform send
	for(int i = 0; i <= BUSY_RETRY_AMOUNT; i++) {
		CCU2SerialFrameMod reqFrame;
		reqFrame.setPayload( copCmd.getCommandString() );
		std::string reqFrameStr = reqFrame.getFrameData();//(getCommandString increments sequence counter automatically)

		//Begin new request
		hmipCommonRequestInfo.requestCmd = copCmd;
		hmipCommonRequestInfo.responseStatus = RESPONSE_WAIT;
		//LOG(Logger::LOG_DEBUG,"CCU2CommControllerMod::sendHmipCommonCommand(): Send command");
		//Send to coprocessor
		int sent = pPortWrapper->SendData( reqFrameStr );
		if(reqFrameStr.size() != (unsigned int)sent) {
			pthread_mutex_unlock(&mutexHmipCommonCommandRequest);
			return false;
		}
		uint32_t timeout = defaultResponseTimeout;
		if(shortTimeout) {
			timeout = shortResponseTimeout;
		}
		waitForCoProcessorResponse(timeout, hmipCommonRequestInfo, &mutexHmipCommonCommandRequest, &waitConditionHmipCommonCommandRequest);

		//Handle response
		if(hmipCommonRequestInfo.responseStatus == RESPONSE_OK) {//OK
			CCU2CoprocessorCommandMod response = hmipCommonRequestInfo.responseCmd;
			if(pResponseValue != NULL) {
				*pResponseValue = response.getCommandData();
			}
			if(i > 0) {
				////LOG(Logger::LOG_INFO, "(%s) CCU2CommControllerMod::sendSystemCommand(): Got response after retry number %d",interfaceSerial.c_str(),(i));
			}
			returnCode = true;
			break;
		}
		else if(hmipCommonRequestInfo.responseStatus == RESPONSE_FAIL_BUSY)//FAIL
		{
			usleep(BUSY_RETRY_WAIT_MICROSECONDS);
			//LOG(Logger::LOG_INFO, "(%s) CCU2CommControllerMod::sendSystemCommand(): Retrying to send request (coprocessor was busy). Retry number %d",interfaceSerial.c_str() ,(i+1));
			continue;
		}
		else {//FAIL state...
			break;
		}
	}
	//Finish request and unlock mutex
	hmipCommonRequestInfo.responseStatus = RESPONSE_IDLE;
	pthread_mutex_unlock(&mutexHmipCommonCommandRequest);
	return returnCode;
}


bool CCU2CommControllerMod::sendHmipTrxAdapterCommand(const HmipTrxAdapterCommand hmipCommonCommand, const std::string& cmdData, std::string* pResponseValue)
{
	bool returnCode = false;
		//Create coprocessor command
		CCU2CoprocessorCommandMod copCmd(hmipCommonCommand, cmdData);
		checkedLock(&mutexHmipTrxAdapterCommandRequest);

		//Perform send
		for(int i = 0; i <= BUSY_RETRY_AMOUNT; i++) {
			CCU2SerialFrameMod reqFrame;
			reqFrame.setPayload( copCmd.getCommandString() );
			std::string reqFrameStr = reqFrame.getFrameData();//(getCommandString increments sequence counter automatically)

			//Begin new request
			hmipTrxAdapterRequestInfo.requestCmd = copCmd;
			hmipTrxAdapterRequestInfo.responseStatus = RESPONSE_WAIT;

			//Send to coprocessor
			int sent = pPortWrapper->SendData( reqFrameStr );
			if(reqFrameStr.size() != (unsigned int)sent) {
				pthread_mutex_unlock(&mutexHmipTrxAdapterCommandRequest);
				return false;
			}

			waitForCoProcessorResponse(defaultResponseTimeout, hmipTrxAdapterRequestInfo, &mutexHmipTrxAdapterCommandRequest, &waitConditionHmipTrxAdapterCommandRequest);

			//Handle response
			if(hmipTrxAdapterRequestInfo.responseStatus == RESPONSE_OK) {//OK
				CCU2CoprocessorCommandMod response = hmipTrxAdapterRequestInfo.responseCmd;
				if(pResponseValue != NULL) {
					*pResponseValue = response.getCommandData();
				}
				if(i > 0) {
					////LOG(Logger::LOG_INFO, "(%s) CCU2CommControllerMod::sendSystemCommand(): Got response after retry number %d",interfaceSerial.c_str(),(i));
				}
				returnCode = true;
				break;
			}
			else if(hmipTrxAdapterRequestInfo.responseStatus == RESPONSE_FAIL_BUSY)//FAIL
			{
				usleep(BUSY_RETRY_WAIT_MICROSECONDS);
				//LOG(Logger::LOG_INFO, "(%s) CCU2CommControllerMod::sendSystemCommand(): Retrying to send request (coprocessor was busy). Retry number %d",interfaceSerial.c_str() ,(i+1));
				continue;
			}
			else {//FAIL state...
				break;
			}
		}
		//Finish request and unlock mutex
		hmipTrxAdapterRequestInfo.responseStatus = RESPONSE_IDLE;
		pthread_mutex_unlock(&mutexHmipTrxAdapterCommandRequest);
		return returnCode;
}

bool CCU2CommControllerMod::sendLowLevelMacCommand(const LowLevelMacCommand lowLevelMacCommand, std::string* pResponseValue/* = NULL*/)
{
	bool returnCode = false;
	//Create coprocessor command
	CCU2CoprocessorCommandMod copCmd(lowLevelMacCommand);
	checkedLock(&mutexLowLevelMacRequest);

	//Perform send
	for(int i = 0; i <= BUSY_RETRY_AMOUNT; i++) {
		CCU2SerialFrameMod reqFrame;
		reqFrame.setPayload( copCmd.getCommandString() );
		std::string reqFrameStr = reqFrame.getFrameData();//(getCommandString increments sequence counter automatically)

		//Begin new request
		lowLevelMacRequestInfo.requestCmd = copCmd;
		lowLevelMacRequestInfo.responseStatus = RESPONSE_WAIT;
		//LOG(Logger::LOG_DEBUG,"CCU2CommControllerMod::sendSystemCommand(): Send command seqence");
		//Send to coprocessor
		int sent = pPortWrapper->SendData( reqFrameStr );
		if(reqFrameStr.size() != (unsigned int)sent) {
			pthread_mutex_unlock(&mutexLowLevelMacRequest);
			return false;
		}

		waitForCoProcessorResponse(shortResponseTimeout, lowLevelMacRequestInfo, &mutexLowLevelMacRequest, &waitConditionLowLevelMacRequest);

		//Handle response
		if(lowLevelMacRequestInfo.responseStatus == RESPONSE_OK) {//OK
			CCU2CoprocessorCommandMod response = lowLevelMacRequestInfo.responseCmd;
			if(pResponseValue != NULL) {
				*pResponseValue = response.getCommandData();
			}
			if(i > 0) {
				LOG(Logger::LOG_INFO, "(%s) CCU2CommControllerMod::sendLowLevelMacCommand(): Got response after retry number %d",interfaceSerial.c_str(),(i));
			}
			returnCode = true;
			break;
		}
		else if(lowLevelMacRequestInfo.responseStatus == RESPONSE_FAIL_BUSY)//FAIL
		{
			usleep(BUSY_RETRY_WAIT_MICROSECONDS);
			LOG(Logger::LOG_INFO, "(%s) CCU2CommControllerMod::sendLowLevelMacCommand(): Retrying to send request (coprocessor was busy). Retry number %d",interfaceSerial.c_str() ,(i+1));
			continue;
		}
		else {//FAIL state...
			LOG(Logger::LOG_INFO, "CCU2CommControllerMod::sendLowLevelMacCommand(): failed");
			break;
		}
	}
	//Finish request and unlock mutex
	lowLevelMacRequestInfo.responseStatus = RESPONSE_IDLE;
	pthread_mutex_unlock(&mutexLowLevelMacRequest);
	return returnCode;
}

bool CCU2CommControllerMod::startReceiver()
{
	//start receive thread
	interfaceState = IFSTATE_ACTIVE;
	//pthread_create( &receiveThread, &receiveThreadAttributes, CCU2CommControllerMod::receiveThreadFunction, (void*)this);
	return true;
}

bool CCU2CommControllerMod::stopReceiver()
{
	//interfaceState = IFSTATE_INACTIVE;
	interfaceState = IFSTATE_INIT;
	//void* whatever;
	//int foo = pthread_join(receiveThread, &whatever);
	//return (foo == 0);
	return true;
}

void* CCU2CommControllerMod::startCoprocessorAppThreadFunction(void* params)
{
	CCU2CommControllerMod* pThis = (CCU2CommControllerMod*) params;

	std::string data;//, devId;
	bool done = false;
	int tryCount = 0;
	bool sendStartCommand = false;
	while(pThis->interfaceState > IFSTATE_INACTIVE && (!done) && tryCount < 4) {
		switch(pThis->coprocessorState) {
			case COPROCESSOR_STATE_APPLICATION:
			case COPROCESSOR_STATE_DUAL_COPRO_APP:
				if(!pThis->startApp) {
					sendStartCommand = true;
				}
				break;
			case COPROCESSOR_STATE_BOOTLOADER:
				if(pThis->startApp) {
					sendStartCommand = true;
				}
				break;
			case COPROCESSOR_STATE_UNDEFINED:
				pThis->performIdentify();
				tryCount--;
				break;
			default:
				LOG(Logger::LOG_DEBUG, "CCU2CommControllerMod::startCoprocessorAppThreadFunction(): Unhandled coprocessor type.");
				break;
		}
		if(sendStartCommand) {
			switch(pThis->coprocessorType) {
				case COPROCESSOR_TYPE_HM:
					done = pThis->sendSystemCommand(SYSTEMCMD_STARTBOOTLOADER, "", NULL);
					break;
				case COPROCESSOR_TYPE_HMIP:
					done = pThis->sendHmipCommonCommand( ( pThis->startApp ? HMIP_COMMON_START_APPLICATION : HMIP_COMMON_START_BOOTLOADER ), "", NULL );
					break;
				default:
					break;
			}
		}
		tryCount++;
	}

	return NULL;
}

void* CCU2CommControllerMod::receiveThreadFunction(void* params)
{
	CCU2CommControllerMod* pThis = (CCU2CommControllerMod*)params;
	std::string buffer;
	CCU2PortWrapperMod* pSerialPortWrapper = pThis->pPortWrapper;
	CCU2SerialFrameMod frame;

	std::string leftOver;
	while(pThis->interfaceState >= IFSTATE_INIT) {
		buffer.clear();
		if(leftOver.size() > 0) {
			buffer = leftOver;
			leftOver.clear();
		}
		else {
			const int read = pSerialPortWrapper->ReadData( &buffer );
			if(read <=  0) { continue; }
		}
		bool frameComplete = frame.addFrameData( buffer, leftOver);
		if(frameComplete) {
			pThis->handleIncomingSerialFrame( frame );
			frame.reset();
		}
	}
	return NULL;
}

void CCU2CommControllerMod::handleIncomingSerialFrame(const CCU2SerialFrameMod& serialFrame) {
	CCU2SerialFrameMod sFrame = serialFrame;
	bool dualCopro = false;
#ifdef DUMP
	LOG(Logger::LOG_DEBUG, "Frame payload: %s", toDebugHexStr(serialFrame.getPayload()).c_str());
#endif
	if(coprocessorType == COPROCESSOR_TYPE_UNDEFINED) {
		const std::string& serialPayload = serialFrame.getPayload();
		if(CCU2CoprocessorCommandMod::isIdentifyResponse(serialPayload)) {
			std::string identifyStr = CCU2CoprocessorCommandMod::getIdentifyResponseIdentificationString(serialPayload);
			updateCoprocessorState(identifyStr);
		}
	}
	if(coprocessorType != COPROCESSOR_TYPE_UNDEFINED) {
		//LOG(Logger::LOG_DEBUG,"CCU2CommControllerMod::handleIncomingSerialFrame(): new serial frame ");
		CCU2CoprocessorCommandMod copCmd(sFrame.getPayload(),(coprocessorType == COPROCESSOR_TYPE_HMIP));

	#ifdef DUMP
		LOG(Logger::LOG_DEBUG, "CCU2CommControllerMod::handleIncomingSerialFrame(): Coprocessor Command is:\n%s", toDebugHexStr( sFrame.getPayload() ).c_str() );
	#endif
		if(!copCmd.isResponseValid()) {
			LOG(Logger::LOG_DEBUG, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Command not parseable. Expected was frame of type %s", interfaceSerial.c_str(), (coprocessorType == COPROCESSOR_TYPE_HMIP ? "HmIP" : "HM"));
			return;
		}
		//EVENTS
		if(copCmd.isEvent()) {
			//LOG(Logger::LOG_DEBUG, "F Event");
			handleIncomingEvent(copCmd);
		}
		//RESPONSES
		else  {
			//LOG(Logger::LOG_DEBUG, "F Response");
			handleIncomingResponse(copCmd, serialFrame);
		}
	}
}


void CCU2CommControllerMod::handleIncomingEvent(const CCU2CoprocessorCommandMod& copCmd)
{
	if(copCmd.getCommandType() == COMMANDTYPE_BIDCOS) {
		if(isBidcosEventABidcosResponse(copCmd)) {
			////LOG(Logger::LOG_ALL, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Handling event as bidcos command response.", interfaceSerial.c_str());
			////LOG(Logger::LOG_DEBUG, "(%s) Response status: %s.", interfaceSerial.c_str(),  copCmd.getBidcosCommandResponseStatusAsString().c_str() );
			bidcosRequestInfo.responseStatus = RESPONSE_OK;
			pthread_cond_signal( &waitConditionBidcosRequest );
		}
		else if(interfaceState == IFSTATE_ACTIVE){

		}
		//TODO Status of authentification
	}
	else if(copCmd.getCommandType() == COMMANDTYPE_SYSTEM) {
		//TODO Implement handling of system events.
		if(copCmd.isIdentifyEvent()) {
			handleIdentifyEvent(copCmd);
		}
		/*else if(copCmd.isDutyCycleEvent()) {
					double dutyCycleValue = copCmd.getDutyCycleValue();
					pBidcosRemoteInterfcace->handleDutyCycleEvent( dutyCycleValue );
					////LOG(Logger::LOG_INFO, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Duty cycle event with value: %f.2", interfaceSerial.c_str(), dutyCycleValue);
				}*/
		else {
			////LOG(Logger::LOG_DEBUG, "(%s) Unkown system event type.\n", interfaceSerial.c_str());
		}
		////LOG(Logger::LOG_INFO, "Handling of system type events not implemented yet.\n");
	}
	else if(copCmd.getCommandType() == COMMANDTYPE_HMIP_COMMON)
	{
		if(copCmd.isIdentifyEvent())
		{
			handleIdentifyEvent(copCmd);
		}
	}
	else {
		////LOG(Logger::LOG_ERROR, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Unknown event type. Dropped message.", interfaceSerial.c_str());
	}
}

void CCU2CommControllerMod::handleIncomingResponse(const CCU2CoprocessorCommandMod& copCmd, const CCU2SerialFrameMod& serialFrame)
{
	//COMMANDTYPE_BIDCOS
	if(copCmd.getCommandType() == COMMANDTYPE_BIDCOS && coprocessorState == COPROCESSOR_STATE_APPLICATION) {
		////LOG(Logger::LOG_DEBUG, "(%s) Response status: %s.", interfaceSerial.c_str(), copCmd.getBidcosCommandResponseStatusAsString().c_str() );
		if( (bidcosTelegramRequestInfo.responseStatus == RESPONSE_WAIT) &&
			(CCU2CoprocessorCommandMod::isResponseOfRequest( bidcosTelegramRequestInfo.requestCmd, copCmd )) )
		{
			////LOG(Logger::LOG_ALL, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Got an bidcos msg. Response waiting --> Handling as response...", interfaceSerial.c_str());
			bidcosTelegramRequestInfo.responseCmd = copCmd;
			if(copCmd.isResponseStatusOk()) {
				bidcosTelegramRequestInfo.responseStatus = RESPONSE_OK;
				////LOG(Logger::LOG_ALL, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Response ok", interfaceSerial.c_str());
			}
			else if(copCmd.isResponseStatusBusy()) {
				bidcosTelegramRequestInfo.responseStatus = RESPONSE_FAIL_BUSY;
				//LOG(Logger::LOG_ALL, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Response not ok. Coprocessor busy.", interfaceSerial.c_str());
			}
			else {
				bidcosTelegramRequestInfo.responseStatus = RESPONSE_FAIL;
				////LOG(Logger::LOG_ALL, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Response not ok", interfaceSerial.c_str());
			}
			pthread_cond_signal( &waitConditionBidcosTelegramRequest );
		}
		else if( (bidcosRequestInfo.responseStatus == RESPONSE_WAIT) &&
			(CCU2CoprocessorCommandMod::isResponseOfRequest( bidcosRequestInfo.requestCmd, copCmd )) )
		{
			////LOG(Logger::LOG_ALL, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Got an bidcos msg. Response waiting --> Handling as response...", interfaceSerial.c_str());
			bidcosRequestInfo.responseCmd = copCmd;
			if(copCmd.isResponseStatusOk()) {
				bidcosRequestInfo.responseStatus = RESPONSE_OK;
				////LOG(Logger::LOG_ALL, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Response ok", interfaceSerial.c_str());
			}
			else if(copCmd.isResponseStatusBusy()) {
				bidcosRequestInfo.responseStatus = RESPONSE_FAIL_BUSY;
				////LOG(Logger::LOG_ALL, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Response not ok. Coprocessor busy.", interfaceSerial.c_str());
			}
			else {
				bidcosRequestInfo.responseStatus = RESPONSE_FAIL;
				////LOG(Logger::LOG_ALL, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Response not ok", interfaceSerial.c_str());
			}
			pthread_cond_signal( &waitConditionBidcosRequest );
		}
		else {//no current request or not a response for that request -> handle as event
			//if(pBidcosRemoteInterfcace != NULL) {
				////LOG(Logger::LOG_ALL, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Got an bidcos msg. No response waiting or not a response --> Handling as event...", interfaceSerial.c_str());
				/*BidcosFrame frame;
				assembleBidcosFrame(copCmd, frame);
				pBidcosRemoteInterfcace->handleEvent( frame );*/
			//}
		}
	}
	//COMMANDTYPE_SYSTEM
	else if(copCmd.getCommandType() == COMMANDTYPE_SYSTEM) {
		//LOG(Logger::LOG_DEBUG,"CCU2CommControllerMod::handleIncomingResponse() System response");
		if(systemCommandRequestInfo.responseStatus == RESPONSE_WAIT) {
			if(copCmd.isResponseValid()) {
				if( CCU2CoprocessorCommandMod::isResponseOfRequest( systemCommandRequestInfo.requestCmd, copCmd ) ) {
					systemCommandRequestInfo.responseCmd = copCmd;
					if(copCmd.isResponseStatusOk()) {
						systemCommandRequestInfo.responseStatus = RESPONSE_OK;
						LOG(Logger::LOG_DEBUG,"CCU2CommControllerMod::handleIncomingResponse() System response OK");
					}
					else if(copCmd.isResponseStatusBusy()) {
						LOG(Logger::LOG_DEBUG,"CCU2CommControllerMod::handleIncomingResponse() System response Busy");
						systemCommandRequestInfo.responseStatus = RESPONSE_FAIL_BUSY;
						////LOG(Logger::LOG_ALL, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Response not ok. Coprocessor busy.", interfaceSerial.c_str());
					}
					else {
						systemCommandRequestInfo.responseStatus = RESPONSE_FAIL;
						LOG(Logger::LOG_DEBUG,"CCU2CommControllerMod::handleIncomingResponse() System response failed");
					}
					pthread_cond_signal(&waitConditionSystemCommandRequest );
				}
				else {
					//TODO Handle other messages!!!!
					////LOG(Logger::LOG_ERROR, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Dropped message, cuz handling of non-response msg is not implemented yet.\nPayload is: %s", interfaceSerial.c_str(),serialFrame.getPayload().c_str());
					//GetConcentrator()->ProcessReceivedFrame(frame);
				}
			}
			else {
				////LOG(Logger::LOG_DEBUG, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Incoming message could not be parsed.\nMessage is:\n%s", interfaceSerial.c_str(),serialFrame.getPayload().c_str());

			}
		}
	}
	//COMMANDTYPE HMIP COMMON
	else if (copCmd.getCommandType() == COMMANDTYPE_HMIP_COMMON) {
		if (copCmd.isResponseValid()) {
			if (CCU2CoprocessorCommandMod::isResponseOfRequest(
					hmipCommonRequestInfo.requestCmd, copCmd)) {
				hmipCommonRequestInfo.responseCmd = copCmd;
				if (copCmd.isResponseStatusOk()) {
					hmipCommonRequestInfo.responseStatus = RESPONSE_OK;
				} else if (copCmd.isResponseStatusBusy()) {
					hmipCommonRequestInfo.responseStatus =
							RESPONSE_FAIL_BUSY;
					LOG(Logger::LOG_ALL, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Response not ok. Coprocessor busy.", interfaceSerial.c_str());
				} else {
					hmipCommonRequestInfo.responseStatus = RESPONSE_FAIL;
				}
				pthread_cond_signal(&waitConditionHmipCommonCommandRequest);
			}
		}
		else {
			LOG(Logger::LOG_DEBUG, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): HMIP Response not valid.", interfaceSerial.c_str());
		}
	}
	//COMMANDTYPE TRX_ADAPTER
	else if(copCmd.getCommandType() == COMMANDTYPE_TRX_ADAPTER && coprocessorState == COPROCESSOR_STATE_DUAL_COPRO_APP)
	{
		if (copCmd.isResponseValid()) {
			if (CCU2CoprocessorCommandMod::isResponseOfRequest(
					hmipTrxAdapterRequestInfo.requestCmd, copCmd)) {
				hmipTrxAdapterRequestInfo.responseCmd = copCmd;
				if (copCmd.isResponseStatusOk()) {
					hmipTrxAdapterRequestInfo.responseStatus = RESPONSE_OK;
				} else if (copCmd.isResponseStatusBusy()) {
					hmipTrxAdapterRequestInfo.responseStatus = RESPONSE_FAIL_BUSY;
					////LOG(Logger::LOG_ALL, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Response not ok. Coprocessor busy.", interfaceSerial.c_str());
				} else {
					hmipTrxAdapterRequestInfo.responseStatus = RESPONSE_FAIL;
				}
				pthread_cond_signal(&waitConditionHmipTrxAdapterCommandRequest);
			}
		}
	}
	//COMMANDTYPE LOW LEVEL MAC
	else if(copCmd.getCommandType() == COMMANDTYPE_LOWLEVELMAC && coprocessorState == COPROCESSOR_STATE_DUAL_COPRO_APP)
	{
		if (copCmd.isResponseValid()) {
			if (CCU2CoprocessorCommandMod::isResponseOfRequest(lowLevelMacRequestInfo.requestCmd, copCmd)) {
				lowLevelMacRequestInfo.responseCmd = copCmd;
				if (copCmd.isResponseStatusOk()) {
					lowLevelMacRequestInfo.responseStatus = RESPONSE_OK;
				} else if (copCmd.isResponseStatusBusy()) {
					lowLevelMacRequestInfo.responseStatus = RESPONSE_FAIL_BUSY;
					////LOG(Logger::LOG_ALL, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Response not ok. Coprocessor busy.", interfaceSerial.c_str());
				} else {
					lowLevelMacRequestInfo.responseStatus = RESPONSE_FAIL;
				}
				pthread_cond_signal(&waitConditionLowLevelMacRequest);
			}
		}
	}
	else {
		LOG(Logger::LOG_ERROR, "(%s) CCU2CommControllerMod::handleIncomingSerialFrame(): Unknown response type. Dropped message.", interfaceSerial.c_str());
	}
}

void CCU2CommControllerMod::handleIdentifyEvent(const CCU2CoprocessorCommandMod& copCmd)
{
	//LOG(Logger::LOG_DEBUG, "(%s) CCU2CommControllerMod::handleIdentifyEvent(): Handling incoming identify event.", interfaceSerial.c_str());
	std::string identifyString = copCmd.getCommandData();
	updateCoprocessorState(identifyString);
}

void CCU2CommControllerMod::updateCoprocessorState(const std::string& identifyString) {
	checkedLock( &mutexCoprocessorState );
	if(identifyString.compare("Co_CPU_BL") == 0) {
		LOG(Logger::LOG_DEBUG, "(%s) CCU2CommControllerMod::handleIdentifyEvent(): Coprocessor is in bootloader.", interfaceSerial.c_str());
		coprocessorState = COPROCESSOR_STATE_BOOTLOADER;
		coprocessorType = COPROCESSOR_TYPE_HM;
	}
	else if(identifyString.compare("HMIP_TRX_Bl") == 0) {
		LOG(Logger::LOG_DEBUG, "(%s) CCU2CommControllerMod::handleIdentifyEvent(): Coprocessor is in homematic ip bootloader.", interfaceSerial.c_str());
		coprocessorState = COPROCESSOR_STATE_BOOTLOADER;
		coprocessorType = COPROCESSOR_TYPE_HMIP;
	}
	else if(identifyString.compare("Co_CPU_App") == 0 ) {
		LOG(Logger::LOG_DEBUG, "(%s) CCU2CommControllerMod::handleIdentifyEvent(): Coprocessor is in application.", interfaceSerial.c_str());
		coprocessorState = COPROCESSOR_STATE_APPLICATION;
		coprocessorType = COPROCESSOR_TYPE_HM;
	}
	else if(identifyString.compare("DualCoPro_App") == 0)
	{
		LOG(Logger::LOG_DEBUG, "(%s) CCU2CommControllerMod::handleIdentifyEvent(): Coprocessor is in dual coprocessor application.", interfaceSerial.c_str());
		coprocessorState = COPROCESSOR_STATE_DUAL_COPRO_APP;
		coprocessorType = COPROCESSOR_TYPE_HMIP;
	}
	else if(identifyString.compare("HMIP_TRX_App") == 0)
	{
		LOG(Logger::LOG_DEBUG, "(%s) CCU2CommControllerMod::handleIdentifyEvent(): Coprocessor is in homematic ip application.", interfaceSerial.c_str());
		coprocessorState = COPROCESSOR_STATE_DUAL_COPRO_APP;
		coprocessorType = COPROCESSOR_TYPE_HMIP;
	}
	else {
		coprocessorState =	COPROCESSOR_STATE_UNDEFINED;
		coprocessorType = COPROCESSOR_TYPE_UNDEFINED;
		LOG(Logger::LOG_ERROR, "(%s) CCU2CommControllerMod::handleIdentifyEvent(): Coprocessor returned unknown identify string: %s.", interfaceSerial.c_str(), identifyString.c_str());
	}
	pthread_mutex_unlock( &mutexCoprocessorState );
}

bool CCU2CommControllerMod::startCoprocessorApp(bool wait)
{

	startApp = true;
	if(coprocessorState == COPROCESSOR_STATE_APPLICATION || coprocessorState == COPROCESSOR_STATE_DUAL_COPRO_APP) {
		return true;
	}
	if(interfaceState == IFSTATE_ACTIVE) {
		interfaceState = IFSTATE_REINIT;
	}
	LOG(Logger::LOG_DEBUG, "(%s) CCU2CommControllerMod::startCoprocessorApp(): Trying to start coprocessor application", interfaceSerial.c_str());
	bool done = true;
	pthread_create( &startCoprocessorAppThread, &startCoprocessorAppThreadAttributes, startCoprocessorAppThreadFunction, (void*)this);
	if(wait) {
		unsigned int cnt = 0;
		while(coprocessorState == COPROCESSOR_STATE_UNDEFINED || coprocessorState == COPROCESSOR_STATE_BOOTLOADER) {
			if(cnt == 10) {
				coprocessorState = COPROCESSOR_STATE_BOOTLOADER;
				return false;
			}
			usleep(500000);
			cnt++;			
		}
	}
	return done;
}

bool CCU2CommControllerMod::startCoprocessorBootloader(bool wait) {

	startApp = false;
	if(coprocessorState == COPROCESSOR_STATE_BOOTLOADER) {
		return true;
	}
	if(interfaceState == IFSTATE_ACTIVE) {
		interfaceState = IFSTATE_REINIT;
	}
	LOG(Logger::LOG_DEBUG, "(%s) CCU2CommControllerMod::startCoprocessorBootloader(): Trying to start coprocessor bootloader", interfaceSerial.c_str());
	bool done = true;

	pthread_create( &startCoprocessorAppThread, &startCoprocessorAppThreadAttributes, startCoprocessorAppThreadFunction, (void*)this);
	if(wait) {
		unsigned int cnt = 0;
		while(coprocessorState != COPROCESSOR_STATE_BOOTLOADER) {
			if(cnt == 10) {
				return false;
			}
			usleep(500000);
			cnt++;
		}
	}
	return done;
}

bool CCU2CommControllerMod::isBidcosEventABidcosResponse(const CCU2CoprocessorCommandMod& eventCmd)
{
	if(bidcosRequestInfo.responseStatus == RESPONSE_WAIT) {
		const int cmdDstAddr = eventCmd.getBidcosTelegramDestinationAddress();
		const int reqCmdSrcAddr = bidcosRequestInfo.requestCmd.getBidcosTelegramSourceAddress();
		if( (cmdDstAddr != -1) &&
			(reqCmdSrcAddr != -1) &&
			(reqCmdSrcAddr == cmdDstAddr) )
		{
			return true;
		}
	}
	return false;
}


bool CCU2CommControllerMod::isBidcosEventABidcosTelegramResponse(const CCU2CoprocessorCommandMod& eventCmd)
{
	if(bidcosTelegramRequestInfo.responseStatus == RESPONSE_WAIT) {
		const int cmdDstAddr = eventCmd.getBidcosTelegramDestinationAddress();
		const int reqCmdSrcAddr = bidcosTelegramRequestInfo.requestCmd.getBidcosTelegramSourceAddress();
		if( (cmdDstAddr != -1) &&
			(reqCmdSrcAddr != -1) &&
			(reqCmdSrcAddr == cmdDstAddr) )
		{
			const int reqBidcosTelegramCounter = bidcosTelegramRequestInfo.requestCmd.getBidcosTelegramCounter();
			const int eventCmdBidcosTelegramCounter = eventCmd.getBidcosTelegramCounter();
			if(reqBidcosTelegramCounter == eventCmdBidcosTelegramCounter) {
				return true;
			}
		}
	}
	return false;
}


CCU2CoprocessorCommandMod CCU2CommControllerMod::sendBidcosRequest(CCU2CoprocessorCommandMod requestCmd, RequestInfo& requestInfo, pthread_mutex_t* mutex, pthread_cond_t* waitCondition, bool& done)
{
	////LOG(Logger::LOG_DEBUG, "CCU2CommControllerMod::sendBidcosRequest() (Internal): Begin");
	CCU2CoprocessorCommandMod responseCmd;
	done = false;
	//Create serial frame
	////LOG(Logger::LOG_DEBUG, "CCU2CommControllerMod::sendBidcosRequest() (Internal): Assembling command string");

	for(int busyRetries = 0; busyRetries <= BUSY_RETRY_AMOUNT; busyRetries++) {
		//Wait for response
		CCU2SerialFrameMod reqFrame;
		reqFrame.setPayload( requestCmd.getCommandString() );//(getCommandString increments sequence counter automatically)

		//Begin new request
		requestInfo.requestCmd = requestCmd;
		requestInfo.responseStatus = RESPONSE_WAIT;

		//Send
		////LOG(Logger::LOG_DEBUG, "CCU2CommControllerMod::sendBidcosRequest() (Internal): Sending data");
		/*int sentBytes = */
		pPortWrapper->SendData( reqFrame.getFrameData() );

		waitForCoProcessorResponse(defaultResponseTimeout, requestInfo, mutex, waitCondition);

		//Handle response
		responseCmd = requestInfo.responseCmd;

		if(requestInfo.responseStatus == RESPONSE_OK) {
			if(busyRetries > 0) {
				////LOG(Logger::LOG_INFO, "(%s) CCU2CommControllerMod::sendBidcosRequest(): Got response after retry number %d", interfaceSerial.c_str(),(busyRetries));
			}
			done = true;
			break;
		}
		else if(requestInfo.responseStatus == RESPONSE_FAIL_BUSY) {
			usleep(BUSY_RETRY_WAIT_MICROSECONDS);
			////LOG(Logger::LOG_INFO, "(%s) CCU2CommControllerMod::sendBidcosRequest(): Retrying to send request (coprocessor was busy). Retry number %d", interfaceSerial.c_str(),(busyRetries+1));
			continue;
		}
		else {
			break;
		}
	}

	//Finish request
	requestInfo.responseStatus = RESPONSE_IDLE;

	////LOG(Logger::LOG_DEBUG, "CCU2CommControllerMod::sendBidcosRequest() (Internal): End");
	//go to hell
	return responseCmd;
}


bool CCU2CommControllerMod::sendBidcosRequest(const BidcosCommand bidcosCommand, const std::string& param, std::string* responseData) {
	////LOG(Logger::LOG_DEBUG, "CCU2CommControllerMod::sendBidcosRequest(): Begin");
	bool done = false;
	checkedLock(&mutexBidcosRequest);

	CCU2CoprocessorCommandMod reqCommand(bidcosCommand, param);
	CCU2CoprocessorCommandMod responseCommand = sendBidcosRequest( reqCommand, bidcosRequestInfo, &mutexBidcosRequest, &waitConditionBidcosRequest, done);

	//handle response
	if(done) {
		if(responseData != NULL) {
			responseData->clear();
			BidcosCommandResponseStatus bcrStatus = responseCommand.getBidcosCommandResponseStatus();
			if( (bcrStatus == BIDCOSCMD_RESPONSE_STATUS_TELEGRAM_SENT_RECVD_ACK) ||
				(bcrStatus == BIDCOSCMD_RESPONSE_STATUS_DATA) )
			{
				responseData->append(responseCommand.getCommandData());
			}
		}
	}
	else {
		//Maybe log here...
	}
	pthread_mutex_unlock(&mutexBidcosRequest);
	////LOG(Logger::LOG_DEBUG, "CCU2CommControllerMod::sendBidcosRequest(): End");
	return done;
}

void CCU2CommControllerMod::waitForCoProcessorResponse(const int64_t& timeout, RequestInfo& requestInfo, pthread_mutex_t* mutex, pthread_cond_t* waitCondition)
{
	timespec ts = millis2abstime(timeout);
	pthread_cond_timedwait( waitCondition, mutex, &ts);
	if(requestInfo.responseStatus == RESPONSE_WAIT) {
		////LOG(Logger::LOG_DEBUG, "(%s) CCU2CommControllerMod::waitForCoProcessorResponse(): Timeout while waiting for response.\n", interfaceSerial.c_str());
		requestInfo.responseStatus = RESPONSE_TIMEOUT;
	}
}

void CCU2CommControllerMod::checkedLock(pthread_mutex_t* mutex)
{
	//const std::string serial(interfaceSerial);
	int val = pthread_mutex_trylock( mutex );
	while(val != 0) {
		std::string msg("CCU2CommControllerMod::checkedLock(): ");
		switch(val) {
			case EBUSY:
				msg.append("The mutex could not be acquired because it was already locked.");
				break;
			case EINVAL:
				msg.append("The value specified by the mutex parameter does not refer to an initialized mutex object.");
				break;
			case EDEADLK:
				msg.append("The current thread already owns the mutex and the mutex type is PTHREAD_MUTEX_ERRORCHECK.");
				break;
			case EPERM:
				msg.append("The current thread does not own the mutex and the mutex type is not PTHREAD_MUTEX_NORMAL.");
				break;
			default:
				msg.append("Unknown return type!!!");
				break;
		}
		////LOG(Logger::LOG_DEBUG, msg.c_str());
		usleep(10000); //10 milliseconds * 1000 <-> 10 000 microseconds
		val = pthread_mutex_trylock( mutex );
		if(val == 0) {
			msg.append("Got the mutex now.");
		}
	}
}

CCU2CommControllerMod::RequestInfo::RequestInfo()
{
	responseStatus = RESPONSE_IDLE;
}

bool CCU2CommControllerMod::readFirmwareVersion(std::string& bootloaderVersion, std::string& firmwareVersion)
{
	std::string requestData;
	std::string responseData;
	bool done = sendSystemCommand(SYSTEMCMD_GETVERSION, requestData, &responseData);
	if(done) {
		if(responseData.size() == 6) {
			char* buffer = new char[5];//3 byte for digits, one for sign and one for ending \0
			//Bootloader
			snprintf(buffer, 5, "%d", (int)responseData.at(0));
			bootloaderVersion.append(buffer);
			bootloaderVersion.append( 1, '.');
			snprintf(buffer, 5, "%d", (int)responseData.at(1));
			bootloaderVersion.append(buffer);
			bootloaderVersion.append( 1, '.');
			snprintf(buffer, 5, "%d", (int)responseData.at(2));
			bootloaderVersion.append(buffer);
			//Firmware
			snprintf(buffer, 5, "%d", (int)responseData.at(3));
			firmwareVersion.append(buffer);//Firmware Major
			firmwareVersion.append( 1, '.');
			snprintf(buffer, 5, "%d", (int)responseData.at(4));
			firmwareVersion.append(buffer);//Firmware Minor
			firmwareVersion.append( 1, '.');
			snprintf(buffer, 5, "%d", (int)responseData.at(5));
			firmwareVersion.append(buffer);//Firmware Revision
			delete[] buffer;
		}
		else {
			////LOG(Logger::LOG_ERROR, "(%s) CCU2CommControllerMod::readFirmwareVersion(): Wrong size of response data.", interfaceSerial.c_str());
			done = false;
		}
	}
	return done;
}

std::string CCU2CommControllerMod::readSerialNumber()
{
	std::string response;
	bool done = sendSystemCommand(SYSTEMCMD_GET_SERIALNR, "", &response);
	if(!done) {
		////LOG(Logger::LOG_ERROR, "CCU2CommControllerMod::readSerialNumber(): Could not read serial number from coprocessor.");
		response.clear();
	}
	return response;
}

bool CCU2CommControllerMod::setCSMACAEnabled(const bool enabled)
{
	std::string params;
	if(enabled) {
		params.append(1, (char)0x01);
	}
	else {
		params.append(1, (char)0x00);
	}
	bool done = sendSystemCommand(SYSTEMCMD_CSCMCA_ON_OFF, params, NULL);
	if(done) {
		////LOG(Logger::LOG_INFO, "(%s) CCU2CommControllerMod::setCSMACAEnabled(): CSMA/CA %s.", interfaceSerial.c_str(), (enabled ? "enabled" : "disabled"));
	}
	else {
		////LOG(Logger::LOG_ERROR, "(%s) CCU2CommControllerMod::setCSMACAEnabled(): Error %s CSMA/CA.", interfaceSerial.c_str(), (enabled ? "enabling" : "disabling"));
	}
	return done;
}

bool CCU2CommControllerMod::setInterfaceClock(const unsigned int utcSeconds, const int offsetMinutes)
{
	std::string cmdParams;
	cmdParams.append(toBigEndianString(utcSeconds));//4 byte utc time in seconds
	cmdParams.append(1, (char) ((offsetMinutes / 30) & 0xFF));//1 byte offset in half-hours (+1 hour --> +2)
	////LOG(Logger::LOG_FATAL_ERROR, "REMOVE ME: %s\n", toHexStr(cmdParams).c_str());
	return sendSystemCommand( SYSTEMCMD_SET_CLOCK, cmdParams );
}
bool CCU2CommControllerMod::setDataRate100k()
{
	std::string cmdParams;
	cmdParams.append(1,(char)0xe9);
	cmdParams.append(1,(char)0xca);
	return sendSystemCommand( SYSTEMCMD_SET_100K_DATARATE,  cmdParams);
}
bool  CCU2CommControllerMod::setDataRate10k()
{
	return sendSystemCommand( SYSTEMCMD_SET_10K_DATARATE,  "");
}
bool CCU2CommControllerMod::getDutyCycle(int *dutyCycle)
{
	std::string response;
	bool done = sendSystemCommand(SYSTEMCMD_GETDUTYCYLCE,"",&response);
	if(!done)
	{
		////LOG(Logger::LOG_ERROR, "(%s) CCU2CommControllerMod::getDutyCycle(): Could not get DutyCycle from coprocessor.", interfaceSerial.c_str());
		response.clear();
	}
	else if(response.size() > 0) 
	{
		*dutyCycle = (unsigned char)response[0];
		*dutyCycle = (*dutyCycle) >> 1; //Values are from 0 to 200 -> Convert to 0% to 100%
	}
	else {
	//	//LOG(Logger::LOG_ERROR, "(%s) CCU2CommControllerMod::getDutyCycle(): Response empty", interfaceSerial.c_str());
	}
	return done;
}
bool CCU2CommControllerMod::isDualCoprocessor()
{
	//return coprocessorState == COPROCESSOR_STATE_DUAL_COPRO_APP;
	return coprocessorType == COPROCESSOR_TYPE_HMIP;
}

bool HM2Mod::CCU2CommControllerMod::setRFLGWInfoLED(const unsigned int state)
{
	return false;
}

void CCU2CommControllerMod::setInterfaceSerial(const std::string& ifSerial)
{
	interfaceSerial = ifSerial;
}

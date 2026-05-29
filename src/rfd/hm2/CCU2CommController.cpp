/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "CCU2CommController.h"

#include <CCU2SerialFrame.h>
#include <CCU2SerialPortWrapper.h>
#include <CCU2BidcosRemoteInterface.h>
#include <BidcosFrame.h>

#include <utils.h>//for time_millis
#include <HM2Utils.h>
#include <Logger.h>
#include <pthread.h>
#include <errno.h>

#include <FileIO.h>

#ifndef WIN32
#include <unistd.h>
#include <stdio.h>
#endif

#include "RFDDefines.h"

#define BUSY_RETRY_AMOUNT 2
#define BUSY_RETRY_WAIT_MICROSECONDS 250000 //100*1000�s -> 130 ms

#define COPRO_IDENTIFY_APP "Co_CPU_App"
#define COPRO_IDENTIFY_BL "Co_CPU_BL"

//#define DUMP 1

using namespace HM2;

const uint32_t CCU2CommController::coproCommandTimeout = 1000;//timeout for sendSystemCommand and sendBidcosCommand
const uint32_t CCU2CommController::defaultResponseTimeout = 4000;//[TWIST-548] //3500; //timeout for sendBidcosTelegram
const uint32_t CCU2CommController::tripleBurstTimeout = 12000;//timeout for sendBidcosTelegram

CCU2CommController::CCU2CommController(CCU2BidcosRemoteInterface* bidcosRemoteInterface)
: pPortWrapper(NULL)//Will be instantiated in inheriting classes
, pBidcosRemoteInterfcace(bidcosRemoteInterface)
, interfaceState(IFSTATE_INACTIVE)
, receiveThread(0)
, startCoprocessorAppThread(0)
, predicateWaitConditionSystemCommandRequest(false)
, predicateWaitConditionBidcosRequest(false)
, predicateWaitConditionBidcosTelegramRequest(false)
, coprocessorState(COPROCESSOR_STATE_UNDEFINED)
, improvedInitialization(false)
{
	pthread_mutexattr_init(&mutexAttr);
	//pthread_mutexattr_settype(&mutexAttr, PTHREAD_MUTEX_ERRORCHECK);

	pthread_mutex_init( &mutexSystemCommandRequest, &mutexAttr );
	pthread_mutex_init( &mutexBidcosRequest, &mutexAttr );
	pthread_mutex_init( &mutexBidcosTelegramRequest, &mutexAttr );

	pthread_cond_init( &waitConditionSystemCommandRequest, NULL);
	pthread_cond_init( &waitConditionBidcosRequest, NULL);
	pthread_cond_init( &waitConditionBidcosTelegramRequest, NULL);

	pthread_attr_init(&receiveThreadAttributes);
	pthread_attr_setstacksize(&receiveThreadAttributes, 512*1024);

	pthread_attr_init( &startCoprocessorAppThreadAttributes );
	pthread_attr_setstacksize(&startCoprocessorAppThreadAttributes, 512*1024);

	pthread_mutex_init( &mutexCoprocessorState, &mutexAttr);
}

CCU2CommController::~CCU2CommController(void)
{
	interfaceState = IFSTATE_INACTIVE;
	void* foo;
	if(receiveThread != 0) {
		pthread_cancel( receiveThread );
		pthread_join( receiveThread, &foo );
		receiveThread = 0;
	}
	if(startCoprocessorAppThread != 0) {
		pthread_cancel( startCoprocessorAppThread );
		pthread_join( startCoprocessorAppThread, &foo );
		startCoprocessorAppThread = 0;
	}
//	pthread_exit( &startCoprocessorAppThread );
//	pthread_exit( &receiveThread );

	pthread_attr_destroy( &receiveThreadAttributes );
	pthread_attr_destroy( &startCoprocessorAppThreadAttributes );

	pthread_mutex_destroy( &mutexSystemCommandRequest );
	pthread_mutex_destroy( &mutexBidcosRequest );
	pthread_mutex_destroy( &mutexBidcosTelegramRequest );
	pthread_mutex_destroy( &mutexCoprocessorState );

	pthread_cond_destroy( &waitConditionBidcosRequest );
	pthread_cond_destroy( &waitConditionBidcosTelegramRequest );
	pthread_cond_destroy( &waitConditionSystemCommandRequest );

	if(pPortWrapper != NULL) { //We can do this here, or in child classes. But in child classes we first must stop the threads....
		pPortWrapper->Disconnect();
		delete pPortWrapper;
		pPortWrapper = NULL;
	}

//	pCurrentRequestBidcosFrame = NULL; //Not our stuff, so we don't delete it here
	//delete pBidcosRemoteInterfcace; <- pointer must not be deleted, it's a back reference
}



bool CCU2CommController::init(const bool csmacaEnabled, const bool improvedCoproInit) {
	improvedInitialization = improvedCoproInit;
	//Start receiver in intialization mode
	interfaceState = IFSTATE_INIT;
	pthread_create( &receiveThread, &receiveThreadAttributes, CCU2CommController::receiveThreadFunction, (void*)this);

	bool done = false;
	if(improvedCoproInit) {
		LOG(Logger::LOG_DEBUG, "CCU2CommController::init() - Improved initialization.");
		done = improvedInit();
	}
	else {
		LOG(Logger::LOG_DEBUG, "CCU2CommController::init() - Legacy initialization.");
		done = legacyInit();
	}

	//Read firmware version and tell BidcosInterface...
	if(done) {
		std::string blVersion;
		std::string fwVersion;
		readFirmwareVersion(blVersion, fwVersion);
		if(Logger::WouldLog(Logger::LOG_INFO)) {
			LOG(Logger::LOG_INFO, "CCU2CommController::init(): Coprocessor Bootloader Version is: %s" , blVersion.c_str());
			LOG(Logger::LOG_INFO, "CCU2CommController::init(): Coprocessor Firmware Version is: %s" , fwVersion.c_str());
		}
		//Now tell the BidcosInterface
		pBidcosRemoteInterfcace->publishFirmwareVersion(fwVersion);
	}

	setCSMACAEnabled(csmacaEnabled);
	setDutyCycleCheck(false);
	return done;
}

bool CCU2CommController::legacyInit() {
	bool done = false;
	int waitCounter = 0;
	//Wait for coprocessor to enter the app
	while((coprocessorState != COPROCESSOR_STATE_APPLICATION) &&
		  (waitCounter != 10) )
	{
		usleep(500 * 1000); //500 ms
		waitCounter++;
	}
	//xtra check for ccu2 coprocessor which cannot be resetted yet.
	if(coprocessorState != COPROCESSOR_STATE_APPLICATION) {

		//xtra check for ccu2 coprocessor which is not resettable yet.
		//FIXME xtra check (remove that later)
		initCoprocessor();
		waitCounter = 0;
		//Wait for coprocessor to enter the app
		while((coprocessorState != COPROCESSOR_STATE_APPLICATION) &&
		      (waitCounter != 10) )
		{
			usleep(500 * 1000); //500 ms
			waitCounter++;
		}
		if(coprocessorState != COPROCESSOR_STATE_APPLICATION) {
			done = false;
			LOG(Logger::LOG_FATAL_ERROR, "CCU2CommController::init(): Init failed. Cannot start coprocessor application.");
		}
		else {
			done = true;
		}
	}
	else {
		done = true;
	}
	//end xtra check
	return done;
}

bool CCU2CommController::improvedInit() {
	bool done = false;
	std::string response;
	pthread_mutex_lock(&mutexCoprocessorState);
	sendSystemCommand(SYSTEMCMD_IDENTIFY, "", &response);
	if(response.compare(COPRO_IDENTIFY_APP) == 0) {
		coprocessorState = COPROCESSOR_STATE_APPLICATION;
		done = true;
		LOG(Logger::LOG_DEBUG, "CCU2CommController::improvedInit() - Coprocessor is in application.");
	}
	else if(response.compare(COPRO_IDENTIFY_BL) == 0){
		LOG(Logger::LOG_DEBUG, "CCU2CommController::improvedInit() - Coprocessor is in bootloader, going to start application.");
		coprocessorState = COPROCESSOR_STATE_BOOTLOADER;
		for(unsigned int i = 0; i < 3; i++) {
			done = sendSystemCommand(SYSTEMCMD_STARTBOOTLOADER, "", NULL);
			if(done) {
				break;
			}
			else {
				LOG(Logger::LOG_DEBUG, "CCU2CommController::improvedInit() Send start bl/app command failed.");
				usleep(2500000);//2,5 seconds
				if(coprocessorState == COPROCESSOR_STATE_APPLICATION) {
					coprocessorState = COPROCESSOR_STATE_APPLICATION;
					LOG(Logger::LOG_DEBUG, "CCU2CommController::improvedInit() Coprocessor is in application now.");
					done = true;
					break;
				}
				LOG(Logger::LOG_DEBUG, "CCU2CommController::improvedInit() retrying...");
			}
		}
	}
	else {
		LOG(Logger::LOG_DEBUG, "CCU2CommController::improvedInit() - Coprocessor state could not be determined.");
		coprocessorState = COPROCESSOR_STATE_UNDEFINED;
		LOG(Logger::LOG_ERROR, "CCU2CommController::improvedInit() - Identify response string not handled: %s", response.c_str());
	}
	pthread_mutex_unlock(&mutexCoprocessorState);
	//wait for copro to reach the app
	done = false;
	for(unsigned int i = 0; i < 20; i++) {
		usleep(250000);
		if(coprocessorState == COPROCESSOR_STATE_APPLICATION) {
			done = true;
			break;
		}
	}
	if(!done) {
		LOG(Logger::LOG_DEBUG, "CCU2CommController::improvedInit() - Timeout while waiting for coprocessor application");
	}
	return done;
}

bool CCU2CommController::isDeviceOpen() const {
	return pPortWrapper->IsConnected();
}

bool CCU2CommController::sendSystemCommand(const SystemCommand systemCommand, const std::string& cmdData, std::string* pResponseValue) {
	bool returnCode = false;
	//Create coprocessor command
	LOG(Logger::LOG_ALL, "CCU2CommController::SendSystemCommdand()");
	CCU2CoprocessorCommand copCmd(systemCommand, cmdData);
	pthread_mutex_lock(&mutexSystemCommandRequest);

	//Perform send
	for(int i = 0; i <= BUSY_RETRY_AMOUNT; i++) {
		CCU2SerialFrame reqFrame;
		reqFrame.setPayload( copCmd.getCommandString() );
		std::string reqFrameStr = reqFrame.getFrameData();//(getCommandString increments sequence counter automatically)

		//Begin new request
		systemCommandRequestInfo.requestCmd = copCmd;
		systemCommandRequestInfo.responseStatus = RESPONSE_WAIT;
		LOG(Logger::LOG_ALL, "CCU2CommController::SendSystemCommdand() sending: %s", toDebugHexStr(reqFrameStr).c_str());
		//Send to coprocessor
		int sent = pPortWrapper->SendData( reqFrameStr );
		if(reqFrameStr.size() != (unsigned int)sent) {
			pthread_mutex_unlock(&mutexSystemCommandRequest);
			return false;
		}

		waitForCoProcessorResponse(systemCommandRequestInfo, &mutexSystemCommandRequest, &waitConditionSystemCommandRequest, &predicateWaitConditionSystemCommandRequest, coproCommandTimeout);

		//Handle response
		if(systemCommandRequestInfo.responseStatus == RESPONSE_OK) {//OK
			CCU2CoprocessorCommand response = systemCommandRequestInfo.responseCmd;
			if(pResponseValue != NULL) {
				*pResponseValue = response.getCommandData();
			}
			if(i > 0) {
				if(Logger::WouldLog(Logger::LOG_INFO)) {
					LOG(Logger::LOG_INFO, "(%s) CCU2CommController::sendSystemCommand(): Got response after retry number %d",interfaceSerial.c_str(),(i));
				}
			}
			returnCode = true;
			break;
		}
		else if(systemCommandRequestInfo.responseStatus == RESPONSE_FAIL_BUSY)//FAIL
		{
			usleep(BUSY_RETRY_WAIT_MICROSECONDS);
			if(Logger::WouldLog(Logger::LOG_INFO)) {
				LOG(Logger::LOG_INFO, "(%s) CCU2CommController::sendSystemCommand(): Retrying to send request (coprocessor was busy). Retry number %d",interfaceSerial.c_str() ,(i+1));
			}
			continue;
		}
		else {//FAIL state...
			break;
		}
	}
	//Finish request and unlock mutex
	systemCommandRequestInfo.responseStatus = RESPONSE_IDLE;
	pthread_mutex_unlock(&mutexSystemCommandRequest);
	return returnCode;
}

bool CCU2CommController::startReceiver()
{
	//start receive thread
	interfaceState = IFSTATE_ACTIVE;
	//pthread_create( &receiveThread, &receiveThreadAttributes, CCU2CommController::receiveThreadFunction, (void*)this);
	return true;
}

bool CCU2CommController::stopReceiver()
{
	//interfaceState = IFSTATE_INACTIVE;
	interfaceState = IFSTATE_INIT;
	//void* whatever;
	//int foo = pthread_join(receiveThread, &whatever);
	//return (foo == 0);
	return true;
}


void* CCU2CommController::receiveThreadFunction(void* params)
{
	CCU2CommController* pThis = (CCU2CommController*)params;
	std::string buffer;
	CCU2PortWrapper* pSerialPortWrapper = pThis->pPortWrapper;
	CCU2SerialFrame frame;

	std::string leftOver;
	while(pThis->interfaceState >= IFSTATE_INIT) {
		buffer.clear();
		if(leftOver.size() > 0) {
			buffer = leftOver;
			leftOver.clear();
		}
		else {
			const int read = pSerialPortWrapper->ReadData( &buffer );
			if(read <=  0) {
				if(read == 0) {
					continue;
				}
				else {//Throw away data on error like connection loss
					leftOver.clear();
					buffer.clear();
					frame.reset();
					continue;
				}
			}
			LOG(Logger::LOG_ALL, "RX SERIAL: %s", toDebugHexStr(std::string(buffer, 0, read)).c_str());
		}
		bool frameComplete = frame.addFrameData( buffer, leftOver);
		if(frameComplete) {
			LOG(Logger::LOG_ALL, "RX FRAME: %s", toDebugHexStr(frame.getPayload()).c_str());
			pThis->handleIncomingSerialFrame( frame );
			frame.reset();
		}
	}
	return NULL;
}

void CCU2CommController::handleIncomingSerialFrame(const CCU2SerialFrame& serialFrame) {
	CCU2SerialFrame sFrame = serialFrame;
	CCU2CoprocessorCommand copCmd(sFrame.getPayload());
#ifdef DUMP
	LOG(Logger::LOG_DEBUG, "CCU2CommController::handleIncomingSerialFrame(): Coprocessor Command is:\n%s", toHexStr( sFrame.getPayload() ).c_str() );
#endif
	if(!copCmd.isResponseValid()) {
		if(Logger::WouldLog(Logger::LOG_DEBUG)) {
			LOG(Logger::LOG_DEBUG, "(%s) CCU2CommController::handleIncomingSerialFrame(): Command not parseable.", interfaceSerial.c_str());
		}
		return;
	}
	//EVENTS
	if(copCmd.isEvent()) {
		handleIncomingEvent(copCmd);
	}
	//RESPONSES
	else  {
		handleIncomingResponse(copCmd, serialFrame);
	}
}


void CCU2CommController::handleIncomingEvent(const CCU2CoprocessorCommand& copCmd)
{
			if(copCmd.getCommandType() == COMMANDTYPE_BIDCOS) {
				//if(interfaceState == IFSTATE_ACTIVE) {
					if(pBidcosRemoteInterfcace != NULL) {
						//LOG(Logger::LOG_DEBUG, "CCU2CommController::handleIncomingSerialFrame(): Got an bidcos msg. Handling as event...");
						if(isBidcosEventABidcosTelegramResponse(copCmd)) {
							if(Logger::WouldLog(Logger::LOG_ALL)) {
								LOG(Logger::LOG_ALL, "(%s) CCU2CommController::handleIncomingSerialFrame(): Handling event as bidcos telegram response.", interfaceSerial.c_str());
							}
							if(Logger::WouldLog(Logger::LOG_DEBUG)) {
								LOG(Logger::LOG_DEBUG, "(%s) Telegram Response status: OK.", interfaceSerial.c_str());
							}
							pthread_mutex_lock(&mutexBidcosTelegramRequest);
							bidcosTelegramRequestInfo.responseStatus = RESPONSE_OK;
							bidcosTelegramRequestInfo.responseCmd = copCmd;
							predicateWaitConditionBidcosTelegramRequest = true;
							pthread_cond_signal( &waitConditionBidcosTelegramRequest );
							pthread_mutex_unlock(&mutexBidcosTelegramRequest);
						}
						else if(isBidcosEventABidcosResponse(copCmd)) {
							if(Logger::WouldLog(Logger::LOG_ALL)) {
								LOG(Logger::LOG_ALL, "(%s) CCU2CommController::handleIncomingSerialFrame(): Handling event as bidcos command response.", interfaceSerial.c_str());
							}
							if(Logger::WouldLog(Logger::LOG_DEBUG)) {
								LOG(Logger::LOG_DEBUG, "(%s) Response status: OK.", interfaceSerial.c_str());
							}
							pthread_mutex_lock(&mutexBidcosRequest);
							bidcosRequestInfo.responseStatus = RESPONSE_OK;
							bidcosRequestInfo.responseCmd = copCmd;
							predicateWaitConditionBidcosRequest = true;
							pthread_cond_signal( &waitConditionBidcosRequest );
							pthread_mutex_unlock(&mutexBidcosRequest);
						}
						else if(interfaceState == IFSTATE_ACTIVE){//normal bidcos event frame
							if(Logger::WouldLog(Logger::LOG_ALL)) {
								LOG(Logger::LOG_ALL, "(%s) CCU2CommController::handleIncomingSerialFrame(): Handling event telegram as event.", interfaceSerial.c_str());
							}

							BidcosFrame frame;
							assembleBidcosFrame(copCmd, frame);
							pBidcosRemoteInterfcace->handleEvent(frame);
						}
					}
				//}
				//TODO Status of authentification
			}
			else if(copCmd.getCommandType() == COMMANDTYPE_SYSTEM) {
				//TODO Implement handling of system events.
				if(copCmd.isIdentifyEvent()) {
					handleIdentifyEvent(copCmd);
				}
				else if(copCmd.isDutyCycleEvent()) {
					double dutyCycleValue = copCmd.getDutyCycleValue();
					pBidcosRemoteInterfcace->handleDutyCycleEvent( dutyCycleValue );
					if(Logger::WouldLog(Logger::LOG_INFO)) {
						LOG(Logger::LOG_INFO, "(%s) CCU2CommController::handleIncomingSerialFrame(): Duty cycle event with value: %f.2", interfaceSerial.c_str(), dutyCycleValue);
					}
				}
				else {
					if(Logger::WouldLog(Logger::LOG_DEBUG)) {
						LOG(Logger::LOG_DEBUG, "(%s) Unkown system event type.\n", interfaceSerial.c_str());
					}
				}
				//LOG(Logger::LOG_INFO, "Handling of system type events not implemented yet.\n");
			}
			else {
				LOG(Logger::LOG_ERROR, "(%s) CCU2CommController::handleIncomingSerialFrame(): Unknown event type. Dropped message.", interfaceSerial.c_str());
			}
}

void CCU2CommController::handleIncomingResponse(const CCU2CoprocessorCommand& copCmd, const CCU2SerialFrame& serialFrame)
{
	//COMMANDTYPE_BIDCOS
	if(copCmd.getCommandType() == COMMANDTYPE_BIDCOS) {
		if(Logger::WouldLog(Logger::LOG_DEBUG)) {
			LOG(Logger::LOG_DEBUG, "(%s) Response status: %s.", interfaceSerial.c_str(), copCmd.getBidcosCommandResponseStatusAsString().c_str() );
		}
		if( (bidcosTelegramRequestInfo.responseStatus == RESPONSE_WAIT) &&
			(CCU2CoprocessorCommand::isResponseOfRequest( bidcosTelegramRequestInfo.requestCmd, copCmd )) )
		{
			if(Logger::WouldLog(Logger::LOG_ALL)) {
				LOG(Logger::LOG_ALL, "(%s) CCU2CommController::handleIncomingSerialFrame(): Got an bidcos msg. Response waiting --> Handling as response...", interfaceSerial.c_str());
			}
			pthread_mutex_lock(&mutexBidcosTelegramRequest);
			bidcosTelegramRequestInfo.responseCmd = copCmd;
			if(copCmd.isResponseStatusOk()) {
				bidcosTelegramRequestInfo.responseStatus = RESPONSE_OK;
				if(Logger::WouldLog(Logger::LOG_ALL)) {
					LOG(Logger::LOG_ALL, "(%s) CCU2CommController::handleIncomingSerialFrame(): Response ok", interfaceSerial.c_str());
				}
			}
			else if(copCmd.isResponseStatusBusy()) {
				bidcosTelegramRequestInfo.responseStatus = RESPONSE_FAIL_BUSY;
				if(Logger::WouldLog(Logger::LOG_ALL)) {
					LOG(Logger::LOG_ALL, "(%s) CCU2CommController::handleIncomingSerialFrame(): Response not ok. Coprocessor busy.", interfaceSerial.c_str());
				}
			}
			else {
				bidcosTelegramRequestInfo.responseStatus = RESPONSE_FAIL;
				if(Logger::WouldLog(Logger::LOG_ALL)) {
					LOG(Logger::LOG_ALL, "(%s) CCU2CommController::handleIncomingSerialFrame(): Response not ok", interfaceSerial.c_str());
				}
			}
			predicateWaitConditionBidcosTelegramRequest = true;
			pthread_cond_signal( &waitConditionBidcosTelegramRequest );
			pthread_mutex_unlock(&mutexBidcosTelegramRequest);
		}
		else if( (bidcosRequestInfo.responseStatus == RESPONSE_WAIT) &&
			(CCU2CoprocessorCommand::isResponseOfRequest( bidcosRequestInfo.requestCmd, copCmd )) )
		{
			if(Logger::WouldLog(Logger::LOG_ALL)) {
				LOG(Logger::LOG_ALL, "(%s) CCU2CommController::handleIncomingSerialFrame(): Got an bidcos msg. Response waiting --> Handling as response...", interfaceSerial.c_str());
			}
			pthread_mutex_lock(&mutexBidcosRequest);
			bidcosRequestInfo.responseCmd = copCmd;
			if(copCmd.isResponseStatusOk()) {
				bidcosRequestInfo.responseStatus = RESPONSE_OK;
				if(Logger::WouldLog(Logger::LOG_ALL)) {
					LOG(Logger::LOG_ALL, "(%s) CCU2CommController::handleIncomingSerialFrame(): Response ok", interfaceSerial.c_str());
				}
			}
			else if(copCmd.isResponseStatusBusy()) {
				bidcosRequestInfo.responseStatus = RESPONSE_FAIL_BUSY;
				if(Logger::WouldLog(Logger::LOG_ALL)) {
					LOG(Logger::LOG_ALL, "(%s) CCU2CommController::handleIncomingSerialFrame(): Response not ok. Coprocessor busy.", interfaceSerial.c_str());
				}
			}
			else {
				bidcosRequestInfo.responseStatus = RESPONSE_FAIL;
				if(Logger::WouldLog(Logger::LOG_ALL)) {
					LOG(Logger::LOG_ALL, "(%s) CCU2CommController::handleIncomingSerialFrame(): Response not ok", interfaceSerial.c_str());
				}
			}
			predicateWaitConditionBidcosRequest = true;
			pthread_cond_signal( &waitConditionBidcosRequest );
			pthread_mutex_unlock(&mutexBidcosRequest);
		}
		else {//no current request or not a response for that request -> handle as event
			if(pBidcosRemoteInterfcace != NULL) {
				if(Logger::WouldLog(Logger::LOG_ALL)) {
					LOG(Logger::LOG_ALL, "(%s) CCU2CommController::handleIncomingSerialFrame(): Got an bidcos msg. No response waiting or not a response --> Handling as event...", interfaceSerial.c_str());
				}
				BidcosFrame frame;
				assembleBidcosFrame(copCmd, frame);
				pBidcosRemoteInterfcace->handleEvent( frame );
			}
		}
	}
	//COMMANDTYPE_SYSTEM
	else if(copCmd.getCommandType() == COMMANDTYPE_SYSTEM) {
		if(systemCommandRequestInfo.responseStatus == RESPONSE_WAIT) {
			if(copCmd.isResponseValid()) {
				if( CCU2CoprocessorCommand::isResponseOfRequest( systemCommandRequestInfo.requestCmd, copCmd ) ) {
					pthread_mutex_lock(&mutexSystemCommandRequest);
					systemCommandRequestInfo.responseCmd = copCmd;
					if(copCmd.isResponseStatusOk()) {
						systemCommandRequestInfo.responseStatus = RESPONSE_OK;
					}
					else if(copCmd.isResponseStatusBusy()) {
						systemCommandRequestInfo.responseStatus = RESPONSE_FAIL_BUSY;
						if(Logger::WouldLog(Logger::LOG_ALL)) {
							LOG(Logger::LOG_ALL, "(%s) CCU2CommController::handleIncomingSerialFrame(): Response not ok. Coprocessor busy.", interfaceSerial.c_str());
						}
					}
					else {
						systemCommandRequestInfo.responseStatus = RESPONSE_FAIL;
					}
					predicateWaitConditionSystemCommandRequest = true;
					pthread_cond_signal( &waitConditionSystemCommandRequest );
					pthread_mutex_unlock(&mutexSystemCommandRequest);
				}
				else {
					//TODO Handle other messages!!!!
					LOG(Logger::LOG_ERROR, "(%s) CCU2CommController::handleIncomingSerialFrame(): Dropped message, cuz handling of non-response msg is not implemented yet.\nPayload is: %s", interfaceSerial.c_str(),serialFrame.getPayload().c_str());
					//GetConcentrator()->ProcessReceivedFrame(frame);
				}
			}
			else {
				if(Logger::WouldLog(Logger::LOG_DEBUG)) {
					LOG(Logger::LOG_DEBUG, "(%s) CCU2CommController::handleIncomingSerialFrame(): Incoming message could not be parsed.\nMessage is:\n%s", interfaceSerial.c_str(),serialFrame.getPayload().c_str());
				}

			}
		}
	}
	else {
		LOG(Logger::LOG_ERROR, "(%s) CCU2CommController::handleIncomingSerialFrame(): Unknown response type. Dropped message.", interfaceSerial.c_str());
	}
}

void CCU2CommController::handleIdentifyEvent(const CCU2CoprocessorCommand& copCmd)
{
	//if( (!improvedInitialization) || interfaceState != IFSTATE_INIT) {
		if(Logger::WouldLog(Logger::LOG_ALL)) {
			LOG(Logger::LOG_DEBUG, "(%s) CCU2CommController::handleIdentifyEvent(): Handling incoming identify event.", interfaceSerial.c_str());
		}
		bool startApp = false;
		std::string identifyString = copCmd.getCommandData();
		if(pthread_mutex_trylock( &mutexCoprocessorState ) == 0) {
			if(identifyString.compare(COPRO_IDENTIFY_BL) == 0) {
				if(Logger::WouldLog(Logger::LOG_DEBUG)) {
					LOG(Logger::LOG_DEBUG, "(%s) CCU2CommController::handleIdentifyEvent(): Coprocessor entered bootloader. Starting application.", interfaceSerial.c_str());
				}
				coprocessorState = COPROCESSOR_STATE_BOOTLOADER;
				startApp = true;
			}
			else if(identifyString.compare(COPRO_IDENTIFY_APP) == 0) {
				if(Logger::WouldLog(Logger::LOG_DEBUG)) {
					LOG(Logger::LOG_DEBUG, "(%s) CCU2CommController::handleIdentifyEvent(): Coprocessor entered application. Life is good.", interfaceSerial.c_str());
				}
				coprocessorState = COPROCESSOR_STATE_APPLICATION;
			}
			else {
				coprocessorState =	COPROCESSOR_STATE_UNDEFINED;
				startApp = true;
				LOG(Logger::LOG_ERROR, "(%s) CCU2CommController::handleIdentifyEvent(): Coprocessor returned unknown identify string.", interfaceSerial.c_str());
			}
			if(startApp) {
				startCoprocessorApp();
			}
			pthread_mutex_unlock( &mutexCoprocessorState );
		}
		/*}
	else {
		LOG(Logger::LOG_ALL, "CCU2CommController::handleIdentifyEvent(): Do nothing"); //<--- Remove this debug message
	}*/
}

void CCU2CommController::initCoprocessor()
{
	coprocessorState = COPROCESSOR_STATE_UNDEFINED;
	startCoprocessorApp();
}

bool CCU2CommController::startCoprocessorApp()
{
	if(interfaceState == IFSTATE_ACTIVE) {
		interfaceState = IFSTATE_REINIT;
	}
	if(Logger::WouldLog(Logger::LOG_DEBUG)) {
		LOG(Logger::LOG_DEBUG, "(%s) CCU2CommController::startCoprocessorApp(): Trying to start coprocessor application", interfaceSerial.c_str());
	}
	bool done = true;
	pthread_create( &startCoprocessorAppThread, &startCoprocessorAppThreadAttributes, startCoprocessorAppThreadFunction, (void*)this);
	return done;
}

void* CCU2CommController::startCoprocessorAppThreadFunction(void* params)
{
	CCU2CommController* pThis = (CCU2CommController*) params;
	std::string data;//, devId;
	bool done = false;
	int tryCount = 0;
	usleep(1000*500);
	while(pThis->interfaceState > IFSTATE_INACTIVE && (!done) && tryCount < 4 ) {
		if(Logger::WouldLog(Logger::LOG_ALL)) {
			LOG(Logger::LOG_ALL, "CCU2CommController::startCoprocessorAppThreadFunction(): Send start command");
		}
		done = pThis->sendSystemCommand(SYSTEMCMD_STARTBOOTLOADER, data);
		if(done) {
			break;
		}
		usleep(2000*1000); //wait 2 sec
		if(pThis->interfaceState == IFSTATE_INACTIVE) {
			return NULL;
		}
		if(pThis->coprocessorState != COPROCESSOR_STATE_APPLICATION) {
			tryCount++;
			if(Logger::WouldLog(Logger::LOG_DEBUG)) {
				LOG(Logger::LOG_ALL, "CCU2CommController::startCoprocessorAppThreadFunction(): Retrying to send start command");
			}
		}
		else {
			break;
		}

	}
	if(!done) {
		LOG(Logger::LOG_ERROR,"CCU2CommController::startCoprocessorAppThreadFunction(): Trying to send SYSTEMCMD_STARTBOOTLOADER failed 3 times.");
	}
	else {//on success starting the app, we wait until coprocessor finished starting the app and after that we republish devices
		tryCount = 0;
		while(pThis->interfaceState > IFSTATE_INACTIVE && pThis->coprocessorState != COPROCESSOR_STATE_APPLICATION && tryCount < 10) {
			usleep(500 * 1000); //500 ms
			tryCount++;
		}
		if(pThis->interfaceState == IFSTATE_REINIT) {
			pThis->restoreConfigToCoprocessor();
		}
	}
	return NULL;
}

bool CCU2CommController::restoreConfigToCoprocessor()
{
	bool done = true;
	done = done && setCSMACAEnabled(false);//FIXME We disable that in any case. If we enable it one day, this should disabled here...
	done = done && setDutyCycleCheck(false); // Disable DutyCycle check !!!! TODO: Remove this line for production.
	done = done && pBidcosRemoteInterfcace->republishAllDevices();//In case of init() the list is empty, otherwise we need to republish all devices
	interfaceState = IFSTATE_ACTIVE;//switch back to active (on initialization this is done elsewhere)
	pBidcosRemoteInterfcace->SetInterfaceClockBySystemTime();
	return done;
}

void CCU2CommController::assembleBidcosFrame(const CCU2CoprocessorCommand& bidcosCmd, BidcosFrame& frame)
{
	const uint64_t time = time_millis();
	std::string bidcosFrameData = bidcosCmd.getCommandData();
	for(unsigned int i = 0; i < bidcosFrameData.size(); i++) {
		frame.SetByteData(i, bidcosFrameData.at(i));
	}
	if(bidcosCmd.isRSSISet()) {
		frame.SetRSSI( -1*bidcosCmd.getRSSI() );
	}
	if(bidcosCmd.isDevWokenUp()) {
		frame.SetDeviceWokenup(true);
	}
	frame.SetTimestamp(time);
	frame.SetInterfaceId(pBidcosRemoteInterfcace->GetSerialNumber());

	updateAuthKey(bidcosCmd, frame);


}

void CCU2CommController::updateAuthKey(const CCU2CoprocessorCommand& bidcosCmd, BidcosFrame& frame)
{
	BidcosCommandEventAuthentificationStatus authStatus = bidcosCmd.getAuthentificationStatus();
	if(authStatus == BIDCOSCMD_EVENT_AUTH_STATUS_AUTH_SUCCESSFUL) {
		//LOG(Logger::LOG_DEBUG, "CCU2CommController::assembleBidcosFrame(): SUCC index: %d",bidcosCmd.getAuthenticationKeyIndex());
		frame.SetAuthKey( bidcosCmd.getAuthenticationKeyIndex() ) ;
	}
	else if(authStatus == BIDCOSCMD_EVENT_AUTH_STATUS_AUTH_KEY_INDEX_UNKNOWN) {
		frame.SetAuthKey( BidcosFrame::UNKNOWN_AUTH_KEY );
		//LOG(Logger::LOG_DEBUG, "CCU2CommController::assembleBidcosFrame(): UNKWN");
	}
	else if(authStatus == BIDCOSCMD_EVENT_AUTH_STATUS_AUTH_FAILED) {
		//LOG(Logger::LOG_DEBUG, "CCU2CommController::assembleBidcosFrame(): FLR");
		frame.SetAuthKey( BidcosFrame::INVALID_AUTH_KEY );
	}

}


bool CCU2CommController::isBidcosEventABidcosResponse(const CCU2CoprocessorCommand& eventCmd)
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


bool CCU2CommController::isBidcosEventABidcosTelegramResponse(const CCU2CoprocessorCommand& eventCmd)
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

bool CCU2CommController::sendBidcosTelegram(BidcosFrame* pMessageFrame, const int burstMode) {
	//LOG(Logger::LOG_DEBUG, "CCU2CommController::sendBidcosMessage(): Begin");
	if(pMessageFrame == NULL) {
		return false;
	}
	pthread_mutex_lock( &mutexBidcosTelegramRequest  );
	//LOG(Logger::LOG_DEBUG, "CCU2CommController::sendBidcosMessage(): BidcosTelegramMutex locked");
	//Extract data from bidcos frame
	std::string frameData;
	for(unsigned int i = 0 ; i < pMessageFrame->GetSize(); i++) {
		frameData.append( 1, pMessageFrame->GetByteData(i) );
	}

	//Create request command send it and wait for response
	CCU2CoprocessorCommand requestCmd( BIDCOSCMD_SEND_TELEGRAM, frameData);
	requestCmd.setBurstMode((BurstMode)burstMode);
	if(Logger::WouldLog(Logger::LOG_ALL)) {
		LOG(Logger::LOG_ALL,  "CCU2CommController::sendBidcosMessage(): Using burstMode=%d", burstMode);
	}
	bool done = false;
	//LOG(Logger::LOG_DEBUG,  "CCU2CommController::sendBidcosMessage(): Sending telegram with tc %d", pMessageFrame->GetTelegramCounter());
	int64_t timeout = 0;
	if(burstMode == TRIPLE_BURST) {
		timeout = tripleBurstTimeout;
	}
	else {
		timeout = defaultResponseTimeout;
	}

	//Set timestamp
	pMessageFrame->SetTimestamp( time_millis() );

	CCU2CoprocessorCommand responseCmd = internalSendBidcosRequest( requestCmd, timeout, bidcosTelegramRequestInfo, &mutexBidcosTelegramRequest, &waitConditionBidcosTelegramRequest, &predicateWaitConditionBidcosTelegramRequest, done);
	if(responseCmd.getCommandData().size() > 0) {
			//Set key index in request (assembleBidcosFrame sets it in response)
			updateAuthKey(responseCmd, *pMessageFrame);

			//Add response to request
			BidcosFrame responseFrame;
			assembleBidcosFrame( responseCmd, responseFrame);
//			pMessageFrame->CheckAndAddResponse(responseFrame);
//
//			//Check if the response is a ACK_STATUS_INFO and must be handled as event too (otherwise no event would be fired for that status change)
//			LOG(Logger::LOG_DEBUG, "CCU2CommController::sendBidcosTelegram(): Response BidCosFrame type is: %d", responseFrame.GetType());
//			const int responseFrameType = responseFrame.GetType();
//			if((responseFrame.GetType() == BidcosFrame::FT_ACK_OR_NACK) &&
//				(responseFrame.MatchType(BidcosFrame::FT_ACK_STATUS))) {
				pBidcosRemoteInterfcace->handleEvent(responseFrame);
//			}
	}

		switch(responseCmd.getBidcosCommandResponseStatus())
		{
		case BIDCOSCMD_RESPONSE_STATUS_ERROR:
			pMessageFrame->SetUnreachReason(BidcosFrame::UNREACH_NO_RESPONSE);
			break;
		case BIDCOSCMD_RESPONSE_STATUS_OK:
			pMessageFrame->SetUnreachReason(BidcosFrame::UNREACH_FALSE);
			break;
		case BIDCOSCMD_RESPONSE_STATUS_TELEGRAM_SENT:
			pMessageFrame->SetUnreachReason(BidcosFrame::UNREACH_FALSE);
			break;
		case BIDCOSCMD_RESPONSE_STATUS_TELEGRAM_SENT_RECVD_ACK:
			pMessageFrame->SetUnreachReason(BidcosFrame::UNREACH_FALSE);
			break;
		case BIDCOSCMD_RESPONSE_STATUS_SEND_FAILED:
			pMessageFrame->SetUnreachReason(BidcosFrame::UNREACH_NO_RESPONSE);
			break;
		case BIDCOSCMD_RESPONSE_STATUS_SEND_FAILED_DUTYCYLCE:
			pMessageFrame->SetUnreachReason(BidcosFrame::UNREACH_INTERFACE_DUTYCYCLE);
			break;
		case BIDCOSCMD_RESPONSE_STATUS_SEND_FAILED_CSMACA:
			pMessageFrame->SetUnreachReason(BidcosFrame::UNREACH_RF_BUSY);
			break;
		default:
			break;
		}


	//LOG(Logger::LOG_DEBUG,  "CCU2CommController::sendBidcosMessage(): Returning  %s", (done ? "true" : "false"));
	//LOG(Logger::LOG_DEBUG, "CCU2CommController::sendBidcosMessage(): End");
	pthread_mutex_unlock( &mutexBidcosTelegramRequest );
	return done;
}

bool CCU2CommController::sendBidcosTelegram(BidcosFrame* pMessageFrame) 
{
	int burstMode = (int)NO_BURST;//no burst
	if( (pMessageFrame->GetCtrl() & BidcosFrame::CTRL_BURST) != 0) {
		burstMode = (int)SINGLE_BURST;
	}
	return sendBidcosTelegram(pMessageFrame, burstMode);
}

bool CCU2CommController::sendBidcosTelegramTripleBurst(BidcosFrame *pMessageFrame) 
{
	return sendBidcosTelegram(pMessageFrame, (int)TRIPLE_BURST);
}


CCU2CoprocessorCommand CCU2CommController::internalSendBidcosRequest(CCU2CoprocessorCommand requestCmd, const int64_t timeout, RequestInfo& requestInfo, pthread_mutex_t* mutex, pthread_cond_t* waitCondition, bool* predicate, bool& done)
{
	//LOG(Logger::LOG_DEBUG, "CCU2CommController::sendBidcosRequest() (Internal): Begin");
	CCU2CoprocessorCommand responseCmd;
	done = false;
	//Create serial frame
	//LOG(Logger::LOG_DEBUG, "CCU2CommController::sendBidcosRequest() (Internal): Assembling command string");

	for(int busyRetries = 0; busyRetries <= BUSY_RETRY_AMOUNT; busyRetries++) {
		//Wait for response
		CCU2SerialFrame reqFrame;
		reqFrame.setPayload( requestCmd.getCommandString() );//(getCommandString increments sequence counter automatically)

		//Begin new request
		requestInfo.requestCmd = requestCmd;
		requestInfo.responseStatus = RESPONSE_WAIT;

		//Send
		//LOG(Logger::LOG_DEBUG, "CCU2CommController::sendBidcosRequest() (Internal): Sending data");
		std::string reqFrameData = reqFrame.getFrameData();
		unsigned int sentBytes = pPortWrapper->SendData( reqFrameData );
		if(reqFrameData.size() != sentBytes) {
			done = false;
			return responseCmd;
		}
		waitForCoProcessorResponse(requestInfo, mutex, waitCondition, predicate, timeout);

		//Handle response
		responseCmd = requestInfo.responseCmd;

		if(requestInfo.responseStatus == RESPONSE_OK) {
			if(busyRetries > 0) {
				if(Logger::WouldLog(Logger::LOG_INFO)) {
					LOG(Logger::LOG_INFO, "(%s) CCU2CommController::sendBidcosRequest(): Got response after retry number %d", interfaceSerial.c_str(),(busyRetries));
				}
			}
			done = true;
			break;
		}
		else if(requestInfo.responseStatus == RESPONSE_FAIL_BUSY) {
			usleep(BUSY_RETRY_WAIT_MICROSECONDS);
			if(Logger::WouldLog(Logger::LOG_INFO)) {
				LOG(Logger::LOG_INFO, "(%s) CCU2CommController::sendBidcosRequest(): Retrying to send request (coprocessor was busy). Retry number %d", interfaceSerial.c_str(),(busyRetries+1));
			}
			continue;
		}
		else {
			break;
		}
	}

	//Finish request
	requestInfo.responseStatus = RESPONSE_IDLE;

	//LOG(Logger::LOG_DEBUG, "CCU2CommController::sendBidcosRequest() (Internal): End");
	//go to hell
	return responseCmd;
}


bool CCU2CommController::sendBidcosRequest(const BidcosCommand bidcosCommand, const std::string& param, std::string* responseData) {
	//LOG(Logger::LOG_DEBUG, "CCU2CommController::sendBidcosRequest(): Begin");
	bool done = false;
	pthread_mutex_lock(&mutexBidcosRequest);

	CCU2CoprocessorCommand reqCommand(bidcosCommand, param);
	CCU2CoprocessorCommand responseCommand = internalSendBidcosRequest( reqCommand, coproCommandTimeout, bidcosRequestInfo, &mutexBidcosRequest, &waitConditionBidcosRequest, &predicateWaitConditionBidcosRequest, done);

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
	//LOG(Logger::LOG_DEBUG, "CCU2CommController::sendBidcosRequest(): End");
	return done;
}

void CCU2CommController::waitForCoProcessorResponse(RequestInfo& requestInfo, pthread_mutex_t* mutex, pthread_cond_t* waitCondition, bool* predicate, const int64_t timeout)
{
	//LOG(Logger::LOG_ALL, "CCU2CommController::waitForCoprocessorResponse: Timeout is %u", timeout);
	timespec ts = millis2abstime(timeout);
	int rc = 0;
	//pthread_mutex_lock(mutex);
	while( (!(*predicate)) &&  rc == 0) {
		rc = pthread_cond_timedwait( waitCondition, mutex, &ts);
	}
	*predicate = false;
	//pthread_mutex_unlock(mutex);
	if(requestInfo.responseStatus == RESPONSE_WAIT) {
		if(Logger::WouldLog(Logger::LOG_DEBUG)) {
			LOG(Logger::LOG_DEBUG, "(%s) CCU2CommController::waitForCoProcessorResponse(): Timeout while waiting for response.\n", interfaceSerial.c_str());
		}
		requestInfo.responseStatus = RESPONSE_TIMEOUT;
	}
}

void CCU2CommController::checkedLock(pthread_mutex_t* mutex)
{
	int val = pthread_mutex_lock(mutex);
	//const std::string serial(interfaceSerial);
	//int val = pthread_mutex_trylock( mutex );
	if(val != 0) {
		std::string msg("CCU2CommController::checkedLock(): ");
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
		LOG(Logger::LOG_DEBUG, msg.c_str());
		usleep(10000); //10 milliseconds * 1000 <-> 10 000 microseconds
		val = pthread_mutex_trylock( mutex );
		if(val == 0) {
			msg.append("Got the mutex now.");
		}
	}

}

CCU2CommController::RequestInfo::RequestInfo()
{
	responseStatus = RESPONSE_IDLE;
	//timeout = defaultResponseTimeout;
}

bool CCU2CommController::readFirmwareVersion(std::string& bootloaderVersion, std::string& firmwareVersion)
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
			LOG(Logger::LOG_ERROR, "(%s) CCU2CommController::readFirmwareVersion(): Wrong size of response data.", interfaceSerial.c_str());
			done = false;
		}
	}
	return done;
}

std::string CCU2CommController::readSerialNumber()
{
	std::string response;
	bool done = sendSystemCommand(SYSTEMCMD_GET_SERIALNR, "", &response);
	if(!done) {
		LOG(Logger::LOG_ERROR, "CCU2CommController::readSerialNumber(): Could not read serial number from coprocessor.");
		response.clear();
	}
	return response;
}

bool CCU2CommController::setCSMACAEnabled(const bool enabled)
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
		if(Logger::WouldLog(Logger::LOG_INFO)) {
			LOG(Logger::LOG_INFO, "(%s) CCU2CommController::setCSMACAEnabled(): CSMA/CA %s.", interfaceSerial.c_str(), (enabled ? "enabled" : "disabled"));
		}
	}
	else {
		LOG(Logger::LOG_ERROR, "(%s) CCU2CommController::setCSMACAEnabled(): Error %s CSMA/CA.", interfaceSerial.c_str(), (enabled ? "enabling" : "disabling"));
	}
	return done;
}

bool HM2::CCU2CommController::setDutyCycleCheck(const bool enable) {
#ifndef PRODUCTION
	std::string params;
	if (enable) {
		params.append(1, (char) 0x01);
	} else {
		params.append(1, (char) 0x00);
	}
	bool done = sendSystemCommand(SYSTEMCMD_DUTYCYLCE_ON_OFF, params, NULL);
	if (done) {
	/*	LOG(Logger::LOG_INFO,
				"CCU2CommController::setDutyCycleCheck(): DutyCycleCheck %s.",
				(enable ? "enabled" : "disabled"));*/
	} else {
		/*LOG(Logger::LOG_ERROR,
				"CCU2CommController::setDutyCycleCheck(): Error %s DutyCycleCheck.",
				(enable ? "enabling" : "disabling"));*/
	}
	return done;
#else
	return true;
#endif
}

bool CCU2CommController::setInterfaceClock(const unsigned int utcSeconds, const int offsetMinutes)
{
	std::string cmdParams;
	cmdParams.append(toBigEndianString(utcSeconds));//4 byte utc time in seconds
	cmdParams.append(1, (char) ((offsetMinutes / 30) & 0xFF));//1 byte offset in half-hours (+1 hour --> +2)
	//LOG(Logger::LOG_FATAL_ERROR, "REMOVE ME: %s\n", toHexStr(cmdParams).c_str());
	return sendSystemCommand( SYSTEMCMD_SET_CLOCK, cmdParams );
}
bool CCU2CommController::setDataRate100k()
{
	std::string cmdParams;
	cmdParams.append(1,(char)0xe9);
	cmdParams.append(1,(char)0xca);
	return sendSystemCommand( SYSTEMCMD_SET_100K_DATARATE,  cmdParams);
}
bool  CCU2CommController::setDataRate10k()
{
	return sendSystemCommand( SYSTEMCMD_SET_10K_DATARATE,  "");
}
bool CCU2CommController::getDutyCycle(int *dutyCycle)
{
	std::string response;
	bool done = sendSystemCommand(SYSTEMCMD_GETDUTYCYLCE,"",&response);
	if(!done)
	{
		LOG(Logger::LOG_ERROR, "(%s) CCU2CommController::getDutyCycle(): Could not get DutyCycle from coprocessor.", interfaceSerial.c_str());
		response.clear();
	}
	else if(response.size() > 0) 
	{
		*dutyCycle = (unsigned char)response[0];
		*dutyCycle = (*dutyCycle) >> 1; //Values are from 0 to 200 -> Convert to 0% to 100%
	}
	else {
		LOG(Logger::LOG_ERROR, "(%s) CCU2CommController::getDutyCycle(): Response empty", interfaceSerial.c_str());
	}
	return done;
}


bool HM2::CCU2CommController::setRFLGWInfoLED(const unsigned int state)
{
	return false;
}

void CCU2CommController::setInterfaceSerial(const std::string& ifSerial)
{
	interfaceSerial = ifSerial;
}

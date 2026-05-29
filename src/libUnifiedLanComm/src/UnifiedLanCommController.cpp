/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <UnifiedLanCommController.h>
#include <LanConnection.h>
#include <UnifiedLanProtocolTypeConverter.h>
#include <UnifiedLanProtocolMessage.h>
#include <CommonConversion.h>
#include <Logger.h>
//#include <pthread.h>
#include <utils.h>
#include <string.h>
#include <stdlib.h>

#ifndef WIN32
#  include <unistd.h>
#endif

using namespace ulc;


//----------------------------------------------------------------------------------------------

UnifiedLanCommController::UnifiedLanCommController(const std::string& hostIP, const int port)
: connectionState(CONN_STATE_NOT_CONNECTED)
,connectError((ConnectError)NOT_CONNECTED)
,connection(hostIP, port)
,encryptionEnabled(false)
,receiveThread(0)
,receiveThreadActive(false)
{
	pthread_mutexattr_t mutexAttr;
	pthread_mutexattr_init(&mutexAttr);
	pthread_mutex_init(&pRxTxMutex, &mutexAttr);
	pthread_mutex_init(&connectionMutex, &mutexAttr);
	pthread_mutexattr_destroy(&mutexAttr);


}

//----------------------------------------------------------------------------------------------

UnifiedLanCommController::~UnifiedLanCommController()
{

	//connection.removeLanConnectionListener(this);
	disconnect(false);//modifying connectError here would break CCU2WebUI LGW status (reason for connection error)
	if(receiveThread != 0) {
		pthread_cancel( (pthread_t)receiveThread );
		pthread_join( (pthread_t)receiveThread , NULL);
	}
	pthread_mutex_destroy(&pRxTxMutex);
	pthread_mutex_destroy(&connectionMutex);
}

//----------------------------------------------------------------------------------------------

bool UnifiedLanCommController::connect()
{
	connectError = (ConnectError)NOT_CONNECTED;
	//pthread_mutex_lock(&connectionMutex);
	encryptionEnabled = false;
	bool done = connection.connect();
	if(done) {
		startReceiveThread();
		//Wait for switch protocol command
		done = waitForState(CONN_STATE_READY, 2000);
		if(!done)
 		{
 			switch(connectionState) {
 				case CONN_STATE_NOT_CONNECTED:
					connectError = CONNECT_FAILED;
 					LOG(Logger::LOG_ERROR, "UnifiedLanCommController::connect(): Could not connect.");
 					break;
 				case CONN_STATE_CONNECTED:
					connectError = UNKNOWN_INITIALIZATION_ERROR;
 					LOG(Logger::LOG_ERROR, "UnifiedLanCommController::connect(): Didn't receive hello message. ... disconnecting");
 					break;
 				case CONN_STATE_RECEIVED_HELLO:
					connectError = UNKNOWN_INITIALIZATION_ERROR;
 					LOG(Logger::LOG_ERROR, "UnifiedLanCommController::connect(): Didn't receive IV or switch protocol command. ... disconnecting");
 					break;
 				case CONN_STATE_RECEIVED_IV:
					connectError = WRONG_KEY;
 					LOG(Logger::LOG_ERROR, "UnifiedLanCommController::connect(): Didn't receive switch protocol command. ... disconnecting");
 					break;
 				case CONN_STATE_RECEIVED_SWITCHING_PROTOCOL:
					connectError = UNKNOWN_INITIALIZATION_ERROR;
 					LOG(Logger::LOG_ERROR, "UnifiedLanCommController::connect(): Problem sending answer to switch protocol command ... disconnecting");
 					break;
 				default:
					connectError = UNKNOWN_INITIALIZATION_ERROR;
 					LOG(Logger::LOG_ERROR, "UnifiedLanCommController::connect(): Unknown problem.");
 					break;
 			}
			disconnect(false);
			stopReceiveThread();
			//disconnect();
			//pthread_mutex_unlock(&connectionMutex);
			return false;
		}
	}
	else {
		LOG(Logger::LOG_ERROR, "UnifiedLanCommController::connect(): Unable to connect.");
		//pthread_mutex_unlock(&connectionMutex);
		disconnect();//must be done here... otherwise the socket does not get closed until keepalive timout (2 hours)
		return false;
	}
	stopReceiveThread();
	//pthread_mutex_unlock(&connectionMutex);
	connectError = NO_CONNECT_ERROR;
	return true;
}

//----------------------------------------------------------------------------------------------

void UnifiedLanCommController::disconnect(const bool setMemberConnectError) 
{
	//pthread_mutex_lock(&connectionMutex);
	//LOG(Logger::LOG_ALL, "UnifiedLanCommController::disconnect()");
	connection.disconnect();
	connectionState = CONN_STATE_NOT_CONNECTED;
	if(setMemberConnectError) {
		connectError = NOT_CONNECTED;
	}
	//LOG(Logger::LOG_ALL, "UnifiedLanCommController::disconnected()");
	//pthread_mutex_unlock(&connectionMutex);
}

//----------------------------------------------------------------------------------------------

void UnifiedLanCommController::disconnect()
{
	disconnect(true);
}


//----------------------------------------------------------------------------------------------

bool UnifiedLanCommController::waitForState(const ConnectionState& connState, const unsigned int timeoutMillis)
{
	unsigned int timeoutCounter = 0;
	while((connectionState != connState) && timeoutCounter < 10) {
		usleep(timeoutMillis*100); //100 ms , 10 times -> timeout 2 second
		timeoutCounter++;
	}
	return (timeoutCounter != 10);
}

//----------------------------------------------------------------------------------------------

void UnifiedLanCommController::startReceiveThread()
{
	//Initialize receive thread attributes
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 512*1024);

	//Start thread
	receiveThreadActive = true;
	pthread_create(&receiveThread, &attr, UnifiedLanCommController::receiveThreadFunction, this);
	//Destroy attributes
	pthread_attr_destroy(&attr);
}

//----------------------------------------------------------------------------------------------

void UnifiedLanCommController::stopReceiveThread()
{

	receiveThreadActive = false;
	void* foo;
	if(receiveThread != 0) {
		//pthread_cancel( (pthread_t)receiveThread );
		pthread_join( (pthread_t)receiveThread, &foo );
		receiveThread = 0;
	}
}

//----------------------------------------------------------------------------------------------

void* UnifiedLanCommController::receiveThreadFunction(void* params)
{
	UnifiedLanCommController* pUnifiedLanCommController = (UnifiedLanCommController*)params;
	std::string buffer;
	std::string leftover;
	bool done = false;
	UnifiedLanProtocolMessage msg;

	while(pUnifiedLanCommController->receiveThreadActive) {
		buffer.clear();
		if(leftover.size() > 0) {
			buffer = leftover;
			leftover.clear();
		}
		else {
			int n = pUnifiedLanCommController->connection.waitForData(100);
			if(n == 0) {
				continue;
			}
			else if(n < 0) {
				break;
			}
			done = pUnifiedLanCommController->connection.receive(buffer);
			if(!done) {
				break;
			}
			pthread_mutex_lock((pthread_mutex_t*)&(pUnifiedLanCommController->pRxTxMutex));
			if(pUnifiedLanCommController->encryptionEnabled) {
				pUnifiedLanCommController->encryption.decrypt(buffer);
			}
			pthread_mutex_unlock((pthread_mutex_t*)&(pUnifiedLanCommController->pRxTxMutex));
		}
		bool msgComplete = msg.appendDataFromDevice(buffer, leftover);
		if(msgComplete) {
			pUnifiedLanCommController->handleIncomingMessage(msg);
			msg.clear();
		}

	}
	return NULL;
}

//----------------------------------------------------------------------------------------------

void UnifiedLanCommController::handleIncomingMessage(const UnifiedLanProtocolMessage& msg)
{
	switch(msg.getMessageCommand()) {
		case ULP_CMD_HELLO:
			handleHelloMessage(msg);
			break;
		case ULP_CMD_SET_INIT_VECTOR:
			handleSetInitVectorMessage(msg);
			break;
		case ULP_CMD_SWITCH_PROTOCOL:
			handleSwitchProtocolMessage(msg);
			break;
		default:
			LOG(Logger::LOG_ERROR, "UnifiedLanCommController::handleIncomingMessage(): Handling of message type %c not implemented.",(char)msg.getMessageCommand());
			break;
	}
}

//----------------------------------------------------------------------------------------------

void UnifiedLanCommController::handleHelloMessage(const ulc::UnifiedLanProtocolMessage &msg)
{
	if(msg.getMessageParameterCount() >= 4) {
		std::string text("Protocol-Version: ");
		text.append(toString(hexStringToUChar(msg.getParameterAt(0))));
		text.append("\n");
		text.append("Product-ID: ");
		text.append(msg.getParameterAt(1));
		text.append("\n");
		text.append("Firmware-Version: ");
		text.append(msg.getParameterAt(2));
		text.append("\n");
		text.append("Serial Number: ");
		lgwSerial = msg.getParameterAt(3);
		text.append(lgwSerial);
		text.append("\n");
		LOG(Logger::LOG_INFO, "Lan Device Information:\n%s",text.c_str());
	}
	if( (desiredSerial.compare(lgwSerial) == 0) || (desiredSerial.empty()) ) {
		connectionState = CONN_STATE_RECEIVED_HELLO;
	}
}

//----------------------------------------------------------------------------------------------

void UnifiedLanCommController::handleSetInitVectorMessage(const UnifiedLanProtocolMessage& msg)
{

	if(connectionState != CONN_STATE_RECEIVED_HELLO) {
		return;//We did not receive a hello or given serial did not match desired serial
	}

	//TESTING
//encKey = "BF27AEB3F36C19C1548C4F1025004781";
//TESTING

	//std::string encKeyHex = hexStringToString(encKey);
	if(encKey.empty()) {
		LOG(Logger::LOG_FATAL_ERROR, "UnifiedLanCommController: Gateway requested encryption but no key was given.");
	}
	else {
		if(msg.getMessageParameterCount() == 1) {
			//Get initialization vector from lgw message
			std::string ivLGW = msg.getParameterAt(0);
			ivLGW = hexStringToString(ivLGW);
			//create our vector
			std::string iv = createIV();
			//initialize TCPEncrypt
			encryption.init(encKey, ivLGW, iv);
			//Send our iv to LGW
			UnifiedLanProtocolMessage setIVMessage(ULP_CMD_SET_INIT_VECTOR, (msg.getMessageCounter()+1));
			setIVMessage.addParam( toHexString(iv) );
			pthread_mutex_lock((pthread_mutex_t*)&pRxTxMutex);
			bool done = connection.send(setIVMessage.getMessageStringToSend());
			encryptionEnabled = true;
			if(done) {
			//Change state -> ready
				connectionState = CONN_STATE_RECEIVED_IV;
			}
			else {
				LOG(Logger::LOG_ERROR, "UnifiedLanCommController::handleSetInitVectorMessage(): Error sending initialization vector.");
			}
			pthread_mutex_unlock((pthread_mutex_t*)&pRxTxMutex);
		}
		else {
			LOG(Logger::LOG_ERROR, "UnifiedLanCommController::handleSetInitVectorMessage(): Wrong parameter count.");
		}
	}
}

//----------------------------------------------------------------------------------------------

void UnifiedLanCommController::handleSwitchProtocolMessage(const UnifiedLanProtocolMessage& msg)
{
//	LOG(Logger::LOG_ALL, "UnifiedLanCommController::handleSwitchProtocolMessage()");
	if( (connectionState != CONN_STATE_RECEIVED_HELLO) && connectionState != CONN_STATE_RECEIVED_IV) {
		return;//This can happen if given serial and desired serial does not match in handleHelloMessage
	}
	//Send answer
	UnifiedLanProtocolMessage answerMsg(ULP_CMD_ANSWER, msg.getMessageCounter());
	answerMsg.addParam(std::string("0000"));
	std::string outMsg = answerMsg.getMessageStringToSend();
	if(encryptionEnabled) {
		encryption.encrypt(outMsg);
	}
	bool done = connection.send(outMsg);
	if(done) {
	//Change state -> ready
		connectionState = CONN_STATE_READY;
		//LOG(Logger::LOG_ALL, "UnifiedLanCommController::handleSwitchProtocolMessage(): CONN_STATE_READY");
	}
	else {
		connectionState = CONN_STATE_RECEIVED_SWITCHING_PROTOCOL;
		//LOG(Logger::LOG_ALL, "UnifiedLanCommController::handleSwitchProtocolMessage(): CONN_STATE_RECEIVED_SWITCHING_PROTOCOL");
	}
}

//----------------------------------------------------------------------------------------------

bool ulc::UnifiedLanCommController::send(const std::string& data)
{
//	LOG(Logger::LOG_DEBUG, "UnifiedLanCommController::send() Lock");
	bool done = false;
	pthread_mutex_lock(&connectionMutex);
	if(connectionState == CONN_STATE_READY ) {
		done = connection.send(data);
		if(!done) {
//			pthread_mutex_lock(&connectionMutex);
			connectionState = CONN_STATE_NOT_CONNECTED;

		}
	}
	pthread_mutex_unlock(&connectionMutex);
//	LOG(Logger::LOG_DEBUG, "UnifiedLanCommController::send() Unlock");
	return done;
}

//----------------------------------------------------------------------------------------------

bool ulc::UnifiedLanCommController::receive(std::string& data)
{

	int w = connection.waitForData(5000);
	bool done = true;
	if(w <= 0) {
		if(w < 0) {
//			LOG(Logger::LOG_ERROR, "UnifiedLanCommController::receive(): waitForData returned: %d", w);
			done = false;
		}
/*		else {
			LOG(Logger::LOG_ALL, "UnifiedLanCommController::receive(): waitForData - timeout");
		}
*/
	}
	else {
		done = connection.receive(data);
	}
	if(!done) {
		pthread_mutex_lock(&connectionMutex);
		connectionState = CONN_STATE_NOT_CONNECTED;
		pthread_mutex_unlock(&connectionMutex);
		done = false;
	}
	return done;
}

bool ulc::UnifiedLanCommController::receiveNonBlocking(std::string& data)
{
//LOG(Logger::LOG_ALL, "UnifiedLanCommController::receiveNonBlocking()");
	bool done = connection.receiveNonBlocking(data);//MSG_DONTWAIT
	return done;
}

//----------------------------------------------------------------------------------------------

bool ulc::UnifiedLanCommController::isConnected()
{
	return connectionState == CONN_STATE_READY;
}

//----------------------------------------------------------------------------------------------

std::string ulc::UnifiedLanCommController::getSerial()
{
	return lgwSerial;
}

//----------------------------------------------------------------------------------------------

std::string ulc::UnifiedLanCommController::createIV()
{
	std::string s;
	//generate pseudo random number
	time_t foo = time(NULL);
	foo = 3 * foo;
	foo = foo / 2;
	srand(foo);
	for(int i = 0; i < 4; i++) {
		int nr = 11 * i + rand();
		nr += 7 * rand();
		char* foo = (char*)&nr;
		for(int k = 0; k < 4; k++) {
			s.append(1, *(foo+k));
		}
	}
	return s;
}

//----------------------------------------------------------------------------------------------

void ulc::UnifiedLanCommController::setEncryptionKey(const std::string key)
{
	encKey = key;
}

//----------------------------------------------------------------------------------------------

bool UnifiedLanCommController::isEncryptionEnabled() const
{
	return encryptionEnabled;
}

//----------------------------------------------------------------------------------------------

ldu::TCPEncryption* UnifiedLanCommController::getTCPEncryption()
{
	return &encryption;
}

//----------------------------------------------------------------------------------------------

UnifiedLanCommController::ConnectError UnifiedLanCommController::getConnectError() const {
	return connectError;
}

//----------------------------------------------------------------------------------------------

std::string UnifiedLanCommController::getConnectErrorAsString() const {
	std::string msg;
	switch(connectError) {
		case NOT_CONNECTED:
			msg = "NOT_CONNECTED";
			break;
		case NO_CONNECT_ERROR:
			msg = "NO_ERROR";
			break;
		case CONNECT_FAILED:
			msg = "CONNECT_FAILED";
			break;
		case WRONG_KEY:
			msg = "WRONG_KEY";
			break;
		case UNKNOWN_INITIALIZATION_ERROR:
		default:
			msg = "UNKNOWN_INITIALIZATION_ERROR";
			break;
	}
	return msg;
}

//----------------------------------------------------------------------------------------------

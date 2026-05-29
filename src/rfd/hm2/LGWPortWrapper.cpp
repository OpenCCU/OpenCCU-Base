/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <LGWPortWrapper.h>
#include <UnifiedLanComm.h>
#include <LanDeviceUtils.h>
#include <CCU2LGWCommController.h>
#include <CCU2CoprocessorCommand.h>
#include <CCU2SerialFrame.h>
#include <utils.h>

#ifndef WIN32
#  include <unistd.h>
#endif

# include <Logger.h>

//#define DUMP 1

#ifdef DUMP
#include <HM2Utils.h>
#endif
#include <HM2Utils.h>

#include <errno.h>

#ifndef WIN32
	#include <fstream>
#endif

using namespace HM2;
using namespace ulc;

LGWPortWrapper::LGWPortWrapper(CCU2LGWCommController* pCCU2CommController)
: CCU2PortWrapper()
, pCommController(NULL)
, encryptionEnabled(false)
, pEncryption(NULL)
, port(0)
, keepAliveThread(0)
, keepAliveThreadActive(true)
, bidcosChannelKeepAliveThreadActive(true)
, infoLEDState(LED_OFF)
, timestampLastBidcosCommunication(0)
, bidcosChannelKeepAliveThread(0)
, reconnectPending(false)
, blockRXTX(true)
, hostIPWasAssignedByUser(false)
, pCCU2CommController(pCCU2CommController)
{
	pthread_mutex_init(&mutexULCCommController, NULL);
	pthread_mutex_init(&mutexReconnect, NULL);
	pthread_mutex_init(&mutexBlockRXTX, NULL);
	timestampLastBidcosCommunication = time_millis();
}

LGWPortWrapper::~LGWPortWrapper()
{

	stopKeepAliveThread();
	stopBidcosChannelKeepAliveThread();
	encryptionEnabled = false;//important to do that, because pointer on encryption in pCommController will become bad when we delete pCommController
	pthread_mutex_lock(&mutexULCCommController);
	if(pCommController != NULL) {
		delete pCommController;
		pCommController = NULL;
	}
	pthread_mutex_unlock(&mutexULCCommController);
	pthread_mutex_destroy(&mutexULCCommController);
	pthread_mutex_destroy(&mutexReconnect);
	pthread_mutex_destroy(&mutexBlockRXTX);
	//LOG(Logger::LOG_DEBUG, "~LGWPortWrapper()");
}

int LGWPortWrapper::ReadData(std::string* data)
{
//	LOG(Logger::LOG_ALL, "LGWPortWrapper::ReadData()");
	if(data != NULL) {
		pthread_mutex_lock(&mutexBlockRXTX);
		if(blockRXTX) {
			//LOG(Logger::LOG_ALL, "LGWPortWrapper::ReadData() - Blocked RXTX");
			pthread_mutex_unlock(&mutexBlockRXTX);
			usleep(25000);
			return -1;
		}
		pthread_mutex_unlock(&mutexBlockRXTX);
		bool done = pCommController->receive( *data );
		if(done) {
#ifdef DUMP
			LOG(Logger::LOG_ALL, "LGWPortWrapper::ReadData(): Received %s", toDebugHexStr(*data).c_str());
#endif
			if(encryptionEnabled && pEncryption != NULL) {
				pEncryption->decrypt(*data);
			}
			timestampLastBidcosCommunication = time_millis();
		}
		else {
			LOG(Logger::LOG_ERROR, "LGWPortWrapper::ReadData(): Receive error");
			asyncReconnect();
		}
		return (done ? data->size() : -1);
	}
	else {
		return 0;
	}
}

int LGWPortWrapper::SendData(const std::string& data)
{
//LOG(Logger::LOG_ALL, "LGWPortWrapper::SendData()");
#ifdef DUMP
		LOG(Logger::LOG_ALL, "LGWPortWrapper::SendData(): Sending %s", toDebugHexStr(data).c_str());
#endif
	pthread_mutex_lock(&mutexBlockRXTX);
	if(blockRXTX) {
		pthread_mutex_unlock(&mutexBlockRXTX);
		return 0;
	}
	pthread_mutex_unlock(&mutexBlockRXTX);
	bool done = false;
	if(encryptionEnabled && pEncryption != NULL) {
		std::string foo(data);
		pEncryption->encrypt(foo);
		done = pCommController->send( foo );
	}
	else {
		done = pCommController->send( data );
	}
	if(!done) {
		LOG(Logger::LOG_ERROR, "LGWPortWrapper::SendData(): Send error");
		asyncReconnect();
		//reconnect(); //we don't send the message, it is timed out until we reconnect...
	}
	else {
		
		timestampLastBidcosCommunication = time_millis(); 
	}
	return (done ? data.size() : 0);
}

int LGWPortWrapper::WaitForData(int msTime)
{
	pthread_mutex_lock(&mutexBlockRXTX);
	if(blockRXTX) {
		pthread_mutex_unlock(&mutexBlockRXTX);
		sleep(2);
		return 0;
	}
	pthread_mutex_unlock(&mutexBlockRXTX);

	pthread_mutex_lock(&mutexULCCommController);
	int retVal = 0;
	if(pCommController != NULL && pCommController->isConnected()) {
		retVal = 1;
	}
	else {
		sleep(2);
		retVal = 0;
	}
	pthread_mutex_unlock(&mutexULCCommController);
	return retVal;
}

bool LGWPortWrapper::connect(const std::string& hostIP, const unsigned int port, const std::string& encKey, const std::string& desiredSerial)
{
//	LOG(Logger::LOG_ALL, "LGWPortWrapper::connect()");
	pthread_mutex_lock(&mutexULCCommController);
	if(pCommController == NULL) {
		pCommController = new UnifiedLanCommController(hostIP, port);
		pCommController->setEncryptionKey(encKey);
	}
	else if((hostIP.compare(this->hostIP) != 0) || (port != this->port))
	{
		delete pCommController;
		pCommController = NULL;
		pCommController = new UnifiedLanCommController(hostIP, port);
		pCommController->setEncryptionKey(encKey);
	}
	pthread_mutex_unlock(&mutexULCCommController);
	this->hostIP = hostIP;
	this->port = port;
	this->encKey = encKey;

	bool connected = pCommController->connect();
	if(connected) {
		this->serial = pCommController->getSerial();
		pEncryption = pCommController->getTCPEncryption();
		if(pEncryption == NULL) {
			LOG(Logger::LOG_FATAL_ERROR, "LGWPortWrapper::connect(): Encryption pointer is NULL.");
		}
		else {
			encryptionEnabled = pCommController->isEncryptionEnabled();
		}
		startKeepAliveThread();
		startBidcosChannelKeepAliveThread();
	}
	writeLGWStatusToFile(getSerial(), pCommController->getConnectErrorAsString());
	pthread_mutex_lock(&mutexBlockRXTX);
	blockRXTX = false;
	pthread_mutex_unlock(&mutexBlockRXTX);
	return connected;
}

void LGWPortWrapper::Disconnect()
{
	if(pCommController != NULL) {
		pCommController->disconnect();
		writeLGWStatusToFile(getSerial(), pCommController->getConnectErrorAsString());
	}
}


void LGWPortWrapper::asyncReconnect() {
	LOG(Logger::LOG_DEBUG, "asyncReconnect");
	pthread_mutex_lock(&mutexReconnect);
	if(reconnectPending) {
		pthread_mutex_unlock(&mutexReconnect);
		LOG(Logger::LOG_DEBUG, "LGWPortWrapper::asyncReconnect(): Reconnect already in progress.");
		usleep(250000);
		return;
	}
	reconnectPending = true;
	pthread_mutex_unlock(&mutexReconnect);

	LOG(Logger::LOG_DEBUG, "asyncReconnect -> reconnect");
	pthread_t arecThread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr, 128*1024);
	pthread_create(&arecThread, &attr, reconnectThreadFunction, this);	
	pthread_attr_destroy(&attr);

}

void* LGWPortWrapper::reconnectThreadFunction(void* param) {
	LGWPortWrapper* pThis = (LGWPortWrapper*)param;
	pThis->reconnect();
	return NULL;
}

void LGWPortWrapper::reconnect()
{
	LOG(Logger::LOG_ALL, "LGWPortWrapper::reconnect()");
	pthread_mutex_lock(&mutexReconnect);
	reconnectPending = true;
	pthread_mutex_unlock(&mutexReconnect);

	pthread_mutex_lock(&mutexBlockRXTX);
	blockRXTX = true;
	pthread_mutex_unlock(&mutexBlockRXTX);

	LOG(Logger::LOG_ALL, "LGWPortWrapper::reconnect(): Perform disconnect.");
	pCommController->disconnect();
	LOG(Logger::LOG_ALL, "LGWPortWrapper::reconnect(): Disconnect performed.");
	writeLGWStatusToFile(getSerial(), pCommController->getConnectErrorAsString());
	//Stop old keepalive
	stopBidcosChannelKeepAliveThread();
	stopKeepAliveThread();

	bool reconnected = false;
	const unsigned int basetimeout = 5; //5 seconds
	unsigned int timeout = basetimeout;

	do {
		LDU::LanDeviceUtils ldUtils;
		LDU::LanDevice lanDev;
		bool foundDev = false;
		if(hostIPWasAssignedByUser) {
			LOG(Logger::LOG_ALL, "LGWPortWrapper::reconnect(): User assigned a host IP. Skipping search of device.");
			foundDev = true;
		}
		else {
			LOG(Logger::LOG_ALL, "LGWPortWrapper::reconnect(): Searching device.");
			foundDev = ldUtils.searchDeviceByTypeAndSerial("eQ3-HM-LGW*",getSerial(), lanDev);
		}

		if(foundDev) {
			if(!hostIPWasAssignedByUser) {
				LOG(Logger::LOG_DEBUG, "LGWPortWrapper::reconnect(): Found device with serial %s.", getSerial().c_str());
				foundDev = ldUtils.readRuntimeNetworkConfiguration(lanDev);
			}
			if(foundDev) {
				timeout = basetimeout;
				LOG(Logger::LOG_DEBUG, "LGWPortWrapper::reconnect(): Trying to reconnect.");
				UnifiedLanCommController* oldController = pCommController;
				if(!hostIPWasAssignedByUser) {
					hostIP = lanDev.getRuntimeIPConfiguration().getIPAddress();
				}
				pthread_mutex_lock(&mutexULCCommController);
				pCommController = new UnifiedLanCommController(hostIP, port);//if this fails, ulcController does not have serial!
				pCommController->setEncryptionKey(this->encKey);
				pthread_mutex_unlock(&mutexULCCommController);
				LOG(Logger::LOG_DEBUG, "LGWPortWrapper::reconnect(): Perform connect.");
				reconnected = pCommController->connect();
				if(reconnected) {
					pEncryption = pCommController->getTCPEncryption();
					if(pEncryption == NULL) {
						LOG(Logger::LOG_FATAL_ERROR, "LGWPortWrapper::connect(): Encryption pointer is NULL.");
					}
					else {
						encryptionEnabled = pCommController->isEncryptionEnabled();
					}
				}
				delete oldController;
				writeLGWStatusToFile(getSerial(), pCommController->getConnectErrorAsString());
			}
			if(reconnected) {
				if(pCCU2CommController != NULL) {
					pthread_mutex_lock(&mutexBlockRXTX);
					blockRXTX = false;
					pthread_mutex_unlock(&mutexBlockRXTX);
					LOG(Logger::LOG_DEBUG, "LGWPortWrapper::reconnect(): Initialize coprocessor.");
					const bool reinitialized = pCCU2CommController->reinitCoprocessor();
					if(reinitialized) {
						LOG(Logger::LOG_DEBUG, "LGWPortWrapper::reconnect(): Initializing keepalives.");
						startKeepAliveThread();
						startBidcosChannelKeepAliveThread();
						LOG(Logger::LOG_DEBUG, "LGWPortWrapper::connect(): Reconnected.");
						break;
					}
					else {
						LOG(Logger::LOG_DEBUG, "LGWPortWrapper::connect(): Coprocessor reinitialization failed.");
						reconnected = false;
						pCommController->disconnect();
						continue;
					}
				}
			}
		}
		else {
			LOG(Logger::LOG_DEBUG, "LGWPortWrapper::reconnect(): Unable to find device with serial %s.", getSerial().c_str());
			writeLGWStatusToFile(getSerial(), std::string("Gateway not found."));
		}
		LOG(Logger::LOG_DEBUG, "LGWPortWrapper::reconnect(): Device not found retrying in %u seconds.", timeout);
		sleep(timeout);
		if(timeout <= 61) {//60 seconds, this is tcp_fin_timeout on linux
			timeout = timeout + basetimeout;
		}
	} while(!reconnected);
	pthread_mutex_lock(&mutexReconnect);
	reconnectPending = false;
	pthread_mutex_unlock(&mutexReconnect);
	LOG(Logger::LOG_DEBUG, "LGWPortWrapper::reconnect(): Reconnect finished.");
}

bool LGWPortWrapper::IsConnected()
{
	if(pCommController != NULL)
	{
		return pCommController->isConnected();
	}
	return false;
}

std::string LGWPortWrapper::getSerial()
{
	if(this->serial.empty()) {
		if(pCommController != NULL) {
			return pCommController->getSerial();
		}
		else {
			return std::string("");
		}
	}
	else {
		return serial;
	}
}


void LGWPortWrapper::startKeepAliveThread()
{
	stopKeepAliveThread();
	//Start keep alive thread
	keepAliveThreadActive = true;
	//Initialize receive thread attributes
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 512*1024);

	//Start thread
	pthread_create(&keepAliveThread, &attr, LGWPortWrapper::keepAliveThreadFunction, this);
	//Destroy attributes
	pthread_attr_destroy(&attr);



}

void LGWPortWrapper::startBidcosChannelKeepAliveThread() {
	//Keep alive thread to avoid inactivity timeout for bidcos channel (port 2000)
	bidcosChannelKeepAliveThreadActive = true;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 128*1024);
	//Start thread
	pthread_create(&bidcosChannelKeepAliveThread, &attr, LGWPortWrapper::bidcosChannelKeepAliveThreadFunction, this);
	pthread_attr_destroy(&attr);
}

void LGWPortWrapper::stopKeepAliveThread() {
	keepAliveThreadActive = false;
	if(keepAliveThread != 0) {//kill old one
		pthread_join(keepAliveThread, NULL);
		keepAliveThread = 0;
	}
}

void LGWPortWrapper::stopBidcosChannelKeepAliveThread() {
	bidcosChannelKeepAliveThread = false;
	if(bidcosChannelKeepAliveThread != 0) {
		pthread_join(bidcosChannelKeepAliveThread, NULL);
		bidcosChannelKeepAliveThread = 0;
	}
}

void* LGWPortWrapper::keepAliveThreadFunction(void* param)
{
	InfoLEDState currentInfoLEDState = LED_UNDEFINED;
	LGWPortWrapper* pThis = (LGWPortWrapper*)param;
	//TCPIPConnection keepAliveConnection(pThis->hostIP, 2001);
	UnifiedLanCommController keepAliveConnection(pThis->hostIP, 2001);
	keepAliveConnection.setEncryptionKey(pThis->encKey);
	bool done = keepAliveConnection.connect();
	if(done) {
		ldu::TCPEncryption* pKeepAliveEncryption = keepAliveConnection.getTCPEncryption();
		bool keepAliveEncryptionEnabled = keepAliveConnection.isEncryptionEnabled();
		//Prepare msg to send
		unsigned char counter = 0;
		//Send messages
		while(pThis->keepAliveThreadActive) {
			for(int i = 0; i < 10 && pThis->keepAliveThreadActive ; i++) {
				usleep(1000);
			}
			std::string counterHexStr;
			//Set INFO LED
			if(currentInfoLEDState != pThis->infoLEDState) {//check info LED status and conditionally update it.
				counterHexStr = toHexString(std::string(1, counter));
				std::string msgData(1, 'L');
				msgData.append(counterHexStr);
				msgData.append(",02,");//Info LED
				switch(pThis->infoLEDState) {
					case LED_UNDEFINED:
						//break; <- Don't break here, handle LED_UNDEFINED like LED_OFF
					case LED_OFF:
						LOG(Logger::LOG_DEBUG, "Switching RF-LGW LED OFF");
						msgData.append("00FF,00");
						break;
					case LED_ON:
						LOG(Logger::LOG_DEBUG, "Switching RF-LGW LED ON");
						msgData.append("FF00,00");
						break;
					case LED_BLINK_SLOW:
						LOG(Logger::LOG_DEBUG, "Switching RF-LGW LED ON (BLINK SLOW)");
						msgData.append("0505,00");
						break;
					case LED_BLINK_FAST:
						LOG(Logger::LOG_DEBUG, "Switching RF-LGW LED ON (BLINK FAST)");
						msgData.append("0101,00");
						break;
				}
				msgData.append(1, 0x0D);
				msgData.append(1, 0x0A);
				if(keepAliveEncryptionEnabled) {
					pKeepAliveEncryption->encrypt(msgData);
				}
				done = keepAliveConnection.send(msgData);
				if(done) {
					currentInfoLEDState = pThis->infoLEDState;
					std::string reply;
					for(int i = 0; (i < 2) && (reply.empty()); i++) {
						usleep(1000000); //500 ms
						keepAliveConnection.receiveNonBlocking(reply);
						if(reply.empty()) {
							continue;
						}
						if(keepAliveEncryptionEnabled) {
							pKeepAliveEncryption->decrypt(reply);
						}
						//LOG(Logger::LOG_ALL, "TEST: Set info led reply: %s", reply.c_str());
					}
				}
				else {
					keepAliveConnection.disconnect();
					pThis->asyncReconnect();
					break;
				}
				counter++;
			}
			//Send Keepalive message
			counterHexStr = toHexString(std::string(1,counter));
			std::string msgData(1, 'K');
			msgData.append(counterHexStr);
			msgData.append(1, 0x0D);
			msgData.append(1, 0x0A);
			counter++;
			if(keepAliveEncryptionEnabled) {
				pKeepAliveEncryption->encrypt(msgData);
			}
			done = keepAliveConnection.send(msgData);
			if(!done) {
				LOG(Logger::LOG_ERROR, "LGWPortWrapper::keepAliveThreadFunction(): Error sending keepalive.");
				keepAliveConnection.disconnect();
				pThis->asyncReconnect();
				break;
			}
			//wait for reply
			int counter = 0;
			bool timedout = true;
			while(counter <= 3) {
				usleep(1000000);
				std::string reply;
				keepAliveConnection.receiveNonBlocking(reply);

				if(!reply.empty()) {
					if(keepAliveEncryptionEnabled) {
						pKeepAliveEncryption->decrypt(reply);
					}
					//LOG(Logger::LOG_ALL, "reply is: %s", reply.c_str());
					std::string desiredReply(">K");
					desiredReply.append(counterHexStr);
					desiredReply.append(1, 0x0D);
					desiredReply.append(1, 0x0A);
					//LOG(Logger::LOG_ALL, "Desired reply: %s",desiredReply.c_str());
					if(desiredReply.compare(reply) == 0) {
						timedout = false;
						break;
					}
				}
				counter++;
			}
			if(timedout) {
				LOG(Logger::LOG_ERROR, "LGWPortWrapper::keepAliveThreadFunction(): Did not receive reply on keepalive.");
				keepAliveConnection.disconnect();
				pThis->asyncReconnect();
				break;
			}
		}
	}
	else {
		LOG(Logger::LOG_ERROR, "LGWPortWrapper::keepAliveThreadFunction(): Cannot connect.");
		pThis->asyncReconnect();
		return NULL;
	}
	//LOG(Logger::LOG_ALL, "LGWPortWrapper::keepAliveThreadFunction(): Going to die");
	keepAliveConnection.disconnect();
	return NULL;
}


void* LGWPortWrapper::bidcosChannelKeepAliveThreadFunction(void* param)
{
	LGWPortWrapper* pThis = (LGWPortWrapper*)param;
	while( pThis->bidcosChannelKeepAliveThreadActive ) {
		//sleep a little...
		for(int i = 0; (i < 16) && pThis->bidcosChannelKeepAliveThreadActive ; i++) {
			usleep(1000000);//1s ( * 16 == 16s)
		}
		const uint64_t timeMillis = time_millis();
		if(timeMillis > (pThis->timestampLastBidcosCommunication + 30000) ) {
			//No communication for at leaset 30 seconds -> Check connection
			LOG(Logger::LOG_DEBUG, "LGWPortWrapper::bidcosChannelKeepAliveThreadFunction(): Check connection because of inactivity.");
			int dc;
			bool succ = pThis->pCCU2CommController->getDutyCycle(&dc);
			if( !succ ) {
				break; //stop this thread, send failure will initiate reconnect!
			}
		}
	}
	return NULL;
}


void HM2::LGWPortWrapper::setInfoLEDState(const InfoLEDState& state)
{
	if( (state >= 0 && state <= 4) || (state == 255)) {
		this->infoLEDState = state;
	}
}

void LGWPortWrapper::writeLGWStatusToFile(const std::string& serial, const std::string& statusText)
{
#ifndef WIN32
	std::string filepath("/var/status/");
	filepath.append(serial);
	filepath.append(".connstat");
	std::ofstream os(filepath.c_str(), std::ofstream::out);
	os << statusText;
	os.close();
#endif
}

void LGWPortWrapper::setHostIPAssignedByUser(bool assignedByUser)
{
	hostIPWasAssignedByUser = assignedByUser;
}


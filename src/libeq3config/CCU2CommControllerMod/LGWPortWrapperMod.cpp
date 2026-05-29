/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <LGWPortWrapperMod.h>
#include <UnifiedLanComm.h>
#include <LanDeviceUtils.h>
#include <CCU2CommControllerMod.h>
#include <CCU2CoprocessorCommandMod.h>
#include <CCU2SerialFrameMod.h>
#include <utils.h>

#ifndef WIN32
#  include <unistd.h>
#endif

//# include <Logger.h>

//#define DUMP 1

#ifdef DUMP
#include <HM2UtilsMod.h>
#endif
#include <HM2UtilsMod.h>

#include <errno.h>

#ifndef WIN32
	#include <fstream>
#endif

using namespace HM2Mod;
using namespace ulc;

LGWPortWrapperMod::LGWPortWrapperMod(CCU2CommControllerMod* pCCU2CommController)
: CCU2PortWrapperMod()
, pCommController(NULL)
, encryptionEnabled(false)
, pEncryption(NULL)
, port(0)
, keepAliveThread(0)
, keepAliveThreadActive(true)
, pCCU2CommController(pCCU2CommController)
, infoLEDState(LED_OFF)
, timestampLastSentMessage(0)
, bidcosChannelKeepAliveThread(0)
, forceReconnect(false)
, reconnectPending(false)
{
	pthread_mutex_init(&mutexULCCommController, NULL);
	pthread_mutex_init(&mutexReconnect, NULL);
	timestampLastSentMessage = time_millis();
}

LGWPortWrapperMod::~LGWPortWrapperMod()
{
	keepAliveThreadActive = false;
	if(keepAliveThread != 0) {
		pthread_join(keepAliveThread, NULL);
	}
	if(bidcosChannelKeepAliveThread != 0) {
		pthread_join(bidcosChannelKeepAliveThread, NULL);
	}
	encryptionEnabled = false;//important to do that, because pointer on encryption in pCommController will become bad when we delete pCommController
	pthread_mutex_lock(&mutexULCCommController);
	if(pCommController != NULL) {
		delete pCommController;
		pCommController = NULL;
	}
	pthread_mutex_unlock(&mutexULCCommController);
	pthread_mutex_destroy(&mutexULCCommController);
	pthread_mutex_destroy(&mutexReconnect);
}

int LGWPortWrapperMod::ReadData(std::string* data)
{
//	//LOG(Logger::LOG_ALL, "LGWPortWrapper::ReadData()");
	if(data != NULL) {
		pthread_mutex_lock(&mutexReconnect);
		if(reconnectPending) {
			pthread_mutex_unlock(&mutexReconnect);
			return 0;
		}
		pthread_mutex_unlock(&mutexReconnect);
		bool done = pCommController->receive( *data );
		if(done) {
#ifdef DUMP
			//LOG(Logger::LOG_ALL, "LGWPortWrapper::ReadData(): Received %s", toDebugHexStr(*data).c_str());
#endif
			if(encryptionEnabled && pEncryption != NULL) {
				pEncryption->decrypt(*data);
			}
		}
		/*else {
			//LOG(Logger::LOG_ERROR, "LGWPortWrapper::ReadData(): Receive error");
		}*/
		return (done ? data->size() : 0);
	}
	else {
		return 0;
	}
}

int LGWPortWrapperMod::SendData(const std::string& data)
{
////LOG(Logger::LOG_ALL, "LGWPortWrapper::SendData()");
#ifdef DUMP
		//LOG(Logger::LOG_ALL, "LGWPortWrapper::SendData(): Sending %s", toDebugHexStr(data).c_str());
#endif
	pthread_mutex_lock(&mutexReconnect);
	if(reconnectPending) {
		pthread_mutex_unlock(&mutexReconnect);
		return 0;
	}
	pthread_mutex_unlock(&mutexReconnect);
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
		//LOG(Logger::LOG_ERROR, "LGWPortWrapper::SendData(): Send error");
		//reconnect(); //we don't send the message, it is timed out until we reconnect...
	}
	else {
		
		timestampLastSentMessage = time_millis(); 
	}
	return (done ? data.size() : 0);
}

int LGWPortWrapperMod::WaitForData(int msTime)
{
	pthread_mutex_lock(&mutexReconnect);
	if(reconnectPending) {
		pthread_mutex_unlock(&mutexReconnect);
		sleep(2);
		return 0;
	}
	pthread_mutex_unlock(&mutexReconnect);
	if(pCommController != NULL && pCommController->isConnected()) {
		return 1;
	}
	else {
		sleep(2);
		return 0;
	}

}

bool LGWPortWrapperMod::connect(const std::string& hostIP, const unsigned int port, const std::string& encKey, const std::string& desiredSerial)
{
//	//LOG(Logger::LOG_ALL, "LGWPortWrapper::connect()");
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
			//LOG(Logger::LOG_FATAL_ERROR, "LGWPortWrapper::connect(): Encryption pointer is NULL.");
		}
		else {
			encryptionEnabled = pCommController->isEncryptionEnabled();
		}
		startKeepAliveThread();
	}
	writeLGWStatusToFile(getSerial(), pCommController->getConnectErrorAsString());
	return connected;
}

void LGWPortWrapperMod::Disconnect()
{
	if(pCommController != NULL) {
		pCommController->disconnect();
		writeLGWStatusToFile(getSerial(), pCommController->getConnectErrorAsString());
	}
}


void LGWPortWrapperMod::asyncReconnect() {
	pthread_t arecThread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED);
	pthread_attr_setstacksize(&attr, 128*1024);
	pthread_create(&arecThread, &attr, reconnectThreadFunction, this);	
	pthread_attr_destroy(&attr);
}

void* LGWPortWrapperMod::reconnectThreadFunction(void* param) {
	LGWPortWrapperMod* pThis = (LGWPortWrapperMod*)param;
	pThis->reconnect();
	return NULL;
}

void LGWPortWrapperMod::reconnect()
{
	reconnectPending = true;
	pCommController->disconnect();
	writeLGWStatusToFile(getSerial(), pCommController->getConnectErrorAsString());
	bool reconnected = false;
	const unsigned int basetimeout = 5; //5 seconds
	unsigned int timeout = basetimeout;
	do {
		LDU::LanDeviceUtils ldUtils;
		LDU::LanDevice lanDev;
		bool foundDev = ldUtils.searchDeviceByTypeAndSerial("eQ3-HM-LGW*",getSerial(), lanDev);

		if(foundDev) {
			//LOG(Logger::LOG_DEBUG, "LGWPortWrapper::reconnect(): Found device with serial %s.", getSerial().c_str());
			foundDev = ldUtils.readRuntimeNetworkConfiguration(lanDev);
			if(foundDev) {
				timeout = basetimeout;
				//LOG(Logger::LOG_DEBUG, "LGWPortWrapper::reconnect(): Trying to reconnect.");
				UnifiedLanCommController* oldController = pCommController;
				//pCommController->disconnect();
				hostIP = lanDev.getRuntimeIPConfiguration().getIPAddress();
				pthread_mutex_lock(&mutexULCCommController);
				pCommController = new UnifiedLanCommController(hostIP, port);//if this fails, ulcController does not have serial!
				pCommController->setEncryptionKey(this->encKey);
				pthread_mutex_unlock(&mutexULCCommController);
				reconnected = pCommController->connect();
				if(reconnected) {
					pEncryption = pCommController->getTCPEncryption();
					if(pEncryption == NULL) {
						//LOG(Logger::LOG_FATAL_ERROR, "LGWPortWrapper::connect(): Encryption pointer is NULL.");
					}
					else {
						encryptionEnabled = pCommController->isEncryptionEnabled();
					}
				}
				delete oldController;
				writeLGWStatusToFile(getSerial(), pCommController->getConnectErrorAsString());
			}
			if(reconnected) {
				reconnectPending = false;
				if(pCCU2CommController != NULL) {
					pCCU2CommController->startCoprocessorApp();
				}
				startKeepAliveThread();
				//LOG(Logger::LOG_DEBUG, "LGWPortWrapper::connect(): Reconnected.");
//				reconnectPending = false;
				break;
			}
		}
		else {
			//LOG(Logger::LOG_DEBUG, "LGWPortWrapper::reconnect(): Unable to find device with serial %s.", getSerial().c_str());
			writeLGWStatusToFile(getSerial(), std::string("Gateway not found."));
		}
		sleep(timeout);
		if(timeout <= 61) {//60 seconds, this is tcp_fin_timeout on linux
			timeout = timeout + basetimeout;
		}
	} while(!reconnected);
	reconnectPending = false;
}

bool LGWPortWrapperMod::IsConnected()
{
	if(pCommController != NULL)
	{
		return pCommController->isConnected();
	}
	return false;
}

std::string LGWPortWrapperMod::getSerial()
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


void LGWPortWrapperMod::startKeepAliveThread()
{
	//KeepAlive Thread over system channel
	keepAliveThreadActive = false;
	if(keepAliveThread != 0) {//kill old one
		pthread_join(keepAliveThread, NULL);
	}
	if(bidcosChannelKeepAliveThread != 0) {//kill old one and releas resources
		pthread_join(bidcosChannelKeepAliveThread, NULL);
	}
	//Start keep alive thread
	keepAliveThreadActive = true;
	//Initialize receive thread attributes
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 512*1024);

	//Start thread
	pthread_create(&keepAliveThread, &attr, LGWPortWrapperMod::keepAliveThreadFunction, this);
	//Destroy attributes
	pthread_attr_destroy(&attr);

	//Keep alive thread to avoid inactivity timeout for bidcos channel (port 2000)
	pthread_attr_t attr2;
	pthread_attr_init(&attr2);
	pthread_attr_setstacksize(&attr2, 128*1024);
	//Start thread
	pthread_create(&bidcosChannelKeepAliveThread, &attr2, LGWPortWrapperMod::bidcosChannelKeepAliveThreadFunction, this);
	pthread_attr_destroy(&attr2);
}


void* LGWPortWrapperMod::keepAliveThreadFunction(void* param)
{
	InfoLEDState currentInfoLEDState = LED_UNDEFINED;
	LGWPortWrapperMod* pThis = (LGWPortWrapperMod*)param;
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
			usleep(10000000);
			std::string counterHexStr;
			if(!pThis->forceReconnect) {
				//Set INFO LED
				if(currentInfoLEDState != pThis->infoLEDState) {//check info LED status and conditionally update it.
					counterHexStr = toHexString(std::string(1, counter));
					std::string msgData(1, 'L');
					msgData.append(counterHexStr);
					msgData.append(",02,");//Info LED
					switch(pThis->infoLEDState) {
						case LED_OFF:
							//LOG(Logger::LOG_DEBUG, "Switching RF-LGW LED OFF");
							msgData.append("00FF,00");
							break;
						case LED_ON:
							//LOG(Logger::LOG_DEBUG, "Switching RF-LGW LED ON");
							msgData.append("FF00,00");
							break;
						case LED_BLINK_SLOW:
							//LOG(Logger::LOG_DEBUG, "Switching RF-LGW LED ON (BLINK SLOW)");
							msgData.append("0505,00");
							break;
						case LED_BLINK_FAST:
							//LOG(Logger::LOG_DEBUG, "Switching RF-LGW LED ON (BLINK FAST)");
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
							////LOG(Logger::LOG_ALL, "TEST: Set info led reply: %s", reply.c_str());
						}
					}
					counter++;
				}
			}//!forceReconnect
			if(!pThis->forceReconnect) {
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
			}
			else {
				pThis->forceReconnect = false;
				done = false;
			}
			if(!done) {
				//LOG(Logger::LOG_ERROR, "LGWPortWrapper::keepAliveThreadFunction(): Error sending keepalive.");
				keepAliveConnection.disconnect();
				pThis->asyncReconnect();
				//pThis->reconnect();
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
					////LOG(Logger::LOG_ALL, "reply is: %s", reply.c_str());
					std::string desiredReply(">K");
					desiredReply.append(counterHexStr);
					desiredReply.append(1, 0x0D);
					desiredReply.append(1, 0x0A);
					////LOG(Logger::LOG_ALL, "Desired reply: %s",desiredReply.c_str());
					if(desiredReply.compare(reply) == 0) {
						timedout = false;
						break;
					}
				}
				counter++;
			}
			if(timedout) {
				//LOG(Logger::LOG_ERROR, "LGWPortWrapper::keepAliveThreadFunction(): Did not receive reply on keepalive.");
				keepAliveConnection.disconnect();
				//pThis->pCommController->disconnect();
				pThis->asyncReconnect();
				//pThis->reconnect();
				break;
			}
		}
	}
	else {
		////LOG(Logger::LOG_ERROR, "LGWPortWrapper::keepAliveThreadFunction(): Cannot connect.");
	}
	////LOG(Logger::LOG_ALL, "LGWPortWrapper::keepAliveThreadFunction(): Going to die");
	return NULL;
}

void* LGWPortWrapperMod::bidcosChannelKeepAliveThreadFunction(void* param)
{
	LGWPortWrapperMod* pThis = (LGWPortWrapperMod*)param;
	usleep(10000000);
	pThis->timestampLastSentMessage = time_millis();
	while( pThis->keepAliveThreadActive ) {
		const uint64_t timeMillis = time_millis();
		if((pThis->timestampLastSentMessage + 30000) < timeMillis) {
			//TODO 
			if((pThis->pCCU2CommController != NULL) && (!pThis->reconnectPending)) {
				int dc;
				//LOG(Logger::LOG_DEBUG, "LGWPortWrapper::bidcosChannelKeepAliveThreadFunction(): Checking connection.");
				bool succ = pThis->pCCU2CommController->getDutyCycle(&dc);
				if((!succ) && (!pThis->reconnectPending)) {
					pThis->keepAliveThreadActive = false;
					//LOG(Logger::LOG_DEBUG, "LGWPortWrapper::bidcosChannelKeepAliveThreadFunction: reconnect");
					if(pThis->keepAliveThread != 0) {
						////LOG(Logger::LOG_ALL, "bidcosChannelKeepAliveThreadFunction: joining");
						//pthread_join(pThis->keepAliveThread, NULL);
						////LOG(Logger::LOG_ALL, "bidcosChannelKeepAliveThreadFunction: joined");
						//pThis->keepAliveThread = NULL;
					}
					//pThis->reconnect();
					pThis->forceReconnect = true;
					break;//reconnect sets keelAliveThreadActive to true, so we must break here.
				}
			}
		}
		usleep(16000000);//16s
		
	}
	return NULL;
}

void HM2Mod::LGWPortWrapperMod::setInfoLEDState(const InfoLEDState& state)
{
	if( (state >= 0 && state <= 4) || (state == 255)) {
		this->infoLEDState = state;
	}
}

void LGWPortWrapperMod::writeLGWStatusToFile(const std::string& serial, const std::string& statusText)
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


/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <LGWPortWrapper.h>
#include <UnifiedLanComm.h>
#include <LanDeviceUtils.h>

#ifndef WIN32
#  include <unistd.h>
#endif

# include <Logger.h>

//#define DUMP 1

#ifdef DUMP
# include <HMWLGWUtils.h>
#endif

#include <errno.h>
#include <fstream>

using namespace ulc;

LGWPortWrapper::LGWPortWrapper()
: PortWrapper()
, pCommController(NULL)
, encryptionEnabled(false)
, pEncryption(NULL)
, port(0)
{
}

LGWPortWrapper::~LGWPortWrapper()
{
	encryptionEnabled = false;//important to do that, because pointer on encryption in pCommController will become bad when we delete pCommController
	if(pCommController != NULL) {
		delete pCommController;
		pCommController = NULL;
	}
}

int LGWPortWrapper::ReadData(std::string* data)
{
	//LOG(Logger::LOG_ALL, "LGWPortWrapper::ReadData()");
	if(data != NULL) {
		bool done = pCommController->receive( *data );
		if(done) {
#ifdef DUMP
			LOG(Logger::LOG_ALL, "LGWPortWrapper::ReadData(): Received %s", toDebugHexStr(*data).c_str());
#endif
			if(encryptionEnabled && pEncryption != NULL) {
				pEncryption->decrypt(*data);
			}
		}
		else {
			//reconnect();
		}
		return (done ? data->size() : 0);
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
		//reconnect(); //we don't send the message, it is timed out until we reconnect...
	}
	return (done ? data.size() : 0);
}

int LGWPortWrapper::WaitForData(int msTime)
{
	//LOG(Logger::LOG_DEBUG, "LGWPortWrapper::WaitForData()");
	if(pCommController != NULL && pCommController->isConnected()) {
		return 1;
	}
	else {
		sleep(2);
		return 0;
	}

}

bool LGWPortWrapper::connect(const std::string& hostIP, const unsigned int port, const std::string& encKey, const std::string& desiredSerial)
{

	LOG(Logger::LOG_ALL, "LGWPortWrapper::connect()");
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
	this->hostIP = hostIP;
	this->port = port;
	this->encKey = encKey;

	bool connected = pCommController->connect();
	if(connected) {
		const std::string determinedSerial = pCommController->getSerial();
		if(desiredSerial.empty()) {
			if(determinedSerial.empty()) {
				LOG(Logger::LOG_WARNING, "LGWPortWrapper::connect(): Got no serial number. Reconnect on failure won't work.");
			}
			else {
				this->serial = determinedSerial;
			}
		}
		else {
			if(desiredSerial.compare(determinedSerial) != 0) {
				LOG(Logger::LOG_WARNING, "LGWPortWrapper::connect(): Desired and determined serial numbers do not match. Storing desired serial number: %s", desiredSerial.c_str());
			}
			this->serial = desiredSerial;
		}

		pEncryption = pCommController->getTCPEncryption();
		if(pEncryption == NULL) {
			LOG(Logger::LOG_FATAL_ERROR, "LGWPortWrapper::connect(): Encryption pointer is NULL.");
		}
		else {
			encryptionEnabled = pCommController->isEncryptionEnabled();
		}
	}
	writeLGWStatusToFile(pCommController->getSerial(), pCommController->getConnectErrorAsString());
	return connected;
}

void LGWPortWrapper::disconnect()
{
	if(pCommController != NULL) {
		pCommController->disconnect();
	}
}

void LGWPortWrapper::reconnect()
{
//	LOG(Logger::LOG_ALL, "LGWPortWrapper::reconnect()");
	//no locking here, because read/write connect and disconnect do that
	disconnect();
	bool reconnected = false;
	const unsigned int basetimeout = 5; //5 seconds
	unsigned int timeout = basetimeout;
	do {
		LDU::LanDeviceUtils ldUtils;
		LDU::LanDevice lanDev;
		const std::string serial = getSerial();
		bool foundDev = ldUtils.searchDeviceBySerial(serial, lanDev);

		if(foundDev) {
			LOG(Logger::LOG_DEBUG, "LGWPortWrapper::reconnect(): Found device with serial %s.", serial.c_str());
			foundDev = ldUtils.readRuntimeNetworkConfiguration(lanDev);
			if(foundDev) {
				LOG(Logger::LOG_DEBUG, "LGWPortWrapper::reconnect(): Trying to reconnect.");
				UnifiedLanCommController* oldController = pCommController;
				//pCommController->disconnect();
				hostIP = lanDev.getRuntimeIPConfiguration().getIPAddress();
				pCommController = new UnifiedLanCommController(hostIP, port);
				pCommController->setEncryptionKey(encKey);
				delete oldController;
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
				writeLGWStatusToFile(pCommController->getSerial(), pCommController->getConnectErrorAsString());
			}
			if(reconnected) {
				LOG(Logger::LOG_FATAL_ERROR, "LGWPortWrapper::connect(): Reconnected.");
				break;
			}
		}
		else {
			LOG(Logger::LOG_DEBUG, "LGWPortWrapper::reconnect(): Unable to find device with serial %s.", serial.c_str());
			writeLGWStatusToFile(pCommController->getSerial(), std::string("Gateway not found."));
		}
		sleep(timeout);
		if(timeout <= 61) {//60 seconds, this is tcp_fin_timeout on linux
			timeout = timeout + basetimeout;
		}
	} while(!reconnected);
}

bool LGWPortWrapper::isConnected()
{
	if(pCommController != NULL)
	{
		return pCommController->isConnected();
	}
	return false;
}

std::string LGWPortWrapper::getSerial()
{
	return this->serial;
}

void LGWPortWrapper::writeLGWStatusToFile(const std::string& serial, const std::string& statusText)
{
	std::string filepath("/var/status/");
	filepath.append(serial);
	filepath.append(".connstat");
	std::ofstream os(filepath.c_str(), std::ofstream::out);
	os << statusText;
	os.close();
}



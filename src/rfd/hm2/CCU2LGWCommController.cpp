/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <CCU2LGWCommController.h>
#include <CCU2BidcosRemoteInterface.h>
#include <LGWPortWrapper.h>
#include <LanDeviceUtils.h>
#include <Logger.h>
#include <string.h>
#include <md5.h>
#include <stdio.h>
#ifndef WIN32
#  include <unistd.h>
#endif
#include <utils.h>

using namespace HM2;

//-----------------------------------------------------------------------------------------------------

CCU2LGWCommController::CCU2LGWCommController(CCU2BidcosRemoteInterface* bidcosRemoteInterface)
: CCU2CommController(bidcosRemoteInterface)
, coprocessorInitPending(false)
{
	pPortWrapper = new LGWPortWrapper(this);
}

//-----------------------------------------------------------------------------------------------------

CCU2LGWCommController::~CCU2LGWCommController()
{
	if(pPortWrapper != NULL) {
		((LGWPortWrapper*)pPortWrapper)->shutdown();
	}
}

//-----------------------------------------------------------------------------------------------------

bool CCU2LGWCommController::init(const std::string host, const int port, const std::string& encKey, const std::string& desiredSerial, const bool csmacaEnabled, const bool improvedCoproInit)
{
	interfaceSerial = desiredSerial;
	//create serial.connstat file for central.
#ifndef WIN32
	if(!desiredSerial.empty()) {
		std::string statfilepath("/var/status/");
		statfilepath.append(desiredSerial);
		statfilepath.append(".connstat");
		FILE* f = fopen(statfilepath.c_str(), "w");
		if(f) {
			fclose(f);
		}
	}
#endif	
	
	//determine ip using serial number or just use host if given
	std::string ip;
	if(host.empty()) {
		//search host using serial number
		LDU::LanDeviceUtils ldUtils;
		LDU::LanDevice lanDevice;
		//printf("Searching for HomeMatic Lan Gateway with serial number %s.\n", lgwSerialNumber.c_str());
		//not found: ip stays empty, the gateway is searched again in the background (see below)
		bool done = ldUtils.searchDeviceByTypeAndSerial("eQ3-HM-LGW*", desiredSerial, lanDevice);
		if(!done) {
			LOG(Logger::LOG_ERROR, "CCU2LGWCommController::init(): Could not find HomeMatic Lan Gateway with serial number %s.", desiredSerial.c_str());
		}
		else {
			done = ldUtils.readRuntimeNetworkConfiguration(lanDevice);
			if(!done) {
				LOG(Logger::LOG_ERROR, "CCU2LGWCommController::init(): Could not determine IP address of HomeMatic Lan Gateway with serial number %s.", desiredSerial.c_str());
			}
			else {
				ip = lanDevice.getRuntimeIPConfiguration().getIPAddress();
				LOG(Logger::LOG_INFO, "Found HomeMatic Lan Gateway with IP Address %s", ip.c_str());
			}
		}
	}
	else {
		((LGWPortWrapper*)pPortWrapper)->setHostIPAssignedByUser(true);
		ip = host;
	}
	
	//if encKey provided, calculate md5 checksum (encKey) of the security key
	std::string encKeyMD5 = calculateMD5(encKey);

	//Connect
	LGWPortWrapper* pLGWPortWrapper = (LGWPortWrapper*)pPortWrapper;
	bool done = pLGWPortWrapper->connect(ip, port, encKeyMD5, desiredSerial);
	if(!done) {
		if(pLGWPortWrapper->isShutdownRequested()) {
			return false;
		}
		//Keep the interface and retry in the background, as after a connection loss: a gateway that is not there yet,
		//e.g. one that starts slower than rfd after a power cut, would otherwise be lost until rfd is restarted.
		//Retrying here would block the whole rfd. The reconnect initializes the coprocessor (reinitCoprocessor())
		//once the gateway answers; the receiver waits for it.
		LOG(Logger::LOG_ERROR, "CCU2LGWCommController::init(): Cannot connect to HomeMatic Lan Gateway with serial number %s. Retrying in the background.", desiredSerial.c_str());
		improvedInitialization = improvedCoproInit;
		coprocessorInitPending = true;
		interfaceState = IFSTATE_INIT;
		pthread_create( &receiveThread, &receiveThreadAttributes, CCU2CommController::receiveThreadFunction, (void*)this);
		pLGWPortWrapper->asyncReconnect();
		return true;
	}
	done = CCU2CommController::init(csmacaEnabled, improvedCoproInit);
	if(!done) {
		((LGWPortWrapper*)pPortWrapper)->Disconnect();
	}
	return done;
}



//-----------------------------------------------------------------------------------------------------

std::string CCU2LGWCommController::calculateMD5(const std::string& s)
{
	md5 md5_calculator;
	unsigned char* buffer = new unsigned char[s.length()];
	memcpy(buffer, s.c_str(), s.length());
	md5_calculator.Update(buffer, s.length());
	md5_calculator.Finalize();
	delete[] buffer;
	std::string digest;
	digest.append((const char*) md5_calculator.Digest(), std::string::size_type(16));
	return digest;
}

//-----------------------------------------------------------------------------------------------------

bool HM2::CCU2LGWCommController::setRFLGWInfoLED(const unsigned int state)
{
	LGWPortWrapper* pLGWPortWrapper = dynamic_cast<LGWPortWrapper*>(pPortWrapper);
	if(pLGWPortWrapper == NULL) {
		return false;
	}
	pLGWPortWrapper->setInfoLEDState((LGWPortWrapper::InfoLEDState)state);
	return true;
}

//-----------------------------------------------------------------------------------------------------

void CCU2LGWCommController::handleCoprocessorRecoveryFailure()
{
	LGWPortWrapper* pLGWPortWrapper = dynamic_cast<LGWPortWrapper*>(pPortWrapper);
	if(pLGWPortWrapper == NULL) {
		CCU2CommController::handleCoprocessorRecoveryFailure();
		return;
	}
	LOG(Logger::LOG_ERROR, "(%s) CCU2LGWCommController::handleCoprocessorRecoveryFailure(): Scheduling LAN gateway reconnect.", interfaceSerial.c_str());
	pLGWPortWrapper->asyncReconnect();
}

bool HM2::CCU2LGWCommController::reinitCoprocessor()
{
	pthread_mutex_lock(&mutexBidcosTelegramRequest);
	interfaceState = IFSTATE_REINIT;
	pthread_mutex_unlock(&mutexBidcosTelegramRequest);
	bool done = improvedInit();
	if(done && coprocessorInitPending) {
		//first connect after a start without the gateway: what init() would have done
		std::string blVersion;
		std::string fwVersion;
		if(readFirmwareVersion(blVersion, fwVersion)) {
			LOG(Logger::LOG_INFO, "(%s) CCU2LGWCommController::reinitCoprocessor(): Coprocessor Firmware Version is: %s", interfaceSerial.c_str(), fwVersion.c_str());
			pBidcosRemoteInterfcace->publishFirmwareVersion(fwVersion);
		}
	}
	if(done) {
		done = restoreConfigToCoprocessor();
	}
	if(done) {
		coprocessorInitPending = false;
	}
	return done;
	/*initCoprocessor();
	unsigned int waitCnt = 10; //10 * 500 ms ==> 5 seconds
	unsigned int cnt = 0;
	const uint32_t waitInterval = 500000;//100 ms
	while(coprocessorState != COPROCESSOR_STATE_APPLICATION && cnt < waitCnt) {
		usleep(waitInterval);
		cnt++;
	}
	return (coprocessorState == COPROCESSOR_STATE_APPLICATION);*/
}

//-----------------------------------------------------------------------------------------------------

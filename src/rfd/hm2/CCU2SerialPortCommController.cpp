/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "CCU2SerialPortCommController.h"

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
#else
#include <algorithm>
#endif



//#define DUMP 1

using namespace HM2;

CCU2SerialPortCommController::CCU2SerialPortCommController(CCU2BidcosRemoteInterface* bidcosRemoteInterface)
: CCU2CommController(bidcosRemoteInterface)
{
	pPortWrapper = CCU2SerialPortWrapper::createCCU2SerialPortWrapper();
}

CCU2SerialPortCommController::~CCU2SerialPortCommController(void)
{
}

bool CCU2SerialPortCommController::init(const std::string& dev, const std::string& access, const std::string& reset, const bool csmacaEnabled, const bool improvedCoproInit) {
	//Reset coprocessor (needed for coprocessor in Qivicon box)
	std::string data;
	std::string devId;
	std::string resetProcedureData;
	bool done = false;
	std::string devStr(dev);

//windows fix com port numbers over 9	
#ifdef WIN32 
	if(dev.length() > 4) {
		devStr = dev.substr(0,3);
		std::transform(devStr.begin(), devStr.end(), devStr.begin(), ::toupper);
		if(devStr.compare("COM") == 0) {
			std::string nr = dev.substr(3);
			devStr = "\\\\.\\COM"+nr;
		}
	}
#endif

	//Open connection to coprocessor via serial comport.
	done = ((CCU2SerialPortWrapper*)pPortWrapper)->Open(devStr);
	if(!done) {
		LOG(Logger::LOG_ERROR, "CCU2CommController::init(): Init failed. Could not open device %s\n",dev.c_str());
		done = false;
		goto init_end;
	}

	if(!improvedCoproInit) {
		//Reset coprocessor
		done = FileIO::readStringFromFile( access, resetProcedureData );
		if(!done) {
			LOG(Logger::LOG_ERROR, "CCU2CommController::init(): Init failed. Cannot reset coprocessor. (Hint: Cannot read access)");
			done = false;
			goto init_end;
		}
		if(resetProcedureData.compare("1") != 0) {//access not 1 -> write 1
			resetProcedureData = "1";
			done = FileIO::writeStringToFile(access, resetProcedureData);
			if(!done) {
				LOG(Logger::LOG_ERROR, "CCU2CommController::init(): Init failed. Cannot reset coprocessor. (Hint: Cannot write access)");
				done = false;
				goto init_end;
			}
		}
			//Write 1 to reset
		done = FileIO::writeStringToFile( reset, resetProcedureData);
		if(!done) {
			LOG(Logger::LOG_ERROR, "CCU2CommController::init(): Init failed. Cannot reset coprocessor. (Hint: Cannot write reset)");
			done = false;
			goto init_end;
		}
	}
	done = CCU2CommController::init(csmacaEnabled, improvedCoproInit);

init_end:

	return done;

}

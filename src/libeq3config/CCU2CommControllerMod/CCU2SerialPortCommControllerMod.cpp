/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "CCU2SerialPortCommControllerMod.h"

//#include <CCU2SerialFrameMod.h>
#include <CCU2SerialPortWrapperMod.h>

//#include <utils.h>//for time_millis
//#include <HM2Utils.h>
//#include <Logger.h>
//#include <pthread.h>
#include <errno.h>
#include <Logger.h>

#include <FileIOMod.h>

#ifndef WIN32
#include <unistd.h>
#include <stdio.h>
#endif

//#define DUMP 1

using namespace HM2Mod;

CCU2SerialPortCommControllerMod::CCU2SerialPortCommControllerMod()
: CCU2CommControllerMod()
{
	pPortWrapper = CCU2SerialPortWrapperMod::createCCU2SerialPortWrapper();
}

CCU2SerialPortCommControllerMod::~CCU2SerialPortCommControllerMod(void)
{
}

bool CCU2SerialPortCommControllerMod::init(const std::string& dev, bool skipStartApp /*= false */) {
	bool done = false;
	//Open connection to coprocessor via serial comport.
	done = ((CCU2SerialPortWrapperMod*)pPortWrapper)->Open( dev );
	if(!done) {
		LOG(Logger::LOG_ERROR, "CCU2CommController::init(): Init failed. Could not open device %s\n",dev.c_str());
		done = false;
		goto init_end;
	}
	done = CCU2CommControllerMod::init(skipStartApp);
init_end:
	return done;
}

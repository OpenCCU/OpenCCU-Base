/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <CCU2PortWrapperMod.h>
#include <CCU2SerialPortWrapperMod.h>
#include <LGWPortWrapperMod.h>
//#include <Logger.h>

using namespace HM2Mod;

CCU2PortWrapperMod::CCU2PortWrapperMod()
{
}

CCU2PortWrapperMod::~CCU2PortWrapperMod()
{
}

/*
CCU2PortWrapper* createCCU2PortWrapper(const CCU2PortWrapper::PortWrapperSubtype& subtype)
{
	CCU2PortWrapper* pWrapper = NULL;
	switch(subtype) {
		case CCU2PortWrapper::SERIAL:
			pWrapper = CCU2SerialPortWrapper::createCCU2SerialPortWrapper();
			break;
		case CCU2PortWrapper::LGW:
			pWrapper = new LGWPortWrapper();
			break;
		default:
			LOG(Logger::LOG_FATAL_ERROR, "Undefined port wrapper type. Defaulting to serial, which might wont't work!");
			pWrapper = CCU2SerialPortWrapper::createCCU2SerialPortWrapper();
			break;
	}
	return pWrapper;
}
*/


/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/


#ifndef INFOLED_H_
#define INFOLED_H_
#include <string>
#include <cstdint>
#include "led.h"
#include "RgbLed.h"
#include "ServiceMessage.h"
#include "AlarmMessage.h"
#include "MessageParser.h"
#include "RFLGWInfoLED.h"
#if defined(PLATFORM_CCU3)
#include "Network.h"
#else
#include "UpdateAvailable.h"
#endif

class InfoLed {
public:
	InfoLed();
	virtual ~InfoLed();
	void updateLedState();
	bool checkMessage(std::string &msg);
	bool isProcessRunning(const char *pidFileName, const char *procName, const bool allowPartialMatch = false);

private:
  #if defined(PLATFORM_CCU3)
	RgbLed rgbLed;
	Network net;
	bool rpiRfModFound;
	bool fullCCUFound;
  #else
	led infoLed;
  #endif
	const uint64_t infoLedRefreshTime;
	uint64_t nextInfoUpdate;
	ServiceMessage service;
	AlarmMessage alarm;
  #if !defined(PLATFORM_CCU3)
	UpdateAvailable update;
  #endif
	MessageParser parser;
	RFLGWInfoLED rflgwInfoLed;
};

#endif /* INFOLED_H_ */

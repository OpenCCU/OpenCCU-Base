/*
 * InfoLed.h
 *
 *  Created on: 18.01.2013
 */

#ifndef INFOLED_H_
#define INFOLED_H_
#include <string>
#include <cstdint>
#include "led.h"
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
	led redLed;
	led greenLed;
	led blueLed;
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

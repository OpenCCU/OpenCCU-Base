/*
 * InfoLed.h
 *
 *  Created on: 18.01.2013
 *      Author: willms
 */

#ifndef INFOLED_H_
#define INFOLED_H_
#include <string>
#include "led.h"
#include "ServiceMessage.h"
#include "AlarmMessage.h"
#include "UpdateAvailable.h"
#include "MessageParser.h"
#include "RFLGWInfoLED.h"
#if defined(PLATFORM_CCU3)
#include "Network.h"
#endif

class InfoLed {
public:
	InfoLed();
	virtual ~InfoLed();
	void updateLedState();
	bool checkMessage(std::string &msg);
	bool isProcessRunning(const char *pidFileName, const char *procName, const char *prgfile);

private:
  #if defined(PLATFORM_CCU3)
	led redLed;
	led greenLed;
	led blueLed;
	Network net;
	bool rpiRfModFound;
  #else
	led infoLed;
  #endif
	const unsigned long infoLedRefreshTime;
	unsigned long nextInfoUpdate;
	ServiceMessage service;
	AlarmMessage alarm;
	UpdateAvailable update;
	MessageParser parser;
	RFLGWInfoLED rflgwInfoLed;
};

#endif /* INFOLED_H_ */

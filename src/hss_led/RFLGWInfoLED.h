/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RFLGWINFOLED_H_
#define _RFLGWINFOLED_H_

#include "led.h"
#include <string>

class RFLGWInfoLED
{
public:
	RFLGWInfoLED();
	virtual ~RFLGWInfoLED();
	
	void ledOff();
	void ledOn();
	void ledFlashSlow();
	void ledFlashFast();
	led::LedState getLedState();
  void switchLed(enum led::LedState state);
protected:
	virtual void setLED(led::LedState ledState);
	virtual bool isRfLgwPresent();

private:
	std::string trim(const std::string& str);
	std::string readPortFromFile(const char* filename);
	void initRFDPort();
	
private:
	bool rfLgwExists;
  led::LedState lastState;
	std::string rfdPort;

};

#endif

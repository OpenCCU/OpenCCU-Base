/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/



#ifndef LED_H_
#define LED_H_
#include <string>
#include <fcntl.h>
class led {
public:
	enum LedState
		{
			UNKNOWN=-1,
			LED_OFF,
			LED_ON,
			LED_SLOW,
			LED_FAST,
		};
	led(std::string ledDirectory);
	virtual ~led();
	void LedOff();
	void LedOn();
	void LedFlashSlow(int start=0);
	void LedFlashFast(int start=0);
	LedState getLedState();
	void switchLed(enum LedState state, int start=0);
private:
	std::string ledDirectory;
	void timerOn();

	LedState checkDelayTimes();
};

#endif /* LED_H_ */

/*
 * led.h
 *
 *  Created on: 18.01.2013
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

/*
 * InternetLed.h
 *
 *  Created on: 18.01.2013
 */

#ifndef INTERNETLED_H_
#define INTERNETLED_H_
#include "led.h"
#include "Network.h"
class InternetLed {
public:
	InternetLed();
	virtual ~InternetLed();
	void updateLedState();
private:
	led internetLed;
	Network net;
};

#endif /* INTERNETLED_H_ */

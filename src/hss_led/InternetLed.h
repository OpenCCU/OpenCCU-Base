/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
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

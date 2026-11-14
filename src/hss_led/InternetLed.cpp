/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "InternetLed.h"

InternetLed::InternetLed() :
		internetLed("internet") {
	// TODO Automatisch generierter Konstruktorstub
	internetLed.LedOff();
}

InternetLed::~InternetLed() {
	// TODO !CodeTemplates.destructorstub.tododesc!
}

void InternetLed::updateLedState() {
	this->net.isInfoPending();
	switch (this->net.getNetState()) {
	case Network::LinkUp:
		this->internetLed.LedFlashFast();
		break;
	case Network::IP:
		this->internetLed.LedFlashSlow();
		break;
	case Network::InternetAvailable:
		this->internetLed.LedOn();
		break;
	default:
		this->internetLed.LedOff();
		break;
	}
}

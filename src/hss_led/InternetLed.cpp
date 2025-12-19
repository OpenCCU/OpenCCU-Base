/*
 * InternetLed.cpp
 *
 *  Created on: 18.01.2013
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

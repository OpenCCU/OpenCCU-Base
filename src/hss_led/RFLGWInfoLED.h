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

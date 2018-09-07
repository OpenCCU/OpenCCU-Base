/*
 * InfoLed.cpp
 *
 *  Created on: 18.01.2013
 *      Author: willms
 */

#include "InfoLed.h"
#include <utils.h>
#include <unistd.h>
#include <stdio.h>
#include <strings.h>
#include <sys/stat.h>
#include <cstdlib>
#include <iostream>
#include <fstream>

InfoLed::InfoLed():
  #if defined(PLATFORM_CCU3)
  redLed("rpi_rf_mod:red"),
  greenLed("rpi_rf_mod:green"),
  blueLed("rpi_rf_mod:blue"),
  rpiRfModFound(false),
  fullCCUFound(false),
  #else
  infoLed("info"),
  #endif
  infoLedRefreshTime(120000),
  nextInfoUpdate(0),
  service(),
  alarm(),
  update(),
  parser(),
  rflgwInfoLed()
{
  #if defined(PLATFORM_CCU3)
  // identify if we have a RPI-RF-MOD
  if(system("lsmod | grep -q rx8130") == 0)
  {
    redLed.LedOn();
    greenLed.LedOn();
    blueLed.LedOff();
    rpiRfModFound = true;
  }

  // identify if we running on a production image or on
  // a full CCU
  struct stat buffer;
  if(stat("/bin/rfd", &buffer) == 0)
    fullCCUFound = true;
  #else
	infoLed.LedOff();
  #endif
}

InfoLed::~InfoLed() {
	// TODO !CodeTemplates.destructorstub.tododesc!
}

bool InfoLed::isProcessRunning(const char *filename, const char *pname, const bool allowPartialMatch/*=false*/) {

  bool res = false;

  std::ifstream pidFile(filename);
  if(pidFile.is_open()) {
    std::string pidNumStr;

    std::getline(pidFile, pidNumStr);

    int pid = atoi(pidNumStr.c_str());
    if(pid > 0 && pidNumStr.empty() == false)
    {
      // Read contents of virtual /proc/{pid}/cmdline file
      std::string cmdPath = std::string("/proc/") + pidNumStr + "/cmdline";
      std::ifstream cmdFile(cmdPath.c_str());
      if(cmdFile.is_open()) {
        std::string cmdLine;
        getline(cmdFile, cmdLine);
        if(!cmdLine.empty())
        {
          // Keep first cmdline item which contains the program path
          size_t pos = cmdLine.find('\0');
          if(pos != std::string::npos)
            cmdLine = cmdLine.substr(0, pos);
          // Keep program name only, removing the path
          pos = cmdLine.rfind('/');
          if(pos != std::string::npos)
            cmdLine = cmdLine.substr(pos + 1);
          // Compare against requested process name
          if(allowPartialMatch) {
        	res = (cmdLine.find(pname) != std::string::npos);
          }
          else {//exact match only - default behavior
			  if(strcasecmp(pname, cmdLine.c_str()) == 0) {
				res = true;
			  }
          }
        }
      }
    }
  }

  return res;
}

#include <stdio.h>

void InfoLed::updateLedState() {
  #if defined(PLATFORM_CCU3)

	this->net.isInfoPending();
  int netState = this->net.getNetState();

  const std::map<std::string, std::string> infoConfigData = Info::readConfig();//calls Info::updateInfoConfig, which reads all cfg data.
  bool serviceState =  (this->service.isInfoEnabled(infoConfigData) && this->service.isInfoPending());
  bool alarmState = (this->alarm.isInfoEnabled(infoConfigData) && this->alarm.isInfoPending());
  //bool updateState = this->update.isInfoPending();

  // get old LED states
	led::LedState oldStateRed = this->redLed.getLedState();
	led::LedState oldStateGreen = this->greenLed.getLedState();
	led::LedState oldStateBlue = this->blueLed.getLedState();
  led::LedState oldStateLGW = this->rflgwInfoLed.getLedState();

  // calculate new LED states
  led::LedState newStateRed = led::UNKNOWN;
  led::LedState newStateGreen = led::UNKNOWN;
  led::LedState newStateBlue = led::UNKNOWN;
  led::LedState newStateLGW = led::UNKNOWN;

  switch(netState)
  {
    case Network::InternetAvailable:
    {
      if(alarmState == false)
      {
        if(serviceState == false)
        {
          // Standard-Mode: light up blue only
          newStateRed = led::LED_OFF;
          newStateGreen = led::LED_OFF;
          newStateBlue = led::LED_ON;
          newStateLGW = led::LED_OFF;
          //printf("STANDARD\n");
        }
        else
        {
          // Standard-Mode + Service-Mode: blink yellow + blue
          newStateRed = led::LED_SLOW;
          newStateGreen = led::LED_SLOW;
          newStateBlue = led::LED_SLOW1;
          newStateLGW = led::LED_SLOW;
          //printf("STANDARD+SERVICE\n");
        }
      }
      else
      {
        // Standard-Mode + Alarm-mode: blink red + blue
        newStateRed = led::LED_SLOW;
        newStateGreen = led::LED_OFF;
        newStateBlue = led::LED_SLOW1;
        newStateLGW = led::LED_FAST;
        //printf("STANDARD+ALARM\n");
      }
    }
    break;

    case Network::IP:
    {
      // No-Internet, but IP available: blink blue fast
      newStateRed = led::LED_OFF;
      newStateGreen = led::LED_OFF;
      newStateBlue = led::LED_FAST;
      newStateLGW = led::LED_ON;
      //printf("NO INTERNET+IP\n");
    }
    break;

    case Network::Disconnected:
    {
      // No-Link: blink yellow fast
      newStateRed = led::LED_FAST;
      newStateGreen = led::LED_FAST;
      newStateBlue = led::LED_OFF;
      newStateLGW = led::LED_ON;
      //printf("DISCONNECTED\n");
    }
    break;

    case Network::LinkUp:
    {
      // Link-Up, but no IP (yet): blink blue slowly
      newStateRed = led::LED_OFF;
      newStateGreen = led::LED_OFF;
      newStateBlue = led::LED_SLOW;
      newStateLGW = led::LED_ON;
      //printf("LINK\n");
    }
    break;
  }

  // check if all essential homematic services are running or not
  // depending on which system we are running on (prodimage vs. full CCU)

  // on all systems (prodimage+CCU)
  bool allProcessesRunning = false;
  if(isProcessRunning("/var/run/eq3configd.pid", "eq3configd") == true &&
     isProcessRunning("/var/run/ReGaHss.pid", "ReGaHss", true) == true &&
     isProcessRunning("/var/run/ssdpd.pid", "ssdpd") == true)
  {
    // on full CCU we test for more
    if(fullCCUFound == false ||
       (isProcessRunning("/var/run/HMIPServer.pid", "java") == true &&
        isProcessRunning("/var/run/multimacd.pid", "multimacd") == true &&
        isProcessRunning("/var/run/rfd.pid", "rfd") == true))
    {
      allProcessesRunning = true;
    }
  }

  if(allProcessesRunning == false)
  {
    newStateRed = led::LED_ON;
    newStateGreen = led::LED_OFF;
    newStateBlue = led::LED_OFF;
  }

  if(((newStateRed != oldStateRed || newStateGreen != oldStateGreen || newStateBlue != oldStateBlue) &&
      (newStateRed != led::UNKNOWN && newStateGreen != led::UNKNOWN && newStateBlue != led::UNKNOWN)) ||
     (this->nextInfoUpdate > time_millis()))
  {
    // identify if we have a RPI-RF-MOD
    if(rpiRfModFound == true)
    {
      // check that a file /var/status/startupFinished exists and if not
      // we skip setting the RPI-RF-MOD led on our own
      struct stat buffer;
      if((stat("/var/status/startupFinished", &buffer) == 0) ||
         (oldStateRed == led::LED_OFF && oldStateGreen == led::LED_OFF && oldStateBlue == led::LED_ON)) {

        this->redLed.LedOff();
        this->greenLed.LedOff();
        this->blueLed.LedOff();

        this->redLed.switchLed(newStateRed);
        this->greenLed.switchLed(newStateGreen);
        this->blueLed.switchLed(newStateBlue);
      }
    }

    if(newStateLGW != oldStateLGW && newStateLGW != led::UNKNOWN)
      this->rflgwInfoLed.switchLed(newStateLGW);

    this->nextInfoUpdate = time_millis() + this->infoLedRefreshTime;
  }

  #else

	led::LedState newState = led::LED_OFF;
	led::LedState oldState = led::UNKNOWN;
	if(this->service.isInfoPending() || this->update.isInfoPending())
	{
		newState = led::LED_SLOW;
	}
	if(this->alarm.isInfoPending())
	{
		newState = led::LED_FAST;
	}
	oldState = this->infoLed.getLedState();
	if((newState != oldState) ||
     (this->nextInfoUpdate > time_millis()))
	{
		switch (newState) {
		case led::LED_OFF:
			this->infoLed.LedOff();
			this->rflgwInfoLed.ledOff();
			break;
		case led::LED_ON:
			this->infoLed.LedOn();
			this->rflgwInfoLed.ledOn();
			break;
		case led::LED_SLOW:
			this->infoLed.LedFlashSlow();
			this->rflgwInfoLed.ledFlashSlow();
			break;
		case led::LED_FAST:
			this->infoLed.LedFlashFast();
			this->rflgwInfoLed.ledFlashFast();
			break;
		default:
			this->infoLed.LedOff();
			this->rflgwInfoLed.ledOff();
			break;
		}
		this->nextInfoUpdate = time_millis() + this->infoLedRefreshTime;
	}
  #endif
}

bool InfoLed::checkMessage(std::string& msg) {
	bool ret = parser.parsMessage(msg);
	if(ret)
	{
		switch(parser.getCommandType())
		{
		case SERVICE:
			this->service.setMessage(parser.getMessageSource(),parser.getServiceValue());
			break;
		case ALARM:
			this->alarm.setMessage(parser.getMessageSource(),parser.getAlarmValue());
			break;
		default:
			break;
		}
	}
	return ret;
}

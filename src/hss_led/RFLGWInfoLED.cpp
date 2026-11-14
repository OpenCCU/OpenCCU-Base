/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "RFLGWInfoLED.h"
#include <XmlRpc.h>
#include <Logger.h>
#include <string>
#include <PropertyMap.h>
#include "led.h"
#include <fstream>

RFLGWInfoLED::RFLGWInfoLED() 
: rfLgwExists(false)
,	lastState(led::UNKNOWN)
,	rfdPort("2001")
{
	rfLgwExists = isRfLgwPresent();
	initRFDPort();
}

RFLGWInfoLED::~RFLGWInfoLED() {
}

void RFLGWInfoLED::ledOff()
{
	setLED(led::LED_OFF);
}

void RFLGWInfoLED::ledOn()
{
	setLED(led::LED_ON);
}

void RFLGWInfoLED::ledFlashSlow()
{
	setLED(led::LED_SLOW);
}

void RFLGWInfoLED::ledFlashFast()
{
	setLED(led::LED_FAST);
}

void RFLGWInfoLED::setLED(led::LedState ledState)
{
	if(rfLgwExists) {
		std::string url("http://127.0.0.1:"+rfdPort);
		XmlRpc::XmlRpcClient client(url);	
		XmlRpc::XmlRpcValue params;
		XmlRpc::XmlRpcValue result;
	
    switch(ledState)
    {
		  case led::LED_OFF:
        params[0] = 0;
      break;

      case led::LED_ON:
        params[0] = 1;
      break;

      case led::LED_SLOW:
        params[0] = 2;
      break;

      case led::LED_FAST:
        params[0] = 3;
      break;

      default:
        return;
      break;
    }
	
		bool success = client.execute("setRFLGWInfoLED", params, result);
		if ((!success) || (client.isFault()))
		{
			LOG(Logger::LOG_ERROR, "Error setting rf-lgw info led");
		}

    lastState = ledState;
	}
}

bool RFLGWInfoLED::isRfLgwPresent()
{
	const std::string confFilePath("/etc/config/rfd.conf");
	PropertyMap configData;
	if( (configData.ReadFromFile(confFilePath) < 0) ) {//try to read file
		return false;
	}
	PropertyMap::StringList sections=configData.ListSections();
    for(PropertyMap::StringList::iterator it=sections.begin();it!=sections.end();it++)
    {
        std::string& section=*it;
        if(section.find("Interface ")==0)
        {
            configData.SetCurrentSection(section);
            std::string type = configData.GetStringValue("Type", "");
			if(type.compare("HMLGW2") == 0) {
	        	return true;
	        }
	        else {
	        	continue;//next section
	        }
        }
    }
    return false;
}

std::string RFLGWInfoLED::trim(const std::string& str) {
    std::string::size_type first = str.find_first_not_of(' ');
    if (std::string::npos == first)
    {
        return str;
    }
    std::string::size_type last = str.find_last_not_of(' ');
    return str.substr(first, (last - first + 1));
}

std::string RFLGWInfoLED::readPortFromFile(const char* filename) {
    std::ifstream ifs;
    ifs.open(filename, std::ifstream::in);
    char* buffer = new char[256];
    std::string base("01234567890");
    while(ifs.good()) {
        ifs.getline(buffer, 256);
        std::string port(buffer);
        port = trim(port);
        if(!port.empty() && (port.find_first_not_of(base) == std::string::npos)) {
            delete[] buffer;
            return port;
        }
    }
    delete[] buffer;
    return std::string("");
}

led::LedState RFLGWInfoLED::getLedState() {
  return lastState;
}

void RFLGWInfoLED::switchLed(enum led::LedState state) {
  switch(state) {
    case led::LED_OFF:
      ledOff();
    break;
    case led::LED_ON:
      ledOn();
    break;
    case led::LED_SLOW:
      ledFlashSlow();
    break;
    case led::LED_FAST:
      ledFlashFast();
    break;
    default:
      // nothing
    break;
  }
}

void RFLGWInfoLED::initRFDPort() {
	std::string port(readPortFromFile("/etc/rfd.port"));
	if(!port.empty()) {
		rfdPort = port;
	}
}

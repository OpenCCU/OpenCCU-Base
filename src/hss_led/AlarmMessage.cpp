/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/


#include "AlarmMessage.h"
#include "hss_led_util.h"

AlarmMessage::AlarmMessage() {
	// TODO Automatisch generierter Konstruktorstub

}

AlarmMessage::~AlarmMessage() {
	// TODO !CodeTemplates.destructorstub.tododesc!
}

bool AlarmMessage::isInfoPending() {
	std::map<std::string,bool>::iterator it;
	for(it = messages.begin();it != messages.end();++it)
	{
		if(it->second)
		{
			return true;
		}
	}

	return false;
}

bool AlarmMessage::isInfoEnabled(const std::map<std::string, std::string>& configData) {
	return !(HSSLedUtil::stringToBool( getConfigValue(configData, ConfigValue::IGNORE_ALARM_MESSAGES) ));
}

void AlarmMessage::setMessage(std::string source, bool value) {
	messages[source] = value;
}

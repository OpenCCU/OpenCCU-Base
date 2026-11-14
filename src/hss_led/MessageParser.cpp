/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "MessageParser.h"
#include <string>
#include <vector>
#include <utils.h>

MessageParser::MessageParser()
  : lastCommand(NONE),
    lastAlarmValue(false),
    lastServicevalue(0) {
	// TODO Automatisch generierter Konstruktorstub

}

MessageParser::~MessageParser() {
	// TODO !CodeTemplates.destructorstub.tododesc!
}

bool MessageParser::parsMessage(std::string& msg) {
	bool ret = false;
		std::vector<std::string> v;
		std::vector<std::string>::iterator iter;
		std::vector<std::string> v_assign;
		std::string key;
		std::string val;
		std::string sender = ""; //Sender-Prozess des UDP-Pakets.


		//LOG(Logger::LOG_DEBUG, "%s", msg.c_str());

		v = SplitString(msg, "\n");

		iter = v.begin();

		//Von welchen Prozessen lassen wir Status-Nachrichten zu?
		if (*iter != "RFD" && *iter != "POWER" && *iter != "LIGHTTPD" && *iter != "HS485D" && *iter != "REGAHSS" && *iter != "HMIP") return false;

		sender = *iter;
		++iter;

		if(iter!=v.end())
		{
			v_assign = SplitString(*iter, "=");

			if (v_assign.size() != 2) return false;

			key = v_assign[0];
			val = v_assign[1];

			if(key == "SERVICE") {
				this->lastCommand = SERVICE;
				this->lastServicevalue =StrToInt(val);
			}
			else if (key == "ALERT")
			{
				this->lastCommand = ALARM;
				this->lastAlarmValue = StrToBool(val);
			}else if (key == "ALARM")
			{
				this->lastCommand = ALARM;
				this->lastAlarmValue = StrToBool(val);
			}
			else{
				this->lastCommand = NONE;
			}
			this->lastMessageSource = sender;
			v_assign.clear();

			//LOG(Logger::LOG_DEBUG, "HandleStatusMsg %s", key.c_str());
		}

		ret = true;
		return ret;
}

Commads_t MessageParser::getCommandType() {
	return this->lastCommand;
}

bool MessageParser::getAlarmValue() {
	return this->lastAlarmValue;
}
int MessageParser::getServiceValue()
{
return this->lastServicevalue;
}

std::string MessageParser::getMessageSource() {
	return this->lastMessageSource;
}

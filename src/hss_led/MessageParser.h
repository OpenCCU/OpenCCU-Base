/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/


#ifndef MESSAGEPARSER_H_
#define MESSAGEPARSER_H_
#include <string>
typedef enum Commands_e
{
	SERVICE,
	ALARM,
	UPDATAE,
	NETWORK,
	NONE,
}Commads_t;
class MessageParser {
public:
	MessageParser();
	virtual ~MessageParser();
	bool parsMessage(std::string &msg);
	Commads_t getCommandType();
	bool getAlarmValue();
	int getServiceValue();
	std::string getMessageSource();
private:
	Commads_t lastCommand;
	bool lastAlarmValue;
	int lastServicevalue;
	std::string lastMessageSource;
};

#endif /* MESSAGEPARSER_H_ */

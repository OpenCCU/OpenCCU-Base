/*
 * MessageParser.h
 *
 *  Created on: 18.01.2013
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

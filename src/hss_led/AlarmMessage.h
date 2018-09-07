/*
 * AlarmMessage.h
 *
 *  Created on: 18.01.2013
 *      Author: willms
 */

#ifndef ALARMMESSAGE_H_
#define ALARMMESSAGE_H_
#include <map>
#include "Info.h"
#include <string>
class AlarmMessage: public Info {
public:
	AlarmMessage();
	virtual ~AlarmMessage();
	virtual bool isInfoPending();
	virtual bool isInfoEnabled(const std::map<std::string, std::string>& configData);
	void setMessage(std::string source, bool value);
private:
	std::map<std::string,bool> messages;
};

#endif /* ALARMMESSAGE_H_ */

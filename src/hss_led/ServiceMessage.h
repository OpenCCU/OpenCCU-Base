/*
 * ServiceMessage.h
 *
 *  Created on: 18.01.2013
 *      Author: willms
 */

#ifndef SERVICEMESSAGE_H_
#define SERVICEMESSAGE_H_
#include <map>
#include <string>
#include "Info.h"

class ServiceMessage: public Info {
public:
	ServiceMessage();
	virtual ~ServiceMessage();
	virtual bool isInfoPending();
	virtual bool isInfoEnabled(const std::map<std::string, std::string>& configData);
	void setMessage(std::string souce, int value);
private:
	std::map<std::string,int> messages;

};

#endif /* SERVICEMESSAGE_H_ */

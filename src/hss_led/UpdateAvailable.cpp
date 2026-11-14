/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/


#if !defined(PLATFORM_CCU3)

#include <fcntl.h>
#include "UpdateAvailable.h"
#include <utils.h>
#include <vector>
#include <unistd.h>
#include <cstdlib>
#include <xmlParser.h>
#include <Logger.h>

UpdateAvailable::UpdateAvailable() :
		updateAvailable(false) {
	// TODO Automatisch generierter Konstruktorstub
	char data[1024];
	int versionFile = open("/VERSION", O_RDONLY);
	if (versionFile < 0) {
	  versionFile = open("/boot/VERSION", O_RDONLY);
	}
	if (versionFile >= 0) {
		const ssize_t bytesRead = read(versionFile, data, sizeof(data));
		close(versionFile);
		std::string versionContetn;
		if (bytesRead > 0) {
			versionContetn.assign(data, static_cast<size_t>(bytesRead));
		}
		std::vector<std::string> versionVec = SplitString(
				versionContetn.c_str(), "=");
		if (versionVec.size() < 2) {
			this->currentVersion = "unknown";
		} else {
			this->currentVersion = versionVec[1];
		}
	} else {
		this->currentVersion = "unknown";
	}
	this->lastUpdateServerRequest = time_millis();
	this->waitRequesTime = 20000 + (rand() % 20000);
	int idsFile = open("/var/ids",O_RDONLY);
	if(idsFile >= 0)
	{
		const ssize_t bytesRead = read(idsFile, data, sizeof(data));
		close(idsFile);
		std::string idsContent;
		if (bytesRead > 0) {
			idsContent.assign(data, static_cast<size_t>(bytesRead));
		}
		std::vector<std::string> idsLines = SplitString(idsContent,"\n");
		std::vector<std::string>::iterator itLines;
		for(itLines = idsLines.begin(); itLines != idsLines.end(); ++itLines)
		{
			if(itLines->find("SerialNumber",0)!= std::string::npos)
			{
				std::vector<std::string> serial = SplitString(*itLines,"=");
				if(serial.size() >=2)
				{
					this->serialNumber = serial[1];
				}
			}
		}
	}
}

UpdateAvailable::~UpdateAvailable() {
	// TODO !CodeTemplates.destructorstub.tododesc!
}

bool UpdateAvailable::isInfoPending() {
	if(time_millis() - this->lastUpdateServerRequest > this->waitRequesTime)
	{
		this->requestUpdateServer();
		this->lastUpdateServerRequest = time_millis();
		this->waitRequesTime = 86400000 + rand()%3600000;
	}
	return this->updateAvailable;
}

bool UpdateAvailable::requestUpdateServer() {
	std::string systemCommand = "wget -T 3 -q -O /tmp/avilableversion.html \"http://update.homematic.com/firmware/download?cmd=check_version&serial=";
	systemCommand += this->serialNumber;
  #if defined(PLATFORM_CCU3)
	systemCommand += "&product=HM-CCU3\"";
  #else
	systemCommand += "&product=HM-CCU2\"";
  #endif
	if (system(systemCommand.c_str()) != 0) {
		LOG(Logger::LOG_ERROR, "UpdateAvailable::requestUpdateServer(): wget failed");
		return false;
	}
	XMLResults xmlResult;
	XMLNode rootNode = XMLNode::parseFile( "/tmp/avilableversion.html", "html", &xmlResult );
	if(!xmlResult.error)
	{
		if(rootNode.isEmpty())
		{
			LOG(Logger::LOG_ERROR,"node is empty");
		}
        XMLNode body = rootNode.getChildNode("body");
        XMLNode div = body.getChildNode("div");
		if(!div.isEmpty())
		{
			this->availableVersion =  div.getText();
			//LOG(Logger::LOG_INFO,"availableversion %s",this->availableVersion.c_str());
			if(this->currentVersion.find(this->availableVersion.c_str()) == std::string::npos)
			{
				this->updateAvailable = true;
				return true;
			}
			return true;
		}
	}
	return false;
}

#endif

/*
 * Network.cpp
 *
 *  Created on: 18.01.2013
 *      Author: willms
 */

#include "Network.h"
#include <fcntl.h>
#include <unistd.h>
#include <Logger.h>
#include <stdlib.h>
#include <sys/stat.h>

Network::Network() :
		netState(Disconnected), linkFileErrorOutput(false), ipFileErrorOutput(
				false), internetFileErrorOutput(false), checkInternetInterval(50) {
	// TODO Automatisch generierter Konstruktorstub

}

Network::~Network() {
	// TODO !CodeTemplates.destructorstub.tododesc!
}

bool Network::isInfoPending() {
	struct stat buffer;
	this->netState = Disconnected;

	if (stat("/var/status/hasLink", &buffer) == 0) {
		this->netState = Network::LinkUp;
		linkFileErrorOutput = false;
	} else {
		if (!linkFileErrorOutput) {
			LOG(Logger::LOG_DEBUG,
					"Network::CheckNetState(): can't open hasLink file");
			linkFileErrorOutput = true;
		}
		return true;
	}

	if (stat("/var/status/hasIP", &buffer) == 0) {
		this->netState = Network::IP;
		ipFileErrorOutput = false;
	} else {
		if (!ipFileErrorOutput) {
			LOG(Logger::LOG_DEBUG,
					"Network::CheckNetState(): can't open hasIP file");
			ipFileErrorOutput = true;
		}
		return true;
	}

	// run the /bin/checkInternet script every X'th interval
	if (--checkInternetInterval <= 0) {
		if (stat("/bin/checkInternet", &buffer) == 0)
			system("/bin/checkInternet");

		checkInternetInterval = 50;
	}

	if (stat("/var/status/hasInternet", &buffer) == 0) {
		this->netState = Network::InternetAvailable;
		internetFileErrorOutput = false;
	} else {
		if (!internetFileErrorOutput) {
			LOG(Logger::LOG_DEBUG,
					"Network::CheckNetState(): can't open hasInternet file");
			internetFileErrorOutput = true;
		}
		return true;
	}

	return true;
}

Network::NetworkState Network::getNetState() {
	return this->netState;
}

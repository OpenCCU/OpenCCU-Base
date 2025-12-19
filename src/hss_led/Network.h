/*
 * Network.h
 *
 *  Created on: 18.01.2013
 */

#ifndef NETWORK_H_
#define NETWORK_H_
#include "Info.h"
class Network:public Info {
public:
	enum NetworkState
	{
		Disconnected,
		LinkUp,
		IP,
		InternetAvailable,
		Invalid = 0xff,
	};
	Network();
	virtual ~Network();
	virtual bool isInfoPending();
	NetworkState getNetState();
private:
	NetworkState netState;
	bool linkFileErrorOutput;
	bool ipFileErrorOutput;
	bool internetFileErrorOutput;
	int checkInternetInterval;
};

#endif /* NETWORK_H_ */

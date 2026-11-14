/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
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

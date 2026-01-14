#ifndef _LIBLANUTILS_NETWORKINTERFACEHELPER_H_
#define _LIBLANUTILS_NETWORKINTERFACEHELPER_H_

#include <vector>
#include <string>

class NetworkInterfaceHelper
{
public:
	NetworkInterfaceHelper(void);
	~NetworkInterfaceHelper(void);

public:
	std::vector<std::string> getIPv4Adresses();
};

#endif
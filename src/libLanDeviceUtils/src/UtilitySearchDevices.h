#ifndef _LIBLANUTILS_UTILITY_SEARCH_DEVICES_H_
#define _LIBLANUTILS_UTILITY_SEARCH_DEVICES_H_

#include <vector>
#include <string>
#include <LanDeviceUtilsTypes.h>

namespace LDU {

class LanDevice;
class Protocol;

class UtilitySearchDevices
{
public:
	UtilitySearchDevices(void);
	~UtilitySearchDevices(void);

	bool searchDevices(const std::vector<std::string>& devTypeFilters, const int protocols, const int routingSchemes, const std::string& firstAddress, const std::string& secondAddress, std::vector<LanDevice>& devices);
	bool searchDeviceBySerial(const std::string& serial, LanDevice& lanDevice);
	bool searchDeviceByTypeAndSerial(const std::string devType, const std::string& serial, LanDevice& dev);

private:	
	std::vector<LanDevice> sendMessage(const std::string& msg, const Protocol* pProt, const std::string& address, const enum RoutingSchemeEnum& routingScheme);
	void mergeLanDeviceVectorAIntoB(const std::vector<LanDevice>& a, std::vector<LanDevice>& b) const;
	std::vector<std::string> createAddressesByRoutingSchemes(const int routingSchemes, const std::string& addrFrom, const std::string& addrTo) const;
	std::vector<Protocol*> createProtocolsByProtocolSchemeInt(const int protocols) const;
};
}
#endif

#ifndef _LIBLANDEVICEUTILS_CONSTANTS_H_
#define _LIBLANDEVICEUTILS_CONSTANTS_H_

namespace LDU {

class Constants {
public:
	//! Constant containing IPv4 multicast address 224.0.0.1
	static const char* MULTICAST_ADDRESS;
	//! Constant containing IPv4 broadcast address 255.255.255.255
	static const char* BROADCAST_ADDRESS;
};
}
#endif
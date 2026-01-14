#ifndef _LIBLANUTILS_UTILITY_DEVICE_CONFIGURATION_H_
#define _LIBLANUTILS_UTILITY_DEVICE_CONFIGURATION_H_

#include <vector>
#include <string>
#include <LanDeviceUtilsTypes.h>
#include <Protocol.h>
#include <Constants.h>
#include <UDPDatagramSender.h>

namespace LDU {

class UtilityDeviceConfiguration
{
public:
	UtilityDeviceConfiguration(void);
	~UtilityDeviceConfiguration(void);

	static bool loadTestStatus(LanDevice& dev);
	static bool changeTestStatus(LanDevice& dev, const TestStatusConfiguration& testStatusConfiguration);
	static bool rebootDevice(LanDevice& dev);
	static bool resetDevice(LanDevice& dev);
	static bool sendUserDataToDevice(LanDevice& dev, unsigned char userData[], const unsigned char AMOUNT);
	static bool enterBootloader(LanDevice& dev);
	static bool enterApplication(LanDevice& dev);
	static bool keyExchange(LanDevice& dev, unsigned char newKey[], const unsigned char AES_KEY_LENGTH);

private:
	enum DeviceState
	{
		DEVICE_STATE_APP,
		DEVICE_STATE_BOOTLOADER
	};

	static bool validateDeviceState(std::string serial, DeviceState deviceState, LDU::LanDevice &dev);
};
}
#endif
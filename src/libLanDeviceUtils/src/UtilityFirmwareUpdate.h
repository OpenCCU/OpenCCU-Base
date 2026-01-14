#ifndef _LIBLANUTILS_UTILITY_FIRMWARE_UPDATE_H_
#define _LIBLANUTILS_UTILITY_FIRMWARE_UPDATE_H_

#include <vector>
#include <string>
#include <LanDeviceUtilsTypes.h>
#include <Protocol.h>
#include <Constants.h>
#include <UDPDatagramSender.h>

namespace LDU {

class UtilityFirmwareUpdate
{
public:
	UtilityFirmwareUpdate(void);
	~UtilityFirmwareUpdate(void);
	static bool doFirmwareUpdate(LDU::LanDevice &dev, std::string file);

private:
	struct FirmwareFrame {
		int index;
		int length;
		unsigned char* data;
	};

	static bool readFirmwareFile(std::string file, std::vector<FirmwareFrame>* frameList);
	static unsigned char ConvertHexStringToByte(char high, char low);
	static unsigned char ConvertHexCharToByte(char value);
};
}
#endif
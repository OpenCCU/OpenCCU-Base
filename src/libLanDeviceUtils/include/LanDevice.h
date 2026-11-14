/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LIBLANDEVICEUTILS_LANDEVICE_H_
#define _LIBLANDEVICEUTILS_LANDEVICE_H_

//Includes
#include <string>
#include <vector>
#include <IPConfiguration.h>
#include <RuntimeIPConfiguration.h>
#include <LanDeviceUtilsTypes.h>
#include <TestStatusConfiguration.h>

#include <DLLImportExport.h>

namespace LDU {

//Class def
class LanDevice
{
public:
	LIBLANDEVICEUTILS_API LanDevice(ProtocolEnum protocolType, RoutingSchemeEnum reachedbyRoutingScheme);
	LIBLANDEVICEUTILS_API LanDevice();
	LIBLANDEVICEUTILS_API virtual ~LanDevice();

    LIBLANDEVICEUTILS_API enum Status
    {
        STATUS_ERROR,
        STATUS_UNKNOWN,
        STATUS_OFFLINE,
        STATUS_ONLINE,
        STATUS_BOOTLOADER,
        STATUS_REBOOT
    };

	LIBLANDEVICEUTILS_API struct ServiceProtocol 
	{
		int id;
		int port;
	};

private:
	static const int AES_BLOCK_SIZE = 16;
	static const int AES_KEY_AND_IV_LENGTH = 16;
	std::string serialNumber;
	std::string type;
	std::string firmwareVersion;
	std::vector<ServiceProtocol> serviceProtocols;

	unsigned char* aesKey;
	unsigned char* iv;
	int lengthOfAesKeyAndIv;

	//std::string lanIfCfgProtocolName;
	ProtocolEnum protocolType;
	IPConfiguration ipConfiguration;
	RuntimeIPConfiguration runtimeIPConfiguration;
	TestStatusConfiguration testStatusConfiguration;
	Status status;
	RoutingSchemeEnum reachedByRoutingScheme;

	void GetUnsignedCharArrayForAes(std::string str, unsigned char result[]);
	std::string filterEncryptedMessage(unsigned char encryptedMsg[], int amountOfAesBlocks);
	std::string assembleHeaderFrameBase(const std::string& devType, const std::string& serial, int& headerLength);
	void parseResponseToAesIv(const std::string responseWithInv, unsigned char aesIv[16]); // ToDo: unsigned char -> uint8_t, 16 als Konstante
	void convertStringToUnsignedCharArray(std::string str, unsigned char result[]);
	void convertUnsignedCharArrayToString(
		const unsigned char* notEncryptedPartOfMessage,
		const int indexNotEncryptedPartOfMessage,
		const unsigned char* encryptedPartOfMessage,
		const int indexEncryptedPartOfMessage,
		std::string& result);

public:
	LIBLANDEVICEUTILS_API const std::string& getSerialNumber() const;
	LIBLANDEVICEUTILS_API void setSerialNumber(const std::string& serialNumber);

	LIBLANDEVICEUTILS_API const std::string& getType() const;
	LIBLANDEVICEUTILS_API void setType(const std::string& typeName);

	LIBLANDEVICEUTILS_API const std::string& getFirmwareVersion() const;
	LIBLANDEVICEUTILS_API void setFirmwareVersion(const std::string& v);

	LIBLANDEVICEUTILS_API const std::vector<ServiceProtocol>& getServiceProtocols() const;
	LIBLANDEVICEUTILS_API void setServiceProtocols(const std::vector<ServiceProtocol>& serviceProtocols);

	LIBLANDEVICEUTILS_API std::string getLanIfCfgProtocolName() const;

	LIBLANDEVICEUTILS_API ProtocolEnum getProtocolType() const;
	LIBLANDEVICEUTILS_API void setProtocolType(ProtocolEnum protocolType);

	LIBLANDEVICEUTILS_API const IPConfiguration& getIPConfiguration() const;
	LIBLANDEVICEUTILS_API void setIPConfiguration(const IPConfiguration& ipConfig);

	LIBLANDEVICEUTILS_API const RuntimeIPConfiguration& getRuntimeIPConfiguration() const;
	LIBLANDEVICEUTILS_API void setRuntimeIPConfiguration(const RuntimeIPConfiguration& ipConfig);

	LIBLANDEVICEUTILS_API const TestStatusConfiguration& getTestStatusConfiguration() const;
	LIBLANDEVICEUTILS_API void setTestStatusConfiguration(const TestStatusConfiguration& testStatusConfiguration);

	LIBLANDEVICEUTILS_API void setAesKey(unsigned char aesKey[], int lengthOfAesKeyAndIv);
	LIBLANDEVICEUTILS_API unsigned char* getAesKey();
	LIBLANDEVICEUTILS_API void setIv(const std::string responseWithInv);
	LIBLANDEVICEUTILS_API void setIv(unsigned char iv[]);
	LIBLANDEVICEUTILS_API unsigned char* getIv();

	//LIBLANDEVICEUTILS_API void getAesKeyAsUInt8t(uint8_t* aesKey);
	LIBLANDEVICEUTILS_API int getLengthOfAesKey();
	LIBLANDEVICEUTILS_API std::string encryptMessage(unsigned char opcode, unsigned char data[], int dataLength);
	LIBLANDEVICEUTILS_API std::string encryptMessageALT(std::string msg);

	LIBLANDEVICEUTILS_API const Status& getStatus() const;

	LIBLANDEVICEUTILS_API const RoutingSchemeEnum& getReachedByRoutingScheme() const;
	LIBLANDEVICEUTILS_API void setReachedByRoutingScheme(RoutingSchemeEnum reachedByRoutingScheme);
};
}
#endif

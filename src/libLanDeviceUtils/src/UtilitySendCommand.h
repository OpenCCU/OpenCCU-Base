/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LIBLANUTILS_UTILITY_SEND_COMMAND_H_
#define _LIBLANUTILS_UTILITY_SEND_COMMAND_H_

#include <vector>
#include <string>
#include <LanDeviceUtilsTypes.h>

namespace LDU {

class LanDevice;
class Protocol;

class UtilitySendCommand
{
public:
	static std::string lastResponse;
	static bool resentCommandEncrypted;
	static bool resentResponseEmpty;
	static bool lastSendWasEncrypted;

	UtilitySendCommand(void);
	~UtilitySendCommand(void);
	static bool createCommand(Protocol* pProt, LanDevice& dev, unsigned char opcode, unsigned char* data, int dataLength, bool resendWithSameCounter = false);
	static bool createEncryptedCommand(Protocol* pProt, LanDevice& dev, unsigned char opcode, unsigned char* data, int dataLength, bool resendWithSameCounter = false);
	static bool getLastResponse(int* length, unsigned char* response);

private:
	static std::string getMessage(const std::string& devType, const std::string& serial, const unsigned char opcode, const unsigned char* data, const int dataLength);
	static bool sendCommand(Protocol* pProt, LanDevice& dev, std::string msg, unsigned char opcode);
	static bool useBroadcast(std::string ipAdress);
};
}
#endif
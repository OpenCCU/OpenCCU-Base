/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

//#pragma once
#ifndef _LIBLANDEVICEUTILS_PROTOCOL_H
#define _LIBLANDEVICEUTILS_PROTOCOL_H

#include <string>
#include <vector>
#include <LanDevice.h>
#include <LanDeviceUtilsTypes.h>
#include <TestStatusConfiguration.h>

namespace LDU {

class Protocol 
{
public:

	virtual ProtocolEnum getProtocolType() const = 0;
	virtual const std::string getProtocolName() const = 0; 
	
	virtual unsigned int getProtocolDefaultPort() const = 0;
	virtual unsigned int getProtocolDefaultReplyPort() const = 0;

	virtual std::string getIdentifyFrame(const std::string& devType, const std::string& serial) const = 0;
	virtual std::vector<LanDevice> parseIdentifyResponses( const std::vector<std::string>& responses) const = 0;

	virtual std::string getRuntimeIPConfigRequestFrame(const std::string& devType, const std::string& serial) const = 0;
	virtual bool parseRuntimeIPConfigRequestResponse( const std::string& response, const std::string& expectedSerial, RuntimeIPConfiguration& ipConfig ) const = 0;

	virtual std::string getGetNetworkConfigurationFrame(const std::string& devType, const std::string& serial) const = 0;
	virtual std::string getSetNetworkConfigurationFrame(const std::string& devType, const std::string& serial, const IPConfiguration& newIPConfiguration) const = 0;
	virtual bool parseGetNetworkConfigurationFrameResponse(const std::string& response, const std::string& expectedSerial, IPConfiguration& ipConfig ) const = 0;

	virtual bool parseTestStatus(const std::string& response, const std::string& expectedSerial, TestStatusConfiguration& testStatusConfiguration) const = 0;
	
	/** \brief  Parses general (n)ack message which does not contain cmd specific payload data.
    *  \param  response Response message to parse. (IN)
	*  \param  returnCode Return code for further evaluation. (OUT)
	*  \param  errorMessage Optional: Error message, if any. (OUT)
	*  \return True on Ack, otherwise False.*/
	virtual bool parseAckRespone(const std::string& response, char* requestOpcode = 0, int* returnCode = 0, std::string* errorMessage = 0) const = 0;

	/** \brief Convenience function to create protocol by given type.
	 * \details Callers must assure to delete the created object.
	 */
	static Protocol* createProtocol(const ProtocolEnum& protType);
//protected:
//	Protocol(void);
	virtual ~Protocol(void);
private:
	
};

}
#endif

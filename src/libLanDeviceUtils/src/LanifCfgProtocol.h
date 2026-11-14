/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LIBLANDEVICEUTILS_LANIFCFGPROTOCOL_H_
#define _LIBLANDEVICEUTILS_LANIFCFGPROTOCOL_H_
#include "Protocol.h"

namespace LDU {

class LanifCfgProtocol :
	public Protocol
{
public:
	LanifCfgProtocol(void);
	virtual ~LanifCfgProtocol(void);

	virtual ProtocolEnum getProtocolType() const;
	virtual const std::string getProtocolName() const;
	virtual unsigned int getProtocolDefaultReplyPort() const;

	virtual unsigned int getProtocolDefaultPort() const;

	virtual std::string getIdentifyFrame(const std::string& devType, const std::string& serial) const;
	virtual std::vector<LanDevice> parseIdentifyResponses( const std::vector<std::string>& responses) const;

	virtual std::string getRuntimeIPConfigRequestFrame(const std::string& devType, const std::string& serial) const;
	virtual bool parseRuntimeIPConfigRequestResponse( const std::string& response, const std::string& expectedSerial, RuntimeIPConfiguration& ipConfig) const;

	virtual std::string getGetNetworkConfigurationFrame(const std::string& devType, const std::string& serial) const;
	virtual std::string getSetNetworkConfigurationFrame(const std::string& devType, const std::string& serial, const IPConfiguration& newIPConfiguration) const;
	virtual bool parseGetNetworkConfigurationFrameResponse(const std::string& response, const std::string& expectedSerial, IPConfiguration& ipConfig ) const;

	virtual bool parseTestStatus(const std::string& response, const std::string& expectedSerial, TestStatusConfiguration& testStatusConfiguration) const;
	
   /** \brief  Parses general (n)ack message which does not contain cmd specific payload data.
    *  \param  response Response message to parse. (IN)
	*  \param  returnCode Return code for further evaluation. (OUT)
	*  \param  errorMessage Optional: Error message, if any. (OUT)
	*  \return True on Ack, otherwise False.*/
	virtual bool parseAckRespone(const std::string& response, char* requestOpcode = 0, int* returnCode = 0, std::string* errorMessage = 0) const;

private:
	/** \brief Assembles frame header base (without opcode). */
	std::string assembleFrameHeaderBase(const std::string& devType, const std::string& serial) const;
	inline void appendAddress(std::string& str, const std::string& addr) const;
};
}
#endif

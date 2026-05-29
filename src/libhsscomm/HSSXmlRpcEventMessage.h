/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HSSXMLRPCEVENTMESSAGE_H_
#define _HSSXMLRPCEVENTMESSAGE_H_

#include <XmlRpc.h>
#include "ActorMessage.h"
#include "dllexport.h"

class HSSXmlRpcEventMessage: public EQ3::ActorMessage
{
public:
	HSSXmlRpcEventMessage(const std::string& address, const std::string& valueKey, const XmlRpc::XmlRpcValue& value);
	virtual ~HSSXmlRpcEventMessage();
	const std::string& GetAddress() const;
	const std::string& GetValueKey() const;
	const XmlRpc::XmlRpcValue& GetValue() const;	
	virtual HSSXmlRpcEventMessage* Clone() const;
private:
	std::string m_address;
	std::string m_valueKey;
	XmlRpc::XmlRpcValue m_value;
};

#endif

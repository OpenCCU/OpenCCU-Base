/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "HSSXmlRpcEventMessage.h"

using namespace EQ3;
using namespace std;
using namespace XmlRpc;

HSSXmlRpcEventMessage::HSSXmlRpcEventMessage(const string& address, const string& valueKey, const XmlRpcValue& value)
{
	m_address = address;
	m_valueKey = valueKey;
	m_value = value;
}

HSSXmlRpcEventMessage::~HSSXmlRpcEventMessage()
{
}

const string& HSSXmlRpcEventMessage::GetAddress() const
{
	return m_address;
}

const string& HSSXmlRpcEventMessage::GetValueKey() const
{
	return m_valueKey;
}

const XmlRpcValue& HSSXmlRpcEventMessage::GetValue() const
{
	return m_value;
}

HSSXmlRpcEventMessage* HSSXmlRpcEventMessage::Clone() const
{
	return new HSSXmlRpcEventMessage(*this);
}

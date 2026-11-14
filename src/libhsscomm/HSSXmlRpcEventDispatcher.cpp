/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <XmlRpc.h>
#include <vector>

#include "HSSXmlRpcEventDispatcher.h"
#include "HSSXmlRpcEventMessage.h"
#include "Logger.h"


using namespace EQ3;
using namespace std;
using namespace XmlRpc;

HSSXmlRpcEventDispatcher::HSSXmlRpcEventDispatcher(string const& url, string const& id)
	: Actor(), m_pClient(NULL)
	, m_clientSendFailureCount(0)
	, m_clientUnreachable(false)
{
	m_pClient = new XmlRpc::XmlRpcClient(url);
	m_url = url;
	m_id = id;
	m_pClient->setTimeout(10000);
}

HSSXmlRpcEventDispatcher::~HSSXmlRpcEventDispatcher()
{
	Actor::Stop();
	m_pClient->close();
	delete m_pClient;
}

void HSSXmlRpcEventDispatcher::Handle(list<ActorMessage*> const& messages)
{
	XmlRpcValue multicall;

	list<ActorMessage*>::const_iterator it;
	int index = 0;
	for (it = messages.begin(); it != messages.end(); it++, index++)
	{
		ActorMessage* message = *it;
		HSSXmlRpcEventMessage* eventMessage = dynamic_cast<HSSXmlRpcEventMessage*>(message);
		if (eventMessage)
		{
			XmlRpcValue params;
			params[0] = m_id;
			params[1] = eventMessage->GetAddress();
			params[2] = eventMessage->GetValueKey();
			params[3] = eventMessage->GetValue();

			XmlRpcValue& call = multicall[index];
			call["methodName"] = "event";
			call["params"] = params;
		}
	}

	if (index > 0)
	{
		LOG(Logger::LOG_DEBUG, "HSSXmlRpcEventDispatcher::Handle send %d events", index); 

		XmlRpcValue result;
		XmlRpcValue params;
		params[0] = multicall;
		bool executed = m_pClient->execute("system.multicall", params, result);
		if(!executed) {
			LOG(Logger::LOG_WARNING, "XmlRpc transport failed (first try), retrying...");
			executed = m_pClient->execute("system.multicall", params, result);
		}
		if(!executed)
		{
			LOG(Logger::LOG_ERROR, "XmlRpcClient error calling %s(%s) on %s:", "event", multicall.toText().c_str(), m_pClient->getURL().c_str());
			LOG(Logger::LOG_ERROR, "XmlRpc transport error");
			m_clientSendFailureCount++;
			if(m_clientSendFailureCount >= 10) {//10 chosen by dice
				m_clientUnreachable = true;
			}
		}
		else
		{
			if(m_pClient->isFault())
			{
				LOG(Logger::LOG_ERROR, "XmlRpcClient error calling %s(%s) on %s:", "event", multicall.toText().c_str(), m_pClient->getURL().c_str());
				LOG(Logger::LOG_ERROR, "XmlRpc fault: %s", result.toText().c_str());
			}
			else {
				m_clientSendFailureCount = 0;
			}
		}
		LOG(Logger::LOG_DEBUG, "HSSXmlRpcEventDispatcher::Handle send completed");
	}

}

const string& HSSXmlRpcEventDispatcher::GetUrl() const
{
	return m_url;
}

const string& HSSXmlRpcEventDispatcher::GetId() const
{
	return m_id;
}

bool HSSXmlRpcEventDispatcher::isClientUnreachable() const 
{
	return m_clientUnreachable;
}

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _LIBHSSCOMM_HSSXMLRPCEVENTDISPACHER_H_
#define _LIBHSSCOMM_HSSXMLRPCEVENTDISPACHER_H_

#include <string>
#include <XmlRpc.h>
#include "Actor.h"

class HSSXmlRpcEventDispatcher: public virtual EQ3::Actor
{
public:
//	HSSXmlRpcEventDispatcher();
	HSSXmlRpcEventDispatcher(std::string const& url, std::string const& id);
	virtual ~HSSXmlRpcEventDispatcher();
	const std::string& GetUrl() const;
	const std::string& GetId() const;

	bool isClientUnreachable() const;
protected:
	virtual void Handle(list<EQ3::ActorMessage*> const& messages);
private:
	XmlRpc::XmlRpcClient* m_pClient;
	std::string m_url;
	std::string m_id;
	unsigned int m_clientSendFailureCount;
	bool m_clientUnreachable;
};

#endif

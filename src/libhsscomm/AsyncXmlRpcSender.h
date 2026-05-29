/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _ASYNCXMLRPCSENDER_H_
#define _ASYNCXMLRPCSENDER_H_

#include "dllexport.h"

#include <XmlRpc.h>
#include <string>

class DLLEXPORT AsyncXmlRpcSender
{
public:
	AsyncXmlRpcSender(const std::string& url);
	~AsyncXmlRpcSender(void);
	void AsyncCall(const std::string& method, XmlRpc::XmlRpcValue& params);
protected:
	static void* _ThreadFunction(void* arg);
	void* ThreadFunction();
	XmlRpc::XmlRpcClient c;
	std::string method;
	XmlRpc::XmlRpcValue params;
};

#endif

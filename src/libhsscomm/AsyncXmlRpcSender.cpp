/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <pthread.h>

#include "AsyncXmlRpcSender.h"
#include "Logger.h"

using namespace XmlRpc;

AsyncXmlRpcSender::AsyncXmlRpcSender(const std::string& url):c(url)
{
}

AsyncXmlRpcSender::~AsyncXmlRpcSender(void)
{
}

void AsyncXmlRpcSender::AsyncCall(const std::string& method, XmlRpc::XmlRpcValue& params)
{
	this->method=method;
	this->params=params;
	pthread_t new_thread;
	pthread_attr_t attr;
	pthread_attr_init(&attr);
	pthread_attr_setstacksize(&attr, 512*1024);
	pthread_create(&new_thread, &attr, _ThreadFunction, reinterpret_cast<void*>(this));
	pthread_attr_destroy(&attr);
}

void* AsyncXmlRpcSender::_ThreadFunction(void* arg)
{
	AsyncXmlRpcSender* This=reinterpret_cast<AsyncXmlRpcSender*>(arg);
	pthread_detach(pthread_self());
	return This->ThreadFunction();
}

void* AsyncXmlRpcSender::ThreadFunction()
{
	XmlRpcValue result;
	if(!c.execute(method.c_str(), params, result)){
		LOG(Logger::LOG_ERROR, "XmlRpcClient error calling %s(%s) on %s:", method.c_str(), params.toText().c_str(), c.getURL().c_str());
		LOG(Logger::LOG_ERROR, "XmlRpc transport error");
	}else{
		if(c.isFault()){
			LOG(Logger::LOG_ERROR, "XmlRpcClient error calling %s(%s) on %s:", method.c_str(), params.toText().c_str(), c.getURL().c_str());
			LOG(Logger::LOG_ERROR, "XmlRpc fault: %s", result.toText().c_str());
		}
	}
	delete this;
	return 0;
}

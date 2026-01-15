/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HS485_LOGICAL_INSTANCE_H_
#define _HS485_LOGICAL_INSTANCE_H_

#include <string>
#include <XmlRpc.h>
#include <LogicalInstance.h>

class HS485Device;
class HS485CommMessage;

//! Abstrakte Basisklasse, die Gemeinsamkeiten von HS485Device und HS485Channel zusammenfasst
class HS485LogicalInstance: public LogicalInstance
{
public:
	HS485LogicalInstance(void);
	virtual ~HS485LogicalInstance(void);
	virtual bool SendMessage(HS485CommMessage* msg)=0;
	virtual int GetLogicalIndex(){return -1;};
	virtual int GetPhysicalIndex(){return -1;};
	virtual int GetEEPromIndex(){return 0;};
	virtual bool SendMessage(const std::string& msg, std::string* response)=0;
	virtual HS485Device* GetDevice()=0;
	inline void SetCurParamsetIndex(int index){cur_paramset_index=index;};
	inline int GetCurParamsetIndex(){return cur_paramset_index;};
	virtual bool Describe(XmlRpc::XmlRpcValue* val)=0;
	virtual bool GetParamsetId(const std::string& type, std::string* id){return false;};
	virtual const std::string& GetCurParamsetPeer(){static std::string null;return null;};
	virtual void SetCurParamsetPeer(const std::string& peer){};
	bool SetStoredValue(const std::string& id, const std::string& peer, XmlRpc::XmlRpcValue& param, int flags=0);
	bool GetStoredValue(const std::string& id, const std::string& peer, XmlRpc::XmlRpcValue* param);
	bool DeleteStoredValues(const std::string& peer);
	virtual bool ReportValueUsage(const std::string& value, int count){return true;};
	virtual bool ActivateLinkParamset(const std::string& peer, bool longpress){return false;};
protected:
	int cur_paramset_index;
};
#endif //_HS485_LOGICAL_INSTANCE_H_

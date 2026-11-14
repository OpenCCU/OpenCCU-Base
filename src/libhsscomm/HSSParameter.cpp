/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSParameter.cpp: Implementierung der Klasse HSSParameter.
//
//////////////////////////////////////////////////////////////////////

#include "HSSParameter.h"
#include "HSSLogicalType.h"
#include "HSSPhysicalType.h"
#include "HSSTypeConversion.h"
#include <typeinfo>
#include "type_registry.h"
#include <Logger.h>

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSParameter::HSSParameter()
:logical_type(0), physical_type(0), tab_order(-1), undefined(true)
{
	operations=(OP_READ|OP_WRITE);
	hidden=false;
	flags=FLG_VISIBLE;
	loopback=false;
	burst_suppression_time=1500;
	paramset=NULL;
	acceptForwardedValues = false;
}

HSSParameter::~HSSParameter()
{
//    LOG(Logger::LOG_DEBUG, "HSSParameter::~HSSParameter() this=%p", this);
    if(logical_type)delete logical_type;
    if(physical_type)delete physical_type;
	for(unsigned int i=0;i<type_conversions.size();i++){
		delete type_conversions[i];
	}
}

bool HSSParameter::InitFromXml(XMLNode &node, XMLNode &root_node)
{
    const char* temp=node.getAttribute("id");
    if(!temp)return false;
    id=temp;

    temp=node.getAttribute("operations");
	if(temp){
		operations=OP_NONE;
		if(strstr(temp, "read"))operations|=OP_READ;
		if(strstr(temp, "write"))operations|=OP_WRITE;
		if(strstr(temp, "event"))operations|=OP_EVENT;
		if(strstr(temp, "determine"))operations|=OP_DETERMINE;
	}
    temp=node.getAttribute("hidden");
	if(temp && temp[0]=='t')hidden=true;

    temp=node.getAttribute("loopback");
	if(temp)loopback=temp[0]=='t';

    temp=node.getAttribute("control");
	if(temp)control=temp;

    temp=node.getAttribute("burst_suppression");
	if(temp)burst_suppression_time=strtol(temp, NULL, 0);

    temp=node.getAttribute("ui_flags");
	if(temp){
		if(strstr(temp, "invisible"))flags &= ~FLG_VISIBLE;
		else if(strstr(temp, "visible"))flags|=FLG_VISIBLE;
		if(strstr(temp, "internal"))flags |= FLG_INTERNAL;
		if(strstr(temp, "transform"))flags |= FLG_TRANSFORM;
		if(strstr(temp, "service"))flags |= FLG_SERVICE;
		if(strstr(temp, "sticky"))flags |= FLG_STICKY;
	}

	if(node.getAttribute("has_write_dependencies")) {
		//TODO Implement me
		XMLNode dependenciesNode = node.getChildNode("write_dependencies");
		if(!dependenciesNode.isEmpty()) {
			XMLNode dependencyNode = dependenciesNode.getChildNode("write_dependency");
			int depencyNodeNr = 1;
			while(!dependencyNode.isEmpty()) {
				WriteDependency depend;
				temp = dependencyNode.getAttribute("param");
				if(temp) {
					const std::string param(temp);
					depend.name = param;
				temp = dependencyNode.getAttribute("command");
					if(temp) {
						depend.interfaceCommandParam = true;
					}
					writeDepencyParams.push_back(depend);
				}
				dependencyNode = dependenciesNode.getChildNode("write_dependency", depencyNodeNr);
				depencyNodeNr++;
			}
		}
	}

	temp = node.getAttribute("accept_forwarded_value");
	acceptForwardedValues = (temp != NULL && temp[0] == 't');

    //LOG(Logger::LOG_ERROR, "HSSParameter id=%s", id.c_str());

    XMLNode logical_node=node.getChildNode("logical");
    if(logical_node.isEmpty())return false;
    temp=logical_node.getAttribute("type");
    if(!temp)return false;
    std::string creation_tag="logical_type_";
    creation_tag+=temp;
    void* obj=hsscomm::type_registry::create(creation_tag.c_str());
    if(!obj){
        LOG(Logger::LOG_ERROR, "Logical type %s not supported", temp);
        return false;
    }
    HSSLogicalType* log_type=dynamic_cast<HSSLogicalType*>((HSSLogicalType*)obj);
    if(!log_type){
        /* TODO: get rid of the allocated memory */
        //delete obj;
        return false;
    }
    if(!log_type->InitFromXml(logical_node, root_node))return false;
    logical_type=log_type;

    XMLNode physical_node=node.getChildNode("physical");
    if(physical_node.isEmpty())return false;
    temp=physical_node.getAttribute("type");
    if(!temp)return false;
    creation_tag="physical_type_";
    creation_tag+=temp;
    obj=hsscomm::type_registry::create(creation_tag.c_str());
    if(!obj){
        LOG(Logger::LOG_ERROR, "Physical type %s not supported", temp);
        return false;
    }
    HSSPhysicalType* phys_type=dynamic_cast<HSSPhysicalType*>((HSSPhysicalType*)obj);
    if(!phys_type){
        /* TODO: get rid of the allocated memory */
        //delete obj;
        return false;
    }
	phys_type->SetParent(this);
    if(!phys_type->InitFromXml(physical_node, root_node))return false;
    physical_type=phys_type;

	int i=0;
    XMLNode conversion_node=node.getChildNode("conversion", &i);
    while(!conversion_node.isEmpty()){
        temp=conversion_node.getAttribute("type");
        if(!temp)return false;
        creation_tag="type_conversion_";
        creation_tag+=temp;
        obj=hsscomm::type_registry::create(creation_tag.c_str());
        if(!obj)return false;
        HSSTypeConversion* type_conversion=dynamic_cast<HSSTypeConversion*>((HSSTypeConversion*)obj);
        if(!type_conversion){
            /* TODO: get rid of the allocated memory */
            //delete obj;
            return false;
        }
        if(!type_conversion->InitFromXml(conversion_node, root_node))return false;
		type_conversions.push_back(type_conversion);
	    conversion_node=node.getChildNode("conversion", &i);
	}
	if(type_conversions.empty()){
        creation_tag="type_conversion_";
        creation_tag+=logical_type->GetType()+"_"+physical_type->GetType();
        
        obj=hsscomm::type_registry::create(creation_tag.c_str());
        if(!obj){
            LOG(Logger::LOG_ERROR, "Conversion %s to %s not supported", logical_type->GetType().c_str(), physical_type->GetType().c_str());

            return false;
        }
        HSSTypeConversion* type_conversion=dynamic_cast<HSSTypeConversion*>((HSSTypeConversion*)obj);
        if(!type_conversion){
            /* TODO: get rid of the allocated memory */
            //delete obj;
            return false;
        }
        if(!type_conversion->InitFromXml(conversion_node, root_node))return false;
		type_conversions.push_back(type_conversion);
    }

    XMLNode description_node=node.getChildNode("description");
	if(!description_node.isEmpty())additional_description.InitFromXml(description_node, root_node);

    return true;
}

const std::string& HSSParameter::GetId()
{
    return id;
}

bool HSSParameter::GetDescription(XmlRpc::XmlRpcValue* val)
{
	try{
		(*val)["ID"]=GetId();
		(*val)["OPERATIONS"]=operations;
		(*val)["FLAGS"]=flags;
		(*val)["TAB_ORDER"]=tab_order;
		if(!control.empty())(*val)["CONTROL"]=control;
	}catch(XmlRpcException){
		return false;
	}
	if(!logical_type->GetDescription(val))return false;
	return additional_description.Describe(val);
}

bool HSSParameter::GetValue(LogicalInstance* inst, XmlRpc::XmlRpcValue* val)
{
    try{
        XmlRpcValue phys_temp;
        XmlRpcValue log_temp;
		if(!physical_type->Get(inst, &phys_temp)){
			if(logical_type->UseDefaultOnFailure()){
				XmlRpcValue description;
				if(!logical_type->GetDescription(&description))return false;
				*val=description["DEFAULT"];
				return true;
			}
			LOG(Logger::LOG_ERROR, "HSSParameter::GetValue() id=%s failed getting physical value.", id.c_str());
			return false;
		}

		if(!PhysicalToLogical(inst, phys_temp, &log_temp, false)){
			LOG(Logger::LOG_ERROR, "HSSParameter::GetValue() id=%s failed converting value %s", id.c_str(), phys_temp.toText().c_str());
			return false;
		}

		if(!logical_type->EnforceConstraints(inst, &log_temp, HSSLogicalType::OP_READ)){
			LOG(Logger::LOG_ERROR, "HSSParameter::GetValue() id=%s failed enforcing constraints on %s", id.c_str(), log_temp.toText().c_str());
			return false;
		}
        *val=log_temp;
    }catch(XmlRpcException e){
		LOG(Logger::LOG_ERROR, "HSSParameter::GetValue() id=%s exception %d (%s)", id.c_str(), e.getCode(), e.getMessage().c_str());
        return false;
    }
    return true;
}
bool HSSParameter::GetValue(LogicalInstance* inst,int mode, XmlRpc::XmlRpcValue* val)
{
	bool retVal = false;
	XmlRpcValue value;
	XmlRpcValue undefValue = this->undefined;
	retVal = GetValue(inst, &value);
	if( retVal ) {
		if (mode == 1) {
			val->assertStruct();
			(*val)["VALUE"] = value;
			(*val)["UNDEFINED"] = undefValue;
			retVal = true; //auch wenn der wert nicht vom Gerät abgefragt werden konnte kann es ausgeliefert werden -> TODO:eventuell solte undefined = true gesetzt werden???
		} 
		else {
			(*val) = value;
		}
	}
	else if(mode == 1) {
		if(this->undefined) {
			val->assertStruct();
			(*val)["VALUE"] = GetLogicalType()->GetDefault();
			(*val)["UNDEFINED"] = undefValue;
			retVal = true;
		}
		else {
			retVal = false;
		}
	}

	return retVal;
}
bool HSSParameter::DetermineValue(LogicalInstance* inst)
{
	if(!physical_type->Determine(inst)){
		return false;
	}
	return true;
}

bool HSSParameter::SetupInstance(LogicalInstance* inst)
{
	return physical_type->SetupInstance(inst);
}

bool HSSParameter::SetValue(LogicalInstance* inst, XmlRpc::XmlRpcValue& val)
{
    try{
        XmlRpcValue log_temp=val;
		if(!logical_type->EnforceConstraints(inst, &log_temp, HSSLogicalType::OP_WRITE)){
		    LOG(Logger::LOG_ERROR, "HSSParameter::SetValue() %s EnforceConstraints failed", val.toText().c_str());
			return false;
		}
        XmlRpcValue phys_temp;
		
		if(!LogicalToPhysical(inst, log_temp, &phys_temp)){
		    LOG(Logger::LOG_ERROR, "HSSParameter::SetValue() %s LogicalToPhysical failed", val.toText().c_str());
			return false;
		}

		if(!physical_type->Put(inst, phys_temp)){
		    LOG(Logger::LOG_ERROR, "HSSParameter::SetValue() %s Put failed", val.toText().c_str());
			return false;
		}
		if(loopback && IsEventable()){
			inst->ReportEvent(id, log_temp, burst_suppression_time);
		}
		if(flags&FLG_SERVICE)inst->ReportServiceMessage(id, val);
    }catch(XmlRpcException e){
	    LOG(Logger::LOG_ERROR, "HSSParameter::SetValue() %s exception %s", val.toText().c_str(), e.getMessage().c_str());
		return false;
    }
    //this->undefined = false;
    return true;
}

bool HSSParameter::CheckType(XmlRpc::XmlRpcValue& val)
{
    try{
        XmlRpcValue dummy;
		if(!logical_type->EnforceConstraints(NULL, &val, HSSLogicalType::OP_WRITE))return false;
		return LogicalToPhysical(NULL, val, &dummy);
    }catch(XmlRpcException e){
        return false;
    }
}
bool HSSParameter::SetDefaultConfig(LogicalInstance *inst)
{
	XmlRpcValue val;
	LogicalToPhysical(inst,logical_type->GetDefault(),&val);
	return physical_type->SetDefaultConfig(inst, val);
}
void HSSParameter::ProcessIncomingFrame(LogicalInstance* inst, StructuredFrame& frame, FrameDescription* fd)
{
    try{
        XmlRpcValue phys_val;
        XmlRpcValue log_val;
        if(!physical_type->GetFromIncomingFrame(inst, frame, fd, &phys_val))return;

		if(!PhysicalToLogical(inst, phys_val, &log_val, true))return;

		if(!logical_type->EnforceConstraints(inst, &log_val, HSSLogicalType::OP_EVENT))return;
		if(!hidden)inst->ReportEvent(id, log_val, burst_suppression_time);
		if(flags&FLG_SERVICE)inst->ReportServiceMessage(id, log_val);
		this->undefined = false;
    }catch(XmlRpcException){
		LOG(Logger::LOG_DEBUG, "HSSParameter::ProcessIncomingFrame exception caught");
    }
}

void HSSParameter::ReportEvent(LogicalInstance* inst, XmlRpc::XmlRpcValue& phys_val)
{
	if(!(operations & OP_EVENT))return;
	XmlRpcValue log_val;
    try{
		if(!PhysicalToLogical(inst, phys_val, &log_val, true)){
			LOG(Logger::LOG_WARNING, "PhysicalToLogical failed for Event %s=%s", id.c_str(), phys_val.toText().c_str());
			return;
		}
		if(!logical_type->EnforceConstraints(inst, &log_val, HSSLogicalType::OP_EVENT)){
			LOG(Logger::LOG_WARNING, "EnforceConstraints failed for Event %s=%s", id.c_str(), log_val.toText().c_str());
			return;
		}

//		LOG(Logger::LOG_DEBUG, "Event %s=%s", id.c_str(), log_val.toText().c_str());
		inst->ReportEvent(id, log_val, burst_suppression_time);
		if(flags&FLG_SERVICE)inst->ReportServiceMessage(id, log_val);
    }catch(XmlRpcException e){
		LOG(Logger::LOG_WARNING, "EnforceConstraints failed for Event %s=%s %s", id.c_str(), log_val.toText().c_str(), e.getMessage().c_str());
    }
}

bool HSSParameter::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue in, XmlRpc::XmlRpcValue *out)
{
	for(unsigned int i=0;i<type_conversions.size();i++){
		out->clear();
		if(!type_conversions[i]->LogicalToPhysical(inst, in, out)){
			return false;
		}
		in=*out;
	}
    return true;
}

bool HSSParameter::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue in, XmlRpc::XmlRpcValue *out, bool event)
{
	for(int i=(int)type_conversions.size()-1;i>=0;i--){
		out->clear();
		if(!type_conversions[i]->PhysicalToLogical(inst, in, out, event)){
			return false;
		}
		in=*out;
	}
	return true;
}

bool HSSParameter::SetToDefault(LogicalInstance* inst)
{
	XmlRpcValue v=GetLogicalType()->GetDefault();
	return SetValue(inst, v);
}

bool HSSParameter::IsUndefined() {
	 return this->undefined;
}
void HSSParameter::SetUndefined(bool val)
{
	this->undefined= val;
}

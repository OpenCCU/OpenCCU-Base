/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSTypeConversionScript.cpp: Implementierung der Klasse HSSTypeConversionScript.
//
//////////////////////////////////////////////////////////////////////

#include "HSSTypeConversionToggle.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSTypeConversionToggle> HSSTypeConversionToggleFactory;

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSTypeConversionToggle::HSSTypeConversionToggle()
{
	m_valueId = "STATE";
	m_off = 0;
	m_on = 1;
}

HSSTypeConversionToggle::~HSSTypeConversionToggle()
{
}

bool HSSTypeConversionToggle::CheckCreationTag(const char *tag)
{
  return (strcmp("type_conversion_toggle", tag) == 0);
}

bool HSSTypeConversionToggle::InitFromXml(XMLNode &node, XMLNode &root_node)
{
	const char* valueId = node.getAttribute("value");
	if (valueId)
	{
		m_valueId = valueId;
	}
	
	const char* on = node.getAttribute("on");
	if (on)
	{
		m_on = strtol(on, NULL, 0);
	}
	
	const char* off = node.getAttribute("off");
	if (off)
	{
		m_off = strtol(off, NULL, 0);
	}
	
	return true;
}

bool HSSTypeConversionToggle::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
	XmlRpcValue value;
	
	if (!inst->GetStoredValue(m_valueId.c_str(), &value))
	{
		LOG(Logger::LOG_ERROR, "HSSTypeConversionToggle::LogicalToPhysical(): %s not found", m_valueId.c_str());		
		return false;
	}
	
	if (value.getType() != XmlRpcValue::TypeInt)
	{
		LOG(Logger::LOG_ERROR, "HSSTypeConversionToggle::LogicalToPhysical(): %s is not int", m_valueId.c_str());		
		return false;
	}
	
	int oldState = (int) value;
	int newState = (oldState == m_off) ? m_on : m_off;

	(int&) *out = newState;
	return true;
}

bool HSSTypeConversionToggle::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
	*out=in;
	return true;
}

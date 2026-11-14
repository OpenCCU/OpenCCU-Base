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

#include <stdio.h>

#include "HSSTypeConversionBlindTest.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSTypeConversionBlindTest> HSSTypeConversionBlindTestFactory;

using namespace XmlRpc;

#define LAST_DIRECTION_ID "_LAST_DIRECTION"
#define IT_COMMAND_ID "IT_COMMAND"

static void store(LogicalInstance* inst, const char* id, int value);
static int getLastDirection(LogicalInstance* inst);

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSTypeConversionBlindTest::HSSTypeConversionBlindTest()
{
	m_value = 255;
}

HSSTypeConversionBlindTest::~HSSTypeConversionBlindTest()
{
}

//////////////////////////////////////////////////////////////////////
// Methoden
//////////////////////////////////////////////////////////////////////

bool HSSTypeConversionBlindTest::CheckCreationTag(const char *tag)
{
  return (strcmp("type_conversion_blind_test", tag) == 0);
}

bool HSSTypeConversionBlindTest::InitFromXml(XMLNode &node, XMLNode &root_node)
{
	const char* value = node.getAttribute("value");
	if (value)
	{
		m_value = strtol(value, NULL, 0);
	}
		
	return true;
}

bool HSSTypeConversionBlindTest::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
	XmlRpcValue result;		
	int lastDirection;
	int newDirection;
	
	puts("HSSTypeConversionBlindTest");

	XmlRpcValue stateFlags;
	
	if (!inst->GetStoredValue("STATE_FLAGS", &stateFlags))
	{
		LOG(Logger::LOG_ERROR, "HSSTypeConversionBlindTest::LogicalToPhysical(): STATE_FLAGS not found");		
		return false;
	}
	
	if (stateFlags.getType() != XmlRpcValue::TypeInt)
	{
		LOG(Logger::LOG_ERROR, "HSSTypeConversionBlindTest::LogicalToPhysical(): STATE_FLAGS is not int");		
		return false;
	}		
	
	switch ( ((int) stateFlags) & 0x03)
	{
		case 0:
		case 3:			
			lastDirection = getLastDirection(inst);
			newDirection = lastDirection ? 0 : 200;
			store(inst, LAST_DIRECTION_ID, newDirection ? 1 : 0);			
			store(inst, IT_COMMAND_ID, 2);
			result = newDirection;
			*out = result;
			break;
		case 1:
			store(inst, LAST_DIRECTION_ID, 1);
			store(inst, IT_COMMAND_ID, 3);
			result = m_value;
			*out = result;
			break;
		case 2:
			store(inst, LAST_DIRECTION_ID, 0);
			store(inst, IT_COMMAND_ID, 3);
			result = m_value;
			*out = result;
			break;
		default:
			break;
	}		
			
	return true;
}

bool HSSTypeConversionBlindTest::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
	*out=in;
	return true;
}

//////////////////////////////////////////////////////////////////////
// Hilfsfunktionen
//////////////////////////////////////////////////////////////////////

static void store(LogicalInstance* inst, const char* id, int value)
{
	XmlRpcValue xmlRpcValue = value;
	inst->SetStoredValue(id, xmlRpcValue, ValueStore::FLAG_VOLATILE);
}

static int getLastDirection(LogicalInstance* inst)
{
	XmlRpcValue lastDirection;
	
	if (inst->GetStoredValue(LAST_DIRECTION_ID, &lastDirection))
	{
		if (lastDirection.getType() != XmlRpcValue::TypeInt)
		{
			return (int) lastDirection;
		}
	}
	
	return 1;
}

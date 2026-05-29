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

#include "HSSTypeConversionCfm.h"
#include <cstdlib>
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSTypeConversionCfm> HSSTypeConversionCfmFactory;

using namespace XmlRpc;

//////////////////////////////////////////////////////////////////////
// Definitionen
//////////////////////////////////////////////////////////////////////

#define MANTISSA_SIZE (11)
#define MANTISSA_START (5)
#define EXPONENT_START (0)
#define EXPONENT_SIZE  (5)

#define DURATION_FACTOR (10.0)

#define LEVEL_SIZE    (1)
#define LEVEL_OFFSET  (0)
#define LEVEL_FACTOR  (200)

#define COUNT_SIZE    (1)
#define COUNT_OFFSET  (LEVEL_OFFSET + LEVEL_SIZE)

#define ACTION_SIZE    (1)
#define ACTIONS_COUNT  (10)
#define ACTIONS_SIZE   (ACTIONS_COUNT * ACTION_SIZE)
#define ACTIONS_OFFSET (COUNT_OFFSET + COUNT_SIZE)

#define DURATION_SIZE   (2)
#define DURATION_OFFSET (ACTIONS_OFFSET + ACTIONS_SIZE)

#define OUT_VALUE_SIZE (LEVEL_SIZE + COUNT_SIZE + ACTIONS_SIZE + DURATION_SIZE)

//////////////////////////////////////////////////////////////////////
// Private Datenstrukturen
//////////////////////////////////////////////////////////////////////

struct cfm_frame_t
{
	double level;
	int count;
	double duration;
	int actions[ACTIONS_COUNT];
};

//////////////////////////////////////////////////////////////////////
// Hilfsfunktionen (Prototypen)
//////////////////////////////////////////////////////////////////////

//! Parst 
static void parse(const char* source, struct cfm_frame_t* target); 

//! Erstellt das 
static void stringify(struct cfm_frame_t* source, char* target);

//! Parst einen Separator (',')
static void parseSeparator(char** pBuffer);

//! Parst eine Ganzzahl
static int parseInt(char** pBuffer);

//! Parst eine Gleitpunktzahl
static double parseDouble(char** pBuffer);

//! Konvertiert eine Gleitpunktzahl in eine TinyFlot-Zahl
static int toTinyFloat(double value);


//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSTypeConversionCfm::HSSTypeConversionCfm()
{
}

HSSTypeConversionCfm::~HSSTypeConversionCfm()
{
}

//////////////////////////////////////////////////////////////////////
// Methoden
//////////////////////////////////////////////////////////////////////

bool HSSTypeConversionCfm::CheckCreationTag(const char *tag)
{
  return (strcmp("type_conversion_cfm", tag) == 0);
}

bool HSSTypeConversionCfm::InitFromXml(XMLNode &node, XMLNode &root_node)
{	
	return true;
}

bool HSSTypeConversionCfm::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
	if (in.getType() != XmlRpcValue::TypeString)
	{
		LOG(Logger::LOG_ERROR, "HSSTypeConversionCfm::LogicalToPhysical(): in is not string");		
		return false;
	}	
	
	struct cfm_frame_t frame;
	std::string str = (std::string) in;
	const char* inValue = str.c_str();
	char outValue[OUT_VALUE_SIZE + 1];

	parse(inValue, &frame);
	stringify(&frame, outValue);

	*out = std::string(outValue, OUT_VALUE_SIZE);
	return true;
}

bool HSSTypeConversionCfm::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
	*out=in;
	return true;
}

//////////////////////////////////////////////////////////////////////
// Hilfsfunktionen
//////////////////////////////////////////////////////////////////////

static void parse(const char* source, struct cfm_frame_t* target)
{
	char* tmp = (char*) source;

	target->level = parseDouble(&tmp);
	target->count = parseInt(&tmp);
	target->duration = parseDouble(&tmp);

	for (int i = 0; i < ACTIONS_COUNT; i++)
	{
		target->actions[i] = parseInt(&tmp);
	}
}

static void stringify(struct cfm_frame_t* source, char* target)
{
	int duration     = toTinyFloat((source->duration) * DURATION_FACTOR);
	int duration_high = (duration >> 8) & 0xff;
	int duration_low  = duration & 0xff;

	target[LEVEL_OFFSET] = (int) (LEVEL_FACTOR * source->level);
	target[COUNT_OFFSET] = source->count;
	target[DURATION_OFFSET + 0] = duration_high;
	target[DURATION_OFFSET + 1] = duration_low;

	for (int i = 0; i < ACTIONS_COUNT; i++)
	{
		target[ACTIONS_OFFSET + i] = source->actions[i];
	}

	target[OUT_VALUE_SIZE] = '\0';
}

static void parseSeparator(char** pBuffer)
{
	char* buffer = *pBuffer;
	bool ready = false;

	while (!ready)
	{
		switch (*buffer)
		{
		case '\0':
			ready = true;
			break;
		case ',':
			buffer++;
			ready = true;
			break;
		default:
			buffer++;
			break;

		}
	}

	*pBuffer = buffer;
}

static int parseInt(char** pBuffer)
{
	int result = strtol(*pBuffer, pBuffer, 10);
	parseSeparator(pBuffer);
	return result;
}

static double parseDouble(char** pBuffer)
{
	double result = strtod(*pBuffer, pBuffer);
	parseSeparator(pBuffer);
	return result;
}

static int toTinyFloat(double value)
{
	uint32_t mantissa = (int) value;
	uint32_t test_mask = 0xffffffff << MANTISSA_SIZE;
	int exponent;

	for(exponent = 0; exponent < (32 - MANTISSA_SIZE); exponent++){
		if(!(mantissa&test_mask))break;
		mantissa >>= 1;
	}
	
	return (exponent << EXPONENT_START)|(mantissa << MANTISSA_START);
}

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

#include "HSSTypeConversionIntegerList.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSTypeConversionIntegerList> HSSTypeConversionIntegerListFactory;

using namespace XmlRpc;

#define MAX_SIZE 31

//! Parst eine (vorzeichenlose) Ganzzahl aus einer Liste. 
static const char* readNext(const char* list, int& value);

//! Entfernt führende Leerzeichen
static const char* trimLeft(const char* list);

//! true, falls c ein Leerzeichen ist
static bool isWhitespace(char c);

//! true, falls c eine dezimale Ziffer ist
static bool isDigit(char c);

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSTypeConversionIntegerList::HSSTypeConversionIntegerList()
{
	size = 0;
}

HSSTypeConversionIntegerList::~HSSTypeConversionIntegerList()
{
}

//////////////////////////////////////////////////////////////////////
// Methoden
//////////////////////////////////////////////////////////////////////

bool HSSTypeConversionIntegerList::CheckCreationTag(const char *tag)
{
  return (strcmp("type_conversion_integerlist", tag) == 0);
}

bool HSSTypeConversionIntegerList::InitFromXml(XMLNode &node, XMLNode &root_node)
{	
  const char* temp=node.getAttribute("size");
	if(temp)size=strtol(temp, NULL, 0);
		
	return true;
}


bool HSSTypeConversionIntegerList::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
	if (in.getType() != XmlRpcValue::TypeString)
	{
		LOG(Logger::LOG_ERROR, "HSSTypeConversionIntegerList::LogicalToPhysical(): in is not string");		
		return false;
	}	
	
	char resultBuffer[MAX_SIZE + 1];
	const char* list = ((std::string) in).c_str();
	
	for (int i = 0; i < size; i++)
	{
		int value = 0;
		list = readNext(list, value);
		resultBuffer[i] = (char) value;
	}

	resultBuffer[size] = '\0';
	XmlRpcValue result = resultBuffer;
	
	*out = result;
	return true;
}

bool HSSTypeConversionIntegerList::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
	*out=in;
	return true;
}

//////////////////////////////////////////////////////////////////////
// Hilfsfunktionen
//////////////////////////////////////////////////////////////////////

static const char* readNext(const char* list, int& result)
{
	int value = 0;
	
	list = trimLeft(list);
	
	while (isDigit(*list))
	{
		value *= 10;
		value += list[0] - '0';
		list++;
	}
	if ('\0' != list) {list++;}
	
	trimLeft(list);
	return list;
}

static const char* trimLeft(const char* str)
{
	while (isWhitespace(*str)) 
	{ 
		str++; 
	}
	
	return str;
}

static bool isWhitespace(char c)
{
	return ((c == ' ') || (c == '\t') || (c == '\r') || (c == '\n'));
}

static bool isDigit(char c)
{
	return (('0' <= c) && (c <= '9'));
}

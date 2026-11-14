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

#include "HSSTypeConversionRc19Display.h"
#include <Logger.h>
#include "type_registry.h"

static hsscomm::type_registry::factory<HSSTypeConversionRc19Display> HSSTypeConversionRc19DisplayFactory;

using namespace XmlRpc;

//! Anzahl der Zeichen, die auf dem Display dargestellt werden.
#define DISPLAY_SIZE 5

//! Liefert ein Zeichen aus einer Zeichenkette oder ' '.
static char getCharAt(char* buffer, int bufferSize, int index);

//! Liefert das nächste Zeichen in einer Zeichenkette oder ' '.
static char getNextChar(std::string &value, size_t &index);

//! Liefert 1, falls es sich bei \c value um ein Komma oder einen Punkt handelt.
static bool isComma(char value);

//! Richtet den Puffer rechtsbündig aus.
static void fitRight(char* buffer, int index);

//////////////////////////////////////////////////////////////////////
// Konstruktion/Destruktion
//////////////////////////////////////////////////////////////////////

HSSTypeConversionRc19Display::HSSTypeConversionRc19Display()
{
}

HSSTypeConversionRc19Display::~HSSTypeConversionRc19Display()
{
}

//////////////////////////////////////////////////////////////////////
// Methoden
//////////////////////////////////////////////////////////////////////

bool HSSTypeConversionRc19Display::CheckCreationTag(const char *tag)
{
  return (strcmp("type_conversion_rc19display", tag) == 0);
}

bool HSSTypeConversionRc19Display::InitFromXml(XMLNode &node, XMLNode &root_node)
{	
	return true;
}


bool HSSTypeConversionRc19Display::LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out)
{
	if (in.getType() != XmlRpcValue::TypeString)
	{
		LOG(Logger::LOG_ERROR, "HSSTypeConversionRc19Display::LogicalToPhysical(): in is not string");		
		return false;
	}	
	
	char resultBuffer[DISPLAY_SIZE + 1];
	std::string inValue = (std::string) in;
	int hasComma = 0;
	size_t inValueIndex = 0;
	for (size_t i = 0; i < DISPLAY_SIZE; i++)
	{
		char x = getNextChar(inValue, inValueIndex);
		while (isComma(x))
		{
			hasComma = 1;
			x = getNextChar(inValue, inValueIndex);
		}
		
		resultBuffer[i] = x;
		
		if (hasComma)
		{
			fitRight(resultBuffer, i);
			break;
		}
	}
	resultBuffer[DISPLAY_SIZE] = '\0';
	
	XmlRpcValue comma = hasComma;
	inst->SetStoredValue("COMMA", comma, ValueStore::FLAG_VOLATILE);

	XmlRpcValue result = resultBuffer;
	
	*out = result;
	return true;
}

bool HSSTypeConversionRc19Display::PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue &in, XmlRpc::XmlRpcValue *out, bool event)
{
	*out=in;
	return true;
}

//////////////////////////////////////////////////////////////////////
// Hilfsfunktionen
//////////////////////////////////////////////////////////////////////

static char getCharAt(char* buffer, int bufferSize, int index)
{
	if ((index >= 0) && (index < bufferSize))
	{
		return buffer[index];
	}
	else
	{
		return ' ';
	}
}

static char getNextChar(std::string &value, size_t &index)
{
	char result;

	if ((index >= 0) && (index < value.length()))
	{
		result = value.at(index);
		index++;
	}
	else
	{
		result = ' ';
	}
	
	return result;	
}

static bool isComma(char value)
{
	return ((value == ',') || (value == '.'));
}

static void fitRight(char* buffer, int index)
{
	for (int i = DISPLAY_SIZE - 1; i >= 0; i--, index--)
	{
		buffer[i] = getCharAt(buffer, DISPLAY_SIZE, index);
	}
}

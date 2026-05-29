/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * HSSTypeConversionHexStringToByteArray.h
 *
 *  Created on: Jun 3, 2014
 *      Author: user
 */

#ifndef HSSTYPECONVERSIONHEXSTRINGTOBYTEARRAY_H_
#define HSSTYPECONVERSIONHEXSTRINGTOBYTEARRAY_H_

#include "dllexport.h"

#include "HSSTypeConversion.h"
#include <XmlRpc.h>


/** \brief Converts physical byte data to a string which contains hexadecimal encoded bytes and vice versa
 */
class HSSTypeConversionHexStringToByteArray : public HSSTypeConversion
{
public:
	//! Converts physical byte data to a string which contains hexadecimal encoded bytes
	/*! \c out = \c in
	 */
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);

	/** \brief Converts a string with hexadecimal encoded bytes to byte data.
	 * \details String "0x01 0x02 x03" -> [1,2,3]
	 */
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out);

	/** \brief Reads information from device description xml file.
	* \details Delimiter of hex value can be defined like delimiter=",".
	* Default delimiter is a white space
	*/
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);

	//! Hilfsmethode f�r die dynamische Erzeugung durch ein Factory-Objekt
	/*! Objekte dieser Klasse lassen sich durch hsscomm::type_registry::create("type_conversion_script") erzeugen
	 */
	static bool CheckCreationTag(const char* tag);

public:
	/** \brief Ctor*/
	HSSTypeConversionHexStringToByteArray();
	/** \brief Dtor*/
	virtual ~HSSTypeConversionHexStringToByteArray();

protected:
	/** \brief Tells if the hexadecimal values have 0x prefix or not.
	 * \details Default: true
	 *  */
	bool has0xPrefix;

	/** \brief Delimiter character.*/
	unsigned char delimiter;

	bool getNextToken(const std::string& inStr, std::string& token, int& offset);
	bool fromHexValue(const std::string& hexValue, unsigned char& value);
	void toHexString(const unsigned char& value,const std::string& prefix, std::string& hexStr);
	void trim(std::string& str);
};

#endif /* HSSTYPECONVERSIONHEXSTRINGTOBYTEARRAY_H_ */

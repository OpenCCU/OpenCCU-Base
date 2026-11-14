/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

/*
 * HSSTypeConversionInt4ToSignedIntX.h
 *
 *  Created on: Jan 14, 2015
 *      Author: user
 */

#ifndef BIDCOS_COMM_HSSTYPECONVERSIONINT4TOSIGNEDINTX_H_
#define BIDCOS_COMM_HSSTYPECONVERSIONINT4TOSIGNEDINTX_H_

#include "dllexport.h"

#include "HSSTypeConversion.h"
#include <XmlRpc.h>


/** \brief Conversion between signed 4 byte integer (logical) and signed X byte integer (phyiscal)
 *
 */
class HSSTypeConversionInt4ToSignedIntX  : public HSSTypeConversion
{
public:
	HSSTypeConversionInt4ToSignedIntX();
	virtual ~HSSTypeConversionInt4ToSignedIntX();


	//! Conversion from physical value (signed int with X bytes) to logical value (signed int with 4 bytes)
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out, bool event);

	//! Conversion from logical value (signed int with 4 bytes) to physical value (signed int with x bytes)
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue& in, XmlRpc::XmlRpcValue* out);

	//! Initialization from xml
	/*! Following attributes are supported:
	 *  - \c physical_bytes -> optional, number of bytes of phyiscal value ; default is 4.
	 */
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);

	//! Helper method for dynamic creation by a factory object.
	/*! Objects of this class are supposed to be created by hsscomm::type_registry::create("type_conversion_sint4_sintx")
	 */
	static bool CheckCreationTag(const char* tag);

private:
	unsigned int physicalBytes;

};

#endif /* BIDCOS_COMM_HSSTYPECONVERSIONINT4TOSIGNEDINTX_H_ */

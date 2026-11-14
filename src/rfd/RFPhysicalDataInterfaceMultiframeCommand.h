/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/


#ifndef RFPHYSICALDATAINTERFACEMULTIFRAMECOMMAND_H_
#define RFPHYSICALDATAINTERFACEMULTIFRAMECOMMAND_H_

#include <HSSPhysicalDataInterface.h>

/** \brief Physical data interface to write data to device that does not fit into one BidcosFrame.*/
class RFPhysicalDataInterfaceMultiframeCommand : public HSSPhysicalDataInterface {
public:

	/** \brief Constructor.*/
	RFPhysicalDataInterfaceMultiframeCommand();

	/** \brief Destructor.*/
	virtual ~RFPhysicalDataInterfaceMultiframeCommand();

	/** \brief Reads information from device desription file.*/
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);

	/** \brief Writes default config to device. Currently not supported by this data interface. */
	virtual bool SetDefaultConfig(LogicalInstance* inst,XmlRpc::XmlRpcValue val);

	/** \brief Retrieves value from device. NOT applicable and not implemented.*/
	virtual bool GetData(LogicalInstance* inst, XmlRpc::XmlRpcValue* param);

	/**\brief Writes value to device.*/
	virtual bool PutData(LogicalInstance* inst, XmlRpc::XmlRpcValue& param);

	/** \brief Helper method for dynamic creation by a factory class
	* \details Objects of this class are created by hsscomm::type_registry::create("data_interface_command") call.
	* This method compares the interfaces identifier.
	 */
	static bool CheckCreationTag(const char *tag);

	/** \brief Helper method for dynamic creation by a factory class
	* \details Objects of this class are created by hsscomm::type_registry::create("data_interface_command") call.
	*/
	virtual bool SetupInstance(LogicalInstance* inst);

protected:

	/** \brief Structure to hold information to assemble the BidcosFrame.*/
	struct FrameStructure {
		int frameType;//!HomeMatic frame type.
		unsigned int frameChannelField; //0 means, no channel field!
		unsigned int payloadIndex;//! Index of first character of the payload.
	};


	/** \brief Maximum number of frames sent for a parameter.
	 * \details Configurable in device description. 'max_frames=x'
	 * Default is 10
	 */
	unsigned int frameLimit;

	std::string valueId;
	std::string setRequestFrameId;
	FrameStructure frameStructure;

	bool sendImmidiately;
	bool wakeupOnImmediateSendFailure;



private:
	static const unsigned int maxBidcosFrameSize = 26;

};

#endif /* RFPHYSICALDATAINTERFACEMULTIFRAMECOMMAND_H_ */

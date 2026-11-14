/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _UNIFIEDLANDPROTOCOL_MESSAGE_H
#define _UNIFIEDLANDPROTOCOL_MESSAGE_H

#include <string>
#include <vector>

namespace ulc {

	enum UnifiedLanProtocolCmd {
		ULP_CMD_HELLO = 'H',
		ULP_CMD_SWITCH_PROTOCOL = 'S',
		ULP_CMD_SET_INIT_VECTOR = 'V',
		ULP_CMD_ANSWER = '>',
		ULP_CMD_UNDEFINED = ' '
	};


/** \brief Instance of this class represents a message of the Unified-Lan-Protocol (Einheitliche_Kommunikation_mit_Ethernetkomponenten)
 * \details It does NOT support all message types of the protocol, but the types used in this library.
 * This class does NOT convert to or from hex string.
 */
class UnifiedLanProtocolMessage {

public:


	/** \brief Constructs a new message, empty message for incoming frame data (from device).*/
	UnifiedLanProtocolMessage();

	/** \brief Constructs a new message to send to device.
	 * \param cmd Message command.
	 * \param messageCounter The message counter for the new message.*/
	UnifiedLanProtocolMessage(const UnifiedLanProtocolCmd cmd, const unsigned char messageCounter);
	
	/** \brief Appends data from device.
	 * \param data (Hex-)Data from device.
	 * \param nextMessageData Data of next message, if a message is complete and there is data of the next message.
	 * \return True, if message is complete.*/
	bool appendDataFromDevice(const std::string data, std::string& nextMessageData);

	/** \brief Adds a parameter to message.
	* \details Intended to be used only for outgoing messages (to device).
	* Given string must already be converted to hexadecimal string.
	* \param hexStr Hexadecicmal string data to append as message parameter.
	*/
	void addParam(const std::string& hexStr);

	/** \brief Returns the complete message as string to send it to device.
	 * \return String containing hexadecimal message string ready to send to device.*/
	std::string getMessageStringToSend() const;

	unsigned int getMessageParameterCount() const;
	std::string getParameterAt(const unsigned int index) const;

	UnifiedLanProtocolCmd getMessageCommand() const;
	unsigned char getMessageCounter() const;

	/**\ brief Clears msg data.*/
	void clear();

private:
	/** \brief Internal counter to assign message counters for new outgoing messages.*/
	//static unsigned char counter;//unified protocol type uint8
	unsigned char messageCounter;


	UnifiedLanProtocolCmd messageCommand;
	std::string messageData;

	/** \brief End of message... CR LF.*/
	std::string msgEndChars;

	/** Holds indices of first character of parameters.*/
	std::vector<int> parameterIndices;

	bool parseReceivedData();
};

}
#endif

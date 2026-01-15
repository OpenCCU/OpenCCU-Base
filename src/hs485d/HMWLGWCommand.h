/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HMWLGWCOMMAND_H_
#define _HMWLGWCOMMAND_H_

#include <string>

enum HMWLGWCommandType
{
	LGW_CMD_KEEPALIVE = 'K',//!Keep-Alive message; Must be send every 30 seconds to LGW
	LGW_CMD_SEND = 'S',//!Send command. Used to send frames to a HMW device.
	LGW_CMD_DISCOVERY = 'D',//!Discovery: Command to start discovery mode.
	LGW_RPL_RESPONSE = 'r',//!Response: Response (of a wired command telegram) from HMW device.
	LGW_RPL_ANSWER = 'a',//!Answer: Send by HMW-LGW in case of an error. (0:No error; 1: Timeout; 2 undefined error)
	LGW_RPL_DISCOVERY_RESULT = 'd',//!Discovery-Result: Send by HMW-LGW; Contains discovery result.
	LGW_EVT_DISCOVERY_END = 'c',//!Discovery-End: Command to stop discovery mode.
	LGW_EVT_EVENT = 'e',//!Event: Send by HMW-LGW when a wired telegram from any HMW-Device had been received.
	LGW_CMD_SET_LOG_LEVEL = 'L',//!Set Log Level: Sets the debug log level of the HMW-LGW
	LGW_CMD_UNDEFINED = (unsigned char)0xff
};

class HMWLGWCommand
{


public:
		///** \brief Constructor for an outgoing command frame.*/
		//HMWLGWCommand(const HMWLGWCommandType cmdType);
		/** \brief Constructor for an incoming command frame.*/
		HMWLGWCommand(const std::string& cmdRawData);
		/** \brief Constructor. */
		HMWLGWCommand();

		virtual ~HMWLGWCommand();

		/** \brief Assembles/prepares raw data from internal data for outgoing messages.
		* \details Increments frameCounter automatically.
		* \return Raw data ready to be sent out as string.*/
		std::string assembleCmdRawData();


		/** \brief Returns the command type of this command.
		 * \return HMWLGWCommandType.*/
		HMWLGWCommandType getCommandType();

		/** \brief Sets command type of this command.
		 * \param cmtType Command type (HMWLGWCommandType) to set.*/
		void setCommandType(const HMWLGWCommandType& cmdType);


		/** \brief Returns command data /payload.*/
		const std::string& getCommandData();
		/** \brief Sets command data/payload.*/
		void setCommandData(const std::string& payload);


		/** \brief Sets HMW sender address
		* \param address HMW sender address*/
		void setSenderAddress(const unsigned int address);

		/** \brief Sets HMW receiver address
		* \param address HMW receiver address*/
		void setReceiverAddress(const unsigned int address);

		/** \brief Returns HMW receiver address
		* \return HMW receiver address*/
		unsigned int getReceiverAddress();

		/** \brief Returns HMW receiver address
		* \return HMW receiver address*/
		unsigned int getSenderAddress();


		/** \brief Sets HMW control byte.
		* \param ctrl HMW control byte.*/
		void setCtrlByte(unsigned char ctrl);

		/** \brief Returns HMW control byte.
		* \return HMW control byte.*/
		unsigned char getCtrlByte();


		unsigned int getTimeout();
		void setTimeout(unsigned int timeout);

		/** \brief Parses incoming raw data.
		 * \param cmdRawData Command data.
		 * \return True on success, false on parse error.
		 */
		bool parseFromRawData(const std::string& cmdRawData);

		bool isResponseOf(const HMWLGWCommand responseCmd);

		/** \brief Transforms a response into an event.
		 * \param requestCommand The request this response belongs to.
		 * \return True, on success otherwise false.*/
		bool transformResponseToEvent(HMWLGWCommand& requestCommand);

protected:

	/** \brief Internal counter to assign frameCounter for new messages.*/
	static unsigned char frameCounterValue;

	/** \brief Messageframe counter.
	 * \details Incrementing counter. Replies must have the same counter as the corresponding request.
	 * The HMW-LGW has knows the last ten counters and commands with one of these won't be executed again.
	 * The value 0xFF enforces command execution and the history of last ten counters get deleted.
	*/
	unsigned char frameCounter;

	/** \brief Command (as defined in LGWCommandType) of this message frame.*/
	HMWLGWCommandType command;

	/** \brief Data/Payload of this command.
	 * \details Contains command specific data.*/
	std::string commandData;



private:

	/** \brief Control byte of HomeMatic-Wired protocol.
	 * \details Only set when applicable for specific command. Default 0x00.*/
	char hmwControlByte;

	/** \brief HMW receiver address.
	* \details Only set when applicable for specific command. Default 0)*/
	unsigned int hmwReceiverAddress;

	/** \brief HMW sender address.
	* \details Only set when applicable for specific command. Default 0)*/
	unsigned int hmwSenderAddress;

	/** \brief HMW Send->Response timout.*/
	unsigned int hmwTimeout;

};


#endif

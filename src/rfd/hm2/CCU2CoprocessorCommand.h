/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _CCU2COPROCESSORCOMMAND_H_
#define _CCU2COPROCESSORCOMMAND_H_

#include <string>
#include <BidcosFrame.h>

namespace HM2 {

/** \brief Type of command. System or BidCoS.
* \details The coprocessor differentiates between two types of command:
* System commands and BidCoS commands.
* COMMAND_TYPE_NOTSUPPORTED is only used in the bidcos service to identify uninitialized command type.
*/
enum CommandType {
	COMMANDTYPE_SYSTEM = 0x00,
	COMMANDTYPE_BIDCOS = 0x01,
	COMMANDTYPE_NOTSUPPORTED = 0xff
};

/** \brief System commands.
* \details System commands control and query coprocessor specific settings and information.
*/
enum SystemCommand {
	SYSTEMCMD_IDENTIFY = 0x00,
	SYSTEMCMD_GETVERSION = 0x02,
	SYSTEMCMD_STARTBOOTLOADER = 0x03,

	SYSTEMCMD_RESPONSE_STATUS = 0x04, //only FROM coprocessor
	SYSTEMCMD_EVENT_DUTYCYCLE = 0x05, //only FROM coprocessor
	
	SYSTEMCMD_SET_10K_DATARATE = 0x06,
	SYSTEMCMD_SET_100K_DATARATE = 0x07,
	SYSTEMCMD_GETDUTYCYLCE = 0x08,
	SYSTEMCMD_DUTYCYLCE_ON_OFF = 0x09,
	SYSTEMCMD_CSCMCA_ON_OFF = 0x0a,
	SYSTEMCMD_GET_SERIALNR = 0x0b,
	//0x0c and 0x0d are commands to read and write the test status ; not needed in rfd
	SYSTEMCMD_SET_CLOCK = 0x0e,
	SYSTEMCMD_UNKNOWN = 0xff
};

/** \brief Responses to system commands.
*/
enum SystemCommandResponseStatus {
	SYSTEMCMD_RESPONSE_STATUS_ERROR = 0x00,			//msg cannot be processed
	SYSTEMCMD_RESPONSE_STATUS_OK = 0x01,			
	SYSTEMCMD_RESPONSE_STATUS_DATA = 0x02,			//delivery of requested data
	SYSTEMCMD_RESPONSE_STATUS_BUSY = 0x03,			//system busy, cmd cannot be processed right now
	SYSTEMCMD_RESPONSE_STATUS_INPUT_ERROR = 0x04,	//msg cannot be processed
	SYSTEMCMD_RESPONSE_STATUS_UNKNOWN = 0xff		//internal state to indentify unitialized state
};

/** \brief BidCoS commands.
*/
enum BidcosCommand {
    BIDCOSCMD_SET_RF_ADDRESS = 0x00,
	BIDCOSCMD_GET_RF_ADDRESS = 0x01,
	BIDCOSCMD_SEND_TELEGRAM = 0x02,
	BIDCOSCMD_SET_CURRENT_AES_KEY = 0x03,//BIDCOSCMD_NEW_AES_KEY = 0x03,
	BIDCOSCMD_STATUS = 0x04,//status, only FROM coprocessor
	BIDCOSCMD_EVENT = 0x05,//event, only FROM coprocessor
	BIDCOSCMD_ADD_LINK_PEER = 0x06,
	BIDCOSCMD_REMOVE_LINK_PEER = 0x07,
	BIDCOSCMD_GET_LINK_PEERS = 0x08,
	BIDCOSCMD_AUTHENTICATION_ON = 0x09,
	BIDCOSCMD_AUTHENTICATION_OFF = 0x0a,
	BIDCOSCMD_SET_TEMP_AES_KEY = 0x0b,
	BIDCOSCMD_SET_PEER_KEY_INDEX = 0x0c,
	BIDCOSCMD_GET_PEER_KEY_INDEX = 0x0d,
	BIDCOSCMD_GET_KEY_CHECKSUM = 0x0e,//BIDCOSCMD_GET_PREVIOUS_KEY = 0x0e,
	BIDCOSCMD_SET_PREVIOUS_KEY = 0x0f,//BIDCOSCMD_SET_PREVIOUS_KEY = 0x0f,
	BIDCOSCMD_GET_DEFAULT_RF_ADDRESS = 0x10,
	BIDCOSCMD_UNKNOWN = 0xff
};

/** \bief Responses to BidCoS commands. */
enum BidcosCommandResponseStatus {
	BIDCOSCMD_RESPONSE_STATUS_ERROR = 0x00,
	BIDCOSCMD_RESPONSE_STATUS_OK = 0x01, //ack rf telegram as data
	BIDCOSCMD_RESPONSE_STATUS_TELEGRAM_SENT = 0x02,
	BIDCOSCMD_RESPONSE_STATUS_TELEGRAM_SENT_RECVD_ACK = 0x03,
	BIDCOSCMD_RESPONSE_STATUS_SEND_FAILED = 0x04,
	BIDCOSCMD_RESPONSE_STATUS_SEND_FAILED_DUTYCYLCE = 0x05,
	BIDCOSCMD_RESPONSE_STATUS_SEND_FAILED_CSMACA = 0x06,
	BIDCOSCMD_RESPONSE_STATUS_DATA = 0x07, //requested data delivered; requested stuff as data
	BIDCOSCMD_RESPONSE_STATUS_BUSY = 0x08,
	BIDCOSCMD_RESPONSE_STATUS_TRX_BUSY = 0x09,
	BIDCOSCMD_RESPONSE_STATUS_NOT_INITIALIZED = 0x0a,
	BIDCOSCMD_RESPONSE_STATUS_INPUT_WRONG = 0x0b,
	BIDCOSCMD_RESPONSE_STATUS_AUTHENTICATION_SUCCESS = 0x0c,
	BIDCOSCMD_RESPONSE_STATUS_AUTHENTICATION_FAILURE = 0x0d,
	BIDCOSCMD_RESPONSE_STATUS_UNKNOWN = 0xff //internal state to identify unitialized state.
};

/** \brief BidCoS type events and respones regarding aes authentification. */
enum BidcosCommandEventAuthentificationStatus {
	BIDCOSCMD_EVENT_AUTH_STATUS_NOT_RESPONSIBLE = 0x00,
	BIDCOSCMD_EVENT_AUTH_STATUS_NO_AUTH_REQUIRED = 0x01,
	BIDCOSCMD_EVENT_AUTH_STATUS_AUTH_SUCCESSFUL = 0x02,
	BIDCOSCMD_EVENT_AUTH_STATUS_AUTH_FAILED = 0x03,
	BIDCOSCMD_EVENT_AUTH_STATUS_AUTH_KEY_INDEX_UNKNOWN = 0x04,
    BIDCOSCMD_EVENT_AUTH_STATUS_UNKNOWN = 0xff
};

enum BurstMode {
	NO_BURST = 0,
	SINGLE_BURST = 1,
	TRIPLE_BURST = 3
};

/** \brief This class 
*/
class CCU2CoprocessorCommand {
public:

	CCU2CoprocessorCommand();
	CCU2CoprocessorCommand(const std::string& coprocessorCmdString);
	CCU2CoprocessorCommand(const SystemCommand sysCommand, const std::string& data);
	CCU2CoprocessorCommand(const BidcosCommand bidcosCommand, const std::string& data);
	virtual ~CCU2CoprocessorCommand();

	/**
	* \brief Assembles a command string for given SystemCommand.
	* \param sysCommand The command.
	* \param value Value to set if applicable (only valid for CMD_DUTYCYLCE_ON_OFF and CMD_CSCMCA_ON_OFF).
	*/

	std::string getCommandString();
	std::string getCommandData() const;

	/** \brief Extracts destination address from bidcos telegram data.
	* \return Bidcos telegram destination address or -1 if not applicable.
	*/
	int getBidcosTelegramDestinationAddress() const;
	int getBidcosTelegramSourceAddress() const;
	int getBidcosTelegramCounter() const;

	BidcosCommandEventAuthentificationStatus getAuthentificationStatus() const;
	bool hasAuthentificationStatus() const;
	
//	static bool parseSystemCommandResponse(const std::string& responseStr, std::string* pResponseValue = NULL);

	bool isResponseValid() const;

	CommandType getCommandType() const;

	bool isEvent() const;
	bool isResponseStatusOk() const;
	BidcosCommandResponseStatus getBidcosCommandResponseStatus();
	std::string getBidcosCommandResponseStatusAsString() const;

	int getRSSI() const;
	bool isRSSISet() const;
	bool isDevWokenUp() const;
	char getAuthenticationKeyIndex() const;

	static bool isResponseOfRequest(const CCU2CoprocessorCommand& request, const CCU2CoprocessorCommand& response);

	bool isDutyCycleEvent() const;
	double getDutyCycleValue() const;

	bool isResponseStatusBusy() const;

	bool isIdentifyEvent() const;

	void setBurstMode(const BurstMode burstMode);

private:
	static unsigned char sequenceCounter;
	static const unsigned char STX;
	static const char rssiUndefinedValue;


	CommandType commandType;
	SystemCommand systemCommand;
	BidcosCommand bidcosCommand;
	SystemCommandResponseStatus systemCommandResponseStatus;
	BidcosCommandResponseStatus bidcosCommandResponseStatus;
	BidcosCommandEventAuthentificationStatus bidcosCommandEventAuthStatus;
	std::string commandData;
	bool responseValid;

	/**\brief Either sequence nr or telegram counter.*/
	unsigned char commandNr;

	/** \brief RSSI value.*/
	int rssi;

	/** \brief True if woken up flag was set.*/
	bool devWokenUp;

	/** \brief Device key index used by coprocessor for authentication.*/
	char authKeyIndex;

	/** \brief Duty cycle value. Value range from 0 to 200.*/
	unsigned char dutyCycleValue;

	/** \brief Burst mode. Used for sending bidcos telegrams (outgoing)*/
	unsigned char burstMode;

	//static unsigned short createCRC(const char* msg, unsigned int offset, unsigned int length);

	std::string assembleSystemCommandString(/*const SystemCommand sysCommand, const char value = 0*/);
	std::string assembleBidcosCommandString(/*const BidcosCommand bidcosCommand, const std::string& data*/);

	bool parseSystemCommandResponse(const std::string& responseStr);
	bool parseBidcosCommandResponse(const std::string& responseStr);

	//static std::string parseIdentifyResponse(/*const std::string& identifyCommand,*/ const std::string& identifyResponse);
	//static std::string parseGetVersionResponse(const std::string& getVersionResponse, std::string& bootloaderVersion, std::string& firmwareVersion);


    /** \brief Converts an int to string.
    * \param i int to convert.
    * \return Digits of i as string.
    */
    static std::string toString(const int i); 
	
	inline std::string responseStatusToString(const BidcosCommandResponseStatus& responseStatus) const;
	

};

}

#endif

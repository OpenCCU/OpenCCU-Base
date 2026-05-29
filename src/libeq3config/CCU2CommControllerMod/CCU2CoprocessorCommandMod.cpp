/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <CCU2CoprocessorCommandMod.h>

#include <stdio.h>
#include <cstring>
#include <Logger.h>
#include <HM2UtilsMod.h>

using namespace HM2Mod;

unsigned char CCU2CoprocessorCommandMod::sequenceCounter = 0;
const unsigned char CCU2CoprocessorCommandMod::STX = 0xfd;




CCU2CoprocessorCommandMod::CCU2CoprocessorCommandMod()
: commandType(COMMANDTYPE_NOTSUPPORTED)
, systemCommand(SYSTEMCMD_UNKNOWN)
, bidcosCommand(BIDCOSCMD_UNKNOWN)
, systemCommandResponseStatus(SYSTEMCMD_RESPONSE_STATUS_UNKNOWN)
, bidcosCommandResponseStatus(BIDCOSCMD_RESPONSE_STATUS_UNKNOWN)
, bidcosCommandEventAuthStatus(BIDCOSCMD_EVENT_AUTH_STATUS_UNKNOWN)
, lowlevelmacCommand(LOWLEVELMAC_COMMMAND_UNKNOWN)
, lowLevelMacResponseStatus(LOWLEVELMAC_RESPONSE_UNKNOWN)
//, responseSequenceCounter((char)0xff)
, responseValid(false)
, commandNr((unsigned char)0xff)
, rssi(0xFF)
, devWokenUp(false)
, authKeyIndex((char)0xFF) //Key index not initialized
, dutyCycleValue((unsigned char)0x00)
, hmipCommonCommandResponseStatus(HMIP_COMMONCMD_RESPONSE_UNKNOWN)
, hmipCommonCommand(HMIP_COMMON_UNKNOWN)
, hmipTrxAdapterCommand(HMIP_TRXADAPTER_UNKNOWN)
, hmipTrxAdapterResponseStatus(HMIP_TRXADAPTERCMD_RESPONSE_UNKNOWN)
, hmipCoproFrame(false)
{
}

CCU2CoprocessorCommandMod::CCU2CoprocessorCommandMod(const std::string& coprocessorCmdString,bool dualCopro)
: commandType(COMMANDTYPE_NOTSUPPORTED)
, systemCommand(SYSTEMCMD_UNKNOWN)
, bidcosCommand(BIDCOSCMD_UNKNOWN)
, hmipCoproFrame(dualCopro)
, systemCommandResponseStatus(SYSTEMCMD_RESPONSE_STATUS_UNKNOWN)
, bidcosCommandResponseStatus(BIDCOSCMD_RESPONSE_STATUS_UNKNOWN)
, bidcosCommandEventAuthStatus(BIDCOSCMD_EVENT_AUTH_STATUS_UNKNOWN)
, lowlevelmacCommand(LOWLEVELMAC_COMMMAND_UNKNOWN)
, lowLevelMacResponseStatus(LOWLEVELMAC_RESPONSE_UNKNOWN)
//, responseSequenceCounter((char)0xff)
, responseValid(false)
, commandNr((unsigned char)0xff)
, rssi(0xFF)
, devWokenUp(false)
, authKeyIndex((char)0xFF)
, dutyCycleValue((unsigned char)0x00)
{
	if(coprocessorCmdString.size() > 2) {
		commandType = (CommandType)coprocessorCmdString.at(0);
		//responseSequenceCounter = (unsigned char)coprocessorCmdString.at(1);
		switch(commandType) {
			case COMMANDTYPE_SYSTEM:
				LOG(Logger::LOG_DEBUG,"CCU2CoprocessorCommandMod::CCU2CoprocessorCommandMod(): System frame");
				responseValid = parseSystemCommandResponse( coprocessorCmdString );
				break;
			case COMMANDTYPE_BIDCOS:
				if(!hmipCoproFrame)
				{
					LOG(Logger::LOG_DEBUG,"CCU2CoprocessorCommandMod::CCU2CoprocessorCommandMod(): Bidcos frame");
					responseValid = parseBidcosCommandResponse( coprocessorCmdString );
				}
				else
				{
					LOG(Logger::LOG_DEBUG,"CCU2CoprocessorCommandMod::CCU2CoprocessorCommandMod(): TrxAdapter frame");
					responseValid = parseHmiptrxAdapterCommand(coprocessorCmdString);
				}
				break;
			case COMMANDTYPE_HMIP_COMMON:
				LOG(Logger::LOG_DEBUG,"CCU2CoprocessorCommandMod::CCU2CoprocessorCommandMod(): hmip common frame");
				responseValid = parseHmipCommonCommand(coprocessorCmdString);
				break;
			case COMMANDTYPE_LOWLEVELMAC:
				LOG(Logger::LOG_DEBUG,"CCU2CoprocessorCommandMod::CCU2CoprocessorCommandMod(): lowlevelmac frame");
				responseValid = parseLowLevelMacResponseFrame(coprocessorCmdString);
				break;
			default:
				break;
		}
	}
}

CCU2CoprocessorCommandMod::CCU2CoprocessorCommandMod(const SystemCommand sysCommand, const std::string& data)
: commandType(COMMANDTYPE_SYSTEM)
, systemCommand(sysCommand)
, bidcosCommand(BIDCOSCMD_UNKNOWN)
, hmipCommonCommand(HMIP_COMMON_UNKNOWN)
, hmipCoproFrame(false)
, hmipTrxAdapterCommand(HMIP_TRXADAPTER_UNKNOWN)
, systemCommandResponseStatus(SYSTEMCMD_RESPONSE_STATUS_UNKNOWN)
, bidcosCommandResponseStatus(BIDCOSCMD_RESPONSE_STATUS_UNKNOWN)
, bidcosCommandEventAuthStatus(BIDCOSCMD_EVENT_AUTH_STATUS_UNKNOWN)
, hmipCommonCommandResponseStatus(HMIP_COMMONCMD_RESPONSE_UNKNOWN)
, lowlevelmacCommand(LOWLEVELMAC_COMMMAND_UNKNOWN)
, lowLevelMacResponseStatus(LOWLEVELMAC_RESPONSE_UNKNOWN)
//, responseSequenceCounter((char)0xff)
, commandData(data)
, responseValid(false)
, commandNr((unsigned char)0xff)
, rssi(0xFF)
, devWokenUp(false)
, authKeyIndex((char)0xFF)
, dutyCycleValue((unsigned char)0x00)
, hmipTrxAdapterResponseStatus(HMIP_TRXADAPTERCMD_RESPONSE_UNKNOWN)
{
}

CCU2CoprocessorCommandMod::CCU2CoprocessorCommandMod(const BidcosCommand bidcosCommand, const std::string& data)
: commandType(COMMANDTYPE_BIDCOS)
, systemCommand(SYSTEMCMD_UNKNOWN)
, bidcosCommand(bidcosCommand)
, hmipCoproFrame(false)
, hmipCommonCommand(HMIP_COMMON_UNKNOWN)
, lowlevelmacCommand(LOWLEVELMAC_COMMMAND_UNKNOWN)
, systemCommandResponseStatus(SYSTEMCMD_RESPONSE_STATUS_UNKNOWN)
, bidcosCommandResponseStatus(BIDCOSCMD_RESPONSE_STATUS_UNKNOWN)
, hmipCommonCommandResponseStatus(HMIP_COMMONCMD_RESPONSE_UNKNOWN)
, bidcosCommandEventAuthStatus(BIDCOSCMD_EVENT_AUTH_STATUS_UNKNOWN)
, lowLevelMacResponseStatus(LOWLEVELMAC_RESPONSE_UNKNOWN)
//, responseSequenceCounter((char)0xff)
, commandData(data)
, responseValid(false)
, commandNr((unsigned char)0xff)
, rssi(0xFF)
, devWokenUp(false)
, authKeyIndex((char)0xFF)
, dutyCycleValue((unsigned char)0x00)
, hmipTrxAdapterResponseStatus(HMIP_TRXADAPTERCMD_RESPONSE_UNKNOWN)
, hmipTrxAdapterCommand(HMIP_TRXADAPTER_UNKNOWN)
{
}

CCU2CoprocessorCommandMod::CCU2CoprocessorCommandMod(const HmipCommonCommand hmipCommonCommand, const std::string& data)
: commandType(COMMANDTYPE_HMIP_COMMON)
, systemCommand(SYSTEMCMD_UNKNOWN)
, bidcosCommand(BIDCOSCMD_UNKNOWN)
, hmipCommonCommand(hmipCommonCommand)
, lowlevelmacCommand(LOWLEVELMAC_COMMMAND_UNKNOWN)
, hmipCoproFrame(true)
, systemCommandResponseStatus(SYSTEMCMD_RESPONSE_STATUS_UNKNOWN)
, bidcosCommandResponseStatus(BIDCOSCMD_RESPONSE_STATUS_UNKNOWN)
, hmipCommonCommandResponseStatus(HMIP_COMMONCMD_RESPONSE_UNKNOWN)
, bidcosCommandEventAuthStatus(BIDCOSCMD_EVENT_AUTH_STATUS_UNKNOWN)
, lowLevelMacResponseStatus(LOWLEVELMAC_RESPONSE_UNKNOWN)
//, responseSequenceCounter((char)0xff)
, commandData(data)
, responseValid(false)
, commandNr((unsigned char)0xff)
, rssi(0xFF)
, devWokenUp(false)
, authKeyIndex((char)0xFF)
, dutyCycleValue((unsigned char)0x00)
, hmipTrxAdapterResponseStatus(HMIP_TRXADAPTERCMD_RESPONSE_UNKNOWN)
, hmipTrxAdapterCommand(HMIP_TRXADAPTER_UNKNOWN)
{

}
CCU2CoprocessorCommandMod::CCU2CoprocessorCommandMod(const HmipTrxAdapterCommand hmipTrxAdapterCommand, const std::string& data)
: commandType(COMMANDTYPE_TRX_ADAPTER)
, systemCommand(SYSTEMCMD_UNKNOWN)
, bidcosCommand(BIDCOSCMD_UNKNOWN)
, hmipCommonCommand(HMIP_COMMON_UNKNOWN)
, hmipCoproFrame(true)
, hmipTrxAdapterCommand(hmipTrxAdapterCommand)
, lowlevelmacCommand(LOWLEVELMAC_COMMMAND_UNKNOWN)
, systemCommandResponseStatus(SYSTEMCMD_RESPONSE_STATUS_UNKNOWN)
, bidcosCommandResponseStatus(BIDCOSCMD_RESPONSE_STATUS_UNKNOWN)
, bidcosCommandEventAuthStatus(BIDCOSCMD_EVENT_AUTH_STATUS_UNKNOWN)
, hmipCommonCommandResponseStatus(HMIP_COMMONCMD_RESPONSE_UNKNOWN)
, lowLevelMacResponseStatus(LOWLEVELMAC_RESPONSE_UNKNOWN)
//, responseSequenceCounter((char)0xff)
, commandData(data)
, responseValid(false)
, commandNr((unsigned char)0xff)
, rssi(0xFF)
, devWokenUp(false)
, authKeyIndex((char)0xFF)
, dutyCycleValue((unsigned char)0x00)
, hmipTrxAdapterResponseStatus(HMIP_TRXADAPTERCMD_RESPONSE_UNKNOWN)
{
}

CCU2CoprocessorCommandMod::CCU2CoprocessorCommandMod(const LowLevelMacCommand lowlevelmacCommand/*, const std::string& data*/)
: commandType(COMMANDTYPE_LOWLEVELMAC)
, systemCommand(SYSTEMCMD_UNKNOWN)
, bidcosCommand(BIDCOSCMD_UNKNOWN)
, hmipCommonCommand(HMIP_COMMON_UNKNOWN)
, lowlevelmacCommand(lowlevelmacCommand)
, hmipCoproFrame(true)
, hmipTrxAdapterCommand(hmipTrxAdapterCommand)
, systemCommandResponseStatus(SYSTEMCMD_RESPONSE_STATUS_UNKNOWN)
, bidcosCommandResponseStatus(BIDCOSCMD_RESPONSE_STATUS_UNKNOWN)
, bidcosCommandEventAuthStatus(BIDCOSCMD_EVENT_AUTH_STATUS_UNKNOWN)
, hmipCommonCommandResponseStatus(HMIP_COMMONCMD_RESPONSE_UNKNOWN)
, lowLevelMacResponseStatus(LOWLEVELMAC_RESPONSE_UNKNOWN)
//, responseSequenceCounter((char)0xff)
, commandData("")
, responseValid(false)
, commandNr((unsigned char)0xff)
, rssi(0xFF)
, devWokenUp(false)
, authKeyIndex((char)0xFF)
, dutyCycleValue((unsigned char)0x00)
, hmipTrxAdapterResponseStatus(HMIP_TRXADAPTERCMD_RESPONSE_UNKNOWN)
{
}



CCU2CoprocessorCommandMod::~CCU2CoprocessorCommandMod()
{
}

bool CCU2CoprocessorCommandMod::isIdentifyResponse(const std::string& cmdString) {
	if(cmdString.size() > 3) {
		char c = cmdString.at(0);
		if( c == (char)COMMANDTYPE_HMIP_COMMON) {
			if(cmdString.at(2) == (char)HMIP_COMMON_RESPONSE) {
				return true;
			}
		}
		else if(c == (char)COMMANDTYPE_SYSTEM) {
			if((cmdString.at(2) == (char)SYSTEMCMD_RESPONSE_STATUS) && (cmdString.at(3) == (char)SYSTEMCMD_RESPONSE_STATUS_DATA)) {
				return true;
			}
		}
		return false;
	}
	return false;
}

std::string CCU2CoprocessorCommandMod::getIdentifyResponseIdentificationString(const std::string& cmdString) {
	if(cmdString.size() > 4) {//
			return cmdString.substr(4);
	}
	else {
		return std::string("");
	}
}


std::string CCU2CoprocessorCommandMod::assembleSystemCommandString() {
	std::string cmdStr;
	cmdStr.append(1, (char)COMMANDTYPE_SYSTEM);
	cmdStr.append(1, sequenceCounter);//Attention: CCU2SerialPortCommController needs incrementation of the counter in this method for it's busy-retrying-stuff.
	commandNr = sequenceCounter;
	sequenceCounter++;
	cmdStr.append(1, (char)systemCommand);
	switch(systemCommand) {
		case SYSTEMCMD_SET_100K_DATARATE:
			cmdStr.append(commandData);
			//TODO FIXME Syncwort
			break;
		case SYSTEMCMD_DUTYCYLCE_ON_OFF:
		case SYSTEMCMD_CSCMCA_ON_OFF:
		case SYSTEMCMD_SET_CLOCK:
		case SYSTEMCMD_EVENT_DUTYCYCLE://MOD fix for coprocessor update (double assigned!)
			cmdStr.append(commandData);
			break;
		//Not commands!!!
		//case EVENT_DUTYCYCLE:		
		//case RESPONSE_STATUS:
		default:
			break;
	}
	return cmdStr;
}

std::string CCU2CoprocessorCommandMod::assembleHmipCommonCommandString() {
	std::string cmdStr;
	cmdStr.append(1, (char)COMMANDTYPE_HMIP_COMMON);
	cmdStr.append(1, sequenceCounter);//Attention: CCU2SerialPortCommController needs incrementation of the counter in this method for it's busy-retrying-stuff.
	commandNr = sequenceCounter;
	sequenceCounter++;
	cmdStr.append(1, (char)hmipCommonCommand);
	if(!commandData.empty())
	{
		cmdStr.append(commandData);
	}
	return cmdStr;
}

std::string CCU2CoprocessorCommandMod::assembleHmipTrxAdapterCommandString() {
	std::string cmdStr;
	cmdStr.append(1, (char)COMMANDTYPE_TRX_ADAPTER);
	cmdStr.append(1, sequenceCounter);//Attention: CCU2SerialPortCommController needs incrementation of the counter in this method for it's busy-retrying-stuff.
	commandNr = sequenceCounter;
	sequenceCounter++;
	cmdStr.append(1, (char)hmipTrxAdapterCommand);
	if(!commandData.empty())
	{
		cmdStr.append(commandData);
	}
	return cmdStr;
}

std::string CCU2CoprocessorCommandMod::assembleLowLevelMacCommandString() {
	std::string cmdStr;
	cmdStr.append(1, (char)COMMANDTYPE_LOWLEVELMAC);
	cmdStr.append(1, sequenceCounter);//Attention: CCU2SerialPortCommController needs incrementation of the counter in this method for it's busy-retrying-stuff.
	commandNr = sequenceCounter;
	sequenceCounter++;
	cmdStr.append(1, (char)lowlevelmacCommand);
	if(!commandData.empty())
	{
		cmdStr.append(commandData);
	}
	return cmdStr;
}


std::string CCU2CoprocessorCommandMod::assembleBidcosCommandString()
{
	std::string cmdStr;
	cmdStr.append(1, (char)COMMANDTYPE_BIDCOS);//type bidcos
	cmdStr.append(1, sequenceCounter);//Attention: CCU2SerialPortCommController needs incrementation of the counter in this method for it's busy-retrying-stuff.
	commandNr = sequenceCounter; //FIXME maybe must be telegram nr
	sequenceCounter++;
	cmdStr.append(1, (char) bidcosCommand);//bidcos command	
	//Data:
	switch(bidcosCommand) {
		case BIDCOSCMD_SET_RF_ADDRESS:
		case BIDCOSCMD_GET_RF_ADDRESS:
		case BIDCOSCMD_SET_CURRENT_AES_KEY:
		case BIDCOSCMD_ADD_LINK_PEER:
		case BIDCOSCMD_REMOVE_LINK_PEER:
		case BIDCOSCMD_GET_LINK_PEERS:
		case BIDCOSCMD_AUTHENTICATION_ON:
		case BIDCOSCMD_AUTHENTICATION_OFF:
		case BIDCOSCMD_SET_TEMP_AES_KEY:
		case BIDCOSCMD_SET_PEER_KEY_INDEX:
		case BIDCOSCMD_GET_PEER_KEY_INDEX:
		case BIDCOSCMD_GET_KEY_CHECKSUM:
		case BIDCOSCMD_SET_PREVIOUS_KEY:
		case BIDCOSCMD_GET_DEFAULT_RF_ADDRESS:
			cmdStr.append(commandData);
			break;
		case BIDCOSCMD_SEND_TELEGRAM:
			if(commandData.size() < 9 || commandData.size() > 64) {
				cmdStr.clear();
				//LOG(Logger::LOG_WARNING, "CCU2CoprocessorCommandMod::assembleBidcosCommandString(): BidCoS telegram size not between specificated limits. Outgoing message dropped.");
			}
			////LOG(Logger::LOG_DEBUG, "BidCos telegram data size: %d\n", commandData.size());
			cmdStr.append(1, (char)0x00);//time to wait before sending in ms (2 byte)
			cmdStr.append(1, (char)0x00);
			cmdStr.append(commandData);
			break;
		default:
			//LOG(Logger::LOG_ERROR, "CCU2CoprocessorCommandMod::assembleBidcosCommandString(): Unknown BidCoS command. Outgoing message dropped.");
			cmdStr.clear();
			break;
	}
	return cmdStr;
}

bool CCU2CoprocessorCommandMod::parseSystemCommandResponse(const std::string& responseStr)
{
	bool returnCode = false;
	if(responseStr.size() >= 4) {
		commandNr = responseStr.at(1);
		systemCommand = (SystemCommand)responseStr.at(2);
		if(systemCommand == SYSTEMCMD_RESPONSE_STATUS) {
			systemCommandResponseStatus = (SystemCommandResponseStatus)responseStr.at(3);
			switch(systemCommandResponseStatus) {
				case SYSTEMCMD_RESPONSE_STATUS_ERROR:
				case SYSTEMCMD_RESPONSE_STATUS_OK:
				case SYSTEMCMD_RESPONSE_STATUS_BUSY:
				case SYSTEMCMD_RESPONSE_STATUS_INPUT_ERROR:
					returnCode = true;
					break;
				case SYSTEMCMD_RESPONSE_STATUS_DATA:
					commandData = responseStr.substr(4);
					returnCode = true;
					break;
				default:
					//LOG(Logger::LOG_ERROR, "CCU2CoprocessorCommandMod::parseSystemCommandResponse(): Unknown response status. Incoming message dropped.");
					break;
			}
		}
		else if(systemCommand == SYSTEMCMD_EVENT_DUTYCYCLE) {
			dutyCycleValue = responseStr.at(3);
			returnCode = true;
		}
		else if(systemCommand == SYSTEMCMD_IDENTIFY) {//comes as event too
			commandData = responseStr.substr(3);
			returnCode = true;
		}
		else {
			//LOG(Logger::LOG_ERROR, "CCU2CoprocessorCommandMod::parseSystemCommandResponse(): Not a response status. Incoming message dropped.");
		}
	}
	else {
		//LOG(Logger::LOG_ERROR, "CCU2CoprocessorCommandMod::parseSystemCommandResponse(): Message size to small. Incoming message dropped.");
	}
	return returnCode;
}

bool CCU2CoprocessorCommandMod::parseBidcosCommandResponse(const std::string& responseStr)
{
	bool returnCode = false;
	if(responseStr.size() >= 4) {
		commandData.append(responseStr.substr(2, 2));	
		commandNr = responseStr.at(1); 
		bidcosCommand = (BidcosCommand)responseStr.at(2);
		if(bidcosCommand == BIDCOSCMD_STATUS) {
			bidcosCommandResponseStatus = (BidcosCommandResponseStatus)responseStr.at(3);
			switch(bidcosCommandResponseStatus) {
				//status without data
				case BIDCOSCMD_RESPONSE_STATUS_ERROR:
				case BIDCOSCMD_RESPONSE_STATUS_OK:
				case BIDCOSCMD_RESPONSE_STATUS_SEND_FAILED:
				case BIDCOSCMD_RESPONSE_STATUS_SEND_FAILED_DUTYCYLCE:
				case BIDCOSCMD_RESPONSE_STATUS_SEND_FAILED_CSMACA:
				case BIDCOSCMD_RESPONSE_STATUS_BUSY:
				case BIDCOSCMD_RESPONSE_STATUS_TRX_BUSY:
				case BIDCOSCMD_RESPONSE_STATUS_NOT_INITIALIZED:
				case BIDCOSCMD_RESPONSE_STATUS_INPUT_WRONG:
				case BIDCOSCMD_RESPONSE_STATUS_UNKNOWN:
					returnCode = true;
					break;
				case BIDCOSCMD_RESPONSE_STATUS_TELEGRAM_SENT:
					bidcosCommandEventAuthStatus = BIDCOSCMD_EVENT_AUTH_STATUS_NO_AUTH_REQUIRED;
					returnCode = true;
					break;
				//status with data
				case BIDCOSCMD_RESPONSE_STATUS_DATA: //FIXME Datenrahmennummer + Anzahl zu �bertragene Daten
					if(responseStr.size() >= 7 && responseStr.size() <= 66) { //6 + 1 till 60 byte data --> 7 - 66 bytes
						int msgFragNr = (int)responseStr.at(4); //(upper threshold to avoid possible buffer overflows
						int msgFrags = (int)responseStr.at(5);
						if(msgFrags > 1) {
							//LOG(Logger::LOG_FATAL_ERROR, "CCU2CoprocessorCommandMod::parseBidcosCommandResponse(): Fragmented messages not supported yet!");
						}
						commandData = responseStr.substr(6);						
						returnCode = true;
					}
					else {
						//LOG(Logger::LOG_ERROR, "CCU2CoprocessorCommandMod::parseBidcosCommandResponse(): BidCoS message (status with data) size not in specified limits. Incoming message dropped.");
					}
					break;
				case BIDCOSCMD_RESPONSE_STATUS_AUTHENTICATION_SUCCESS:
					if(responseStr.size() >= 15 && responseStr.size() <= 32) {//6 byte until rssi (inclusive) + 9 up to 26 byte telegram data --> 15 - 32 bytes
						authKeyIndex = responseStr.at(4); 
						bidcosCommandEventAuthStatus = BIDCOSCMD_EVENT_AUTH_STATUS_AUTH_SUCCESSFUL;
						rssi = responseStr.at(5);
						if(!rssi)
						{
							//rssi = BidcosFrame::INVALID_RSSI_VALUE;
						}
						commandData = responseStr.substr(6);
						////LOG(Logger::LOG_DEBUG, "ACK telegram data: %s", toHexStr(commandData).c_str());
						returnCode = true;
					}
					else {
						//LOG(Logger::LOG_ERROR, "CCU2CoprocessorCommandMod::parseBidcosCommandResponse(): BidCoS message size not in specified limits. Incoming message dropped.");
					}
					break;
				case BIDCOSCMD_RESPONSE_STATUS_TELEGRAM_SENT_RECVD_ACK:
					if(responseStr.size() >= 15 && responseStr.size() <= 32) {//6 byte until rssi (inclusive) + 9 up to 26 byte telegram data --> 15 - 32 bytes
						//authKeyIndex = responseStr.at(4); //usage only allowed on BIDCOSCMD_RESPONSE_STATUS_AUTHENTICATION_SUCCESS
						bidcosCommandEventAuthStatus = BIDCOSCMD_EVENT_AUTH_STATUS_NO_AUTH_REQUIRED;
						rssi = responseStr.at(5);
						if(!rssi)
						{
							//rssi = BidcosFrame::INVALID_RSSI_VALUE;
						}
						commandData = responseStr.substr(6);
						////LOG(Logger::LOG_DEBUG, "ACK telegram data: %s", toHexStr(commandData).c_str());
						returnCode = true;
					}
					else {
						//LOG(Logger::LOG_ERROR, "CCU2CoprocessorCommandMod::parseBidcosCommandResponse(): BidCoS message size not in specified limits. Incoming message dropped.");
					}
					break;
				case BIDCOSCMD_RESPONSE_STATUS_AUTHENTICATION_FAILURE:
					if(responseStr.size() >= 5) {
						bidcosCommandEventAuthStatus = BIDCOSCMD_EVENT_AUTH_STATUS_AUTH_FAILED;
						authKeyIndex = responseStr.at(4);
						if(authKeyIndex == (char)0xFE) {//TWIST-532
							bidcosCommandEventAuthStatus = 	BIDCOSCMD_EVENT_AUTH_STATUS_AUTH_KEY_INDEX_UNKNOWN;
						}
						returnCode = true;
						break;
					}
					else {
						//LOG(Logger::LOG_ERROR, "CCU2CoprocessorCommandMod::parseBidcosCommandResponse(): BidCoS message size not in specified limits. Incoming message dropped.");
					}
				default:
					commandData.clear();
					break;
			}
		}
		else if(bidcosCommand == BIDCOSCMD_EVENT) {
			bidcosCommandEventAuthStatus = (BidcosCommandEventAuthentificationStatus)(responseStr.at(3) & 0x07); //only last three bits determine auth type
			devWokenUp = ((responseStr.at(3) & (char)0x10) != 0);
			if(responseStr.size() >= 15 && responseStr.size() <= 32) { //has allowed amount of data?
				authKeyIndex = responseStr.at(4);
				rssi = responseStr.at(5);
				if(!rssi)
				{
					//rssi = BidcosFrame::INVALID_RSSI_VALUE;
				}
				commandData = responseStr.substr(6);
				////LOG(Logger::LOG_ALL, "Event telegram data: %s", toHexStr(commandData).c_str());
				returnCode = true;
			}
			else {
				//LOG(Logger::LOG_ERROR, "CCU2CoprocessorCommandMod::parseBidcosCommandResponse(): BidCoS event-message size not in specified limits. Incoming message dropped.");
			}
		}
	}
	else {
		//LOG(Logger::LOG_ERROR, "CCU2CoprocessorCommandMod::parseBidcosCommandResponse(): Unknown BidCoS Command");
	}
	return returnCode;
}

BidcosCommandEventAuthentificationStatus CCU2CoprocessorCommandMod::getAuthentificationStatus() const
{
	return bidcosCommandEventAuthStatus;
}

bool CCU2CoprocessorCommandMod::hasAuthentificationStatus() const {
	return (bidcosCommandEventAuthStatus != BIDCOSCMD_EVENT_AUTH_STATUS_UNKNOWN);
}

/*
std::string CCU2CoprocessorCommandMod::parseIdentifyResponse( const std::string& identifyResponse)
{
    std::string identification;
    if(identifyResponse.size() > 4) {
        //check sequence nr                         
       // if(identifyCommand.at(2) == identifyResponse.at(1)) {
        if(identifyResponse.at(2) == '\4') {//status
            if(identifyResponse.at(3) == '\2') {//ok
                identification = identifyResponse.substr(4);                          
            }                         
        }                          
        //}
    }
    return identification;
}
*/

std::string CCU2CoprocessorCommandMod::getCommandString()
{
	switch(commandType) {
		case COMMANDTYPE_SYSTEM:
			return assembleSystemCommandString();
		case COMMANDTYPE_BIDCOS:
			if(hmipCoproFrame)
			{
				return assembleHmipTrxAdapterCommandString();
			}
			else
			{
				return assembleBidcosCommandString();
			}
		case COMMANDTYPE_HMIP_COMMON:
			return assembleHmipCommonCommandString();
		case COMMANDTYPE_LOWLEVELMAC:
			return assembleLowLevelMacCommandString();
		default:
			return "";
	}
}

CommandType CCU2CoprocessorCommandMod::getCommandType() const
{
	return commandType;
}

bool CCU2CoprocessorCommandMod::isResponseValid() const
{
	return responseValid;
}

std::string CCU2CoprocessorCommandMod::getCommandData() const
{
	return commandData;
}

int CCU2CoprocessorCommandMod::getBidcosTelegramDestinationAddress() const
{
	int addr = -1;
	if(commandData.size() >= 9) { //destination address are byte 6 to 8
		std::string addrStr = commandData.substr(6, 3);
		//big endian -> little endian
		addr = convertBidEndianStringToBidcosAddress( addrStr );
	}
	return addr;
}

int CCU2CoprocessorCommandMod::getBidcosTelegramSourceAddress() const
{
	int addr = -1;
	if(commandData.size() >= 6) {
		std::string addrStr = commandData.substr(3, 3);
		addr = convertBidEndianStringToBidcosAddress( addrStr );
	}
	return addr;
}

int CCU2CoprocessorCommandMod::getBidcosTelegramCounter() const
{
	int tc = 0xEEEEEEEE;//Not same "error" value as in CommController, which is important!
	if(commandData.size() >= 1) {
		tc = (int)commandData.at(0);
	}
	return tc;
}

bool CCU2CoprocessorCommandMod::isEvent() const
{
	if(commandType == COMMANDTYPE_SYSTEM) {
		if(systemCommand == SYSTEMCMD_EVENT_DUTYCYCLE) {
			return true;
		}
		else if(systemCommand == SYSTEMCMD_IDENTIFY) {
			return true;
		}
		else {
			return false;
		}
	}
	else if(commandType == COMMANDTYPE_BIDCOS) {
		if(bidcosCommand == BIDCOSCMD_EVENT) {
			return true;
		}
		else {
			return false;
		}
	}
	else if(commandType == COMMANDTYPE_HMIP_COMMON){
		if(hmipCommonCommand == HMIP_COMMON_IDENTIFY_EVENT)
		{
			return true;
		}
		else {
			return false;
		}
	}
	else{
		return false;
	}
}

bool CCU2CoprocessorCommandMod::isDutyCycleEvent() const
{
	return (systemCommand == SYSTEMCMD_EVENT_DUTYCYCLE);
}

double CCU2CoprocessorCommandMod::getDutyCycleValue() const
{
	return ((double)dutyCycleValue) / (double)2;//maybe multiply with 100, then divide with 200
}

bool CCU2CoprocessorCommandMod::isIdentifyEvent() const
{
	return (systemCommand == SYSTEMCMD_IDENTIFY) || (hmipCommonCommand == HMIP_COMMON_IDENTIFY_EVENT);
}

bool CCU2CoprocessorCommandMod::isResponseStatusOk() const
{
	if(commandType == COMMANDTYPE_SYSTEM) {
		if( (systemCommandResponseStatus == SYSTEMCMD_RESPONSE_STATUS_OK) ||
			(systemCommandResponseStatus == SYSTEMCMD_RESPONSE_STATUS_DATA)) {
			LOG(Logger::LOG_DEBUG,"CCU2CoprocessorCommandMod::isResponseStatusOk(): System status OK");
			return true;
		}
	}
	else if(commandType == COMMANDTYPE_BIDCOS && !hmipCoproFrame) {
		if( (bidcosCommandResponseStatus == BIDCOSCMD_RESPONSE_STATUS_DATA) ||
			(bidcosCommandResponseStatus == BIDCOSCMD_RESPONSE_STATUS_OK) ||
			(bidcosCommandResponseStatus == BIDCOSCMD_RESPONSE_STATUS_TELEGRAM_SENT) ||
			(bidcosCommandResponseStatus == BIDCOSCMD_RESPONSE_STATUS_TELEGRAM_SENT_RECVD_ACK) ||
			(bidcosCommandResponseStatus == BIDCOSCMD_RESPONSE_STATUS_AUTHENTICATION_SUCCESS) ) {
			return true;
		}
	}
	else if(commandType == COMMANDTYPE_HMIP_COMMON){
		if( hmipCommonCommandResponseStatus == HMIP_COMMONCMD_RESPONSE_OK)
		{
			return true;
		}
	}
	else if( commandType == COMMANDTYPE_TRX_ADAPTER && hmipCoproFrame )
	{
		if(hmipTrxAdapterResponseStatus == HMIP_TRXADAPTERCMD_RESPONSE_OK)
		{
			return true;
		}
	}
	else if( commandType == COMMANDTYPE_LOWLEVELMAC && hmipCoproFrame) {
		if(lowlevelmacCommand == LOWLEVELMAC_COMMAND_RESPONSE && lowLevelMacResponseStatus == LOWLEVELMAC_RESPONSE_ACK) {
			return true;
		}
	}
	return false;
}

bool CCU2CoprocessorCommandMod::isResponseStatusBusy() const
{
	return ( (bidcosCommandResponseStatus == BIDCOSCMD_RESPONSE_STATUS_BUSY) || 
	         (bidcosCommandResponseStatus == BIDCOSCMD_RESPONSE_STATUS_TRX_BUSY) ||
			 (systemCommandResponseStatus == SYSTEMCMD_RESPONSE_STATUS_BUSY) ||
			 (hmipCommonCommandResponseStatus == HMIP_COMMONCMD_RESPONSE_BUSY) ||
			 (lowLevelMacResponseStatus == LOWLEVELMAC_RESPONSE_BUSY) ||
			 (lowLevelMacResponseStatus == LOWLEVELMAC_RESPONSE_BUSY_TRX)
			 );
}

bool CCU2CoprocessorCommandMod::isResponseOfRequest(const CCU2CoprocessorCommandMod& request, const CCU2CoprocessorCommandMod& response)
{
	if(response.commandType == COMMANDTYPE_BIDCOS) {
		return (request.commandNr == response.commandNr);
	}
	else if(response.commandType == COMMANDTYPE_SYSTEM) {
		return (request.commandNr == response.commandNr);
	}
	else if(response.commandType == COMMANDTYPE_HMIP_COMMON) {
		return (request.commandNr == response.commandNr);
	}
	else if(response.commandType == COMMANDTYPE_LOWLEVELMAC) {
		return (request.commandNr == response.commandNr);
	}
	else {
		//TODO Log an error
		return false;
	}
}

BidcosCommandResponseStatus CCU2CoprocessorCommandMod::getBidcosCommandResponseStatus()
{
	return bidcosCommandResponseStatus;
}

std::string CCU2CoprocessorCommandMod::getBidcosCommandResponseStatusAsString() const
{
	return responseStatusToString( bidcosCommandResponseStatus );
}

int CCU2CoprocessorCommandMod::getRSSI() const
{
	return (rssi);
}

bool CCU2CoprocessorCommandMod::isRSSISet() const
{
	return false;//(rssi != BidcosFrame::INVALID_RSSI_VALUE);
}

bool CCU2CoprocessorCommandMod::isDevWokenUp() const
{
	return devWokenUp;
}

char CCU2CoprocessorCommandMod::getAuthenticationKeyIndex() const {
	return authKeyIndex;
}

bool CCU2CoprocessorCommandMod::parseHmipCommonCommand(
		const std::string& frameData) {
	bool returnCode = false;
		if(frameData.size() >= 4) {
			commandData.append(frameData.substr(2, 2));
			commandNr = frameData.at(1);
			hmipCommonCommand = (HmipCommonCommand)frameData.at(2);
			switch(hmipCommonCommand)
			{
				case HMIP_COMMON_IDENTIFY_EVENT:
					returnCode = true;
					commandData = frameData.substr(3);
					break;
				case HMIP_COMMON_GET_IDENTIFY:
					break;
				case HMIP_COMMON_START_BOOTLOADER:
					break;
				case HMIP_COMMON_START_APPLICATION:
					break;
				case HMIP_COMMON_GET_SGTIN:
					break;
				case HMIP_COMMON_RESPONSE:
				{
					hmipCommonCommandResponseStatus = (HmipCommonCommandResponseStatus)frameData.at(3);
					switch(hmipCommonCommandResponseStatus)
					{
					case HMIP_COMMONCMD_RESPONSE_OK:
						returnCode = true;
						if(frameData.size() > 4)
						{
							commandData = frameData.substr(4);
						}
						break;
					case HMIP_COMMONCMD_RESPONSE_BUSY:
						returnCode = true;
						break;
					case HMIP_COMMONCMD_RESPONSE_WRON_INPUT:
						returnCode = true;
						break;
						case HMIP_COMMONCMD_RESPONSE_ERROR:
					default:
						break;
					}
				}
					break;
				default:
					break;
			}
		}
		return returnCode;
}

bool CCU2CoprocessorCommandMod::parseHmiptrxAdapterCommand(const std::string& frameData)
{

bool returnCode = false;
	if(frameData.size() >= 4) {
		commandData.append(frameData.substr(2, 2));
		commandNr = frameData.at(1);
		hmipTrxAdapterCommand = (HmipTrxAdapterCommand)frameData.at(2);
		switch(hmipTrxAdapterCommand)
		{
		case HMIP_TRXADAPTER_Get_VERSION:
			break;
		case HMIP_TRXADAPTER_RESPONSE:
		{
			hmipTrxAdapterResponseStatus =
					(HmipTrxAdapterCommandResponseStatus) frameData.at(3);
			switch (hmipTrxAdapterResponseStatus) {
			case HMIP_TRXADAPTERCMD_RESPONSE_OK:
				returnCode = true;
				if (frameData.size() > 4) {
					commandData = frameData.substr(4);
				}
				break;
			case HMIP_TRXADAPTERCMD_RESPONSE_BUSY:
				returnCode = true;
				break;
			case HMIP_TRXADAPTERCMD_RESPONSE_WRON_INPUT:
				returnCode = true;
				break;
			case HMIP_TRXADAPTERCMD_RESPONSE_ERROR:
			default:
				break;
			}
		}
			break;
		default:
			break;
		}
	}
	return returnCode;
}

bool CCU2CoprocessorCommandMod::parseLowLevelMacResponseFrame(const std::string& frameData)
{
	bool validResponse = false;
	if(frameData.size() >= 4) {
			commandData.append(frameData.substr(2, 2));
			commandNr = frameData.at(1);
			lowlevelmacCommand = (LowLevelMacCommand)frameData.at(2);
			switch(lowlevelmacCommand)
			{
				case LOWLEVELMAC_COMMAND_RESPONSE://only type from copro
					lowLevelMacResponseStatus = (LowLevelMacResponseStatus)frameData.at(3);//To be checked!!!!!!!!!!!
					if(frameData.size() > 4)
					{
						commandData = frameData.substr(4);
					}
					validResponse = true;
					break;
				default:
					//TODO May log here
					break;
			}
		}
		return validResponse;
}

std::string CCU2CoprocessorCommandMod::responseStatusToString(const BidcosCommandResponseStatus& responseStatus) const
{
	std::string text;
	switch(responseStatus) {
		case BIDCOSCMD_RESPONSE_STATUS_ERROR:
			text.append("Error");
			break;
		case BIDCOSCMD_RESPONSE_STATUS_OK:
			text.append("OK");
			break;
		case BIDCOSCMD_RESPONSE_STATUS_TELEGRAM_SENT:
			text.append("Telegram sent successfully");
			break;
		case BIDCOSCMD_RESPONSE_STATUS_TELEGRAM_SENT_RECVD_ACK:
			text.append("Telegram sent, received ACK");
			break;
		case BIDCOSCMD_RESPONSE_STATUS_SEND_FAILED:
			text.append("Send failed");
			break;
		case BIDCOSCMD_RESPONSE_STATUS_SEND_FAILED_DUTYCYLCE:
			text.append("Send failed because of full duty cycle");
			break;
		case BIDCOSCMD_RESPONSE_STATUS_SEND_FAILED_CSMACA:
			text.append("Send failed (CSMA/CA");
			break;
		case BIDCOSCMD_RESPONSE_STATUS_DATA:
			text.append("OK, Data");
			break;
		case BIDCOSCMD_RESPONSE_STATUS_BUSY:
			text.append("Busy");
			break;
		case BIDCOSCMD_RESPONSE_STATUS_TRX_BUSY:
			text.append("Transceiver busy");
			break;
		case BIDCOSCMD_RESPONSE_STATUS_NOT_INITIALIZED:
			text.append("Not initialized");
			break;
		case BIDCOSCMD_RESPONSE_STATUS_INPUT_WRONG:
			text.append("Input wrong");
			break;
		case BIDCOSCMD_RESPONSE_STATUS_AUTHENTICATION_SUCCESS:
			text.append("Authentication succeeded");
			break;
		case BIDCOSCMD_RESPONSE_STATUS_AUTHENTICATION_FAILURE:
			text.append("Authentication failed.");
			break;
		default:
			text.append("FATAL ERROR: Unknown response code.");
			break;
	}
	return text;
}

std::string CCU2CoprocessorCommandMod::toString(const int i)
{
    char* buffer = new char[11];
    memset(buffer, 0, 11);
    snprintf(buffer, 11, "%d", i);
    std::string s(buffer);
    delete[] buffer;
    return s;
}

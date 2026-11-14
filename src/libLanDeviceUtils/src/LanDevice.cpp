/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "LanDevice.h"
#include <Protocol.h>
#include <EQ3ConfigProtocol.h>
#include <sstream>
#include <stdlib.h>
#include <openssl/evp.h>

using namespace LDU;

LanDevice::LanDevice(ProtocolEnum protocolType, RoutingSchemeEnum reachedByRoutingScheme)
:status(STATUS_UNKNOWN)
{
	LanDevice::protocolType = protocolType;
	LanDevice::reachedByRoutingScheme = reachedByRoutingScheme;
}

LanDevice::LanDevice()
:status(STATUS_UNKNOWN)
{
	LanDevice::protocolType = PROTOCOL_UNKNOWN;
}


LanDevice::~LanDevice()
{
}

const std::string& LanDevice::getSerialNumber() const {
	return serialNumber;
}

void LanDevice::setSerialNumber(const std::string &serialNumber) {
	this->serialNumber = serialNumber;
}

const std::string& LanDevice::getType() const {
	return type;
}

void LanDevice::setType(const std::string &typeName) {
	type = typeName;
}

const std::string& LanDevice::getFirmwareVersion() const {
	return firmwareVersion;
}

void LanDevice::setFirmwareVersion(const std::string& v) {
	firmwareVersion = v;
}

const std::vector<LanDevice::ServiceProtocol>& LanDevice::getServiceProtocols() const {
	return serviceProtocols;
}
	
void LanDevice::setServiceProtocols(const std::vector<LanDevice::ServiceProtocol>& sps) {
	serviceProtocols = sps;
}

std::string LanDevice::getLanIfCfgProtocolName() const {
	Protocol* pProt = Protocol::createProtocol(protocolType);
	if(pProt != NULL) {
		std::string name = pProt->getProtocolName();
		delete pProt;
		return name;
	}

	return "";
}

const IPConfiguration& LanDevice::getIPConfiguration() const {
	return ipConfiguration;
}

void LanDevice::setIPConfiguration(const IPConfiguration& ipConfig) {
	ipConfiguration = ipConfig;
}

const RuntimeIPConfiguration& LanDevice::getRuntimeIPConfiguration() const {
	return runtimeIPConfiguration;
}

void LanDevice::setRuntimeIPConfiguration(const RuntimeIPConfiguration& ipConfig) {
	runtimeIPConfiguration = ipConfig;
}

const TestStatusConfiguration& LanDevice::getTestStatusConfiguration() const {
	return this->testStatusConfiguration;
}

void LanDevice::setTestStatusConfiguration(const TestStatusConfiguration& testStatusConfiguration) {
	this->testStatusConfiguration = testStatusConfiguration;
}

const LanDevice::Status& LanDevice::getStatus() const {
	return status;
}

ProtocolEnum LanDevice::getProtocolType() const {
	return protocolType;
}
	
void LanDevice::setProtocolType(ProtocolEnum protocolType) {
	this->protocolType = protocolType;
}

const RoutingSchemeEnum& LanDevice::getReachedByRoutingScheme() const {
	return reachedByRoutingScheme;
}
void LanDevice::setReachedByRoutingScheme(RoutingSchemeEnum reachedByRoutingScheme) {
	this->reachedByRoutingScheme = reachedByRoutingScheme;
}

void LanDevice::setAesKey(unsigned char aesKey[], int lengthOfAesKeyAndIv)
{
	this->aesKey = aesKey;
	this->lengthOfAesKeyAndIv = lengthOfAesKeyAndIv;
}

unsigned char* LanDevice::getAesKey()
{
	return this->aesKey;
}

void LanDevice::setIv(std::string responseWithInv)
{
	unsigned char temp[16] = { 0x00, 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0a, 0x0b, 0x0c, 0x0d, 0x0e, 0x0f };
	this->parseResponseToAesIv(responseWithInv, temp);
	this->setIv(temp);
}

void LanDevice::setIv(unsigned char iv[])
{
	this->iv = iv;
}

unsigned char* LanDevice::getIv()
{
	return this->iv;
}


int LanDevice::getLengthOfAesKey()
{
	return this->lengthOfAesKeyAndIv;
}

std::string LanDevice::encryptMessage(unsigned char opcode, unsigned char data[], int dataLength)
{
	const unsigned char OPCODE_WRTIE_FIRMWARE = 'W';

	const unsigned char OPCODE_SEND_ENCRYPTED = '*';
	const int LENGTH_OF_SIGNATURE = AES_BLOCK_SIZE;
	const unsigned char AES_SIGNATURE_TO_ENCRYPT[LENGTH_OF_SIGNATURE] = { 'e', 'Q', '-', '3', '_', '_', 'U', 'D', 'P', '-', 'C', 'r', 'y', 'p', 't', '\0' };

	EVP_CIPHER_CTX* pCtx = EVP_CIPHER_CTX_new();
	
	unsigned char* aesIv = this->getIv();
	unsigned char* aesKey = this->getAesKey();
	EVP_EncryptInit_ex(pCtx, EVP_aes_128_cbc(), NULL, aesKey, aesIv);
	// raw block CBC, no PKCS7 padding (payload is manually padded to block size)
	EVP_CIPHER_CTX_set_padding(pCtx, 0);

	unsigned char notEncryptedPartOfMessage[1200];
	unsigned char encryptedPartOfMessage[1200];
	int indexNotEncryptedPartOfMessage = 0;
	int indexEncryptedPartOfMessage = 0;

	// <-- beginning of not encrypted part of message -->
	// write header
	int headerLength = 0;
	std::string header(EQ3ConfigProtocol::assembleHeaderFrameBase(this->getType(), this->getSerialNumber(), headerLength));
	indexNotEncryptedPartOfMessage += headerLength;
	this->convertStringToUnsignedCharArray(header, notEncryptedPartOfMessage);

	// write opcode * for encryption
	notEncryptedPartOfMessage[header.length()] = OPCODE_SEND_ENCRYPTED;
	indexNotEncryptedPartOfMessage++;
	// <-- end of not encrypted part of message -->


	// <-- beginning of encrypted part of message -->
	// 16 byte random data
	for (indexEncryptedPartOfMessage = 0; indexEncryptedPartOfMessage < this->AES_BLOCK_SIZE; indexEncryptedPartOfMessage++)
	{
		encryptedPartOfMessage[indexEncryptedPartOfMessage] = (unsigned char)(rand() & 0xFF);
	}

	// n byte signature
	for (int i = 0; i < LENGTH_OF_SIGNATURE; i++)
	{
		encryptedPartOfMessage[indexEncryptedPartOfMessage] = AES_SIGNATURE_TO_ENCRYPT[i];
		indexEncryptedPartOfMessage++;
	}

	// 1 byte opcode
	encryptedPartOfMessage[indexEncryptedPartOfMessage++] = opcode;

	// n byte data
	for (int i = 0; i < dataLength; i++)
	{
		encryptedPartOfMessage[indexEncryptedPartOfMessage++] = data[i];
	}

	// fill last block with random data if not already full
	while ((indexEncryptedPartOfMessage % this->AES_BLOCK_SIZE) != 0)
	{
		encryptedPartOfMessage[indexEncryptedPartOfMessage] = (unsigned char)(rand() & 0xFF);
		indexEncryptedPartOfMessage++;
	}
	// <-- end of encrypted part of message -->

	unsigned char newdata[1200];
	unsigned char temp[AES_KEY_AND_IV_LENGTH];
	for (int i = 0; i < indexEncryptedPartOfMessage / AES_KEY_AND_IV_LENGTH; i++)
	{
		for (int j = 0; j < AES_KEY_AND_IV_LENGTH; j++)
		{
			temp[j] = encryptedPartOfMessage[i * AES_KEY_AND_IV_LENGTH + j];
		}

		//aesProcessData(&aesOptions, temp, indexEncryptedPartOfMessage);
		int outLen = 0;
		EVP_EncryptUpdate(pCtx, temp, &outLen, temp, AES_KEY_AND_IV_LENGTH);


		// set IV to if opcode = w && i last part of message
		if(opcode == OPCODE_WRTIE_FIRMWARE && i == ((indexEncryptedPartOfMessage / AES_KEY_AND_IV_LENGTH) - 1))
		{	
			unsigned char newiv[AES_KEY_AND_IV_LENGTH];
			for (int j = 0; j < AES_KEY_AND_IV_LENGTH; j++)
			{
				newiv[j]  = temp[j];
			}

			this->setIv(newiv);
		}

		for (int j = 0; j < AES_KEY_AND_IV_LENGTH; j++)
		{
			newdata[i * AES_KEY_AND_IV_LENGTH + j]  = temp[j];
		}
	}

	std::string result;
	this->convertUnsignedCharArrayToString(
		notEncryptedPartOfMessage,
		indexNotEncryptedPartOfMessage,
		//encryptedPartOfMessage,
		newdata,
		indexEncryptedPartOfMessage,
		result);
	if(pCtx != NULL) {
		EVP_CIPHER_CTX_free(pCtx);
	}
	return result;
}

void LanDevice::parseResponseToAesIv(const std::string responseWithInv, unsigned char aesIv[AES_KEY_AND_IV_LENGTH])
{
	const int OFFSET_OPCPODE_RETURN_CODE = 2;
	std::string::size_type beginOfAesIv = responseWithInv.find_first_of('>') + 1 + OFFSET_OPCPODE_RETURN_CODE; // first sign after '>'
	for (std::string::size_type i = 0; i < AES_KEY_AND_IV_LENGTH; i++)
	{
		aesIv[i] = responseWithInv.at(beginOfAesIv + i);
	}
}

std::string LanDevice::assembleHeaderFrameBase(const std::string& devType, const std::string& serial, int& headerLength)
{
	std::string frame("");
	frame.append(1, 0x1); // UDP-Version
	frame.append(devType); // Ger�tekennung  
	frame.append(1, 0x0);
	frame.append(serial); // Seriennummer
	frame.append(1, 0x0);

	headerLength = frame.length();
	return frame;
}

void LanDevice::convertStringToUnsignedCharArray(std::string str, unsigned char result[])
{
	for (unsigned int i = 0; i < str.length(); i++)
	{
		result[i] = str.at(i);
	}
}

void LanDevice::convertUnsignedCharArrayToString(
	const unsigned char* notEncryptedPartOfMessage,
	const int indexNotEncryptedPartOfMessage,
	const unsigned char* encryptedPartOfMessage,
	const int indexEncryptedPartOfMessage,
	std::string& result)
{
	std::stringstream out;

	for (int i = 0; i < indexNotEncryptedPartOfMessage; i++)
	{
		out << notEncryptedPartOfMessage[i];
		result.append(out.str());
		
		// reset stringstream
		out.str("");
		out.clear();
	}

	for (int i = 0; i < indexEncryptedPartOfMessage; i++)
	{
		out << encryptedPartOfMessage[i];
		result.append(out.str());
		
		// reset stringstream
		out.str("");
		out.clear();
	}
}

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <TCPEncryption.h>
#include <openssl/evp.h>
#include <string.h>


using namespace ldu;


TCPEncryption::TCPEncryption()
:pCtxEnc(NULL)
,pCtxDec(NULL)
{
	pCtxEnc = EVP_CIPHER_CTX_new();
	pCtxDec = EVP_CIPHER_CTX_new();
}

TCPEncryption::~TCPEncryption()
{
	if(pCtxEnc != NULL) {
		EVP_CIPHER_CTX_free(pCtxEnc);
	}
	if(pCtxEnc != NULL) {
		EVP_CIPHER_CTX_free(pCtxDec);
	}
}


TCPEncryption::TCPEncryption(const TCPEncryption& other)
:pCtxEnc(NULL)
,pCtxDec(NULL)
{
	pCtxEnc = EVP_CIPHER_CTX_new();
	pCtxDec = EVP_CIPHER_CTX_new();
	copy(other);
}

TCPEncryption& TCPEncryption::operator =(const TCPEncryption& other)
{
	if(this == &other) {
		return *this;
	}
	copy(other);
	return *this;
}


void TCPEncryption::copy(const TCPEncryption& other)
{
	EVP_CIPHER_CTX_copy(pCtxEnc, other.pCtxEnc);
	EVP_CIPHER_CTX_copy(pCtxDec, other.pCtxDec);
}

void TCPEncryption::init(const std::string& key, const std::string& ivEncryption, const std::string& ivDecryption)
{
	const unsigned char* pKey = (unsigned char*)key.c_str();
	const unsigned char* pIvEnc = (unsigned char*)ivEncryption.c_str();
	const unsigned char* pIvDec = (unsigned char*)ivDecryption.c_str();

	EVP_EncryptInit_ex(pCtxEnc, EVP_aes_128_cfb(), NULL, pKey, pIvEnc);
	EVP_DecryptInit_ex(pCtxDec, EVP_aes_128_cfb(), NULL, pKey, pIvDec);
}

void TCPEncryption::encrypt(std::string& data)
{
	unsigned char* outBuffer = new unsigned char[data.size() + blockSize];
	int outLen = 0;
	int sum = 0;
	EVP_EncryptUpdate(pCtxEnc, outBuffer, &outLen, (const unsigned char*) data.c_str(), data.size());
	sum = outLen;
	EVP_EncryptFinal_ex(pCtxEnc, outBuffer+sum, &outLen);
	sum += outLen;
	data.assign((const char*)outBuffer, sum);
	delete[] outBuffer;
}

void TCPEncryption::decrypt(std::string& data)
{
	unsigned char* outBuffer = new unsigned char[data.size() + blockSize];
	int outLen = 0;
	int sum = 0;
	EVP_DecryptUpdate(pCtxDec, outBuffer, &outLen, (const unsigned char*)data.c_str(), data.size());
	sum = outLen;
	EVP_DecryptFinal_ex(pCtxDec, outBuffer+sum, &outLen);
	sum += outLen;
	data.assign((const char*)outBuffer, sum);
	delete[] outBuffer;
}

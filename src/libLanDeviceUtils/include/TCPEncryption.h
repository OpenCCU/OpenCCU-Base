/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _TCPENCRYPTION_H_
#define _TCPENCRYPTION_H_

#include "DLLImportExport.h"
#include <string>

typedef struct evp_cipher_ctx_st EVP_CIPHER_CTX;

namespace ldu {

LIBLANDEVICEUTILS_API class TCPEncryption {

public:
	LIBLANDEVICEUTILS_API TCPEncryption();
	LIBLANDEVICEUTILS_API TCPEncryption(const TCPEncryption& other);
	LIBLANDEVICEUTILS_API TCPEncryption& operator=(const TCPEncryption& other);
	LIBLANDEVICEUTILS_API virtual ~TCPEncryption();

	LIBLANDEVICEUTILS_API void init(const std::string& key, const std::string& ivEncryption, const std::string& ivDecryption);
	LIBLANDEVICEUTILS_API void encrypt(std::string& data);
	LIBLANDEVICEUTILS_API void decrypt(std::string& data);

private:
	EVP_CIPHER_CTX* pCtxEnc;
	EVP_CIPHER_CTX* pCtxDec;
	static const int blockSize = 16;

	void copy(const TCPEncryption& other);
};

}

#endif

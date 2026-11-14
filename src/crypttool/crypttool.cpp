/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// crypttool.cpp : Definiert den Einstiegspunkt f�r die Konsolenanwendung.
//
#include <string.h>
#include <cstdlib>
#include <string>
#include <fstream>
#include <iostream>

// bidcos_utils
#include <SyslogLogger.h>
#include <ConsoleLogger.h>
#include <md5.h>

#include "openssl/evp.h"

#include "crypttoolconfig.h"
#include "generated-crypttool-version.h"
#include "elvutils_version.h"

const char* defaultKey = "Markus is awesome";

static void printVersion()
{
	printf("crypttool %s (%s)\n", CRYPTTOOL_VERSION, __DATE__);
	printf("  libelvutils %s\n", elvutils_version());
}

static void usage(const char* procname)
{
	printf("crypttool %s (%s)\n\n", CRYPTTOOL_VERSION, __DATE__);
	printf("%s [-k key] [-t key_type] [-e] [-d] [-s] [-S] [-v] [-r] [-g] [-y] [-l loglevel] [-f path/to/configFile]\n", procname);
	printf("  Actions: (Default action is to encrypt with the current user key)\n");
	printf("    -d: decrypt\n");
	printf("    -e: encrypt\n");
	printf("    -s: sign\n");
	printf("    -v: validate key against current user key\n");
	printf("    -S: set current user key\n");
	printf("    -r: reset all keys\n");
	printf("    -g: get key indizes\n");
	printf("  Options:\n");
	printf("    -k: set (temporary) key to use\n");
	printf("    -t: set key type to use\n");
	printf("        0 = default key\n");
	printf("        1 = current user key\n");
	printf("        2 = previous user key\n");
	printf("        3 = temporary key\n");
	printf("    -i: key index for -S\n");
	printf("    -y: log to syslog instead of console\n");
	printf("    -l: set log level\n");
	printf("    --version: show the versions of crypttool and its libraries\n");
	printf("    -f: path to crypttool configuration file\n");

	if(logger)
	{
		delete logger;
	}

	exit(-1);
}

int validate(crypttoolconfig& crypttoolconfig, int key_type, const char* key)
{
	LOG(Logger::LOG_DEBUG, "validate(%d, %s)", key_type, key);

	// get last key from config file
	std::string curTemp = crypttoolconfig.getCurrentKey();
	if(key_type == 0)
	{
		if(!curTemp.empty())
		{
			// Key was changed
			printf("Key mismatch\n");
			return 1;
		}
	}
	else if(curTemp.empty())
	{
		// Default Key is active but was not expected
		printf("Key mismatch\n");
		return 1;
	}
	else
	{
		std::string strTemp = md5::MD5String(key);
		if(strTemp.compare(curTemp) != 0)
		{
			printf("Key mismatch\n");
			return 1;
		}
	}	

	printf("Key OK\n");
	return 0;
}

int crypt(int key_type, bool decrypt, const char* key)
{
	LOG(Logger::LOG_DEBUG, "crypt(%d, %d, %s)", key_type, (int)decrypt, key);

	LOG(Logger::LOG_ERROR, "Encryption and decryption not implemented");
	printf("Encryption and decryption not implemented");
	return -1;
}

int sign(crypttoolconfig& crypttoolconfig, int key_type, const char* key)
{
	LOG(Logger::LOG_DEBUG, "sign(%d, %s)", key_type, key);

	// calculate md5 for file, could not use method in md5 because file is in stdin
	md5 checksum;
	
	LOG(Logger::LOG_DEBUG, "Reading input");
	char buffer[128];
	char *p = buffer;
	int c;
	while((c = fgetc(stdin))!=EOF)
	{
		*p++ = c;
		if(p == buffer + sizeof(buffer))
		{
			p = buffer;
			// MD5 (BidCosUtils)
			checksum.Update((const unsigned char*)buffer, sizeof(buffer));
		}
	}

	LOG(Logger::LOG_DEBUG, "Done");
	if(p != buffer)
	{
		// MD5 (BidCosUtils)
		checksum.Update((const unsigned char*)buffer, p - buffer);
	}


	// MD5 (BidCosUtils)
	checksum.Finalize();
	const unsigned char* digests = checksum.Digest();
	
	// get aes key dependen on the keytype, fallback default key
	std::string aesKey = md5::MD5String(defaultKey);
	if(key_type == 1)
	{
		aesKey = crypttoolconfig.getCurrentKey();
		if(aesKey.empty())
		{
			LOG(Logger::LOG_DEBUG, "Empty or not existing config file, use default key");
			aesKey = md5::MD5String(defaultKey);
		}
	}
	else if(key_type == 3)
	{
		aesKey = md5::MD5String(key);
	}
	else if(key_type != 0)
	{
		LOG(Logger::LOG_ERROR, "No valid key type for signing");
		return -1;
	}

	if(aesKey.empty())
	{
		LOG(Logger::LOG_ERROR, "No valid key for signing");
		return -1;
	}


	
	// encrypt md5 checksum with aes key
	const char * aesKeyp = aesKey.c_str();
	unsigned char aesKeyValue[16];
	unsigned char * aesKeyValuep = (unsigned char *) &aesKeyValue;

	// hexstring to uint8_t array
	for(int i = 0; i < 16; i++)
	{
		sscanf(aesKeyp, "%2hhx", &aesKeyValue[i]);
		aesKeyp += 2 * sizeof(char);
	}

	unsigned char* iv = new unsigned char[16];
	memset(iv, 0, 16);

	EVP_CIPHER_CTX* ctx;
	ctx = EVP_CIPHER_CTX_new();

	// Initialize aes routines with aeskey, cbc-mode and initial vecotr
	
	
	EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, aesKeyValuep ,iv);
	//int blockSize = EVP_CIPHER_CTX_get_block_size(ctx);
	int blockSize = 16;

	// Copy data to new array (const to non const)
	unsigned char signature[16 + blockSize];
	
	// Encrypt data
	int length, ciphertextLength;
	EVP_EncryptUpdate(ctx, signature, &length, digests, 16);
	ciphertextLength = length;

	EVP_EncryptFinal_ex(ctx, signature + length, &length);
	ciphertextLength += length;

	EVP_CIPHER_CTX_free(ctx);

	// print signature used by the security-cgi-script
	for(unsigned int i = 0; i < 16; i++)
	{
		printf("%02x", signature[i]);
	}

	printf("\n");

	delete[] iv;

	return 0;
}

int set_current_user_key(crypttoolconfig& crypttoolconfig, int key_index, const char* key)
{
	// Adds a new key with the index into the config file
	return crypttoolconfig.setCurrentKey(key_index, md5::MD5String(key));
}

int reset_keys(crypttoolconfig& crypttoolconfig)
{
	// Clears the config file
	return crypttoolconfig.resetConfig();
}

int get_index(crypttoolconfig& crypttoolconfig, int key_type)
{
	static const char* KEY_NAMES[]={"Default key", "Current user key", "Previous user key", "Temporary key"};
	// Gets the index of the last line from the config
	int curIndex = crypttoolconfig.getCurrentKeyIndex();
	int indizes[4];

	indizes[0] = 0;
	if(curIndex > 0)
	{
		// Calculate previos key index with current index if avaiable
		indizes[1] = curIndex;
		indizes[2] = curIndex - 1;
	}
	else
	{
		indizes[1] = 0;
		indizes[2] = 0;
	}

	indizes[3] = 0;

	if(key_type >= 0 && key_type <= 3)
	{
		printf("%d\n", indizes[key_type]);
	}
	else
	{
		for(int i = 0; i < 4; i++)
		{
			printf("%s = %d\n", KEY_NAMES[i], indizes[i]);
		}
	}

	return 0;
}

int main(int argc, char* argv[])
{
	if(argc == 2 && (strcmp(argv[1], "--version") == 0)) {
		printVersion();
		return 0;
	}

	int key_type = -1;
	int key_index = -1;
	char* key = NULL;
	enum{ACT_ENCRYPT, ACT_DECRYPT, ACT_VALIDATE, ACT_SIGN, ACT_RESET, ACT_SET, ACT_GET_INDEX} action=ACT_ENCRYPT;
	logger = new ConsoleLogger();
	Logger::LogLevel loglevel = Logger::LOG_INFO;
	std::string configFilePath("/etc/config/crypttool.cfg");


	// analyse commandline parameters
	for(int i=1;i<argc;i++)
	{
		if(strcmp(argv[i], "-l") == 0 && argc > i + 1)
		{
			i++;
			loglevel=(Logger::LogLevel)atoi(argv[i]);
		}
		else if(strcmp(argv[i], "-k") == 0 && argc > i + 1)
		{
			i++;
			key=argv[i];
		}
		else if(strcmp(argv[i], "-t") == 0 && argc > i + 1)
		{
			i++;
			key_type=atoi(argv[i]);
		}
		else if(strcmp(argv[i], "-i") == 0 && argc > i + 1)
		{
			i++;
			key_index=atoi(argv[i]);
		}
		else if(strcmp(argv[i], "-d") == 0)
		{
			action=ACT_DECRYPT;
		}
		else if(strcmp(argv[i], "-e") == 0)
		{
			action=ACT_ENCRYPT;
		}
		else if(strcmp(argv[i], "-v") == 0)
		{
			action=ACT_VALIDATE;
		}
		else if(strcmp(argv[i], "-s") == 0)
		{
			action=ACT_SIGN;
		}
		else if(strcmp(argv[i], "-r") == 0)
		{
			action=ACT_RESET;
		}
		else if(strcmp(argv[i], "-S") == 0)
		{
			action=ACT_SET;
		}
		else if(strcmp(argv[i], "-g") == 0)
		{
			action=ACT_GET_INDEX;
		}
		else if(strcmp(argv[i], "-y") == 0)
		{
			delete logger;
			logger=new SyslogLogger();
		}
		else if(strcmp(argv[i], "-f") == 0) {
			if(argc > i+1) {
				i++;
				configFilePath = argv[i];
			}
			else {
				usage(argv[0]);
			}
		}
		else
		{
			usage(argv[0]);
		}
	}

	logger->SetLevel(loglevel);

	crypttoolconfig crypttoolconfig(configFilePath);

	// Start action
	int retval=1;
	switch(action){
		case ACT_VALIDATE:
			if(key_type < 0)
			{
				printf("no key type given\n");
				usage(argv[0]);
			}

			if(key_type == 3 && !key)
			{
				printf("no temp key given\n");
				usage(argv[0]);
			}

			if(key_index >= 0)
			{
				printf("key index unexpected\n");
				usage(argv[0]);
			}

			retval=validate(crypttoolconfig, key_type, key);
			break;
		case ACT_ENCRYPT:
		case ACT_DECRYPT:
			if(key_type < 0)
			{
				printf("no key type given\n");
				usage(argv[0]);
			}

			if(key_type == 3 && !key)
			{
				printf("no key given\n");
				usage(argv[0]);
			}

			if(key_index >= 0)
			{
				printf("key index unexpected\n");
				usage(argv[0]);
			}

			retval=crypt(key_type, action==ACT_DECRYPT, key);
			break;
		case ACT_SIGN:
			if(key_type < 0)
			{
				printf("no key type given\n");
				usage(argv[0]);
			}

			if(key_type == 3 && !key)
			{
				printf("no key given\n");
				usage(argv[0]);
			}

			if(key_index >= 0)
			{
				printf("key index unexpected\n");
				usage(argv[0]);
			}

			retval=sign(crypttoolconfig, key_type, key);
			break;
		case ACT_RESET:
			if(key_index >= 0)
			{
				printf("key index unexpected\n");
				usage(argv[0]);
			}

			if(key_type >= 0)
			{
				printf("key type unexpected\n");
				usage(argv[0]);
			}

			if(key)
			{
				printf("key unexpected\n");
				usage(argv[0]);
			}

			retval=reset_keys(crypttoolconfig);
			break;
		case ACT_SET:
			if(key_index < 0)
			{
				printf("no key index given\n");
				usage(argv[0]);
			}

			if(key_type >= 0)
			{
				printf("key type unexpected\n");
				usage(argv[0]);
			}

			if(!key)
			{
				printf("no key given\n");
				usage(argv[0]);
			}

			retval=set_current_user_key(crypttoolconfig, key_index, key);
			break;
		case ACT_GET_INDEX:
			retval=get_index(crypttoolconfig, key_type);
			break;

	}

	delete logger;
	exit(retval);
}


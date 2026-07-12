/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/uio.h>

#include <netinet/in.h>
#include <arpa/inet.h>

#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fstream>
#include <sstream>
#include <time.h>

// bidcos_utils
#include <SyslogLogger.h>
#include <ConsoleLogger.h>
#include <md5.h>

#include "include/udpframe.h"
#include "include/networkconfig.h"

#include <openssl/evp.h>

#define closesocket(x) close(x)

#define LISTEN_PORT 43439
#define BUFSIZE 2048

#define DSTADDR_SOCKOPT IP_PKTINFO
#define DSTADDR_DATASIZE (CMSG_SPACE(sizeof(struct in_pktinfo)))
#define dstaddr(x) (&(((struct in_pktinfo *)(CMSG_DATA(x)))->ipi_addr))

union control_data
{
	struct cmsghdr  cmsg;
	unsigned char   data[DSTADDR_DATASIZE];
};

const char* defaultKey = "Markus is awesome";
const std::string CCU2_IDENTIFY = "eQ3-HM-CCU2-App";
const std::string CCU3_IDENTIFY = "eQ3-HmIP-CCU3-App";
const std::string CCU3_RESCUE_IDENTIFY = "eQ3-HmIP-CCU3-RS";

// Decryption parameters
const uint8_t UDP_CRYPT_RANDOM_SIZE = 16;
const uint8_t UDP_CRYPT_SIGNATURE_SIZE = 16;
const uint8_t UDP_CRYPT_SIGNATURE[UDP_CRYPT_SIGNATURE_SIZE] = "eQ-3__UDP-Crypt";

static void usage(const char* procname)
{
	printf("%s [-c] [-l loglevel]\n", procname);

	printf("  Options:\n");
	printf("    -t: device type identifier to publish\n");
	printf("    -c: log to console instead of syslog\n");
	printf("    -l: set log level\n");
}

int main(int argc, char* argv[])
{
	int                 sock;
	int                 sockopt;
	struct sockaddr_in  srvaddr;
	struct sockaddr_in  cliaddr;
	struct msghdr       msg;
	union control_data  cmsg;
	struct cmsghdr *    cmsgptr;
	struct iovec        iov[1];
	ssize_t             nbytes;
	char                buf[BUFSIZE];
	
	logger = new SyslogLogger();
	Logger::LogLevel loglevel=Logger::LOG_INFO;
	std::string deviceType = "";
	
	// analyse commandline parameters
	for(int i = 1; i < argc; i++)
	{
		if(strcmp(argv[i], "-l") == 0 && argc > i + 1)
		{
			i++;
			loglevel = (Logger::LogLevel)atoi(argv[i]);
		}
		else if(strcmp(argv[i], "-t") == 0 && argc > i + 1)
		{
			i++;
			std::string deviceTypeArg(argv[i]);
			deviceType = deviceTypeArg;
		}
		else if(strcmp(argv[i], "-c")==0)
		{
			delete logger;
			logger = new ConsoleLogger();
		}
		else
		{
			usage(argv[0]);	
		}
	}
	
	logger->SetLevel(loglevel);

	setbuf(stdout, NULL);

	sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
	if(sock == -1)
	{
		LOG(Logger::LOG_ERROR, "Could not create socket");
		delete logger;
		exit(EXIT_FAILURE);
	}

	sockopt = 1;
	if(setsockopt(sock, IPPROTO_IP, DSTADDR_SOCKOPT, &sockopt, sizeof(sockopt)) == -1)
	{
		LOG(Logger::LOG_ERROR, "Could not set socket options");
		delete logger;
		exit(EXIT_FAILURE);
	}

	memset(&srvaddr, 0, sizeof(struct sockaddr_in));
	srvaddr.sin_family      = AF_INET;
	srvaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	srvaddr.sin_port        = htons(LISTEN_PORT);
	
	// Bind to socket after initialization pf the srvaddr
	if(bind(sock, (struct sockaddr *)&srvaddr, sizeof(struct sockaddr_in)) == -1)
	{
		LOG(Logger::LOG_ERROR, "Could not bind on socket");
		delete logger;
		exit(EXIT_FAILURE);
	}

	iov[0].iov_base = buf;
	iov[0].iov_len = sizeof(buf);

	memset(&msg, 0, sizeof(msg));
	msg.msg_name        = &cliaddr;
	msg.msg_namelen     = sizeof(cliaddr);
	msg.msg_iov         = iov;
	msg.msg_iovlen      = 1;
	msg.msg_control     = &cmsg;
	msg.msg_controllen  = sizeof(cmsg);

	// Initialize serial number with dummy value
	std::string serialnumber = "DUMMY12345";
	std::ifstream serialnumberfile;
	serialnumberfile.open("/var/board_sgtin", std::ios_base::in);
	if(serialnumberfile.is_open() == false)
	  serialnumberfile.open("/var/board_serial", std::ios_base::in);
	if(serialnumberfile.is_open() == false)
	  serialnumberfile.open("/sys/module/plat_eq3ccu2/parameters/board_serial", std::ios_base::in);

	if(serialnumberfile.is_open())
	{
		// Get serial number from file
		std::getline(serialnumberfile, serialnumber);		
	}
	else
	{
		closesocket(sock);
		LOG(Logger::LOG_ERROR, "Could not read serial number");
		delete logger;
		exit(EXIT_FAILURE);
	}
	
	serialnumberfile.close();
	
	// initialize version with dummy value
	std::string version = "0.0.42";
	std::ifstream versionfile;

	versionfile.open("/VERSION", std::ios_base::in);
	if(versionfile.is_open() == false)
		versionfile.open("/boot/VERSION", std::ios_base::in);

	if(versionfile.is_open())
	{
		// Get version from file
		std::string findStr;
		findStr.append(1, '=');
		std::getline(versionfile, version);
		
		std::string::size_type index = version.find(findStr, 0);
		if(index != std::string::npos)
		{			
			version = version.substr(index + 1);
		}	
	}
	else
	{
		closesocket(sock);
		LOG(Logger::LOG_ERROR, "Could not read version");
		delete logger;
		exit(EXIT_FAILURE);
	}
	
	versionfile.close();

	// Legacy code, used if no device type is given
	if(deviceType.empty())
	{
		// identify CCU2/CCU3 and write in type string
		if(version.at(0) == '3')
		{
			if (version.find ("-") != std::string::npos) 
				deviceType = CCU3_RESCUE_IDENTIFY;
			else
				deviceType = CCU3_IDENTIFY;
		}
		else
			deviceType = CCU2_IDENTIFY;
	}

	networkconfig config;
	bool restartNetwork = false;

	// Initialize the initial vector with dummy data
	unsigned char iv[16];
	for(unsigned char i = 0; i < 16; i++)
	{
		iv[i] = i;
	}

	for(;;)
	{
		nbytes = recvmsg(sock, &msg, 0);
		if(nbytes == -1)
		{
			closesocket(sock);
			LOG(Logger::LOG_ERROR, "Could not receive data from socket");
			delete logger;
			exit(EXIT_FAILURE);
		}
		if(nbytes == 0) {
			continue;//no need to process frames with no payload
		}
		
		for(cmsgptr = CMSG_FIRSTHDR(&msg); cmsgptr != NULL; cmsgptr = CMSG_NXTHDR(&msg, cmsgptr))
		{
			if(cmsgptr->cmsg_level == IPPROTO_IP && cmsgptr->cmsg_type == DSTADDR_SOCKOPT)
			{
				std::string destip(inet_ntoa(*(struct in_addr*)dstaddr(cmsgptr)));
				LOG(Logger::LOG_DEBUG, "%ld bytes from %s:%hu to %s", static_cast<int32_t>(nbytes), inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), destip.c_str());

				if(destip.compare(config.Getcurrent_ip()) != 0)
				{
					//not adressed to on of our unicast ips -> reply by broadcast
					cliaddr.sin_addr = *(struct in_addr*)dstaddr(cmsgptr); //destination addr should contain 124.0.0.1 or any broadcast i.e. 255.255.255.255 
					sockopt = 1;
					setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &sockopt, sizeof(sockopt));
				}
				else
				{
					// Disable broadcast if message was single cast
					sockopt = 0;
					setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &sockopt, sizeof(sockopt));
				}
			}
		}

		std::string result(buf, nbytes);

		udpframe resultframe(result);
		bool isTarget = resultframe.IsTarget(deviceType, serialnumber);
		bool isCrypted = false;
		if(isTarget)
		{
			LOG(Logger::LOG_DEBUG, "Version: %i SenderId: %i Counter: %i Devicetype: %s SerialNumber: %s Opcode: %c Payload: %s", resultframe.Getversion(), resultframe.Getsenderid(), resultframe.Getcounter(), resultframe.Getdevicetype().c_str(), resultframe.Getserialnumber().c_str(), resultframe.Getopcode(), resultframe.Getpayload().c_str());
			
			std::string clientSerial = resultframe.Getserialnumber();//store for later before GetResponseHeader overwrites it
			// Get data from received telegramm for later use
			std::string response = resultframe.GetResponseHeader(deviceType, serialnumber);
			std::string payload = resultframe.Getpayload();
			char opcode = resultframe.Getopcode();
			unsigned char cryptflag = config.Getconfig_crypt() & 0x01;

			if(opcode == '*')
			{
				isCrypted = true;
				std::string aesKey = networkconfig::GetCurrentAesKey();
				// Use default key if key was not changed
				if(aesKey.empty())
				{
					aesKey = md5::MD5String(defaultKey);
				}
		
				const char * aesKeyp = aesKey.c_str();
				uint8_t * aesIv = (uint8_t*)iv;
				uint8_t aesKeyValue[16];
				uint8_t * aesKeyValuep = (uint8_t *) &aesKeyValue;

				// hexstring to uint8_t array
				for(int i = 0; i < 16; i++)
				{
					sscanf(aesKeyp, "%2hhx", &aesKeyValue[i]);
					aesKeyp += 2 * sizeof(char);
				}
				
				EVP_CIPHER_CTX* ctx = EVP_CIPHER_CTX_new();
				// This path DECRYPTS the received '*'-encrypted message; the
				// context must be initialised for decryption (was EncryptInit,
				// which left EVP_DecryptUpdate returning 0 bytes -> the payload
				// was never decrypted and the signature check always failed).
				// The sender (LanDevice::encryptMessage) pads the plaintext to a
				// block boundary itself and uses no PKCS7 padding, so disable it.
				EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, aesKeyValuep ,aesIv);
				EVP_CIPHER_CTX_set_padding(ctx, 0);
				int blockSize = EVP_CIPHER_CTX_block_size(ctx);
				const int newDataLength = payload.length() + blockSize;//ensure there is enough space even if final adds padding

				// Copy data to an unsigned char array
				unsigned char newdata[newDataLength];
				for (unsigned int i = 0; i < payload.length(); i++)
				{
					newdata[i] = (unsigned char)payload.at(i);

				}
				
				// Decrypt data
				int decBytes = 0;
				int totalDecBytes = 0;
				EVP_DecryptUpdate(ctx, newdata, &decBytes, newdata, payload.length());
				totalDecBytes = decBytes;
				EVP_DecryptFinal_ex(ctx, newdata + totalDecBytes, &decBytes);
				totalDecBytes += decBytes;
				
			
				// Check if the signature is correct
				if((newDataLength > (UDP_CRYPT_RANDOM_SIZE+UDP_CRYPT_SIGNATURE_SIZE)) && memcmp(&newdata[UDP_CRYPT_RANDOM_SIZE], UDP_CRYPT_SIGNATURE, UDP_CRYPT_SIGNATURE_SIZE) == 0)
				{
					// Get opcode from decrypted data
					opcode = (char)newdata[UDP_CRYPT_RANDOM_SIZE + UDP_CRYPT_SIGNATURE_SIZE];
					std::string newpayload;
					// Get payload from decrypted data
					for (unsigned int i = UDP_CRYPT_RANDOM_SIZE + UDP_CRYPT_SIGNATURE_SIZE + 1; i < payload.length(); i++)
					{
						newpayload.append(1, (char)newdata[i]);
					}
			
					payload = newpayload;
					LOG(Logger::LOG_DEBUG, "Received encrypted frame, Opcode: %c Payload: %s", payload.c_str());
				}
				else
				{
					LOG(Logger::LOG_DEBUG, "Received encrypted frame, with no valid signature");
				}
				if(ctx != NULL) {
					EVP_CIPHER_CTX_free(ctx);
				}
			}	

			// no response on responses
			if(opcode != '>')
			{
				// renew config, it could be changed by the WebUI
				config.LoadConfigFile();
				if(config.Getcurrent_ip().compare("0.0.0.0") == 0)
				{
					// restart network if /etc/config/netconfig has no CURRENT_IP
					LOG(Logger::LOG_ERROR, "Invalid config, no current ip, restarting network");
					restartNetwork = true;
				}
				else
				{
					// Append opcode to the response		
		    			response.append(1, opcode);

					switch(opcode)
					{
						case 'I':
							// Send identify result
							response.append(1, (char)0x01);
							response.append(version);

							// No service protocols supported
							response.append(1, (char)0x00);
							response.append(1, (char)0x00);

							if(clientSerial.find('*') != std::string::npos) {
								// we have to wait up to 1.3 seconds to reply identify request with wildcard in serial
								srand(time(NULL));
								long r = rand();
								int micros = r % 1300001; // (random % (max - min + 1)) + min -> min=0;max=1300 -> random % 1301
								usleep(micros);
							}
							break;

						case 'n':
							// Send ipconfig result;
							config.AppendCurrentConfigToResponse(response);

							break;

						case 'c':
							// Send network setting
							config.AppendConfigToResponse(response);

							break;

						case 'C':
							// Check if encryption is enabled
							if(cryptflag == 0x00 || (cryptflag == 0x01 && isCrypted))
							{
								// Change networksettings and send ack
								config.SetConfigAndAppendResultResponse(response, payload);
								restartNetwork = true;
							}
							else
							{
								// Send need encrypted and create new initial vector
								response.append(1, (char)0x03);
								for(unsigned char i = 0; i < 16; i++)
								{		
									iv[i] = (unsigned char)(rand() & 0xFF);					
									response.append(1, (char)iv[i]);
								}
							}

							break;

						default:
							// Send nak if the opcode is not supported or is unknown
							response.append(1, (char)0x00);
						break;
					}

					nbytes = sendto(sock, response.c_str(), (int)response.size(), 0, (struct sockaddr*)&cliaddr, sizeof(cliaddr));

					LOG(Logger::LOG_DEBUG, "Send to %s:%hu length: %i", inet_ntoa(cliaddr.sin_addr), ntohs(cliaddr.sin_port), nbytes);
				}
			}
		}

		// Restart network if the configuration was changed
		if(restartNetwork)
		{
			int ret = system("/etc/init.d/S40network restart");
			if(ret != 0)
				LOG(Logger::LOG_ERROR, "Could not restart network");

			restartNetwork = false;
		}
	}

	delete logger;
	closesocket(sock);
	return EXIT_SUCCESS;
}

/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifdef WIN32
#pragma warning(disable:4786)
#endif

const char* DEV = "/dev/hss_rs485";
//const char* DEV = "/dev/hss_power";

const char* PIDFILE = "/var/hs485d.pid";
const char* UDS_PATH = "/var/socket_hs485d";

const unsigned int LISTEN_PORT_XML = 2000;

char buffer[128];
#include <string>
#include "CommController.h"


#ifndef WIN32
#include "UnixSerialPortWrapper.h"
#endif

#include "SocketPortWrapper.h"

#include "HS485Controller.h"
#include "HS485Manager.h"
#include <TimerTarget.h>

#include <SyslogLogger.h>
#include <ConsoleLogger.h>
#include <FileLogger.h>
#include <signal.h>
#include <fstream>
#include <iostream>
#include <vector>
#ifndef WIN32
#include <sys/resource.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#endif
#include <XmlRpc.h>
#include "XmlRpcMethods.h"
#include <LGWPortWrapper.h>
#include <HS485ControllerCCU1.h>
#include <HS485ControllerLGW.h>
#include <LanDeviceUtils.h>
#include <PropertyMap.h>
#include <md5.h>

#include "generated-hs485d-version.h"

using namespace XmlRpc;

//declarations
bool readHS485dConfig(const std::string& cfgPath, const std::string& interfaceId, std::string& serial, std::string& key, std::string& ip,
		std::string& logTargetStr, std::string& logFileName, int& logLevel,
		std::string& listenIP, int& listenPort);
bool determineIPAddressBySerial(const std::string& serial, std::string& ip);

HS485Manager mgr;

// The XML-RPC server listening on a TCP port
XmlRpcServer s;

#ifndef WIN32
// The server proxy listening on a unix-domain-socket
XmlRpcServerProxy serverProxy(&s);
#endif

bool run=true;

/*
static void sigterm_handler(int sig)
{
	printf("Received SIGTERM. Shutting down...\n");

	run=false;
}
*/

static void usage(const char* procname)
{
  std::cout << "HS485D " << HS485D_VERSION << " (" << __DATE__ << ")" << std::endl
            << std::endl
            << "Usage: " << procname << " [-f config file] [-h host -p port] [-c] [-l loglevel] [-g] [-s LGWSerialNumber] [-i InterfaceNr]  [-k MYPASSWORD]" << std::endl
            << "    -f: Path to hs485d configuration file. Default: /etc/config/hs485d.conf" << std::endl
            << "    -h: remote ip host to connect to" << std::endl
            << "    -p: remote tcp port to connect to" << std::endl
            << "    -c: log to console instead of syslog" << std::endl
            << "    -l: set log level" << std::endl
            << "    -g: use HM2 wired lan gateway. (Either -s, or -i, or -h and -p must be supplied too.)" << std::endl
            << "    -s: HM2 LGW serial number" << std::endl
            << "    -i: HM2 LGW Interface Number. (From /etc/config/hs485d.conf) Use 0 for [Interface 0]" << std::endl
            << "    -k: Encryption Key. (Only valid for HM2 LGW.)" << std::endl
            << std::endl
            << "Examples for HM2 Wired LGW:" << std::endl
            << std::endl
            << "Start with serial number: hs485d -l 5 -g -s ABC1234567 [-k MYPASSWORD]" << std::endl
            << "Start with port and ip  : hs485d -l 5 -g -h 192.168.1.2 -p 1000 [-k MYPASSWORD]" << std::endl
            << "Start with interface-nr : hs485d -l 5 -g -i 0" << std::endl;

  if(logger)
    delete logger;

  exit(1);
}

static std::string calculateMD5(const std::string& s)
{
	md5 md5_calculator;
	unsigned char* buffer = new unsigned char[s.length()];
	memcpy(buffer, s.c_str(), s.length());
	md5_calculator.Update(buffer, s.length());
	md5_calculator.Finalize();
	delete[] buffer;
	std::string digest;
	digest.append((const char*) md5_calculator.Digest(), std::string::size_type(16));
	return digest;
}

int main(int argc, char* argv[])
{
	bool useHM2LGW = false;
	const char* host=NULL;
	int port=-1;
	std::string lgwSerialNumber;
	std::string interfaceId;
	std::string encryptionKey;
	std::string cfgFilePath("/etc/config/hs485d.conf");
	int xmlrpcListenPort = LISTEN_PORT_XML;
	std::string xmlrpcListenIP;
	bool logTargetOverridden = false;
	bool logLevelOverridden = false;


    const char* device=DEV;
	logger=new SyslogLogger("hs485d");
    Logger::LogLevel loglevel=Logger::LOG_INFO;

	//signal(SIGTERM, sigterm_handler);
	//signal(SIGINT, sigterm_handler);
#ifndef WIN32
	signal(SIGCHLD, SIG_IGN);//we are not interested in exit messages from our childs.
	const struct rlimit rl={RLIM_INFINITY, RLIM_INFINITY};
	setrlimit(RLIMIT_CORE, &rl);
#endif
    for(int i=1;i<argc;i++)
    {
        if(strcmp(argv[i], "-p")==0&&argc>i+1){
            i++;
			port=atoi(argv[i]);
        }
        else if(strcmp(argv[i], "-h")==0&&argc>i+1){
            i++;
			host=argv[i];
        }
        else if(strcmp(argv[i], "-d")==0&&argc>i+1){
            i++;
			device=argv[i];
        }
        else if(strcmp(argv[i], "-f")==0&&argc>i+1){
			i++;
			cfgFilePath = argv[i];
		}
        else if(strcmp(argv[i], "-l")==0&&argc>i+1){
            i++;
            loglevel=(Logger::LogLevel)atoi(argv[i]);
            logLevelOverridden = true;
        }
        else if(strcmp(argv[i], "-c")==0){
			delete logger;
			logger=new ConsoleLogger();
			logTargetOverridden = true;
        }
        else if(strcmp(argv[i], "-g")==0) {
			useHM2LGW = true;
        }
        else if(strcmp(argv[i], "-s")==0&&argc>i+1) {
        	i++;
        	lgwSerialNumber = argv[i];
        }
        else if(strcmp(argv[i], "-i")==0&&argc>i+1) {
        	i++;
        	interfaceId = argv[i];
        }
        else if(strcmp(argv[i], "-k")==0&&argc>i+1) {
        	i++;
        	encryptionKey = argv[i];
        }
        else{
            usage(argv[0]);
        }
    }

#ifndef WIN32
	umask(0000);
	std::ofstream os(PIDFILE);
	os<<getpid();
	os.close();
#endif

	logger->SetLevel(loglevel);
	
	PortWrapper* pw=NULL;
	if(host!=NULL && port>0){
		if(useHM2LGW) {
			LGWPortWrapper* pLGWPortWrapper = new LGWPortWrapper();
			if(!encryptionKey.empty()) {
				encryptionKey = calculateMD5(encryptionKey);
			}
			bool done = pLGWPortWrapper->connect(host, port, encryptionKey, "");
			if(!done) {
				LOG(Logger::LOG_FATAL_ERROR, "Unable to open network connection to HomeMatic Wired Lan Gateway.");
				delete pLGWPortWrapper;
				delete(logger);
				exit(1);
			}
			pw = dynamic_cast<PortWrapper*>(pLGWPortWrapper);
		}
		else {
			SocketPortWrapper* spw=new SocketPortWrapper();
			if(spw->Open(host, port)<0){
				LOG(Logger::LOG_FATAL_ERROR, "Unable to open network connection");
				delete spw;
				delete logger;
				exit(1);
				return 1;
			}
			pw=spw;
		}
	}
	else{
		if(useHM2LGW) {//LGW
			LGWPortWrapper* pLGWPortWrapper = new LGWPortWrapper();
			const int failureSleepTime = 65000000;
			std::string lgwIPAddress("");
			//Search via serial number for the LGW or use interfaceId
			if(!interfaceId.empty()) {//use interface id
				std::string logTargetStr, logFileName;
				int logLevel = -1;
				bool done = readHS485dConfig(cfgFilePath, interfaceId, lgwSerialNumber, encryptionKey, lgwIPAddress, logTargetStr, logFileName, logLevel, xmlrpcListenIP, xmlrpcListenPort);
				if( (!logTargetOverridden) && (!logTargetStr.empty())) {
					if(logTargetStr.compare("File") == 0) {
						if(logger) { delete logger; }
						logger = new FileLogger();
						((FileLogger*)logger)->SetFilename(logFileName.c_str());
					}
					else if(logTargetStr.compare("Syslog") == 0) {
						if(logger) { delete logger; }
						logger = new SyslogLogger("hs485d");
					}
					else if(logTargetStr.compare("Console") == 0) {
						if(logger) { delete logger; }
						logger = new ConsoleLogger();
					}
					else {
						LOG(Logger::LOG_WARNING, "Unknown log target defined in configuration file");
					}

				}
				if( (!logLevelOverridden) && logLevel >= 0) {
					loglevel = (Logger::LogLevel)logLevel;
				}
				logger->SetLevel(loglevel);

				if(!done) {
					LOG(Logger::LOG_FATAL_ERROR, "Could not gather indispensable information from /etc/config/hs485d.conf");
					delete logger;
					return 2;//important, hs485dLoader checks that.
				}
				//may apply values from configuration file


				if(!encryptionKey.empty()) {
					encryptionKey = calculateMD5(encryptionKey);
				}
				//create serial.connstat file. CCU2WebUI checks for existence
				#ifndef WIN32
					std::string statfilepath("/var/status/");
					statfilepath.append(lgwSerialNumber);
					statfilepath.append(".connstat");  
					FILE* f = fopen(statfilepath.c_str(), "w");
					if(f) {
						fclose(f);
					}
				#endif	
				if(lgwIPAddress.empty()) {//default: using search by serial
					done = false;
					while(!done) {
						done = determineIPAddressBySerial(lgwSerialNumber, lgwIPAddress);
						if(!done) {
							LOG(Logger::LOG_FATAL_ERROR, "Cannot determine IP Address for HomeMatic Wired Lan Gateway.");
							usleep(failureSleepTime); //60 seconds
							continue;
						}
						done = pLGWPortWrapper->connect(lgwIPAddress, 1000, encryptionKey, lgwSerialNumber);
						if(!done) {
							LOG(Logger::LOG_FATAL_ERROR, "Could not connect to HomeMatic Lan Gateway with IP Address %s", lgwIPAddress.c_str());
							usleep(failureSleepTime); //60 seconds
							continue;
						}
					}

				}
				else {//IP address overridden in webui -> don't search it by serial
					done = false;
					while(!done) {
						done = pLGWPortWrapper->connect(lgwIPAddress, 1000, encryptionKey, lgwSerialNumber);
						if(!done) {
							LOG(Logger::LOG_FATAL_ERROR, "Could not connect to HomeMatic Lan Gateway with IP Address %s", lgwIPAddress.c_str());
							usleep(failureSleepTime); //60 seconds
							continue;
						}
					}
				}
			}
			else if(!lgwSerialNumber.empty()) {//use serial number
				bool done = determineIPAddressBySerial(lgwSerialNumber, lgwIPAddress);
				if(!done) {
					LOG(Logger::LOG_FATAL_ERROR, "Can't determine ip address for serial number %s", lgwSerialNumber.c_str());
					delete logger;
					exit(1);
				}
				if(!encryptionKey.empty()) {
					encryptionKey = calculateMD5(encryptionKey);
				}
				done = pLGWPortWrapper->connect(lgwIPAddress, 1000, encryptionKey, lgwSerialNumber);
				if(!done) {
					LOG(Logger::LOG_FATAL_ERROR, "Could not connect to HomeMatic Lan Gateway with IP Address %s", lgwIPAddress.c_str());
					delete logger;
					exit(1);
				}
			}
			else {
				LOG(Logger::LOG_FATAL_ERROR, "Neither serial number nor interfaceId of HomeMatic Wired Lan Gateway was given.");
				delete logger;
				exit(1);
			}

			//Connect
			//Create the LGWPortWrapper

			pw=pLGWPortWrapper;
		}
		else {
#ifndef WIN32
			UnixSerialPortWrapper* uspw=new UnixSerialPortWrapper();
			if(uspw->Open(device)<0){
				LOG(Logger::LOG_FATAL_ERROR, "Unable to open line port");
				delete uspw;
				delete logger;
				exit(1);
				return 1;
			}
			pw=uspw;
#endif
		}
	}

	HS485Controller* pController = NULL;
	if(useHM2LGW) {
		pController = HS485ControllerLGW::CreateSingletonInstance();
	}
	else {
		pController = HS485ControllerCCU1::CreateSingletonInstance();
	}
	//HS485Controller ctrl;

    //ctrl.SetPortWrapper(pw);
    pController->SetPortWrapper(pw);

	//if(!ctrl.IsCommunicationStarted()){
	if(!pController->IsCommunicationStarted()){
		if(pw)delete pw;
		delete logger;
		delete pController;
		exit(1);
	}

	InitXmlRpcMethods(s);
//	XmlRpc::setVerbosity(5);

	// Create the server socket on the specified port
	if(xmlrpcListenIP.empty()) {
		s.bindAndListen(xmlrpcListenPort,100);
	}
	else {
		s.bindAndListen(xmlrpcListenIP.c_str(), xmlrpcListenPort, 100);
	}

/*#ifndef WIN32
	serverProxy.bindAndListen(UDS_PATH,100);
#endif
*/
	// Enable introspection
	s.enableIntrospection(true);

	//s.getDispatcher()->addSource(&ctrl, XmlRpcDispatch::ReadableEvent);
	s.getDispatcher()->addSource(pController, XmlRpcDispatch::ReadableEvent);

/*
	ctrl.SendMessage(0xffffffff, "u", NULL);
	usleep(500000);
	ctrl.SendBootloaderMessage(0xffffffff, "u", NULL);
	usleep(500000);
	ctrl.SendBootloaderMessage(0xffffffff, "g", NULL);
*/
	mgr.Init(cfgFilePath);

	try{
		while(run ){
			int64_t timeout=TimerTarget::s_timerQueue.TimeBeforeNextDue();
			while(!timeout){
				TimerTarget::s_timerQueue.Execute();
				timeout=TimerTarget::s_timerQueue.TimeBeforeNextDue();
			}
			s.work(timeout);
		}
	}catch(XmlRpcException e){
		LOG(Logger::LOG_FATAL_ERROR, "exception catched at top level: %s", e.getMessage().c_str());
	}
	delete pw;
	delete logger;
	delete pController;
	exit(0);
}


bool readHS485dConfig(const std::string& cfgPath, const std::string& interfaceId, std::string& serial, std::string& key, std::string& ip,
		std::string& logTargetStr, std::string& logFileName, int& logLevel,
		std::string& listenIP, int& listenPort)
{
	LOG(Logger::LOG_INFO, "Using configuration file: %s", cfgPath.c_str());
	//Read configuration from file.
	PropertyMap configData;
	if(configData.ReadFromFile(cfgPath) < 0) {
		return false;
	}
	//Gather non interface specific information
	std::string tmpStr;
	configData.SetCurrentSection("");
	tmpStr = configData.GetStringValue("Log Destination");
	if(!tmpStr.empty()) {
		logTargetStr.assign(tmpStr);
		if(tmpStr.compare("File") == 0) {
			tmpStr = configData.GetStringValue("Log Filename", "/var/log/hs485d.log");
			logFileName.assign(tmpStr);
		}
	}
	int tmpInt = configData.GetIntValue("Log Level", -1);
	if(tmpInt >= 0) {
		logLevel = tmpInt;
	}
	tmpStr = configData.GetStringValue("Listen IP");
	if(!tmpStr.empty()) {
		listenIP = tmpStr;
	}
	tmpInt = configData.GetIntValue("Listen Port", -1);
	if(tmpInt >= 0) {
		listenPort = tmpInt;
	}

	//Get information from file.
	std::string desiredSection("Interface ");
	desiredSection.append(interfaceId);
	bool done = configData.SetCurrentSection(desiredSection);
	if(done) {
		std::string type = configData.GetStringValue("Type", "");
		if(type.compare("HMWLGW") == 0) {
			serial = configData.GetStringValue("Serial Number", "");
			key = configData.GetStringValue("Encryption Key", "");
			ip = configData.GetStringValue("IP Address", "");//optional
			if( (!serial.empty()) && (!key.empty()) ) {
				return true;
			}
			else {
				LOG(Logger::LOG_FATAL_ERROR, "Could not find serial number and encryption key.");
				return false;
			}
		}
		else {
			LOG(Logger::LOG_FATAL_ERROR, "Interface type %s not supported", type.c_str());
			return false;
		}
	}
	else {
		LOG(Logger::LOG_FATAL_ERROR, "Cannot find %s in /etc/config/hs485d.conf", desiredSection.c_str());
		return false;
	}
	return false;
}

bool determineIPAddressBySerial(const std::string& serial, std::string& ip)
{
	LDU::LanDeviceUtils ldUtils;
	LDU::LanDevice lanDevice;
	LOG(Logger::LOG_DEBUG, "Searching for HomeMatic Lan Gateway with serial number %s.", serial.c_str());
	bool done = ldUtils.searchDeviceByTypeAndSerial("eQ3-HMW-LGW*", serial, lanDevice);
	if(!done) {
		LOG(Logger::LOG_FATAL_ERROR, "Could not find HomeMatic Lan Gateway with serial number %s.", serial.c_str());
		return false;
	}
	done = ldUtils.readRuntimeNetworkConfiguration(lanDevice);
	if(!done) {
		LOG(Logger::LOG_FATAL_ERROR, "Could not determine IP address of HomeMatic Lan Gateway with serial number %s.", serial.c_str());
		return false;
	}
	ip = lanDevice.getRuntimeIPConfiguration().getIPAddress();
	return (!ip.empty());
}


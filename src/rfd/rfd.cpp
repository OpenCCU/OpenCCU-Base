/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifdef WIN32
#pragma warning(disable:4786)
#endif

const char* PIDFILE = "/var/rfd.pid";
const char* UDS_PATH = "/var/socket_rfd";

const unsigned int LISTEN_PORT_XML = 2001;
const int BACKLOG = 100;

const char* DEFAULT_LISTEN_IP = "";


char buffer[128];

#include "CommController.h"

#ifndef WIN32
#include "UnixSerialPortWrapper.h"
#include <unistd.h>
#include <sys/types.h>
#endif

#include "SocketPortWrapper.h"

#include "RFController.h"
#include "BidcosLanInterface.h"
#include "RFManager.h"
#include "RFLoggingManager.h"
#include <TimerTarget.h>
#include <SyslogLogger.h>
#include <ConsoleLogger.h>
#include <FileLogger.h>
#include <PropertyMap.h>
#include <signal.h>
#include <fstream>
#include <iostream>
#include <vector>
#ifndef WIN32
#include <sys/resource.h>
#include <sys/stat.h>
#endif
#include <XmlRpc.h>
#include "XmlRpcMethods.h"
#include <OSCompat.h>
#include "win32_service.h"
#include <iostream>
#include <string.h>
#include <errno.h>

#include "generated-rfd-version.h"

using namespace XmlRpc;

// The XML-RPC server listening on a TCP port
XmlRpcServer s;

#ifndef WIN32
// The server proxy listening on a unix-domain-socket
XmlRpcServerProxy serverProxy(&s);
#endif

static bool run=true;


//#ifdef DEBUG_SHUTDOWN
// XmlRpc-Methode, die nach Aufruf rfd sauber herunterf�hrt.
// Sollte aus Sicherheitsgr�nden nur einkompiliert werden, wenn 
// das Herunterfahren debuggt werden soll.
class XmlRpcMethodExit : public XmlRpcServerMethod  
{
public:
	//! Konstruktor
	XmlRpcMethodExit(XmlRpcServer* s):XmlRpcServerMethod("exit", s){};
	void execute(XmlRpc::XmlRpcValue& params, XmlRpc::XmlRpcValue& result)
	{
		LOG(Logger::LOG_DEBUG, "XmlRpc_exitCommand: -> shutting rfd down");		
		run=false;
	}
}xmlRpcMethodExit(&s);
//#endif

static class CustomXmlRpcLogHandler : public XmlRpcLogHandler {
public:
  void log(int level, const char* msg) { 
	  LOG( Logger::LOG_DEBUG, "XmlRpc<%d>: %s", level, msg );
  }
} customXmlRpcLogHandler;

static class CustomXmlRpcErrorHandler : public XmlRpcErrorHandler {
public:

  void error(const char* msg) {
	  LOG( Logger::LOG_ERROR, "XmlRpc: %s", msg );
  }
} customXmlRpcErrorHandler;

static const char* CONFIG_DEFAULTS[] = {
    "Listen Port",          "2001",
    "Log Level",            "3",
    "Log Identifier",       "bidcos",
    "Persist Keys",         "0",
#ifdef WIN32
    "Log Destination",      "None",
#else
    "Log Destination",      "Syslog",
#endif
    NULL
};

static const char* CONFIG_FILENAMES[] = {
#ifdef WIN32
    "${LocalAppData}/Bidcos-Service/bidcos.conf",
    "${CommonAppData}/Bidcos-Service/bidcos.conf",
#else
    "/etc/bidcos/bidcos.conf",
    "/etc/config/rfd.conf",
#endif
    NULL
};

static void usage(const char* procname)
{
  std::cout << "RFD " << RFD_VERSION << " (" << __DATE__ << ")" << std::endl
            << std::endl
            << "Usage: " << procname << " [-f config file] [-c] [-l loglevel] [-s [address1 [address2 [...]]]]"
#ifdef WIN32
            << " [-i] [-u] [-e]" << std::endl
            << "Options:" << std::endl
            << "    -i: install as service" << std::endl
            << "    -u: uninstall as service" << std::endl
            << "    -e: signal event when startup complete" << std::endl
#else
            << " [-d]" << std::endl
            << "Options:" << std::endl
            << "    -d: run as daemon/service" << std::endl
#endif
            << "    -f: use alternate config file" << std::endl
            << "    -c: log to console instead of syslog" << std::endl
            << "    -l: set log level" << std::endl
            << "    -s: sniffing mode" << std::endl;

	if(logger)
    delete logger;

	exit(0);
}

void writeInitDoneFile(const PropertyMap& config_data) {
#ifndef WIN32
//currently only Linux OS feature
	std::string path = config_data.GetStringValue("StatusfileDir", "/var/status/");
	path = OSCompat::FixPath(path);
	OSCompat::MakeDirectory(path.c_str());
	path.append("rfd.status");
	std::ofstream ofstream;
	ofstream.open(path.c_str());
	if(ofstream.is_open()) {
		ofstream << getpid();
		ofstream.flush();
		ofstream.close();
	}
	else {
		LOG(Logger::LOG_INFO, "Cannot write rfd status file to %s", path.c_str());
	}
#endif
}

int main(int argc, char* argv[])
{
    int cmdline_loglevel=-1;
    bool cmdline_log_console=false;
    bool cmdline_sniffing_mode=false;
    std::string config_filename;
    bool cmdline_daemonize=false;

    std::vector<int> sniffing_filter;
#ifdef WIN32
    bool service_install=false;
    bool service_uninstall=false;
    HANDLE hEvent=INVALID_HANDLE_VALUE;
#else
#endif

#ifndef WIN32
	signal(SIGCHLD, SIG_IGN);//we are not interested in exit messages from our childs.
	const struct rlimit rl={RLIM_INFINITY, RLIM_INFINITY};
	setrlimit(RLIMIT_CORE, &rl);
#endif
    for(int i=1;i<argc;i++)
    {
        if(strcmp(argv[i], "-l")==0&&argc>i+1){
            i++;
            cmdline_loglevel=atoi(argv[i]);
        }else if(strcmp(argv[i], "-f")==0&&argc>i+1){
            i++;
            config_filename=argv[i];
        }else if(strcmp(argv[i], "-c")==0){
            cmdline_log_console=true;
        }else if(strcmp(argv[i], "-s")==0){
            cmdline_sniffing_mode=true;
            while( argc>i+1 && argv[i+1][0] != '-' )
            {
                i++;
                char* endp;
                int address = strtol( argv[i], &endp, 0 );
                if( *endp )usage(argv[0]);
                sniffing_filter.push_back( address );
            }
#ifdef WIN32
        }else if(strcmp(argv[i], "-e")==0&&argc>i+1){
            i++;
            hEvent=OpenEvent(EVENT_MODIFY_STATE, FALSE, argv[i]);
            if(hEvent==INVALID_HANDLE_VALUE)perror("OpenEvent");
        }else if(strcmp(argv[i], "-i")==0){
            service_install=true;
            cmdline_log_console=true;
        }else if(strcmp(argv[i], "-u")==0){
            service_uninstall=true;
            cmdline_log_console=true;
#else
#endif
        }else if(strcmp(argv[i], "-d")==0){
            cmdline_daemonize=true;
        }else{
            usage(argv[0]);
        }
    }

    PropertyMap config_data;
    if(config_filename.empty()){
        const char** p=CONFIG_FILENAMES;
        while(*p){
            config_filename=OSCompat::FixPath(*p);
            if(config_data.ReadFromFile(config_filename)>0)break;
            p++;
        }
        if(!*p){
            std::cerr << "No config file found" <<std::endl;
            exit(0);
        }
        std::cout << "Config file is " << config_filename.c_str() << std::endl;
    }else if(config_data.ReadFromFile(config_filename)<=0){
        std::cerr << "Error reading config file " << config_filename.c_str() << std::endl;
    }

    const char **p=CONFIG_DEFAULTS;
    while(*p){
        if(config_data.GetStringValue(*p).empty()){
            const char* name=*p;
            p++;
            config_data.SetStringValue(name, *p);
        }else{
            p++;
        }
        p++;
    }

#ifndef WIN32
    if(config_data.GetStringValue("PID File")!=""){
	    umask(0000);
	    std::ofstream os(config_data.GetStringValue("PID File").c_str());
	    os<<getpid();
	    os.close();
    }
    if( cmdline_daemonize || config_data.GetIntValue("Daemonize") ){
        std::cout << "Daemonizing to background" << std::endl;
        if(daemon(0, 0)){
            std::cerr << "Could not daemonize: " << strerror(errno) << std::endl;
            exit(-1);
        }
    }
#endif

    if(cmdline_log_console || (config_data.GetStringValue("Log Destination")=="Console") ){
        logger=new ConsoleLogger();
    }else if(config_data.GetStringValue("Log Destination")=="Syslog"){
        std::string sid = config_data.GetIntValue("Sniffing Mode", 0)?"hm_sniffer":config_data.GetStringValue("Log Identifier").c_str();
        static std::string const syslogid (sid.empty() ? "rfd" : sid.c_str());
        logger=new SyslogLogger(syslogid.c_str());
    }else if(config_data.GetStringValue("Log Destination")=="File"){
        FileLogger* file_logger=new FileLogger();
        std::string logfile=OSCompat::FixPath(config_data.GetStringValue("Log Filename").c_str());
        file_logger->SetFilename(logfile.c_str());
        std::string::size_type pos=logfile.rfind(OSCompat::PATH_SEPARATOR);
        if(pos != std::string::npos)
        {
            logfile=logfile.substr(0, pos);
            OSCompat::MakeDirectory(logfile.c_str());
        }
        logger=file_logger;
    }else if(config_data.GetStringValue("Log Destination")=="None"){
        logger = NULL;
    }else{
        std::cerr << "Unknown log destination " << config_data.GetStringValue("Log Destination").c_str() << std::endl;
    }

    if(logger){
        if(cmdline_loglevel>=0)logger->SetLevel((Logger::LogLevel)cmdline_loglevel);
        else logger->SetLevel((Logger::LogLevel)config_data.GetIntValue("Log Level"));
    }

#ifdef WIN32
    if(service_install){
		win32_service_install();
        delete logger;
        exit(0);
    }
    if(service_uninstall){
		win32_service_uninstall();
        delete logger;
        exit(0);
    }
#endif

    LOG(Logger::LOG_INFO, "BidCoS-Service started");

#ifdef XMLRPC_DEBUG
	if( logger->GetLevel() <= logger->LOG_DEBUG )
	{
		XmlRpcLogHandler::setLogHandler(&customXmlRpcLogHandler);
		XmlRpcErrorHandler::setErrorHandler(&customXmlRpcErrorHandler);
		XmlRpc::setVerbosity(5);
	}
#endif

	InitXmlRpcMethods(s);

	// Create the server socket on the specified port
	int listenPort = config_data.GetIntValue("Listen Port");
	std::string listenIp = config_data.GetStringValue("Listen IP", DEFAULT_LISTEN_IP);
	if(!s.bindAndListen(listenIp.c_str(), listenPort, BACKLOG))
    {
        LOG(Logger::LOG_ERROR, "Could not bind to TCP port %d", config_data.GetIntValue("Listen Port"));
        delete logger;
        exit(0);
    }

    LOG(Logger::LOG_INFO, "XmlRpc Server is listening on TCP port %d", s.getListenPort());

#ifndef WIN32
	if(config_data.GetStringValue("UDS File")!="")serverProxy.bindAndListen(config_data.GetStringValue("UDS File").c_str(),100);
#endif

#ifdef WIN32
    if(hEvent!=INVALID_HANDLE_VALUE){
        if(!SetEvent(hEvent))perror("SetEvent");
        CloseHandle(hEvent);
    }
#endif

	s.enableIntrospection(true);
	
	RFManager* mgr=NULL;

	if(cmdline_sniffing_mode || config_data.GetIntValue("Sniffing Mode", 0)){
        RFLoggingManager* loggingManager = new RFLoggingManager();
        if( !sniffing_filter.empty() )
        {
            loggingManager->SetAddressFilter( sniffing_filter );
        }
        mgr = loggingManager;
	}else{
		mgr=new RFManager();
	}

    if(!mgr->Init(config_filename.c_str())){
        delete mgr;
        delete logger;
        exit(0);
    }

	s.getDispatcher()->addSource(mgr->GetInterfaceConcentrator(), XmlRpcDispatch::ReadableEvent);

#ifdef WIN32
    if(cmdline_daemonize || config_data.GetIntValue("Daemonize"))
    {
        try_to_run_as_nt_service();
        s.getDispatcher()->removeSource(mgr->GetInterfaceConcentrator());
	    delete mgr;
    	delete logger;
	    exit(0);
    }
#endif

    writeInitDoneFile(config_data);

	while(run ){
		int64_t timeout=TimerTarget::s_timerQueue.TimeBeforeNextDue();
		while(!timeout){
			TimerTarget::s_timerQueue.Execute();
			timeout=TimerTarget::s_timerQueue.TimeBeforeNextDue();
		}
		s.work(timeout);
	}

    s.getDispatcher()->removeSource(mgr->GetInterfaceConcentrator());
	delete mgr;
	delete logger;
	exit(0);
}

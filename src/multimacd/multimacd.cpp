/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include <stdio.h>
#include <signal.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <iostream>
#include <SyslogLogger.h>
#include <ConsoleLogger.h>
#include <FileLogger.h>
#include <PropertyMap.h>
#include "MultimacManager.h"
#include <OSCompat.h>
#ifndef WIN32
#include <unistd.h>
#endif

#include "generated-multimacd-version.h"

static const char* CONFIG_DEFAULTS[] = {
	"Coprocessor Device Path",		"/dev/mxs_auart_raw.0",
    "Log Level",					"3",
    "Log Identifier",				"multimacd",
    "Log Destination",				"Syslog",
//	"HmIP Cmdline Pattern",			"*/crRFD*",
//	"Bidcos Cmdline Pattern",		"*rfd -c*",
//	"Transparent Cmdline Pattern",	"*update*",
	"Bidcos Exe Pattern",			"*/bin/rfd",
	"Default Subsystem",			"HmIP",
	"Traffic Log",					"0",
	"Traffic Log Directory",		"/var/log",
//	"Loop Master Device",			"/dev/eq3loop",
//	"Loop Slave Device Bidcos",			"mmd_bidcos",
//	"Loop Slave Device HmIP",			"mmd_hmip",
    NULL
};

static const char* CONFIG_FILENAME = "/etc/config/multimacd.conf";

void sighandler(int sig, siginfo_t *siginfo, void *context)
{
	std::cerr << "Signal " << sig << " caught" << std::endl;
	if( (sig == SIGINT) || (sig == SIGTERM) )
	{
		MultimacManager::Instance().Exit();
	}
}

void installSignalHandlers()
{
	struct sigaction act;
	memset (&act, '\0', sizeof(act));
 
	/* Use the sa_sigaction field because the handles has two additional parameters */
	act.sa_sigaction = &sighandler;
 
	/* The SA_SIGINFO flag tells sigaction() to use the sa_sigaction field, not sa_handler. */
	act.sa_flags = SA_SIGINFO;
 
	if (sigaction(SIGTERM, &act, NULL) < 0) {
		perror ("sigaction");
		exit(1);
	}
 
	if (sigaction(SIGINT, &act, NULL) < 0) {
		perror ("sigaction");
		exit(1);
	}

	if (sigaction(SIGQUIT, &act, NULL) < 0) {
		perror ("sigaction");
		exit(1);
	}

	if (sigaction(SIGUSR1, &act, NULL) < 0) {
		perror ("sigaction");
		exit(1);
	}

}

static void usage(const char* procname)
{
  std::cout << "multimacd " << MULTIMACD_VERSION << " (" << __DATE__ << ")" << std::endl
            << std::endl
            << "Usage: " << procname << " [-f config file] [-c] [-l loglevel] [-p devnode] [-d] [-t]" << std::endl
            << "    -f: use alternate config file" << std::endl
            << "    -c: log to console instead of syslog" << std::endl
            << "    -l: set log level" << std::endl
            << "    -p: specify coprocessor device node" << std::endl
            << "    -d: run as daemon/service" << std::endl
            << "    -t: log all radio rx/tx traffic to daily files in /var/log" << std::endl;

  if(logger)
    delete logger;

  exit(1);
}

PropertyMap read_config( std::string configFile )
{
	configFile = OSCompat::FixPath(configFile);
	PropertyMap config_data;
    if(!(config_data.ReadFromFile(configFile)>0))
	{
        std::cerr << "Error reading config file " << configFile << std::endl;
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
	return config_data;
}

void setup_logging( PropertyMap& configData )
{
    if( configData.GetStringValue("Log Destination")=="Console" ){
        logger=new ConsoleLogger();
    }else if(configData.GetStringValue("Log Destination")=="Syslog"){
        std::string const id = configData.GetStringValue("Log Identifier").c_str();
        static std::string const syslogid = id.empty() ? "multimacd" : id;//must be static cmp. syslog documentation regarding id
        logger=new SyslogLogger(syslogid.c_str());
    }else if(configData.GetStringValue("Log Destination")=="File"){
        FileLogger* file_logger=new FileLogger();
        std::string logfile=OSCompat::FixPath(configData.GetStringValue("Log Filename").c_str());
        file_logger->SetFilename(logfile.c_str());
        std::string::size_type pos=logfile.rfind(OSCompat::PATH_SEPARATOR);
        if(pos != std::string::npos)
        {
            logfile=logfile.substr(0, pos);
            OSCompat::MakeDirectory(logfile.c_str());
        }
        logger=file_logger;
    }else if(configData.GetStringValue("Log Destination")=="None"){
        logger = NULL;
    }else{
        std::cerr << "Unknown log destination " << configData.GetStringValue("Log Destination").c_str() << std::endl;
    }
    if(logger){
		logger->SetLevel((Logger::LogLevel)configData.GetIntValue("Log Level"));
    }


}

int main(int argc, char **argv)
{
    int cmdline_loglevel=-1;
    bool cmdline_log_console=false;
    std::string config_filename = CONFIG_FILENAME;
    bool cmdline_daemonize=false;
    bool cmdline_traffic_log=false;
	const char* device = NULL;

	installSignalHandlers();

    for(int i=1;i<argc;i++)
    {
        if(strcmp(argv[i], "-l")==0&&argc>i+1){
            i++;
            cmdline_loglevel=atoi(argv[i]);
        }else if(strcmp(argv[i], "-f")==0&&argc>i+1){
            i++;
            config_filename=argv[i];
        }else if(strcmp(argv[i], "-p")==0&&argc>i+1){
            i++;
            device=argv[i];
        }else if(strcmp(argv[i], "-c")==0){
            cmdline_log_console=true;
        }else if(strcmp(argv[i], "-d")==0){
            cmdline_daemonize=true;
        }else if(strcmp(argv[i], "-t")==0){
            cmdline_traffic_log=true;
        }else{
            usage(argv[0]);
        }
    }

    if( cmdline_daemonize ){
        std::cout << "Daemonizing to background" << std::endl;
        if(daemon(0, 0)){
            std::cerr << "Could not daemonize: " << strerror(errno) << std::endl;
            exit(-1);
        }
    }

    PropertyMap config_data = read_config( config_filename );
	if( cmdline_log_console )
	{
		config_data.SetStringValue("Log Destination", "Console");
	}
	if( cmdline_loglevel >= 0 )
	{
		config_data.SetIntValue("Log Level", cmdline_loglevel);
	}
	if( cmdline_traffic_log )
	{
		config_data.SetIntValue("Traffic Log", 1);
	}

	setup_logging( config_data );

	if( device )
	{
		config_data.SetStringValue("Coprocessor Device Path", device);
	}

	MultimacManager::Instance().Run( config_data );

}

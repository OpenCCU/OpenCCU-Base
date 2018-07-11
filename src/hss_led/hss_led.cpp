/*
 * hss_led.cpp
 *
 *  Created on: 17.01.2013
 *      Author: willms
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#include <Logger.h>
#include <SyslogLogger.h>
#include <ConsoleLogger.h>
#include "led.h"
#include "InfoLed.h"
#if !defined(PLATFORM_CCU3)
#include "InternetLed.h"
#endif
#include "utils.h"
#include "MessageParser.h"

#include "UdpCannel.h"
#include "defines.h"

#define VERSION "2.0 (11.07.2018)"

//sigterm_handler / Aufruf von Systemfunktion signal(...)

//-------------------------------------------------------

void PrintUsage();
bool run = true;
int main(int argc, char **argv)
{
	int ret = 0;
	int receivedSize = 0;
		UdpCannel udp;
		std::string msg;

		MessageParser parser;
		InfoLed infoLed;
	//Zeitmessung z("Start von hss_led");

	logger = 0; //Variable in <Logger.h>
	Logger::LogLevel loglevel = Logger::LOG_INFO;

	for (int i=1; i<argc; i++)
	{
		if (strcmp(argv[i], "-c") == 0)
		{
			if (logger) delete logger;
			logger = new ConsoleLogger();
			continue;
		}
		else if (strcmp(argv[i], "-l") == 0 && argc > i+1)
		{
			int level = atoi(argv[i+1]);
			if      (level < 0) level = 0;
			else if (level > 6) level = 6;
			loglevel = (Logger::LogLevel) level;
			++i;
			continue;
		}
		else if (strcmp(argv[i],"-h") == 0 && argc > i+1)
		{

			++i;
			continue;
		}
		else
		{
			PrintUsage();
			ret = -1;
			return ret;
		}
	}

	//LOG(Logger::LOG_DEBUG, "%s", z.Str().c_str());

	if (! logger) logger = new SyslogLogger();

	logger->SetLevel(loglevel);

	LOG(Logger::LOG_INFO, "hss_led: Programm initialisiert.");
	udp.OpenSocket();
	udp.BindServer();
  #if !defined(PLATFORM_CCU3)
	led power("power");
	power.LedOn();
  #endif

  #if !defined(PLATFORM_CCU3)
	InternetLed internetLed;
  #endif

	while(run)
	{
		receivedSize = udp.ReceiveMessage(msg);
		if(receivedSize > 0)
		{
			//LOG(Logger::LOG_INFO,"%s",msg.c_str());
			if(!infoLed.checkMessage(msg))
			{
				LOG(Logger::LOG_ERROR,"UDP Nachricht konnte nicht gelesen werden");
			}
		}
		infoLed.updateLedState();

    #if !defined(PLATFORM_CCU3)
		internetLed.updateLedState();
    #endif
	}


	if (logger)
	{
		delete logger;
		       logger = 0;
	}

	return ret;
}
//----------------------------------------------------------------------
void PrintUsage()
{
	std::cout << "hss_led " VERSION << std::endl;
	std::cout << "-c: Log-Meldungen auf Konsole ausgeben."   << std::endl;
	std::cout << "-l: Setze Log-Level: 0..6\n" <<
						"    (0=Alles,\n" <<
						"     1=Debug, \n" <<
						"     2=Info, \n" <<
						"     3=Notizen, \n" <<
						"     4=Warnungen, \n" <<
						"     5=Fehler, \n" <<
						"     6=Fatale Fehler)" << std::endl;

}


/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
*
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RFD_TRAFFICLOGGER_H_
#define _RFD_TRAFFICLOGGER_H_

#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <string>

class BidcosFrame;
class BidcosInterface;

//! Loggt Bidcos-Telegramme, die ueber nicht-lokale Interfaces (Funk-LAN-Gateways)
//! laufen, in eine Tagesdatei (z.B. /var/log/rfd-traffic-2026-07-08.log).
/*!
 *  Das Zeilenformat entspricht dem des multimacd-TrafficLoggers (der die Sicht des
 *  lokal angeschlossenen Funkmoduls loggt), ergaenzt um das Feld IFACE=<serial>
 *  mit der Seriennummer des Gateways. Beide Logger schreiben bewusst in getrennte
 *  Dateien, damit sich keine zwei Prozesse eine Logdatei teilen muessen.
 *  Aktivierung ueber rfd.conf (globale Sektion):
 *    Traffic Log = 1
 *    Traffic Log Directory = /var/log   (optional, default /var/log)
 */
class TrafficLogger
{
public:
	static TrafficLogger& Instance();
	void Configure( bool enabled, const std::string& directory );
	bool IsEnabled()const;
	/// Telegramm ueber ein LAN-Gateway empfangen (rx) bzw. gesendet (tx);
	/// Aufrufer stellt sicher, dass iface ein nicht-lokales Interface ist
	void LogTelegram( const char* direction, const BidcosFrame& frame, const std::string& interfaceId );
private:
	TrafficLogger();
	~TrafficLogger();
	void WriteLine( const char* direction, const std::string& fields );
	FILE* GetLogFile( const struct tm& localTime );

	bool _enabled;
	std::string _directory;
	FILE* _file;
	int _fileYear;
	int _fileMonth;
	int _fileDay;
	pthread_mutex_t _mutex;
	static TrafficLogger* _instance;
};

#endif // _RFD_TRAFFICLOGGER_H_

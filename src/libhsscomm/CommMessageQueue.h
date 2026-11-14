/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// CommMessageQueue.h: Schnittstelle für die Klasse CommMessageQueue.
//
//////////////////////////////////////////////////////////////////////

#ifndef _COMMMESSAGEQUEUE_H_
#define _COMMMESSAGEQUEUE_H_

#include "dllexport.h"


#include <map>
#include <list>
#include <vector>
#include <pthread.h>
#include <cstdint>

class  CommMessage;
//! Warteschlange für Objekte der Klasse CommMessage
/*! Diese Klasse wird als Warteschlange in Senderichting sowie in Empfangsrichtung verwendet.
 *  Die Objekte werden gemäß ihrer Priorität (CommMessage::GetPriority()) in die Warteschlange einsortiert.
 *  Dadurch können insbesondere beim Senden bestimmte Nachrichten bevorzugt behandelt werden.
 */
class DLLEXPORT CommMessageQueue  
{
public:
	//! Gibt die Anzahl von Nachrichten in der Queue zurück
	size_t GetSize();
	//! Gibt einen Zeiger auf die nächste Nachricht aus der Queue zurück.
	/*! Die Nachricht wird nicht aus der Queue entfernt.
	 *  \return Zeiger auf die erste Nachricht der Queue, \c NULL wenn die Queue leer ist
	 */
	CommMessage* GetNextMessage();
	//! Gibt einen Zeiger auf die nächste Nachricht aus der Queue zurück.
	/*! Diese Methode blockiert, wenn keine zu sendende Nachricht vorhanden ist und kehrt erst
	 *  zurück, wenn die Queue wieder mindestens eine Nachricht enthält oder der übergebene
	 *  Timeout abgelaufen ist.
	 *  Die Nachricht wird nicht aus der Queue entfernt.
	 *  \param timeout Timeout in ms. Bei \c timeout=NULL wird ewig gewartet. Bei \c *timeout=0 
	 *                 kehrt die Methode sofort zurück.
	 */
	CommMessage* WaitNextMessage(uint64_t *timeout);
	//! Eine Nachricht aus der Queue entfernen
	/*! \return \c true wenn die Nachricht in der Queue vorhanden war und entfernt werden konnte, \c false sonst
	 */
	bool DeleteMessage(CommMessage* p);
	//! Eine Nachricht in die Queue einfügen
	void AddMessage(CommMessage* p);
	//! Konstruktor
	CommMessageQueue();
	//! Destruktor
	virtual ~CommMessageQueue();
protected:
	//! Typedef für die eigentliche Queue, die in Wirklichkeit eine Liste ist
	typedef std::list<CommMessage*> t_message_list;
	//! In dieser Liste werden die Nachrichten gehalten
    t_message_list message_list;
	//! Mutex für den multithreaded Zugriff auf die Queue
	pthread_mutex_t mutex;
	//! Condition für die Signalisierung zwischen mehreren Threads
	pthread_cond_t cond;
};

#endif // _COMMMESSAGEQUEUE_H_

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _COMMCONTROLLER_H_
#define _COMMCONTROLLER_H_

#include "dllexport.h"
#include "PortWrapper.h"
#include <list>
#include "CommMessage.h"
#include "CommMessageQueue.h"
#include <pthread.h>
#include <map>
#include <string>

//! Diese Klasse ist verantwortlich für die Kommunikation mit einem Kanal des Kommunikationsprozessors
/*! Diese Klasse stellt die grundlegende Infrastruktur bereit, um mit einem Kanal des Kommunikationsprozessors (ARM7)
 *  zu kommunizieren. Diese Klasse wird selten direkt verwendet. Normalerweise wird das Verhalten in einer davon
 *  abgeleiteten Klasse modifiziert. Für diese Modifikationen stehen virtuelle Methoden bereit, welche überladen werden können.
 *  Diese Klasse startet zwei Threads, einen für den Empfang und einen zum Senden. Diese beiden Threads kommunizieren mit
 *  dem Hauptthread der Applikation über Queues, Mutexe und Conditions sowie über eine Pipe.
 */
class DLLEXPORT CommController
{
public:
	//! Konstruktor
	CommController();
	//! Destruktor
	virtual ~CommController();
	//! Gibt \c true zurück, wenn der Sende- und Empfangsthread erfolgreich gestartet wurden
	bool IsCommunicationStarted();
	//! Startet den Sende- und Empfangsthread
	/*! Vor dem Aufruf dieser Methode muss unbedingt ein PortWrapper mittels SetPortWrapper() gesetzt werden.
	 */
	int Start();
	//! Beendet den Sende- und Empfangsthread
	int Stop();
	//! Setzt den von CommController zu verwendenden PortWrapper.
	/*! Über die Klasse PortWrapper wird der Kanal abstrahiert, mittels dessen mit dem Kommunikationsprozessor kommuniziert wird.
	 *  Es wird eine von PortWrapper abgeleitete konkrete Implementierung verwendet. Von PortWrapper abgeleitete Klassen gibt
	 *  es z.B. für Unix-Seriellschnittstellen, Windows-Seriellschnittstellen oder TCP.
	 */
	void SetPortWrapper(PortWrapper* pw, bool check_controller=true);
	//! Stellt eine zu sendende Nachricht in die Sendequeue
	virtual int TxQueueAddMessage( CommMessage* msg );
protected:
	//! Wandelt die nackten vom PortWrapper empfangenen Daten in ein Objekt der Klasse CommMessage um
	/*! Es wird vom Anfang her versucht, Zeichen aus \c *s in ein Objekt der Klasse CommMessage einzulesen. Wenn das gelingt,
	 *  wird der verarbeitete Teil aus \c *s entfernt.
	 *  \param s Zeiger auf die Rohdaten. Der String wird am Anfang beschnitten, wenn eine Nachricht gelesen werden konnte
	 *  \return Die gelesenen Nachricht oder \c NULL wenn keine komplette Nachricht in \c s enthalten ist.
	 */
	CommMessage* MessageFromReceivedData(std::string *s);
	//! Factory-Funktion für CommMessage bzw. abgeleitete Klassen
	/*! Diese Methode wird aufgerufen, wenn aufgrund einer empfangenen Nachricht ein neues Objekt der Klasse CommMessage
	 *  erzeugt werden soll. Von CommController abgeleitete Klassen haben die Gelegenheit, diese Methode zu überladen
	 *  und ein spezialisiertes Objekt einer von CommMessage abgeleiteten Klasse zu erzeugen.
	 */
	inline virtual CommMessage* NewMessage(){return new CommMessage();};
	//! Überprüfung und Modifikation ausgehender Nachrichten durch abgeleitete Klassen
	/*! Diese Methode gibt \c true zurück. Sie dient dazu, in abgeleiteten Klassen überladen zu werden,
	 *  um applikationsspezifische Überprüfungen und Modifikationen an ausgehenden Nachrichten vorzunehmen.
	 *  Diese Methode wird unmittelbar vor dem Senden im Kontext des Sendethreads aufgerufen.
	 *  \param msg Zeiger auf die zu überprüfende Nachricht
	 *  \return Der Rückgabewert wird in de derzeitigen Implementierung nicht ausgewertet. Es gibt also keine
	 *          Möglichkeit, das Senden der Nachricht zu verhindern.
	 */
	virtual bool CheckBeforeSend(CommMessage* msg){return true;};
	//! Überprüfung eingehender Nachrichten durch abgeleitete Klassen
	/*! Diese Methode gibt \c true zurück. Sie dient dazu, in abgeleiteten Klassen überladen zu werden,
	 *  um applikationsspezifische Überprüfungen an eingehenden Nachrichten vorzunehmen.
	 *  Diese Methode wird unmittelbar nach dem Empfang im Kontext des Empfangsthreads aufgerufen.
	 *  \param msg Zeiger auf die zu überprüfende Nachricht
	 *  \return Bei \c true wird die Nachricht weiter verarbeitet, bei \c false wird diese verworfen
	 */
	virtual bool CheckAfterReceive(CommMessage* msg){return true;};
	//! Wird in abgeleiteten Klassen überschrieben, um eine empfangene Nachricht zu verarbeiten
	virtual void ProcessReceivedMessage(CommMessage* msg)=0;
	//! Verarbeitung der nackten Empfangsdaten
	/*! Diese Methode versucht zunächst, die bisher empfangenen Daten durch Aufruf von MessageFromReceivedData()
	 *  in ein Objekt der Klasse CommMessage umzuwandeln. Gelingt dies, werden einige Prüfungen an dem neu erzeugten
	 *  Objekt durchgeführt (u.a. Aufruf von CheckAfterReceive() und cur_tx_message->ProcessResponse())
	 *  Falls die empfangene Nachricht nicht der zuletzt gesendeten Nachricht als Antwort
	 *  zugeordnet werden kann, wird sie in die Empfangsqueue eingereiht.
	 *  Falls durch die empfangene Nachricht \c cur_tx_message bestätigt wurde, wird der Sendethread benachrichtigt.
	 *  Diese Methode wird im Kontext des Empfangsthreads ausgeführt
	 */
	virtual bool ProcessReceivedData(std::string* s);
	//! Hauptmethode des Empfangsthreads
	/*! Diese Methode bildet den Empfangsthread. Es werden in einer Endlosschleife Daten über den PortWrapper gelesen.
	 *  Wurden Daten empfangen, wird anschließend mit diesen Daten ProcessReceivedData() aufgerufen.
	 */
	void* RxThreadFunction();
	//! Hauptmethode des Sendethreads
	/*! Diese Methode bildet den Sendethread. In einer Endlosschleife wird gewartet, bis eine zu sendende Nachricht
	 *  In die Sendequeue gestellt wurde.
	 *  - Diese Nachricht wird der Sendewarteschlange entnommen,
	 *  - es wird CheckBeforeSend() aufgerufen,
	 *  - die Nachricht wird durch Aufruf von CommMessage::PrepareRawData in die zu sendenden Binärdaten überführt,
	 *  - die Binärdaten werden über den PortWrapper gesendet.
	 *  - Jetzt wird bei \c CommMessage::GetResponseTimeout()!=0 auf den Empfang einer Bestätigung gewartet
	 */
	void* TxThreadFunction();
	//! Kapselt den sendenden Zugriff auf den PortWrapper zwecks Synchronisation
	int Send(const std::string& data);

	//! Zeiger auf das für die Low-Level-Kommunikation verwendete Objekt der Klasse PortWrapper
	PortWrapper* port_wrapper;
	//! Zeiger auf die zuletzt gesendete und noch nicht bestätigte Nachricht
	/*! Wird eine Nachricht empfangen, wird zuerst versucht, sie dieser Nachricht als Antwort zuzordnen.
	 *  Sobald eine Bestätigung für \c cur_tx_message empfangen wurde, wird dies vom Empfangsthread an den
	 *  Sendethread signalisiert.
	 */
	CommMessage* cur_tx_message;
	//! Sendequeue
	CommMessageQueue txq;
	//! Internes Flag, wird von Stop() gesetzt und von den beiden Threads ausgewertet
	bool exit;
	//! Status von cur_tx_message für die Kommunikation zwischen Sendethread und Empfangsthread
    CommMessage::t_state tx_state;

	//! Empfangspuffer für Empfangsdaten, die noch keine komplette Nachricht bilden
    std::string rx_buffer;
	//! Mutex für den Zugriff auf \c exit
	pthread_mutex_t mutex_exit;
	//! Mutex für den Zugriff auf \c cond_tx_data_available
	pthread_mutex_t mutex_tx_data_available;
	//! Mutex für den Zugriff auf \c tx_state und \c cond_tx_state
	pthread_mutex_t mutex_tx_state;
	//! Mutex für die Methode \c Send()
	pthread_mutex_t mutex_send;
	//! Condition für die Signalisierung einer neuen Nachricht in der Sendequeue
	pthread_cond_t cond_tx_data_available;
	//! Condition für die Signalisierung einer Veränderung an \c tx_state
	pthread_cond_t cond_tx_state;
	//! Der Empfangsthread
	pthread_t thread_rx;
	//! Der Sendethread
	pthread_t thread_tx;
	friend void* buscontroller_rx_thread_function(void* arg);
	friend void* buscontroller_tx_thread_function(void* arg);
};
#endif

/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// CommMessage.h: Schnittstelle für die Klasse CommMessage.
//
//////////////////////////////////////////////////////////////////////

#ifndef _COMMMESSAGE_H_
#define _COMMMESSAGE_H_

#include "dllexport.h"

#include "StructuredFrame.h"
#include <string>
#include <cstdint>
#include <vector>
#include <pthread.h>

//! Nachricht zwischen Kommunikationscontroller und einem Schnittstellenprozess
/*!
 *  Die Klasse CommMessage kapselt die zwischen Kommunikationscontroller
 *  und der restlichen Software ausgetauschten Nachrichten.
 *  Eine solche Nachricht besteht aus einer ID, einem Befehl sowie einem Nutzdatenblock.
 *  Die Klasse CommMessage wird nicht direkt verwendet, vielmehr wird jeweils eine applikationsspezifische
 *  Nachrichtenklasse von CommMessage abgeleitet.
 *  Der Datenblock enthält je nach Befehl Daten der Sicherungsschicht (z.B. BidCoS-RF) und/oder
 *  Daten, die nur in der Kommunikation mit dem Kommunikationscontroller relevant sind
 *  (Zeitstempel, o.ä.). Bei vielen Methoden dieser Klasse gibt es dazu einen Parameter \c payload. Bei \c payload=true
 *  bezieht sich der Aufruf auf die Daten der Sicherungsschicht, ansonsten auf die gesamten Daten.
 */

class DLLEXPORT CommMessage: public StructuredFrame
{
public:
	//! Typedef für Vector von Nachrichten
	typedef std::vector<CommMessage*> t_messageVect;

	//! Die verschiedenen Stati, die eine Nachricht während ihres Lebenszyklus haben kann
	typedef enum{
		/*! die Nachricht wurde gerade erzeugt */
		INITIAL,
		/*! die Nachricht wurde in die Sendewarteschlange gestellt */
		QUEUED, 
		/*! die Nachricht wurde gesendet und es wird auf die Bestätigung gewartet */
		WAIT_ACK, 
		/*! die Nachricht wurde von der Gegenseite (ARM7) empfangen */
		RECEIVED, 
		/*! die Nachricht wurde gesendet und bestätigt */
		ACKED, 
		/*! die Nachricht wurde gesendet und negativ bestätigt */
		NACKED, 
		/*! die Nachricht wurde verworfen, z.B. weil sie nicht bestätigt wurde */
		DROPPED 
	} t_state;

	//! Konstruktor
	CommMessage();
	//! Destruktor
	virtual ~CommMessage();
	//! Gibt bei Antworten die beantwortete Nachricht zurück
	/*! Empfangene Antworten enthalten einen Zeiger auf die beantwortete Nachricht.
	 *  Diese Methode liefert diesen Zeiger zurück.
	 *  Wird üblicherweise in abgeleiteten Klassen mit spezifischem Rückgabetyp erneut implementiert.
	 */
	inline CommMessage* GetParent(){return parent;};
	//! Setzt bei Antwortnachrichten die Ursprungsnachricht
	inline void SetParent(CommMessage* parent){this->parent=parent;};
	//! Gibt den Befehlscode der Nachricht zurück
	/*! Der Befehlscode ist applikationsspezifisch und üblicherweise in einer abgeleiteten Klasse als enum
	 *  deklariert. Genaueres findet sich in der Dokumentation zur Kommunikation zwischen ARM9 und ARM7 der
	 *  HM-CCU.
	 */
    inline int GetCommand()const{return command;}
	//! Gibt die ID der Nachricht zurück
	/*! 
	 *  Die ID ist 8 Bit lang und wird fortlaufend vergeben. Die ID wird verwendet, um Nachrichten und 
	 *  Antworten einander zuzuordnen.
	 */
    inline int GetID()const{return id;}
	//! Vergibt eine neue ID
	/*!
	 *  Es wird eine neue ID zugewiesen, wenn vorher keine ID gesetzt war oder bei \c force=true
	 *  \param force \c =true erzwingt die Zuweisung einer neuen ID. Wird benötigt bei Wiederverwendung einer Instanz von CommMessage
	 */
	inline void AssignID(bool force=false){if(id<0 || force)id=GetNextID();};
	//! Setzt den Befehlscode der Nachricht
	/*! Der Befehlscode ist applikationsspezifisch und üblicherweise in einer abgeleiteten Klasse als enum
	 *  deklariert. Genaueres findet sich in der Dokumentation zur Kommunikation zwischen ARM9 und ARM7 der
	 *  HM-CCU.
	 */
	inline void SetCommand(int cmd){command=cmd;}
	//! Liefert die Priorität der Nachricht zurück
	/*! Nachrichten werden in der durch die Priorität gegebene Reihenfolge in eine CommMessageQueue
	 *  zum Senden einsortiert. Nachrichten mit höherer Priorität werden zuerst gesendet. Der Defaultwert 
	 *  für die Priorität ist \c 0.
	 */
    inline int GetPriority()const{return priority;}
	//! Setzt die Priorität der Nachricht
	/*! Nachrichten werden in der durch die Priorität gegebene Reihenfolge in eine CommMessageQueue
	 *  zum Senden einsortiert. Nachrichten mit höherer Priorität werden zuerst gesendet. Der Defaultwert 
	 *  für die Priorität ist \c 0.
	 */
	inline void SetPriority(int p){priority=p;}
	//! Liefert das Flag \c dont_delete der Nachricht zurück
	/*! Nachrichten werden normalerweise nach dem Senden automatisch gelöscht.
	 *  Bei gesetztem Flag \c dont_delete wird dieses verhindert. Dies ist wichtig, wenn eine zu
	 *  sendende Nachricht vom Aufrufer als lokale Variable angelegt wird oder nach dem Senden die 
	 *  Antworten der Nachricht ausgewertet werden sollen.
	 */
    inline bool GetDontDelete()const{return dont_delete;}
	//! Setzt das Flag \c dont_delete der Nachricht
	/*! Nachrichten werden normalerweise nach dem Senden automatisch gelöscht.
	 *  Bei gesetztem Flag \c dont_delete wird dieses verhindert. Dies ist wichtig, wenn eine zu
	 *  sendende Nachricht vom Aufrufer als lokale Variable angelegt wird oder nach dem Senden die 
	 *  Antworten der Nachricht ausgewertet werden sollen.
	 */
    inline void SetDontDelete(bool dd){dont_delete=dd;}
	//! Liefert das Flag \c collect_responses der Nachricht zurück
	/*! Antworten auf gesendete Nachrichten werden per default in dem Vektor \c vect_responses
	 *  der Ursprungsnachricht gesammelt, damit der Aufrufer der Sendefunktion die Antworten
	 *  auswerten kann.
	 *  Wenn keine Antworten erwartet werden, kann mit dem Flag \c collect_responses das Sammeln
	 *  der Antworten unterbunden werden.
	 */
    inline bool GetCollectResponses()const{return collect_responses;}
	//! Setzt das Flag \c collect_responses der Nachricht
	/*! Antworten auf gesendete Nachrichten werden per default in dem Vektor \c vect_responses
	 *  der Ursprungsnachricht gesammelt, damit der Aufrufer der Sendefunktion die Antworten
	 *  auswerten kann.
	 *  Wenn keine Antworten erwartet werden, kann mit dem Flag \c collect_responses das Sammeln
	 *  der Antworten unterbunden werden.
	 */
    inline void SetCollectResponses(bool cr){collect_responses=cr;}
	//! Liefert den Zustand im Lebenszyklus der Nachricht zurück
	/*! Jede gesendete Nachricht durchläuft einen Lebenszyklus.
	 *  Beim erfolgreichen Senden mit positiver Bestätigung:\n
	 *  \c INITIAL -> \c QUEUED -> \c WAIT_ACK -> \c ACKED \n
	 *  Beim erfolgreichen Senden mit negativer Bestätigung:\n
	 *  \c INITIAL -> \c QUEUED -> \c WAIT_ACK -> \c NACKED \n
	 *  Beim fehlgeschlagenen Senden (timeout):\n
	 *  \c INITIAL -> \c QUEUED -> \c WAIT_ACK -> \c DROPPED \n
	 *  Jede empfangene Nachricht hat den Zustand \c RECEIVED.
	 */
    t_state GetState() const;
	//! Wartet auf eine Zustandsänderung der Nachricht
	/*! Diese Funktion wartet bis sich im Lebenszyklus einer Nachricht eine Änderung
	 *  ergibt.
	 *  \param timeout Zeiger auf eine Variable, die die maximale Wartezeit enthält. \c NULL wartet ewig.
	 *                 Nach dem Aufruf enthält \c timeout die verbleibende Wartezeit
	 */
    t_state WaitStateChanged(uint64_t *timeout);
	//! Wartet bis die Nachricht gesendet oder verworfen wurde
	/*! \param timeout Zeiger auf eine Variable, die die maximale Wartezeit enthält. \c NULL wartet ewig.
	 *                 Nach dem Aufruf enthält \c timeout die verbleibende Wartezeit
	 *  \return \c true wenn das Senden erfolgreich war (\c state==ACKED)
	 */
    bool WaitUntilSent(uint64_t *timeout=NULL);
	
	//! Liefert die Antwort(en) auf eine Nachricht zurück
	/*! \param index Gibt bei mehreren Antwortnachrichten den Index der gewünschten Nachricht an
	 *  \return Zeiger auf die entsprechende Antwortnachricht oder \c NULL wenn diese nicht existiert.
	 */
	CommMessage* GetResponse(unsigned int index=0);
	
	//! Bestimmt wie Antwortnachrichten abhängig von deren Befehlscode behandelt werden
	/*! Es werden drei Bitmasken übergeben. Jedes Bit steht für den der Bitwertigkeit 
	 *  entsprechenden Befehlscode. Dabei wird das höchstwertigste Bit (Bit 7) des Befehlscodes
	 *  nicht berücksichtigt.
	 *  \param mask_response Bitmaske für als Antwort zu sammelnde Antwortnachrichten
	 *  \param mask_ack Bitmaske für als positive Bestätigung behandelnde Antwortnachrichten
	 *  \param mask_nack Bitmaske für als negative Bestätigung behandelnde Antwortnachrichten
	 */
	inline void SetReceiveMasks(uint32_t mask_response, uint32_t mask_ack, uint32_t mask_nack)
	{
		cmd_mask_response=mask_response;
		cmd_mask_ack=mask_ack;
		cmd_mask_nack=mask_nack;
	}
	//! Liefert den Empfangs-Timeout zurück
	/*! Der Empfangs-Timeout bestimmt wie lange auf Antwortnachrichten gewartet wird.
	 *  Default-Timeout ist 500ms
	 */
    inline uint32_t GetResponseTimeout()
    {
        return response_timeout;
    }
	//! Setzt den Empfangs-Timeout
	/*! Der Empfangs-Timeout bestimmt wie lange auf Antwortnachrichten gewartet wird.
	 *  Default-Timeout ist 500ms
	 */
    inline void SetResponseTimeout(uint32_t to)
    {
        response_timeout=to;
    }
	//! Bereitet die Nachricht für das Senden auf
	/*! Diese Methode erzeugt aus den Member-Variablen eine Darstellung, die für die Übertragung geeignet ist.
	 *  \return String aus Binärdaten, der direkt zum Senden an den Sendekanal übergeben werden kann.
	 */
    virtual std::string PrepareRawData();
	/*! Löscht alle mit der Nachricht verbundenen Antwortnachrichten
	 *  Diese Methode wird aufgerufen, wenn ein Nachrichtenobjekt mit leichten Modifikationen mehrfach verwendet werden soll.
	 *  Dabei werden bei der erneuten Verwendung die Antworten aus dem vorherigen Durchlauf verworfen.
	 */
	void ClearResponses();
protected:
	//! ID für das nächste erzeugte Objekt von CommMessage
	static int s_id;
	//! Setzt den Zustand im Lebenszyklus der Nachricht
	/*! Eventuell auf Zustandsänderungen wartende Threads werden benachrichtigt.
	 */
    void SetState(t_state s);
	//! Gibt die nächste freie Nachrichten-ID zurück
	static inline int GetNextID()
	{
		int id=s_id;
		s_id++;
        s_id&=0xff;
		return id;
	}
	//! Verarbeitet eine potentielle Antwortnachricht
	/*! Prüft, ob es sich bei der übergebenen Nachricht um eine gültige Antwort handelt. Falls ja, wird die
	 *  übergebene Nachricht abhängig vom flag \c collect_responses als Antwortnachricht gespeichert.
	 *  Unabhängig davon kann der Zustand im Lebenszyklus \c (state) von der übergebenen Antwortnachricht
	 *  beeinflusst werden.
	 *  Dabei werden die mit \c SetReceiveMasks() gesetzten Empfangsmasken berücksichtigt.
	 *  \param m Die potentiell als Antwort zu verarbeitende Nachricht.
	 *  \param new_state Der Zustand der Ursprungsnachricht nach der Verarbeitung der Antwort
	 *  \return Bei \c true wurde die Antwort akzeptiert und verarbeitet. Falls GetParent() der übergebenen 
	 *          Nachricht nicht \c NULL ist, ist jetzt dieses Objekt für das Löschen des Antwortobjektes verantwortlich.
	 */
	virtual bool ProcessResponse(CommMessage* m, t_state* new_state);
	//! Befehlscode der Nachricht
	/*! Der Befehlscode ist applikationsspezifisch und üblicherweise in einer abgeleiteten Klasse als enum
	 *  deklariert. Genaueres findet sich in der Dokumentation zur Kommunikation zwischen ARM9 und ARM7 der
	 *  HM-CCU.
	 */
	int command;
	//! ID der Nachricht für die Zuordnung von Antworten
	/*! 
	 *  Die ID ist 8 Bit lang und wird fortlaufend vergeben. Die ID wird verwendet, um Nachrichten und 
	 *  Antworten einander zuzuordnen.
	 */
	int id;
	//! Priorität der Nachricht
	/*! Nachrichten werden in der durch die Priorität gegebene Reihenfolge in eine CommMessageQueue
	 *  zum Senden einsortiert. Nachrichten mit höherer Priorität werden zuerst gesendet. Der Defaultwert 
	 *  für die Priorität ist \c 0.
	 */
    int priority;
	//! Zustand im Lebenszyklus der Nachricht
	/*! Jede gesendete Nachricht durchläuft einen Lebenszyklus.
	 *  Beim erfolgreichen Senden mit positiver Bestätigung:\n
	 *  \c INITIAL -> \c QUEUED -> \c WAIT_ACK -> \c ACKED \n
	 *  Beim erfolgreichen Senden mit negativer Bestätigung:\n
	 *  \c INITIAL -> \c QUEUED -> \c WAIT_ACK -> \c NACKED \n
	 *  Beim fehlgeschlagenen Senden (timeout):\n
	 *  \c INITIAL -> \c QUEUED -> \c WAIT_ACK -> \c DROPPED \n
	 *  Jede empfangene Nachricht hat den Zustand \c RECEIVED.
	 */
	t_state state;
	//! Bei \c true wird die Nachricht nicht automatisch gelöscht
	/*! Nachrichten werden normalerweise nach dem Senden automatisch gelöscht.
	 *  Bei gesetztem Flag \c dont_delete wird dieses verhindert. Dies ist wichtig, wenn eine zu
	 *  sendende Nachricht vom Aufrufer als lokale Variable angelegt wird oder nach dem Senden die 
	 *  Antworten der Nachricht ausgewertet werden sollen.
	 */
    bool dont_delete;

	//! Bitmaske für als Antwort zu sammelnde Nachrichten
	/*! Jedes Bit steht für den der Bitwertigkeit entsprechenden Befehlscode. Dabei wird das höchstwertigste
	 *  Bit (Bit 7) des Befehlscodes nicht berücksichtigt.
	 */
	uint32_t cmd_mask_response;
	//! Bitmaske für als positive Bestätigung behandelnde Antwortnachrichten
	/*! Jedes Bit steht für den der Bitwertigkeit entsprechenden Befehlscode. Dabei wird das höchstwertigste
	 *  Bit (Bit 7) des Befehlscodes nicht berücksichtigt.
	 */
	uint32_t cmd_mask_ack;
	//! Bitmaske für als negative Bestätigung behandelnde Antwortnachrichten
	/*! Jedes Bit steht für den der Bitwertigkeit entsprechenden Befehlscode. Dabei wird das höchstwertigste
	 *  Bit (Bit 7) des Befehlscodes nicht berücksichtigt.
	 */
	uint32_t cmd_mask_nack;
	//! Timeout in ms beim Warten auf Antworten
  uint32_t response_timeout;
	//! Gesammelte Antwortnachrichten
	t_messageVect vect_responses;
	//! Bei Antwortnachrichten Zeiger auf die Ursprungsnachricht
	CommMessage* parent;
	//! Bei \c true werden Antwortnachrichten gesammelt
	bool collect_responses;
	//! Mutex für Threadsynchronisation
	pthread_mutex_t mutex;
	//! Condition für die Kommunikation von Zustandsänderungen zwischen Threads
	pthread_cond_t cond_state_changed;
    friend class CommController;
};

#endif // _COMMMESSAGE_H_

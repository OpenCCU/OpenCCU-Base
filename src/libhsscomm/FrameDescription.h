/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef __FRAME_DESCRIPTION_H_
#define __FRAME_DESCRIPTION_H_

#include "dllexport.h"

#include <xmlParser.h>
#include <XmlRpc.h>
#include "ValueStore.h"
#include <string>

class  StructuredFrame;

//! Abstrakte Beschreibung eines Rahmens der Sicherungsschicht
/*! Durch diese Klasse wird ein Rahmen der Sicherungsschicht abstrakt beschrieben.
 *  Mittels dieser abstrakten Beschreibung lassen sich aus einem konkreten Rahmen der Klasse StructuredFrame
 *  die relevanten Daten in Form von Name-Werte-Paaren extrahieren.
 *  Umgekehrt kann mittels der abstrakten Beschreibung sowie den entsprechenden 
 *  Name-Werte-Paaren ein konkreter Rahmen der Sicherungsschicht erzeugt werden.
 *  Das funktioniert unabh�ngig vom Protokoll der Sicherungsschicht (BidCoS-RF, BidCoS-Wired, etc.)
 */
class DLLEXPORT FrameDescription
{
public:
	//! Aufz�hlung f�r die Richtung in die der Rahmen �bertragen wird
	enum{
		DIR_TO_DEVICE,//!< Von der Zentrale zum Ger�t
		DIR_FROM_DEVICE//!< Vom Ger�t an die Zentrale oder ein anderes Ger�t
	};
	//! Symbolischer Wert, der anstelle einer Kanalnummer verwendet werden kann
	enum{
		ALL_CHANNELS=(unsigned int)-1
	};

	//! Beschreibung eines einzelnen Parameters innerhalb einer Rahmenbeschreibung
	/*! Diese Klasse repr�sentiert einen einzelnen Parameter innerhalb eines Rahmens.
	 *  Ein Parameter hat einen Datentyp (String oder Integer), einen Namen und einen Wert.
	 *  Bei dem Wert kann es sich um einen konstanten Wert handelt, der gegen einen konkreten Rahmen
	 *  gepr�ft wird, einen konstanten Wert, der zur�ckgegeben wird oder einen variablen Wert, der aus
	 *  dem konkreten Rahmen extrahiert wird.
	 */
	class DLLEXPORT Parameter{
	public:
		//! Typedef f�r den Datentyp des Parameters
		typedef enum{TYPE_INTEGER, TYPE_STRING} type_t;
		//! Typedef f�r verschiedenen Flags des Parameters
		/*! Es sind nicht alle Kombinationen von Flags zul�ssig
		 */
		typedef enum{
			FLAG_NONE=0, //!< Kein Flag gesetzt
			FLAG_CONST=(1<<0), //!< Konstanter Wert in \c value
			FLAG_CHECK=(1<<1), //!< Konstanten Wert in MatchFrame() pr�fen
			FLAG_OMIT=(1<<2), //!< Parameter in InitFrame() ignorieren, wenn der konkrete Wert dem Wert in \c value entspricht
			FLAG_SIGNED=(1<<3) //!< Es handelt sich um einen vorzeichenbehafteten Wert
		} flags_t;
		//! Operatoren f�r die Pr�fung gegen einen konstanten Wert mittels MatchFrame()
		enum{
			OP_EQ, //!< Gleich
			OP_LE, //!< Kleiner oder gleich
			OP_LT, //!< Kleiner
			OP_GT, //!< Gr��er
			OP_GE, //!< Gr��er oder gleich
			OP_NE  //!< Ungleich
		};

		//! Konstruktor
		Parameter();
		//! Copy-Konstruktor
		Parameter(const Parameter& p);
		//! Destruktor
		~Parameter();
		//! Parameter aus einer XML-Beschreibung initialisieren
		/*! Wird w�hrend des Einlesens einer Ger�tebeschreibungsdatei aufgerufen
		 */
		bool InitFromXml(XMLNode &node, XMLNode &root_node);
		//! Einen Parameter gegen einen konkreten Rahmen pr�fen oder aus diesem extrahieren
		/*! Diese Methode wird f�r jeden Parameter der FrameDescription aufgerufen, wenn gepr�ft 
		 *  werden soll, ob ein empfangener Rahmen einer gegebenen FrameDescription entspricht. 
		 *  Optional werden noch die im empfangenen Rahmen enthaltenen Werte aus dem Rahmen extrahiert 
		 *  und in \c store gespeichert.
		 *  Abh�ngig von \c flags wird folgendes gemacht:
		 *  - FLAG_CONST und FLAG_CHECK gesetzt: Wert aus \c frame extrahieren und gegen \c value pr�fen
		 *  - FLAG_CONST und nicht FLAG_CHECK gesetzt: Wert aus \c value dem Schl�ssel \c param_name in \c store eintragen
		 *  - sonst: Wert aus frame extrahieren und mit dem Schl�ssel \c param_name in \c store eintragen
		 *  \param store Ein Objekt der Klasse ValueStore, das die extrahierten Werte aufnimmt.
		 *               Bei \c store==NULL wird nur gepr�ft
		 *  \param frame Der zu pr�fende konkrete Rahmen
		 *  \return \c false wenn FLAG_CONST und FLAG_CHECK gesetzt und Wertepr�fung fehlgeschlagen,
		 *          \c true sonst
		 *          Mit anderen Worten gibt der R�ckgabewert an, ob der beschriebene Parameter zum
		 *          �bergebenen konkreten Rahmen passt.
		 */
		bool MatchFrame(ValueStore* store, StructuredFrame& frame);
		//! Einen Parameter in einen zu erzeugenden Rahmen �bernehmen
		/*! Diese Methode wird f�r jeden Parameter der FrameDescription aufgerufen, wenn ein zu sendender 
		 *  Rahmen erstellt wird. 
		 *  Abh�ngig von \c flags wird folgendes gemacht:
		 *  - FLAG_CONST und FLAG_CHECK gesetzt: Nichts tun und \c true zur�ckgeben
		 *  - FLAG_CONST und nicht FLAG_CHECK gesetzt: Wert aus \c value in \c frame �bernehmen
		 *  - sonst: Wert mit Schl�ssel \c param_name aus \c store in Frame �bernehmen
		 *  \param store Ein Objekt der Klasse ValueStore, das die in den Rahmen zu �bernehmenden Werte enth�lt.
		 *  \param frame Zeiger auf den zu initialisierenden konkreten Rahmen
		 *  \return \c true wenn der Rahmen mit diesem Parameter initialisiert werden konnte
		 *          \c false im Fehlerfall
		 */
		bool InitFrame(ValueStore* store, StructuredFrame* frame);
	protected:
		//! �bernimmt den (konstanten) Wert aus \c value in den �bergebenen Rahmen
		/*! Wird aus InitFrame() heraus aufgerufen
		 *  \return \c true im Erfolgsfall
		 */
		bool FrameSetValue(StructuredFrame* frame);
		//! Pr�ft den (konstanten) Wert in \c value gegen den �bergebenen Rahmen
		/*! Wird aus MatchFrame() heraus aufgerufen
		 *  \return \c true bei erfolgreicher Pr�fung
		 */
		bool FrameCheckValue(StructuredFrame& frame);
		//! Datentyp Integer oder String
		type_t type;
		//! Gibt an, wie mit dem Parameter bei der Initialisierung oder Pr�fung eines Rahmens verfahren wird
		int flags;
		//! Startposition in Bytes dieses Parameters im konkreten Rahmen
		int index_bytes;
		//! Startposition in Bits innerhalb des Startbytes dieses Parameters im konkreten Rahmen
		int index_bits;
		//! L�nge in Bytes dieses Parameters im konkreten Rahmen
		int size_bytes;
		//! L�nge in Bits innerhalb des Startbytes dieses Parameters im konkreten Rahmen
		int size_bits;
		//! Vergleichsoperator f�r die Pr�fung eines konstanten Wertes
		int cond_op;
		//! Enth�lt den konstanten Wert
		union{
			//! Wird verwendet bei \c type==TYPE_INTEGER
			uint32_t as_int;
			//! Wird verwendet bei \c type==TYPE_STRING
			std::string *as_string;
		}value;
		//! Name (Schl�ssel) dieses Parameters in ValueStore
		std::string param_name;
	};
	//! Konstruktor
	FrameDescription(void);
	//! Destruktor
	~FrameDescription(void);
	//! FrameDescription aus einer XML-Beschreibung initialisieren
	/*! Wird w�hrend des Einlesens einer Ger�tebeschreibungsdatei aufgerufen
	 */
	bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Einen Rahmen, der gesendet werden soll initialisieren
	/*! �bernimmt die Werte aus \c store sowie in den Parametern spezifizierte konstante Werte in den
	 *  zu initialisierenden Rahmen. Ruft dazu f�r jeden Parameter aus \c params die Methode
	 *  Parameter::InitFrame() auf.
     *  \param frame Zeiger auf den zu initialisierenden Rahmen
	 *  \param store ValueStore, der die zu �bernehmenden Werte enth�lt
	 *  \param channel Nummer des Kanals, f�r den der Rahmen bestimmt ist. Wird an die durch \c channel_field
	 *                 beschriebene Stelle im zu initialisierenden Rahmen �bernommen
	 *  \param receiver_channel Zweite Kanalnummer, die in wenigen F�llen (simulierter Tastendruck BidCoS-Wired)
	 *                          n�tig ist
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	bool InitFrame(StructuredFrame* frame, ValueStore* store, int channel=0, int receiver_channel=0);
	//! Pr�fen, ob ein empfangener Rahmen dieser Rahmenbeschreibung entspricht und Extrahieren der Werte
	/*! Pr�ft, ob der in \c frame �bergebene Rahmen den konstanten zu pr�fenden Parametern dieser FrameDescription
	 *  entspricht. Extrahiert die nicht-konstanten Parameter und speichert diese in \c matched_values.
	 *  Ruft dazu f�r jeden Parameter aus \c params die Methode Parameter::MatchFrame() auf.
	 *  \param frame Der empfangene Rahmen
	 *  \param channel Zeiger auf eine Variable, die die aus \c frame extrahierte Kanalnummer aufnimmt
	 *  \return \c true wenn der �bergebene Rahmen zur Beschreibung pa�t
	 */
	bool MatchFrame(StructuredFrame& frame, int* channel=NULL);
	//! Gibt die ID dieses Objektes zur�ck
	/*! Jede FrameDescription hat eine ID, �ber welche sie z.B. innerhalb einer Ger�tebeschreibungsdatei 
	 *  referenziert werden kann
	 */
	inline const std::string& GetId(){return id;};
	//! Gibt den Rahmentyp zur�ck, den die FrameDescription beschreibt
	/*! Der zur�ckgegebene Wert enth�lt den Rahmentyp und einen eventuellen Untertyp in gepackter
	 *  Form. Der hier zur�ckgegebene Wert kann direkt an StructuredFrame::MatchType() �bergeben werden.
	 */
	inline int GetType(){return type;};
	//! Gibt die Senderichtung f�r diese FrameDescription zur�ck
	int GetDirection(){return direction;};
	//! Gibt den Feld-Identifier f�r die zweite Kanalnummer zur�ck
	/*! Der Wert kommt in der Form zur�ck, die direkt an StructuredFrame::GetIntValue() �bergeben werden kann
	 */
	unsigned int GetReceiverChannelField(){return receiver_channel_field;};
	//! Gibt eine Referenz auf den internen ValueStore f�r von MatchFrame() extrahierte Werte zur�ck
	ValueStore& GetMatchedValues(){return matched_values;};

	//!Gibt an, ob die Werte dieses Frames auf andere Kanäle verteilt werden sollen (unabhängig von der channel_field Angabe)
	inline bool isValueFowardingEnabled() { return valueForwarding;}
protected:

	//! Senderichtung f�r diese FrameDescription
	int direction;
	//! Typedef f�r Vektor, der die Parameter dieser FrameDescription enth�lt
	typedef std::vector<Parameter> params_t;
	//! Vektor, der die Parameter dieser FrameDescription enth�lt
	params_t params;
	//! Rahmentyp, den die FrameDescription beschreibt
	/*! Enth�lt den Rahmentyp und einen eventuellen Untertyp in gepackter
	 *  Form. Der hier zur�ckgegebene Wert kann direkt an StructuredFrame::MatchType() �bergeben werden.
	 */
	int type;
	//! Feld-Identifier f�r die Kanalnummer
	/*! Gepackter Wert in der Form, die direkt an StructuredFrame::GetIntValue() �bergeben werden kann
	 */
	unsigned int channel_field;
	//! Feld-Identifier f�r die zweite Kanalnummer
	/*! Gepackter Wert in der Form, die direkt an StructuredFrame::GetIntValue() �bergeben werden kann
	 */
	unsigned int receiver_channel_field;
	//! Fest vorgegebene Kanalnummer
	/*! Durch die FrameDescription vorgegebene Kanalnummer. Ist n�tig, um Rahmen der Sicherungsschicht, 
	 *  die evtl. �berhaupt keine Kanalnummer tragen flexibel den Kan�len zugeordnet werden k�nnen.
	 *  Insbesondere kann hier \c ALL_CHANNELS eingetragen sein, wenn ein eingehender Rahmen von allen
	 *  Kan�len verarbeitet werden soll.
	 *  Ist nur g�ltig, wenn \c channel_field=0 und wird mit \c 0 initialisiert.
	 */
	unsigned int fixed_channel;
	//! ID dieses Objektes
	/*! Jede FrameDescription hat eine ID, �ber welche sie z.B. innerhalb einer Ger�tebeschreibungsdatei 
	 *  referenziert werden kann
	 */
	std::string id;
	//! Bitfeld aus mit Werten aus StructuredFrame::RCV_*
	/*! Gibt an, an wen ein Rahmen, auf den diese FrameDescription passen soll, gerichtet sein darf.
	 */
	int receiver_mask;
	//! Interner ValueStore f�r von MatchFrame() extrahierte Werte
	ValueStore matched_values;

	//! Gibt an, ob die Werte dieses Frames auf Parameter anderer Kanäle
	bool valueForwarding;
};
#endif //__FRAME_DESCRIPTION_H_

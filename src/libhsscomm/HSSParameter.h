/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// HSSParameter.h: Schnittstelle f�r die Klasse HSSParameter.
//
//////////////////////////////////////////////////////////////////////

#ifndef _HSSPARAMETER_H_
#define _HSSPARAMETER_H_

#include "dllexport.h"


#include <string>
#include "xmlParser.h"
#include <XmlRpc.h>
#include "CommMessage.h"
#include "HSSDescription.h"
#include <vector>

class  LogicalInstance;
class  HSSLogicalType;
class  HSSPhysicalType;
class  HSSTypeConversion;
class  FrameDescription;
class  HSSParamset;

#ifdef ReportEvent
#undef ReportEvent
#endif


//! Klasse f�r einen Parameter eines Parametersets
/*! Aus Parametern bestehende Parametersets (HSSParamset) sind die Grundlage der Kommunikation �ber
 *  die XmlRpc-Schnittstelle.
 *  Ein Parameter setzt sich zusammen aus einem logischen Typ (HSSLogicalType) auf der Seite der XmlRpc-Schnittstelle
 *  sowie einem physikalischen Typ (HSSPhysicalType) auf der Seite des Ger�tes.
 *  Die Umwandlung von logischem Typ in den physikalischen Typ und zur�ck geschieht �ber eine Menge von Konvertierungen
 *  (HSSTypeConversion). Wird keine Konvertierung angegeben, so wird automatisch eine Standardkonvertierung aus dem
 *  logischen und physikalischen Typ abgeleitet.
 */
class DLLEXPORT HSSParameter  
{
public:
	//! Aufz�hlung f�r Bitfeld der mit dem Parameter m�glichen Operationen aus Sicht des logischen Typs
	/*! Werte f�r das Feld \c OPERATIONS in der Parameterbeschreibung an der XmlRpc-Schnittstelle
	 */
	enum{
		OP_NONE=0, //!< Keine Operation, nur f�r Initialisierung
		OP_READ=1, //!< Parameter kann �ber die XmlRpc-Schnittstelle gelesen werden
		OP_WRITE=2, //!< Parameter kann �ber die XmlRpc-Schnittstelle geschrieben werden
		OP_EVENT=4, //!< Parameter kann �ber die XmlRpc-Schnittstelle ein Ereignis ausl�sen
		OP_DETERMINE=8 //!< Parameterwert kann vom Ger�t selbst ermittelt werden
	};
	//! Aufz�hlung f�r verschiedene Flags
	/*! Werte f�r das Feld \c FLAGS in der Parameterbeschreibung an der XmlRpc-Schnittstelle
	 */
	enum{
		FLG_NONE=0, //!< Kein Flag gesetzt. Wert f�r Initialisierung.
		FLG_VISIBLE=(1<<0), //!< Der Parameter erscheint an der Oberfl�che nicht (obsolet)
		FLG_INTERNAL=(1<<1), //!< Der Parameter wird nur intern verwendet und erscheint an der Oberfl�che nicht
		FLG_TRANSFORM=(1<<2), //!< Eine �nderung dieses Parameters kann die Beschreibung des zugeh�rigen Kanals ver�ndern
		FLG_SERVICE=(1<<3), //!< Dieser Parameter soll von der Oberfl�che als Servicenachricht behandelt werden
		FLG_STICKY=(1<<4)  //!< Servicenachricht setzt sich nicht selbst zur�ck, wenn der Grund f�r die Nachricht entf�llt
	};

	struct WriteDependency {
		std::string name;
		bool interfaceCommandParam;

		WriteDependency() {
			interfaceCommandParam = false;
		}

	};

	//! Holt den default Wert vom logischen Typ und ruft mit diesem Wert die SetDefaultConfig Methode des physical_type auf
	virtual bool SetDefaultConfig(LogicalInstance *inst);
	//! Pr�fen auf Lesbarkeit �ber die XmlRpc-Schnittstelle
	inline bool IsReadable(){return (operations & OP_READ)!=0;};
	//! Pr�fen auf Schreibbarkeit �ber die XmlRpc-Schnittstelle
	inline bool IsWriteable(){return (operations & OP_WRITE)!=0;};
	//! Pr�fen, ob der Parameter �ber die XmlRpc-Schnittstelle ein Ereignis ausl�sen kann
	inline bool IsEventable(){return (operations & OP_EVENT)!=0;};
	//! Pr�fen, ob der Parameterwert vom Ger�t selbst ermittelt werden kann
	inline bool IsDeterminable(){return (operations & OP_DETERMINE)!=0;};
	//! Pr�fen, ob der Parameterwert an der XmlRpc-Schnittstelle erscheint. Hat nichts mit \c FLG_VISIBLE zu tun
	inline bool IsVisible(){return !hidden;};
	//! Gibt die Flags zur�ck
	inline int GetFlags(){return flags;};
	//! Verarbeitet eine eingehende Nachricht, z.B. ein vom zugeh�rigen Ger�t gesendetes Funkpaket
	/*! Extrahiert den physikalischen Wert aus der Nachricht, konvertiert diesen in den logischen Wert
	 *  und l�st dann ein entsprechendes Ereignis aus
	 *  \param inst Zeiger auf das Kanalobjekt, dem die eingehende Nachricht zugeordnet ist
	 *  \param msg Referenz auf die zu verarbeitende Nachricht
	 *  \param fd Zeiger auf die zu \c msg passende abstrakte Beschreibung der Nachricht
	 */
	void ProcessIncomingFrame(LogicalInstance* inst, StructuredFrame& frame, FrameDescription* fd);
	//! Gibt die Beschreibung des Parameters gem�� XmlRpc-Schnittstelle zur�ck
	bool GetDescription(XmlRpc::XmlRpcValue* val);
	//! Versucht, den aktuellen logischen Wert zu ermitteln
	/*! Liest den physikalischen Wert vom Ger�t, konvertiert diesen in den logischen Wert und
	 *  liefert den logischen Wert zur�ck.
	 *  Diese Methode l�st normalerweise eine Kommunikation mit dem Ger�t aus.
	 *  \param inst Zeiger auf das Kanal- oder Ger�teobjekt, von dem der Wert gelesen werden soll
	 *  \param val Zeiger auf die Variable, die den ermittelten Wert aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
    bool GetValue(LogicalInstance* inst, XmlRpc::XmlRpcValue* val);
    bool GetValue(LogicalInstance* inst,int mode, XmlRpc::XmlRpcValue* val);
	//! Versucht, den aktuellen logischen Wert zu setzen
	/*! Konvertiert den �bergebenen logischen Wert in den physikalischen Wert und schreibt diesen auf das Ger�t
	 *  Diese Methode l�st normalerweise eine Kommunikation mit dem Ger�t aus.
	 *  \param inst Zeiger auf das Kanal- oder Ger�teobjekt, auf das der Wert geschrieben werden soll
	 *  \param val Referenz auf den zu schreibenden Wert
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
    bool SetValue(LogicalInstance* inst, XmlRpc::XmlRpcValue& val);
	//! Weist das Ger�t an, den logischen Wert zu ermitteln
	/*! Durch den Aufruf dieser Methode wird das entsprechende Ger�t veranlasst, den Wert des Parameters
	 *  selbstst�ndig zu ermitteln. Ein Beispiel f�r einen Wert, der dies unterst�tzt ist der von der
	 *  angeschlossenen Last abh�ngige Schwellwert f�r die Lastausfallerkennung eines Dimmers.
	 *  Der ermittelte Wert kann �ber GetValue() abgefragt werden.
	 *  \param inst Ger�te- oder Kanalobjekt, f�r das der Wert ermittelt werden soll
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
    bool DetermineValue(LogicalInstance* inst);
	//! Pr�ft, ob der �bergebene Wert grunds�tzlich f�r einen Aufruf von SetValue() in Frage kommt
	/*! Wird w�hrend des Parsens von XML-Dateien f�r die Typpr�fung dort angegebener Werte aufgerufen
	 *  \param val Referenz auf den zu pr�fenden Wert
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	bool CheckType(XmlRpc::XmlRpcValue& val);
	//! Wird beim Erzeugen eines Ger�te- oder Kanalobjekts f�r alle zugeordneten Parameter aufgerufen
	/*! Dieser Aufruf wird weitergereicht an HSSPhysicalType::SetupInstance(). Der Parameter bekommt hier
	 *  die Gelegenheit, in dem zugeordneten Ger�te- oder Kanalobjekt abgelegte Werte 
	 *  (siehe LogicalInstance::SetStoredValue()) zu initialisieren.
	 *  \param inst Zeiger auf das zugeordnete Ger�te- oder Kanalobjekt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	bool SetupInstance(LogicalInstance* inst);
	//! Konvertiert einen physikalischen Wert in einen logischen Wert
	/*! Ruft dazu HSSTypeConversion::PhysicalToLogical() f�r alle Objekte im Vektor \c type_conversions
	 *  in der umgekehrten durch den Vektor gegebenen Reihenfolge auf. \c type_conversions[0].PhysicalToLogical()
	 *  wird also zuletzt aufgerufen.
	 *  \param inst Zeiger auf das Ger�te- oder Kanalobjekt, auf das sich die Konvertierung bezieht
	 *  \param in zu konvertierender Wert
	 *  \param out Zeiger auf die Variable, die den konvertierten Wert aufnimmt
	 *  \param event bei \c true soll aufgrund eines eingehenden Ereignisses konvertiert werden, 
	 *               bei \c false aufgrund eines Lesevorgangs
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	bool PhysicalToLogical(LogicalInstance* inst, XmlRpc::XmlRpcValue in, XmlRpc::XmlRpcValue* out, bool event);
	//! Konvertiert einen logischen Wert in einen physikalischen Wert
	/*! Ruft dazu HSSTypeConversion::LogicalToPhysical() f�r alle Objekte im Vektor \c type_conversions
	 *  in der durch den Vektor gegebenen Reihenfolge auf. \c type_conversions[0].PhysicalToLogical()
	 *  wird also zuerst aufgerufen.
	 *  \param inst Zeiger auf das Ger�te- oder Kanalobjekt, auf das sich die Konvertierung bezieht
	 *  \param in zu konvertierender Wert
	 *  \param out Zeiger auf die Variable, die den konvertierten Wert aufnimmt
	 *  \return \c true im Erfolgsfall, \c false im Fehlerfall
	 */
	bool LogicalToPhysical(LogicalInstance* inst, XmlRpc::XmlRpcValue in, XmlRpc::XmlRpcValue* out);
	//! Dient der Definition einer Reihenfolge von Parametern innerhalb eines Parametersets
	/*! Die Definition der Reihenfolge ist nur f�r die Darstellung an der Oberfl�che relevant.
	 */
	inline void SetTabOrder(int o){tab_order=o;};
	//! Dient der Abfrage der definierten Reihenfolge von Parametern innerhalb eines Parametersets
	/*! Die Definition der Reihenfolge ist nur f�r die Darstellung an der Oberfl�che relevant.
	 */
	inline int GetTabOrder(){return tab_order;};
	//! Gibt den Namen des zugeordneten UI-Controls zur�ck
	const std::string& GetControl(){return control;};
	//! Liest den Parameter aus einer XML-Datei ein
	virtual bool InitFromXml(XMLNode &node, XMLNode &root_node);
	//! Gibt das zugeordnete Objekt f�r den logischen Typ zur�ck
	HSSLogicalType* GetLogicalType(){return logical_type;};
	//! Hilfsfunktion f�r das Senden eines Ereignisses �ber die XmlRpc-Schnittstelle
	/*! F�hrt eine Konvertierung des �bergebenen physikalischen Wertes in einen logischen Wert durch
	 *  und ruft mit dem logischen Wert LogicalInstance::ReportEvent() auf.
	 *  \param inst Zeiger auf das zugeordnete Ger�te- oder Kanalobjekt
	 *  \param phys_val Wert f�r das Ereignis in physikalischer Darstellung
	 */
	void ReportEvent(LogicalInstance* inst, XmlRpc::XmlRpcValue& phys_val);
	//! Ruft mit dem Defaultwert SetValue() auf
	bool SetToDefault(LogicalInstance* inst);
	//! Konstruktor
	HSSParameter();
	//! Destruktor
	virtual ~HSSParameter();
	//! Gibt die ID des Parameters zur�ck
	/*! �ber die ID wird ein Parameter innerhalb eines Parametersets eindeutig spezifiziert.
	 */
    const std::string& GetId();
	//! Hier�ber wird w�hrend der Initialisierung ein Zeiger auf das Parameterset gesetzt
	void SetParamset(HSSParamset* paramset){this->paramset=paramset;};
	//! Liefert den Zeiger auf das zugeh�rige Parameterset zur�ck
	HSSParamset* GetParamset(){return paramset;};
	bool IsUndefined();
	void SetUndefined(bool val);

	/**\brief Gibt an, ob dieser Parameter beim Setzen (über SetValue) fachlich von anderen Parametern abhängig ist.
	* \details Fachlich abhängig bedeutet hier, dass die Werte der anderen Parameter für die Write Operation erforderlich sind und zuerst gesetzt werden.
	* Üblicherweise sind die anderen Parameter dem 'interface=store' zugeordnet.
	*/
	bool HasWriteDependencies() { return (!writeDepencyParams.empty()); };

	const std::vector<WriteDependency>& getWriteDependencies() { return writeDepencyParams; };

	bool IsForwardingEnabled() { return acceptForwardedValues; }

protected:
	//! Die innerhalb eines Parametersets eindeutige ID des Parameters
    std::string id;
	//! Zeiger auf das Objekt f�r den zugeordneten logischen Typ
    HSSLogicalType *logical_type;
	//! Zeiger auf das Objekt f�r den zugeordneten physikalischen Typ
    HSSPhysicalType *physical_type;
	//! Vektor mit Zeigern auf die zugeordneten Konvertierungsobjekte
	/*! Wird von den Konvertierungsmethoden LogicalToPhysical() und PhysicalToLogical() verwendet
	 */
	std::vector<HSSTypeConversion*> type_conversions;
	//! Bitfeld f�r die �ber die XmlRpc-Schnittstelle m�glichen Operationen
	int operations;
	//! Bitfeld f�r die �ber die XmlRpc-Schnittstelle abzufragenden Flags
	int flags;
	//! Wenn dieses Flag \c true ist, ist der Parameter aus Sicht der XmlRpc-Schnittstelle nicht vorhanden
	bool hidden;
	//! Wenn dieses Flag \c true ist, erzeugt ein Aufruf von SetValue() automatisch ein Ereignis f�r den gesetzten Wert
	bool loopback;
	//! Zus�tzliche aus der XML-Datei gelesene Schl�ssel-Werte-Paare, die in die XmlRpc-Parameterbeschreibung aufgenommen werden
	HSSDescription additional_description;
	//! Aus der XML-Datei gelesenes UI-Control
	std::string control;
	//! Aus der XML-Datei gelesene Zeit in ms f�r die Unterdr�ckung kurz aufeinanderfolgender Ereignisse
	uint32_t burst_suppression_time;
	//! UI-Relevante Position des Parameters innerhalb des Parametersets
	/*! Die Definition der Reihenfolge ist nur f�r die Darstellung an der Oberfl�che relevant.
	 */
	int tab_order;
	//! Zeiger auf das enthaltende Parameterset
	HSSParamset* paramset;
	bool undefined;

	
	/** \brief List of parameters which are dependencies of another parameter*/
	std::vector<WriteDependency> writeDepencyParams;

	bool acceptForwardedValues;
};

#endif // _HSSPARAMETER_H_

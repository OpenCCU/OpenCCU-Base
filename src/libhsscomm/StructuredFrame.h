/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

// StructuredFrame.h: Schnittstelle für die Klasse StructuredFrame.
//
//////////////////////////////////////////////////////////////////////

#ifndef _STRUCTUREDFRAME_H_
#define _STRUCTUREDFRAME_H_

#include "dllexport.h"

#include <string>
#include <vector>
#include <cstdint>

#define STRUCTURED_FRAME_FIELD(by_pos, bi_pos, by_size, bi_size, type) ((((by_pos<<3) | bi_pos) | ((by_size<<3) | bi_size)<<8) | ((type&0x7f) <<24))
#define STRUCTURED_FRAME_FIELD_STR(pos, size) STRUCTURED_FRAME_FIELD(pos, 0, size, 0, 1)
#define STRUCTURED_FRAME_FIELD_INT(by_pos, bi_pos, by_size, bi_size) STRUCTURED_FRAME_FIELD(by_pos, bi_pos, by_size, bi_size, 0)
#define STRUCTURED_FRAME_TYPE(type, subtype_index, subtype) (type | (subtype_index<<8) | (subtype<<16))

#define STRUCTURED_FRAME_BYTE_POS_FROM_FIELD(field) ((field&0xf8)>>3)
#define STRUCTURED_FRAME_BIT_POS_FROM_FIELD(field) (field&0x07)
#define STRUCTURED_FRAME_BYTE_SIZE_FROM_FIELD(field) ((field&0xf800)>>11)
#define STRUCTURED_FRAME_BIT_SIZE_FROM_FIELD(field) ((field&0x0700)>>8)
#define STRUCTURED_FRAME_TYPE_FROM_FIELD(field) ((field&0x7f000000)>>24)

//! Generische Klasse für einen Kommunikationsrahmen

class DLLEXPORT StructuredFrame  
{
public:
	//! Aufzählung für Empfängertyp für den eine empfangene Nachricht bestimmt ist
	enum ReceiverType{
		/*! unbekannter Empfängertyp */
		RCV_UNKNOWN, 
		/*! Nachricht ist direkt an die Zentrale adressiert */
		RCV_CENTRAL, 
		/*! Nachricht ist ein Broadcast */
		RCV_BROADCAST, 
		/*! Nachricht ist direkt an einen anderen Teilnehmer adressiert */
		RCV_OTHER 
	};

	//! Konstruktor
	StructuredFrame();
	//! Konstruktor
	StructuredFrame(const std::string& data);
	//! Destruktor
	virtual ~StructuredFrame();
	//! Parst einen String aus einer XML-Datei in eine Field-ID
	/*! Ein String der Form \c startbyte[.startbit][:bytelength[.bitlength]] wird geparst und für die
	 *  weitere Verwendung in einen 32bit-Wert gepackt. Die genaue Codierung des gepackten Wertes ist außerhalb der
	 *  Klasse StructuredFrame nicht interessant.
	 *  Strings in dieser Form werden von verschiedenen Klassen aus XML-Dateien gelesen.
	 *  \param s String, der geparst werden soll
	 *  \return gepackter Field-Identifier für weitere Verwendung. Dieser bezieht
	 *          sich auf die Daten der Sicherungsschicht (BidCoS-RF, etc) und nicht auf den
	 *          kompletten Datenblock gemäß Rahmenformat zwischen Kommunikationsprozessor und Hauptprozessor
	 */
	static unsigned int FieldIdFromString(const std::string& s);
	void SetULongData(int address, uint32_t v);
	//! Gibt einen unsigned 32Bit-Wert aus dem Datenblock zurück
	/*! \param address Byte-Adresse des gewünschten Wertes. Bezieht sich direkt auf den Datenblock 
	 *                 gemäß Rahmenformat zwischen Kommunikationsprozessor und Hauptprozessor und nicht 
	 *                 nur auf die Daten der Sicherungsschicht (BidCoS-RF, etc).
	 */
	uint32_t GetULongData(int address) const;
	//! Setzt einen unsigned 16Bit-Wert im Datenblock
	/*! \param address Byte-Adresse des zu setzenden Wertes. Bezieht sich direkt auf den Datenblock 
	 *                 gemäß Rahmenformat zwischen Kommunikationsprozessor und Hauptprozessor und nicht 
	 *                 nur auf die Daten der Sicherungsschicht (BidCoS-RF, etc).
	 *  \param v Der zu setzende Wert
	 */
	void SetUShortData(int address, unsigned short v);
	//! Gibt einen unsigned 16Bit-Wert aus dem Datenblock zurück
	/*! \param address Byte-Adresse des gewünschten Wertes. Bezieht sich direkt auf den Datenblock 
	 *                 gemäß Rahmenformat zwischen Kommunikationsprozessor und Hauptprozessor und nicht 
	 *                 nur auf die Daten der Sicherungsschicht (BidCoS-RF, etc).
	 */
	unsigned short GetUShortData(int address) const;
	//! Setzt einen unsigned 8Bit-Wert im Datenblock
	/*! \param address Byte-Adresse des zu setzenden Wertes. Bezieht sich direkt auf den Datenblock 
	 *                 gemäß Rahmenformat zwischen Kommunikationsprozessor und Hauptprozessor und nicht 
	 *                 nur auf die Daten der Sicherungsschicht (BidCoS-RF, etc).
	 *  \param v Der zu setzende Wert
	 */
	void SetByteData(int address, unsigned char v);
	//! Gibt einen unsigned 8Bit-Wert aus dem Datenblock zurück
	/*! \param address Byte-Adresse des gewünschten Wertes. Bezieht sich direkt auf den Datenblock 
	 *                 gemäß Rahmenformat zwischen Kommunikationsprozessor und Hauptprozessor und nicht 
	 *                 nur auf die Daten der Sicherungsschicht (BidCoS-RF, etc.).
	 */
	unsigned char GetByteData(int address) const;
	//! Gibt einen Integer-Wert aus dem Datenblock zurück
	/*! \param field_id Identifier für das gewünschte Feld. Um diesen Identifier zu erzeugen, stehen 
	 *                  Makros bereit. Ein solcher Identifier wird auch von FieldIdFromString zurückgeliefert.
	 *                  Abgeleitete Klassen sollten symbolische Feldbezeichner als \c enum deklarieen.
	 */
	inline uint32_t GetIntValue(unsigned int field_id) const
	{
		int by_pos=STRUCTURED_FRAME_BYTE_POS_FROM_FIELD(field_id);
		int bi_pos=STRUCTURED_FRAME_BIT_POS_FROM_FIELD(field_id);
		int by_size=STRUCTURED_FRAME_BYTE_SIZE_FROM_FIELD(field_id);
		int bi_size=STRUCTURED_FRAME_BIT_SIZE_FROM_FIELD(field_id);
		int type=STRUCTURED_FRAME_TYPE_FROM_FIELD(field_id);
		if(type!=0)return (uint32_t)-1;
		uint32_t val = 0;
		if(!GetIntValue(by_pos, bi_pos, by_size, bi_size, &val))return(uint32_t)-1;
		return val;

	};
	//! Setzt einen Integer-Wert im Datenblock
	/*! \param field_id Identifier für das gewünschte Feld. Um diesen Identifier zu erzeugen stehen 
	 *                  Makros bereit. Ein solcher Identifier wird auch von FieldIdFromString zurückgeliefert.
	 *                  Abgeleitete Klassen sollten symbolische Feldbezeichner als \c enum deklarieen.
	 *  \param value Der zu setzende Wert
	 */
	inline void SetIntValue(unsigned int field_id, uint32_t value)
	{
		int by_pos=STRUCTURED_FRAME_BYTE_POS_FROM_FIELD(field_id);
		int bi_pos=STRUCTURED_FRAME_BIT_POS_FROM_FIELD(field_id);
		int by_size=STRUCTURED_FRAME_BYTE_SIZE_FROM_FIELD(field_id);
		int bi_size=STRUCTURED_FRAME_BIT_SIZE_FROM_FIELD(field_id);
		int type=STRUCTURED_FRAME_TYPE_FROM_FIELD(field_id);
		if(type!=0)return;
		SetIntValue(by_pos, bi_pos, by_size, bi_size, value);

	};
	//! Gibt einen Integer-Wert aus dem Datenblock zurück
	/*! \param index_bytes Startbyte im Datenblock für den gewünschten Wert
	 *  \param index_bits Startbyte innerhalb des Startbyte im Datenblock für den gewünschten Wert
	 *  \param size_bytes Länge in Bytes des gewünschten Wertes
	 *  \param size_bits Länge in Bits des gewünschten Wertes
	 *  \param value Zeiger auf die Variable, die den gewünschten Wert aufnimmt
	 *  \param map Bei \c map=true beziehen sich die übergebenen Indizes auf auf die
	 *             Daten der Sicherungsschicht (BidCoS-RF, etc), ansonsten auf den kompletten Datenblock 
	 *             gemäß Rahmenformat zwischen Kommunikationsprozessor und Hauptprozessor
	 */
	bool GetIntValue(int index_bytes, int index_bits, int size_bytes, int size_bits, uint32_t* value) const;
	//! Setzt einen Integer-Wert im Datenblock
	/*! \param index_bytes Startbyte im Datenblock für den zu setzenden Wert
	 *  \param index_bits Startbyte innerhalb des Startbyte im Datenblock für den zu setzenden Wert
	 *  \param size_bytes Länge in Bytes des zu setzenden Wertes
	 *  \param size_bits Länge in Bits des zu setzenden Wertes
	 *  \param value Der zu setzende Wert
	 *  \param map Bei \c map=true beziehen sich die übergebenen Indizes auf auf die
	 *             Daten der Sicherungsschicht (BidCoS-RF, etc), ansonsten auf den kompletten Datenblock 
	 *             gemäß Rahmenformat zwischen Kommunikationsprozessor und Hauptprozessor
	 */
	bool SetIntValue(int index_bytes, int index_bits, int size_bytes, int size_bits, uint32_t value);
	//! Gibt einen String-Wert aus dem Datenblock zurück
	/*! \param field_id Identifier für das gewünschte Feld. Um diesen Identifier zu erzeugen stehen 
	 *                  Makros bereit. Ein solcher Identifier wird auch von FieldIdFromString zurückgeliefert.
	 *                  Abgeleitete Klassen sollten symbolische Feldbezeichner als \c enum deklarieen.
	 */
	inline std::string GetStringValue(unsigned int field_id) const
	{
		int pos=(field_id&0xff)>>3;
		int size=(field_id&0xff00)>>11;
		int type=field_id>>24;
		if(type!=1)return "";
		std::string val;
		if(!GetStringValue(pos, size, &val))return "";
		return val;

	};

	//! Gibt einen String-Wert aus dem Datenblock zurück
	/*! \param index Startbyte im Datenblock für den gewünschten String.
	 *               Bezieht sich auf die Daten der Sicherungsschicht (BidCoS-RF, etc) und nicht auf den
	 *               kompletten Datenblock gemäß Rahmenformat zwischen Kommunikationsprozessor und Hauptprozessor
	 *  \param size Länge in Bytes des gewünschten Strings
	 *  \param value Zeiger auf die Variable, die den gewünschten Wert aufnimmt
	 */
	bool GetStringValue(int index, int size, std::string* value) const;
	//! Setzt einen String-Wert im Datenblock
	/*! \param index Startbyte im Datenblock für den zu setzenden String.
	 *               Bezieht sich auf die Daten der Sicherungsschicht (BidCoS-RF, etc) und nicht auf den
	 *               kompletten Datenblock gemäß Rahmenformat zwischen Kommunikationsprozessor und Hauptprozessor
	 *  \param size Länge in Bytes des zu setzenden Strings. Falls die Länge nicht paßt, wird abgeschnitten bzw.
	 *              mit 0 aufgefüllt.
	 *  \param value Zeiger auf die Variable, die den gewünschten Wert aufnimmt
	 */
	bool SetStringValue(int index, int size, const std::string& value);
	//! Setzt einen String-Wert im Datenblock
	/*! \param field_id Identifier für das gewünschte Feld. Um diesen Identifier zu erzeugen stehen 
	 *                  Makros bereit. Ein solcher Identifier wird auch von FieldIdFromString zurückgeliefert.
	 *                  Abgeleitete Klassen sollten symbolische Feldbezeichner als \c enum deklarieen.
	 *  \param value Der zu setzende Wert
	 */
	inline bool SetStringValue(unsigned int field_id, const std::string& value)
	{
		int pos=(field_id&0xff)>>3;
		int size=(field_id&0xff00)>>11;
		int type=field_id>>24;
		if(type!=1)return false;
		return SetStringValue(pos, size, value);
	}
	//! Gibt z.B. zur Persistierung eine String-Repräsentation des Datenblocks der Nachricht zurück
	/*! Diese Methode gibt den Datenblock der Nachricht als Hex-String zurück. Dieser String kann dann
	 *  z.B. als Attribut in einer XML-Datei persistiert werden.
	 */
	std::string ToString() const;
	//! Liest den Datenblock der Nachricht aus einer String-Repräsentation ein.
	/*! \param s Der einzulesende String. Sollte irgendwann einmal von ToString() erzeugt worden sein
	 */
	bool FromString(const std::string& s);

	//! Gibt die Größe des Datenblocks zurück.
	unsigned int GetSize() const;

	//! Vergrößert oder verkleinert den Datenblock
	bool Resize(unsigned int new_size);

	//! Setzt den Rahmentyp und Untertyp. Dafür muss von abgeleiteter Klasse type_index gesetzt worden sein.
	bool SetType(unsigned int type);

	//! Prüft den Rahmentyp und Untertyp. Dafür muss von abgeleiteter Klasse type_index gesetzt worden sein.
	bool MatchType(unsigned int type) const;

	//! Gibt den Empfängertyp zurück. Sollte in einer abgeleiteten Klasse überschrieben werden.
	virtual ReceiverType GetReceiverType() const;

	//! Prüft den Datenblock auf Gleichheit
	bool operator==(const StructuredFrame& sf) const;

	//! Prüft den Datenblock auf Ungleichheit
	bool operator!=(const StructuredFrame& sf) const;
protected:
	//! Datenblock der Nachricht
	std::string data;

	//! Index für den Rahmentypen
	int type_index;
};

#endif // _STRUCTUREDFRAME_H_

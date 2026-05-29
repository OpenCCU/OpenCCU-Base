/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _RF_CONFIG_DATA_H_
#define _RF_CONFIG_DATA_H_

#include <map>
#include <vector>
#include <xmlParser.h>
#include "BidcosFrame.h"

class RFLogicalInstance;

//! Eine Instanz dieser Klasse speichert ein HomeMatic-Konfigurationsprofil für einen Kanal oder ein Gerät
/*! 
 *  Eine Instanz ist einem Verknüpfungspartner mit BidCoS-Adresse und -Kanalnummer zugeordnet. Sind Adresse
 *  und Kanalnummer \c 0, so handelt es sich um ein Geräte- oder Kanalprofil.
 *
 *  Die Klasse bietet Unterstützung für das Lesen und Schreiben der Daten vom und zum Gerät sowie
 *  für das Speichern und Laden der Daten aus einer XML-Datei.
 *
 *  Die Daten werden in Chunks zu 32 Byte verwaltet. So reicht pro Chunk ein 32bit-Wert für die Speicherung
 *  von byteweiser Gültigkeitsinformation
 */
class RFConfigData
{
public:
	//! Repräsentation von 32 Byte HomeMatic-Konfigurationsdaten mit byteweisen Dirty-Flags
	class Chunk
	{
	public:
		//! Konstante für die Chunkgröße in Bytes
		enum{SIZE=sizeof(uint32_t)*8};
		//! Konstruktor
		Chunk(void);
		//! Setzt ein Byte im Chunk auf einen Wert
		/*! \param address Adresse des zu beschreibenden Bytes. Es werden nur die niederwertigsten Bits
		 *         gemäß \c SIZE betrachtet
		 *  \param b Der neue Wert des Bytes
		 *  \param dev_dirty Gibt an, ob das beschriebene Byte in Bezug auf das HomeMatic-Gerät als "dirty"
		 *         gilt, also noch an das Gerät übertragen werden muss.
		 *  \param file_dirty Gibt an, ob das beschriebene Byte in Bezug auf die Persistierung als "dirty"
		 *         gilt, also noch in eine Datei gespeichert werden muss.
		 */
		void SetByte(unsigned int address, unsigned char b, bool dev_dirty, bool file_dirty);
		//! Liefert ein Byte aus dem Chunk zurück
		/*! \param address Adresse des zu lesenden Bytes. Es werden nur die niederwertigsten Bits
		 *         gemäß \c SIZE betrachtet
	     */
		inline unsigned char GetByte(unsigned int address)
		{
			return data[address&(SIZE-1)];
		}
		//! Setzt den kompletten Chunk in Bezug auf das Gerät auf "clean", d.h. es sind keine Bytes mehr an das Gerät zu übertragen
		inline void SetDevClean()
		{
			dev_dirty_bits=0;
		};
		//! Setzt den kompletten Chunk in Bezug auf das Gerät auf "dirty", d.h. es sind alle Bytes noch an das Gerät zu übertragen
		inline void SetDevDirty()
		{
			dev_dirty_bits=used_bits;
		};
		//! Liefert das "dirty"-Bit in Bezug auf das Gerät für ein bestimmtes Byte zurück
		/*! \param address Adresse des betrachteten Bytes. Es werden nur die niederwertigsten Bits
		 *         gemäß \c SIZE betrachtet
	     */
		inline bool IsDevDirty(unsigned int address)
		{
			return (dev_dirty_bits&(1<<(address&(SIZE-1))))!=0;
		};
		//! Prüft, ob irgendein Byte des Chunks in Bezug auf das Gerät "dirty" ist
		inline bool IsDevDirty()
		{
			return dev_dirty_bits!=0;
		};
		//! Liefert das "dirty"-Bit in Bezug auf die persistente Speicherung für ein bestimmtes Byte zurück
		/*! \param address Adresse des betrachteten Bytes. Es werden nur die niederwertigsten Bits
		 *         gemäß \c SIZE betrachtet
	     */
		inline bool IsFileDirty(unsigned int address)
		{
			return (file_dirty_bits&(1<<(address&(SIZE-1))))!=0;
		};
		//! Liefert das "used"-Bit bestimmtes Byte zurück
		/*! Das "used"-Bit gibt an, ob ein bestimmtes Byte des Chubks relevante Daten enthält.
		 *  \param address Adresse des betrachteten Bytes. Es werden nur die niederwertigsten Bits
		 *         gemäß \c SIZE betrachtet
	     */
		inline bool IsUsed(unsigned int address)
		{
			return (used_bits&(1<<(address&(SIZE-1))))!=0;
		};
		//! Liefert zurück, ob der Chunk noch vom Gerät eingelesen werden muss
		inline bool MustBeRead(){
			return must_be_read;
		};
		//! Markiert den Chunk als bereits vom Gerät gelesen
		inline void SetRead(){
			must_be_read=false;
		};
		//! Speichert den Chunk mit allen Flags in eine XML-Datei
		bool SaveToXml(XMLNode* node);
		//! Liest den Chunk mit allen Flags aus einer XML-Datei
		bool LoadFromXml(XMLNode& node);
	protected:
		//! Array für die Datenbytes
		unsigned char data[SIZE];
		//! "dirty"-Bits in Bezug auf das Gerät. Für jedes Datenbyte ein Bit.
		uint32_t dev_dirty_bits;
		//! "dirty"-Bits in Bezug auf die Persistierung. Für jedes Datenbyte ein Bit.
		uint32_t file_dirty_bits;
		//! "used"-Bits. Für jedes Datenbyte ein Bit.
		uint32_t used_bits;
		//! Flag für "Es wurden noch keine Daten vom Gerät übernommen"
		bool must_be_read;
	};
	//! Konstruktor
	RFConfigData(void);
	//! Destruktor
	~RFConfigData(void);
	//! Setzt die Werte in der entsprechenden Liste und unterbindet das Lesen vom Gerät 
	void PushData(int list, unsigned char by_pos, unsigned char bi_pos, unsigned char by_size, unsigned char bi_size, uint32_t value, uint32_t mask=0xffffffff);
	//! Liest die Konfigurationsparameter einer Liste aus dem Gerät aus
	bool ReadFromDevice(RFLogicalInstance* inst, int list);
	//! Überträgt alle Konfigurationsparameter an das Gerät unter Berücksichtigung der "used"- und "dirty"-Bits
	bool CommitToDevice(RFLogicalInstance* inst);
	//! Gibt einen Integerwert mit variabler Bitlänge aus dem Speicher zurück
	/*!
	 *  \param list Nummer der HomeMatic-Parameterliste, in der der Wert definiert ist
	 *  \param by_pos Adresse in Bytes des gewünschten Wertes
	 *  \param bi_pos Adresse in Bits innerhalb des Bytes des gewünschten Wertes
	 *  \param by_size Länge in Bytes des gewünschten Wertes
	 *  \param bi_size Länge in Bits innerhalb des Bytes des gewünschten Wertes
	 *  \param mask Bitmaske für den zu lesenden Wert. Bits, die hier \c 0 sind, werden aus dem Ergebnis ausgeblendet
	 *         und als \c 0 zurückgegeben.
	 */
	uint32_t GetValue(int list, unsigned char by_pos, unsigned char bi_pos, unsigned char by_size, unsigned char bi_size, uint32_t mask =0xffffffff);
	// es wird ein Flag gesetzt wodurch das auslesen von nicht vorhandenen Parameterlisten unterbunden wird 
	void InitDefault(void);
	//! Schreibt einen Integerwert mit variabler Bitlänge in den Speicher
	/*!
	 *  \param list Nummer der HomeMatic-Parameterliste, in der der Wert definiert ist
	 *  \param by_pos Adresse in Bytes des zu schreibenden Wertes
	 *  \param bi_pos Adresse in Bits innerhalb des Bytes des zu schreibenden Wertes
	 *  \param by_size Länge in Bytes des zu schreibenden Wertes
	 *  \param bi_size Länge in Bits innerhalb des Bytes des zu schreibenden Wertes
	 *  \param value Der zu schreibende Wert
	 *  \param mask Bitmaske für den zu schreibenden Wert. Bits, die hier \c 0 sind, werden nicht verändert und es  werden
	 *         für diese Bits auch die "used"- und "dirty"-Bits nicht beeinflusst.
	 */
	void PutValue(int list, unsigned char by_pos, unsigned char bi_pos, unsigned char by_size, unsigned char bi_size, uint32_t value, uint32_t mask =0xffffffff);
	//! Weist das Gerät an, einen Parameterwert selbstständig zu ermitteln
	/*!
	 *  \param inst Zeiger auf das Geräte- oder Kanalobjekt
	 *  \param list Nummer der HomeMatic-Parameterliste, in der der Wert definiert ist
	 *  \param by_pos Adresse in Bytes des zu ermittelnden Wertes
	 */
	bool DetermineValue(RFLogicalInstance* inst, int list, unsigned char by_pos);
	//! Prüft, ob die Parameter einer bestimmten HomeMatic-Parameterliste bereits vom Gerät gelesen wurden
	bool MustBeRead(int list);
	//! Setzt den Verknüpfungspartner, für den die Konfigurationsdaten verwaltet werden
	/*! Sind beide Parameter \c 0, so werden Konfigurationsdaten für das Gerät oder den Kanal verwaltet.
	 */
	inline void SetPeer(int address, int channel){
		peer_address=address;
		peer_channel=channel;
	};
	//! Speichert die Konfigurationsdaten mit der Verknüpfungspartneradresse und allen Chunks in eine XML-Datei
	bool SaveToXml(XMLNode* node);
	//! Liest die Konfigurationsdaten mit der Verknüpfungspartneradresse und allen Chunks aus einer XML-Datei
	bool LoadFromXml(XMLNode& node);
	//! Prüft, ob ein bestimmtes Byte aus einer bestimmten HomeMatic-Parameterliste noch an das Gerät gesendet werden muss
	bool IsDevDirty(int list, unsigned char index);
	//! Prüft, ob noch irgendetwas das Gerät gesendet werden muss
	bool IsDevDirty();
	//! Markiert alle vorhandenen Chunks in allen vorhandenen Listen als noch an das Gerät zu senden
	void SetDevDirty();
	//! Prüft, ob sich die Konfigurationsdaten auf einen bestimmten Verknüpfungspartner beziehen
	inline bool IsForPeer(int address, int channel){
		return (address==peer_address) && (channel<0 || channel==peer_channel);
	}
	//! Verarbeitet eine vom Gerät empfangene asynchrone Parameter-Änderungs-Mitteilungsnachricht
	bool ProcessAsyncParamInfo(BidcosFrame& frame);
	//! Methode zum Zurücksetzen des Ungültig-Flags
	/*! Das Ungültig-Flag wird automatisch gesetzt, wenn das Gerät eine negative Bestätigung "Target Invalid"
	 *  schickt, also den Verknüpfungspartner, auf den sich die Konfigurationsdaten beziehen, nicht kennt.
	 *  So wird verhindert, dass dann immer wieder versucht wird, das Gerät anzusprechen.
	 *
	 *  Wird dann die entsprechende Verknüpfung doch noch angelegt, werden mit dieser Methode die 
	 *  Konfigurationsdaten wieder gültig gesetzt.
	 */
	inline void SetValid(){invalid=false;};
protected:
	//! Hilfsmethode zum Senden von Konfigurationsdaten an das Gerät
	/*! 
	 *  \param inst Geräte- oder Kanalobjekt
	 *  \param list Nummer der HomeMatic-Parameterliste, die übertragen werden soll
	 *  \param data Zu übertragende Daten. Immer abwechselnd Index, Wert.
	 */
	bool SendConfigData(RFLogicalInstance* inst, int list, const std::vector<unsigned char>& data);
	//! Liest ein einzelnes Byte aus den Konfigurationsdaten
	unsigned char GetByteData(int list, unsigned char index);
	//! Schreibt ein einzelnes Byte unter Setzen der "dirty"-Bits
	void SetByteData(int list, unsigned char index, unsigned char value, bool dev_dirty, bool file_dirty);
	//! Typedef für die Map Chunknummer -> Chunk zur Speicherung der Chunks einer Parameterliste
	typedef std::map<int, Chunk> chunks_t;
	//! Typedef für die Map Listennumer -> Chunk-Map zur Speicherung der Parameterlisten zu einem Parametersatz
	typedef std::map<int, chunks_t> lists_t;
	//! In diesem Objekt werden die Daten gespeichert
	lists_t lists;
	//! Adresse des Verknüpfungspartners oder \c 0 für Geräte- oder Kanalparameter
	int peer_address;
	//! Kanalnummer des Verknüpfungspartners oder \c 0 für Geräte- oder Kanalparameter
	int peer_channel;
	//! Ungültig-Flag, gesetzt, wenn der Verknüpfungspartner dem Gerät nicht bekannt ist
	bool invalid;
	bool initDefault;
};
#endif //_RF_CONFIG_DATA_H_

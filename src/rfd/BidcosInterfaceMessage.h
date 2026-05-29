/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _BIDCOS_INTERFACE_MESSAGE_H_
#define _BIDCOS_INTERFACE_MESSAGE_H_

#include <string>
#include <vector>
#include "BidcosFrame.h"

//! Diese Klasse kapselt die Nachrichten, die mit intelligenten Interfaces ausgetauscht werden.
/*!
 *  Die Klasse unterstützt zwei verschiedene Codierungen für die Nachrichten:
 *  - Eine lesbare ASCII-Codierung, wie sie vom Lan-Interface verwendet wird
 *  - Eine kompakte binäre Codierung, wie sie vom USB-Interface verwendet wird
 *
 *  Die Klasse kennt alle möglichen Nachrichten und deren Parametertypen.
 */
class BidcosInterfaceMessage
{
public:
    //! Typedef für Binärdaten
    typedef std::basic_string<unsigned char> BinaryType;
    //! Konstruktor
    BidcosInterfaceMessage();
    //! Destruktor
    ~BidcosInterfaceMessage();
    //! Initialisiert das Objekt aus einer ASCII-codierten Nachricht
    /*!
     *  \param s ASCII-Darstellung der Nachricht wie vom Lan-Interface geliefert
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
    bool FromString(const std::string& s);
    //! Erzeugt aus dem Objekt eine ASCII-codierte Nachricht
    /*!
     *  \return ASCII-Darstellung der Nachricht wie vom Lan-Interface benötigt. Im Fehlerfall wird der Leerstring
     *          zurückgeliefert.
     */
    std::string ToString();

    //! Initialisiert das Objekt aus einer binär codierten Nachricht
    /*!
     *  \param s Binärdarstellung der Nachricht wie vom USB-Interface geliefert
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
    bool FromBinary(const BinaryType& b);
    //! Erzeugt aus dem Objekt eine binär codierte Nachricht
    /*!
     *  \return Binärdarstellung der Nachricht wie vom Lan-Interface benötigt. Im Fehlerfall wird ein leerer
     *          BinaryType zurückgeliefert.
     */
    BinaryType ToBinary();

    //! Gibt einen Nachrichtenparameter als vorzeichenbehafteter Integer zurück
    /*!
     *  \param index Parameterindex, 0 für den ersten Parameter
     */
    int GetSIntParam(unsigned int index);
    //! Gibt einen Nachrichtenparameter als vorzeichenloser Integer zurück
    /*!
     *  \param index Parameterindex, 0 für den ersten Parameter
     */
    unsigned int GetUIntParam(unsigned int index);
    //! Gibt einen Nachrichtenparameter als String zurück
    /*!
     *  \param index Parameterindex, 0 für den ersten Parameter
     */
    std::string GetStringParam(unsigned int index);
    //! Gibt einen Nachrichtenparameter als Bidcos-Rahmen zurück
    /*!
     *  \param index Parameterindex, 0 für den ersten Parameter
     */
    BidcosFrame GetFrameParam(unsigned int index);
    //! Gibt einen Nachrichtenparameter als Binärdaten zurück
    /*!
     *  \param index Parameterindex, 0 für den ersten Parameter
     */
    BinaryType GetBinaryParam(unsigned int index);

    //! Setzt einen Nachrichtenparameter als vorzeichenbehafteter Integer 
    /*!
     *  \param index Parameterindex, 0 für den ersten Parameter
     *  \param value zu setzender Wert
     */
    bool PutUIntParam( unsigned int index, unsigned int value);
    //! Setzt einen Nachrichtenparameter als vorzeichenloser Integer 
    /*!
     *  \param index Parameterindex, 0 für den ersten Parameter
     *  \param value zu setzender Wert
     */
    bool PutSIntParam( unsigned int index, int value);
    //! Setzt einen Nachrichtenparameter als String
    /*!
     *  \param index Parameterindex, 0 für den ersten Parameter
     *  \param value zu setzender Wert
     */
    bool PutStringParam( unsigned int index, const std::string& value);
    //! Setzt einen Nachrichtenparameter als Bidcos-Rahmen
    /*!
     *  \param index Parameterindex, 0 für den ersten Parameter
     *  \param frame zu setzender Wert
     */
    bool PutFrameParam( unsigned int index, const StructuredFrame* frame);
    //! Setzt einen Nachrichtenparameter als Binärdaten aus einem String
    /*!
     *  \param index Parameterindex, 0 für den ersten Parameter
     *  \param s zu setzender Wert
     */
    bool PutBinaryParam( unsigned int index, const std::string& s);
    //! Setzt einen Nachrichtenparameter als Binärdaten
    /*!
     *  \param index Parameterindex, 0 für den ersten Parameter
     *  \param b zu setzender Wert
     */
    bool PutBinaryParam( unsigned int index, const BinaryType& b);

    //! Liefert die Anzahl der Parameter zurück
    unsigned int GetParamCount();

    //! Liefert den Nachrichten-Opcode zurück
    int GetCommand();

    //! setzt den Nachrichten-Opcode
    bool SetCommand(int cmd);

    //! Aufzählung der möglichen Opcodes
    enum
    {
        CMD_HELLO = 'H',
        CMD_DEVICE = 'D',
        CMD_RESPONSE = 'R',
        CMD_EVENT = 'E',
        CMD_SEND = 'S',
        CMD_ADD_DEVICE = '+',
        CMD_DELETE_DEVICE = '-',
        CMD_CLEAR_DEVICES = 'C',
        CMD_LIST_DEVICES = 'L',
        CMD_TIME = 'T',
        CMD_SET_ADDRESS = 'A',
        CMD_KEEPALIVE = 'K',
        CMD_GET_KEY_INDEXES = 'i',
        CMD_KEY_INDEXES = 'I',
        CMD_SET_KEY = 'Y',
        CMD_SET_IV = 'V',
		CMD_DUMP = 'D',
		CMD_DUMP_RESULT = 'd'
    };

    //! Parameter-Indizes für ausgewählte Nachrichten
    enum
    {
        PARAM_INDEX_HELLO_PRODUCT        = 0,
        PARAM_INDEX_HELLO_FWVERS         = 1,
        PARAM_INDEX_HELLO_SERIAL         = 2,
        PARAM_INDEX_HELLO_NATIVE_ADDRESS = 3,
        PARAM_INDEX_HELLO_GIVEN_ADDRESS  = 4,
        PARAM_INDEX_HELLO_TIMER          = 5,
        PARAM_INDEX_HELLO_DEVCOUNT       = 6,
		PARAM_INDEX_HELLO_DUTYCYCLE      = 7
    };


private:
    //! Vektor zur internen Speicherung der Parameter
    std::vector<BinaryType> vec_params;
    //! Statisches Array, dass die möglichen Nachrichten beschreibt
    static const struct MessageDefinition{
        unsigned char opcode; //!< Opcode einer möglichen Nachricht
        unsigned char min_param_count; //!< minimale Anzahl der Parameter
        unsigned char max_param_count; //!< maximale Anzahl der Parameter
        //! Jedes Zeichen dieses Strings beschreibt den Typen eines Nachrichtenparameters
        /*!
         *  '1' - '4' : Integer der Länge 1 bis 4 Bytes
         *  's' : String
         *  'b' : Binärdaten oder Bidcos-Rahmen
         */
        const char* fields;
    } s_message_definitions[];
    //! Zeiger auf die Beschreibung der aktuellen Nachricht. Null bedeutet "ungültige Nachricht".
    const struct MessageDefinition* msg_def;
    //! Setzt die Beschreibung anhand des Opcodes
    /*!
     *  \param cmd Opcode der Nachricht
     *  \return \c true im Erfolgsfall, \c false im Fehlerfall
     */
    bool GetMessageDefinition(int cmd);
};
#endif

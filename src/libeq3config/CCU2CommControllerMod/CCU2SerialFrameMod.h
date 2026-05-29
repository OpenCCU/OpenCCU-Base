/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _CCU2SERIALFRAMEMOD_H_
#define _CCU2SERIALFRAMEMOD_H_

#include<string>

namespace HM2Mod {

/** \brief Represents the data frame used to communicate with the CCU2 co-processor.
* \details The frame consists of a start character, two length bytes, the payload and two crc bytes.
*/
class CCU2SerialFrameMod {
public:

        /** \brief First character of serial frame.*/
        static const char frameStartChar = (char)0xfd;


       /** \brief CTOR: Constructs a fresh frame with empty payload.*/
       CCU2SerialFrameMod();
       /** \brief CTOR: Constructs a serial frame from raw frame data. (e.g. used for responses.
       *\param serialFrameData String with complete serial frame data.
       */
       //CCU2SerialFrame(const std::string& serialFrameData);
       
       /**\brief Destructor.*/
       virtual ~CCU2SerialFrameMod();
              
       /** \brief Returns the payload.
       * \return Payload as string.
       */
       const std::string& getPayload() const;

       /** \brief Sets the payload 
       * \param payload Payload. 
       */
       void setPayload(const std::string& payload);

       std::string getFrameData() const;
       
       

	   /** \brief Adds data to payload.
	   * \param frameData Data to append to payload.
	   * \return True if msg is complete, otherwise false.
	   */
	   bool addFrameData(const std::string& frameData, std::string& leftOver);

	   void reset();
private:
		static const char escapeChar = (char)0xfc;

		static const unsigned short CRC16_POLY = (unsigned short)0x8005;
        
        /** \brief Payload data.
        * \details Payload of this frame is the application frame.
        */
        std::string payload;

		int expectedMsgSize;
		bool deEscapePending;

        /** Assembles the frame. */
        std::string assembleFrame() const;  
        
        /** \brief Converts an int to string.
        * \param i int to convert.
        * \return Digits of i as string.
        */
        std::string toString(const int i);   
        
        /** \brief Calculates CRC16 checksum for a string.
        * \param msg String to calculate CRC for.
        * \param offset First index (starting at 0) in string.
        * \param length Length beginning from offset.
        * \param outHighByte Most significant byte of crc.
        * \param outLowByte Least significant byte of crc.
        */
        static void calculateCRC(const char* msg, const unsigned int offset, const unsigned int length, char& outHighByte, char& outLowByte);  
       
		/** \brief Updates current crc16.
		* \param crc_req Current crc16 value
		* \param Val Next byte to add.
		*/
		static unsigned short crc16_update(unsigned short crc_reg, unsigned char Val );
       
		static std::string escape(const std::string& data);
		//std::string deEscape(const std::string& data);
		inline void deEscapeChar(char* c);

		std::string extractPayload(const std::string& serialFrameData);
};

} //namespace HM2

#endif

/*
* Copyright 2025 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#ifndef _HMWLGWTRANSPORTFRAME_H_
#define _HMWLGWTRANSPORTFRAME_H_

#include <string>

/** \brief Frame to encapsulate data for communication from/to HM Wired LGW.
*/
class HMWLGWTransportFrame
{
	public:
	        /** \brief First character of serial frame.*/
        static const char frameStartChar = (char)0xfd;

		/** Contructor*/
		HMWLGWTransportFrame();

		/** Destructor.*/
		virtual ~HMWLGWTransportFrame();

		/** \brief Sets the payload (user data) for this frame.
		 * \param payload User data encapsulated by this frame.*/
		void setPayload(const std::string& payload);

		/** \brief Returns the payload of this frame.
		 * \return Payload of this frame.*/
		std::string getPayload();

		bool addIncomingFrameData(const std::string& frameData, std::string& leftOver);

		/** \brief Assembles and returns frame data to send to HMW-LGW.
		 * \return frame data ready to send to HM-LGW.*/
		std::string getFrameData();

	private:
		static const char escapeChar = (char)0xfc;

		/**brief User data encapsulated by this frame.*/
		std::string payload;

		/** \brief Excpected size of incoming message.
		 * \details Used internally by addIncomingFrameData() method.*/
		int expectedMsgSize;

		/**Flag used internally by deEscapeChar() method.*/
		bool deEscapePending;

		/** \brief Escapes a string.
		 * \details Escapes a string to prepare it to send. */
		std::string escape(const std::string& data);

		/** \brief Deescapes a character.*/
		inline void deEscapeChar(char* c);

};



#endif

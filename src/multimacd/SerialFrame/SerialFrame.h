/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#pragma once
#include <stdint.h>
#include "../BinaryData.h"
#include "../tinythread.h"
#include "../Dynamic.h"
#include "../Crc16.h"

class Subsystem;

class SerialFrame
{
public:

	enum SubsystemType 
	{
		FrameSubsystemType_Bootloader,
		FrameSubsystemType_TrxAdapter,
		FrameSubsystemType_HmIpStack,
		FrameSubsystemType_LowLevelMac,
		FrameSubsystemType_LowLevelMacIRx,	
		FrameSubsystemType_LowLevelMacITx,
		FrameSubsystemType_Router = 0x0a,
		FrameSubsystemType_Backbone = 0x0c,
		FrameSubsystemType_Internal = 0xf0,
		FrameSubsystemType_Common = 0xfe,
		FrameSubsystemType_Unknown = 0xff,
	};

	SerialFrame(void);
	SerialFrame( uint8_t subsystemType, uint8_t command, int sequenceCounter = -1 );

	virtual ~SerialFrame(void);
	/// <summary>
	/// Parses the binary on-wire representation of a serial frame into this object.
	/// </summary>
	///
	/// <param name="buffer">
	/// The binary data buffer.
	/// </param>
	/// <param name="length">
	/// The length of data.
	/// </param>
	/// <param name="pNumEaten">
	/// [out] The number of bytes consumed.
	/// </param>
	///
	/// <returns>
	/// true if it succeeds, false if it fails.
	/// </returns>

	bool Parse( const unsigned char* buffer, size_t length, size_t* pNumEaten );


	/// <summary>
	/// Encodes this object into the on-wire binary representation.
	/// </summary>
	///
	/// <param name="buffer">
	/// The buffer receiving the binary data.
	/// </param>
	/// <param name="maxLength">
	/// Length of the buffer.
	/// </param>
	///
	/// <returns>
	/// number of bytes put into buffer if it succeeds, 0 if it fails.
	/// </returns>

	size_t GetRawData( unsigned char* buffer, size_t maxLength )const;

	static SerialFrame* Create( const unsigned char* buffer, size_t length, size_t* pNumEaten );

	SerialFrame* Clone()const;

	bool CheckType( uint8_t subsystemType, uint8_t command )const;

	uint16_t GetQualifiedCommand()const;

	uint8_t GetCommand()const;

	void SetCommand( uint8_t command );

	uint8_t GetSubsystem()const;

	uint8_t GetSequenceCounter()const;

	void SetSequenceCounter( uint8_t sequenceCounter );

	bool IsSequenceCounterValid()const;

	uint32_t GetId() const;

	void SetId( uint32_t id );

	Subsystem* GetResponsibleSubsystem()const;
	void SetResponsibleSubsystem( Subsystem* subsystem );

	BinaryData& Data();
	const BinaryData& Data()const;
	virtual bool isResponseFrame();

	virtual std::string ToString()const;
	enum {
		EscapeChar = 0xfc,
		StartChar = 0xfd,
	};

protected:
	static bool Parse( const unsigned char* buffer, size_t length, size_t* pNumEaten, uint8_t* sequenceCounter, uint8_t* subsystemType, uint8_t* command, BinaryData* data );
	virtual const char* GetSubsystemName()const;
	virtual std::string GetCommandName()const;


private:
	enum {
		Crc16Poly = 0x8005,
	};

	static bool AddEscapedByte( unsigned char* buffer, size_t& off, size_t maxLength, uint8_t b, Crc16* crc = NULL );
	static bool GetUnescapedByte( const unsigned char* buffer, size_t& off, size_t& numEaten, size_t length, uint8_t* b, Crc16* crc = NULL );

	static uint32_t GetNextId();

	uint8_t _subsystemType;
	int _sequenceCounter;
	uint8_t _command;
	BinaryData _data;
	uint32_t _id;

	static tthread::mutex s_mutex;
	typedef tthread::lock_guard<tthread::mutex> LockGuard;
	static uint32_t s_nextId;
	static uint32_t s_instanceCount;

	Subsystem* _responsibleSubsystem;

};


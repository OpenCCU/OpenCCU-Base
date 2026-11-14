/*
* Copyright 2026 eQ-3 AG - All Rights Reserved.
* 
* Licensed under the HMSL 2 (the "License"). You may not use
* this file except in compliance with the License.  You can obtain a copy
* in the file HMSL.txt in the source distribution.
*/

#include "SerialFrame.h"
#include <Logger.h>
#include <stdio.h>

/*static*/ tthread::mutex SerialFrame::s_mutex;
/*static*/ uint32_t SerialFrame::s_nextId = 1;
/*static*/ uint32_t SerialFrame::s_instanceCount = 0;

#define LOG_CREATION 0

SerialFrame::SerialFrame(void)
{
	_id = 0;
	_subsystemType = 0;
	_command = 0;
	_sequenceCounter = -1;
	_responsibleSubsystem = NULL;
#if LOG_CREATION
	uint32_t instanceCount;
	{
		LockGuard lock( s_mutex );
		instanceCount = ++s_instanceCount;
	}
	LOG( Logger::LOG_DEBUG, "SerialFrame create %p cnt=%u", this, instanceCount );
#endif
}

SerialFrame::SerialFrame( uint8_t subsystemType, uint8_t command, int sequenceCounter /*= -1*/ )
{
	_id = 0;
	_subsystemType = subsystemType;
	_command = command;
	_sequenceCounter = sequenceCounter;
	_responsibleSubsystem = NULL;
#if LOG_CREATION
	uint32_t instanceCount;
	{
		LockGuard lock( s_mutex );
		instanceCount = ++s_instanceCount;
	}
	LOG( Logger::LOG_DEBUG, "SerialFrame create %p cnt=%u", this, instanceCount );
#endif
}


SerialFrame::~SerialFrame(void)
{
#if LOG_CREATION
	uint32_t instanceCount;
	{
		LockGuard lock( s_mutex );
		instanceCount = --s_instanceCount;
	}
	LOG( Logger::LOG_DEBUG, "SerialFrame delete %p cnt=%u", this, instanceCount );
#endif
}

/*static*/ uint32_t SerialFrame::GetNextId()
{
	LockGuard lock( s_mutex );
	uint32_t id = s_nextId++;
	if( !s_nextId )
	{
		s_nextId++;
	}
	return id;
}


/*static*/ bool SerialFrame::AddEscapedByte( unsigned char* buffer, size_t& off, size_t maxLength, uint8_t b, Crc16* crc /*= NULL*/)
{
	if( (b == EscapeChar) || (b == StartChar) )
	{
		if( maxLength < off + 2 )
		{
			return false;
		}
		buffer[off++] = EscapeChar;
		buffer[off++] = b & 0x7f;
	}else{
		if( maxLength < off + 1 )
		{
			return false;
		}
		buffer[off++] = b;
	}

	if( crc )
	{
		crc->Update( b );
	}
	return true;
}

/*static*/ bool SerialFrame::GetUnescapedByte( const unsigned char* buffer, size_t& off, size_t& numEaten, size_t length, uint8_t* b, Crc16* crc /*= NULL*/ )
{
	if( length <= off )
	{
		return false;
	}
	uint8_t c = buffer[off];
	if( c == StartChar )
	{
		LOG( Logger::LOG_DEBUG, "Unexpected start of frame after %d bytes", off );
		numEaten = off;
		return false;
	}
	off++;
	if( c == EscapeChar )
	{
		if( length <= off )
		{
			return false;
		}
		c = buffer[off++] | 0x80;
	}
	*b = c;
	if( crc )
	{
		crc->Update( c );
	}
	return true;
}

/*static*/ bool SerialFrame::Parse( const unsigned char* buffer, size_t length, size_t* pNumEaten, uint8_t* sequenceCounter, uint8_t* subsystemType, uint8_t* command, BinaryData* data )
{
	//LOG( Logger::LOG_DEBUG, "SerialFrame::Parse(%s)", BinaryData( buffer, length ).ToString().c_str() );
	size_t numEaten = 0;
	size_t off = 0;
	bool success = false;
	uint8_t b;
	uint16_t dataLength;

	Crc16 crc( Crc16Poly );

	if( length < 1 )
	{
		goto end;
	}
	while( (length - off >= 1) &&  buffer[off] != StartChar )
	{
		off++;
	}
	numEaten = off;
	if( buffer[off] != StartChar )
	{
		numEaten = ++off;
		LOG( Logger::LOG_DEBUG, "Discarded %d bytes", off );
		goto end;
	}
	off++;

	if( length - off < 5 )
	{
		goto end;
	}
	if( !GetUnescapedByte( buffer, off, numEaten, length, &b, &crc ) )
	{
		goto end;
	}
	dataLength = b << 8;
	if( !GetUnescapedByte( buffer, off, numEaten, length, &b, &crc ) )
	{
		goto end;
	}
	dataLength |= b;
	if( dataLength < 3 )
	{
		LOG(Logger::LOG_DEBUG, "SerialFrame: Expected msg size too short (corrupt data?).");
		numEaten = off;//fixes endless loop i.e. if dataLength < 3 on corrupt data
		goto end;
	}
	if( !GetUnescapedByte( buffer, off, numEaten, length, subsystemType, &crc ) )
	{
		goto end;
	}
	if( !GetUnescapedByte( buffer, off, numEaten, length, &b, &crc ) )
	{
		goto end;
	}
	*sequenceCounter = b;
	if( !GetUnescapedByte( buffer, off, numEaten, length, command, &crc ) )
	{
		goto end;
	}
	data->resize(dataLength - 3);
	for( uint16_t i=0; i<dataLength-3; i++ )
	{
		if( !GetUnescapedByte( buffer, off, numEaten, length, &b, &crc ) )
		{
			goto end;
		}
		(*data)[i] = b;
	}
	//crc
	if( !GetUnescapedByte( buffer, off, numEaten, length, &b, &crc ) )
	{
		goto end;
	}
	if( !GetUnescapedByte( buffer, off, numEaten, length, &b, &crc ) )
	{
		goto end;
	}
	if( crc.Result() != 0 )
	{
		//CRC error
		LOG( Logger::LOG_DEBUG, "CRC error. Calculated 0x%04X.", crc.Result() );
		numEaten = off;
		goto end;
	}
	success = true;
	numEaten = off;
end:
	if( (!numEaten) && (!success) )
	{
		//LOG( Logger::LOG_DEBUG, "Premature end after %d bytes", off );
	}
	*pNumEaten = numEaten;
	return success;

}

bool SerialFrame::Parse( const unsigned char* buffer, size_t length, size_t* pNumEaten )
{
	size_t numEaten = 0;
	size_t off = 0;
	bool success = false;
	uint8_t b;
	uint16_t dataLength;

	Crc16 crc( Crc16Poly );

	if( length < 1 )
	{
		goto end;
	}
	while( (length - off >= 1) &&  buffer[off] != StartChar )
	{
		off++;
	}
	numEaten = off;
	if( buffer[off] != StartChar )
	{
		numEaten = ++off;
		LOG( Logger::LOG_DEBUG, "Discarded %d bytes", off );
		goto end;
	}
	off++;

	if( length - off < 5 )
	{
		goto end;
	}
	if( !GetUnescapedByte( buffer, off, numEaten, length, &b, &crc ) )
	{
		goto end;
	}
	dataLength = b << 8;
	if( !GetUnescapedByte( buffer, off, numEaten, length, &b, &crc ) )
	{
		goto end;
	}
	dataLength |= b;
	if( dataLength < 3 )
	{
		goto end;
	}
	if( !GetUnescapedByte( buffer, off, numEaten, length, &_subsystemType, &crc ) )
	{
		goto end;
	}
	if( !GetUnescapedByte( buffer, off, numEaten, length, &b, &crc ) )
	{
		goto end;
	}
	_sequenceCounter = b;
	if( !GetUnescapedByte( buffer, off, numEaten, length, &_command, &crc ) )
	{
		goto end;
	}
	_data.resize(dataLength - 3);
	for( uint16_t i=0; i<dataLength-3; i++ )
	{
		if( !GetUnescapedByte( buffer, off, numEaten, length, &b, &crc ) )
		{
			goto end;
		}
		_data[i] = b;
	}
	//crc
	if( !GetUnescapedByte( buffer, off, numEaten, length, &b, &crc ) )
	{
		goto end;
	}
	if( !GetUnescapedByte( buffer, off, numEaten, length, &b, &crc ) )
	{
		goto end;
	}
	if( crc.Result() != 0 )
	{
		//CRC error
		LOG( Logger::LOG_DEBUG, "CRC error. Calculated 0x%04X.", crc.Result() );
		numEaten = off;
		goto end;
	}
	success = true;
	numEaten = off;
end:
	if( (!numEaten) && (!success) )
	{
		//		LOG( Logger::LOG_DEBUG, "Premature end after %d bytes", off );
	}
	*pNumEaten = numEaten;
	return success;
}

size_t SerialFrame::GetRawData( unsigned char* buffer, size_t maxLength )const
{
	size_t off = 0;
	if( maxLength - off < 5 )
	{
		return 0;
	}
	buffer[off++] = StartChar;
	uint16_t length = _data.size() + 3;

	Crc16 crc( Crc16Poly );
	if( !AddEscapedByte( buffer, off, maxLength, length >> 8, &crc ) )
	{
		return 0;
	}
	if( !AddEscapedByte( buffer, off, maxLength, length & 0xff, &crc ) )
	{
		return 0;
	}
	if( !AddEscapedByte( buffer, off, maxLength, _subsystemType, &crc ) )
	{
		return 0;
	}
	if( !AddEscapedByte( buffer, off, maxLength, _sequenceCounter & 0xff, &crc ) )
	{
		return 0;
	}
	if( !AddEscapedByte( buffer, off, maxLength, _command, &crc ) )
	{
		return 0;
	}
	for( uint16_t i=0; i<_data.size(); i++ )
	{
		if( !AddEscapedByte( buffer, off, maxLength, _data[i], &crc ) )
		{
			return 0;
		}
	}
	if( !AddEscapedByte( buffer, off, maxLength, crc.Result() >> 8 ) )
	{
		return 0;
	}
	if( !AddEscapedByte( buffer, off, maxLength, crc.Result() & 0xff ) )
	{
		return 0;
	}
	return off;
}

/*static*/ SerialFrame* SerialFrame::Create( const unsigned char* buffer, size_t length, size_t* pNumEaten )
{
	size_t numEaten = 0;
	while( length && (*buffer != StartChar) )
	{
		numEaten++;
		buffer++;
		length--;
	}
	if( numEaten )
	{
		*pNumEaten = numEaten;
		return NULL;
	}
	uint8_t sequenceCounter;
	uint8_t subsystemType;
	uint8_t command;
	BinaryData data;
	if( !Parse( buffer, length, pNumEaten, &sequenceCounter, &subsystemType, &command, &data ) )
	{
		return NULL;
	}
	SerialFrame* frame = Dynamic::Order<SerialFrame>::Create( (subsystemType << 8) | command );
	if( !frame )
	{
		frame = new SerialFrame( subsystemType, command, sequenceCounter );
	}else{
		frame->_sequenceCounter = sequenceCounter;
	}
	frame->_data = data;
	return frame;
}

SerialFrame* SerialFrame::Clone()const
{
	SerialFrame* clone = Dynamic::Order<SerialFrame>::Create( (GetSubsystem() << 8) | GetCommand() );
	if( !clone )
	{
		clone = new SerialFrame();
	}
	((SerialFrame&)*clone) = *this;
	return clone;
}

bool SerialFrame::CheckType( uint8_t subsystemType, uint8_t command )const
{
	return _subsystemType == subsystemType && _command == command;
}

uint32_t SerialFrame::GetId() const
{
	if( !_id )
	{
		const_cast<uint32_t&>(_id) = GetNextId();
	}
	return _id;
}

void SerialFrame::SetId( uint32_t id )
{
	if( !_id )
	{
		_id = id;
	}
}


uint16_t SerialFrame::GetQualifiedCommand()const
{
	return (_subsystemType << 8) | _command;
}

uint8_t SerialFrame::GetCommand()const
{
	return _command;
}

void SerialFrame::SetCommand( uint8_t command )
{
	_command = command;
}

uint8_t SerialFrame::GetSubsystem()const
{
	return _subsystemType;
}

uint8_t SerialFrame::GetSequenceCounter()const
{
	return _sequenceCounter & 0xff;
}

void SerialFrame::SetSequenceCounter( uint8_t sequenceCounter )
{
	_sequenceCounter = sequenceCounter;
}

bool SerialFrame::IsSequenceCounterValid()const
{
	return _sequenceCounter >= 0;
}

Subsystem* SerialFrame::GetResponsibleSubsystem()const
{
	return _responsibleSubsystem;
}

void SerialFrame::SetResponsibleSubsystem( Subsystem* subsystem )
{
	_responsibleSubsystem = subsystem;
}

BinaryData& SerialFrame::Data()
{
	return _data;
}

const BinaryData& SerialFrame::Data()const
{
	return _data;
}

const char* SerialFrame::GetSubsystemName()const
{
	switch( _subsystemType )
	{
	case FrameSubsystemType_Bootloader:
		return "BOOTLOADER";
	case FrameSubsystemType_Common:
		return "COMMON";
	case FrameSubsystemType_HmIpStack:
		return "HMIP";
	case FrameSubsystemType_Internal:
		return "INTERNAL";
	case FrameSubsystemType_LowLevelMac:
		return "LLMAC";
	case FrameSubsystemType_LowLevelMacIRx:
		return "LLiRX";
	case FrameSubsystemType_LowLevelMacITx:
		return "LLiTX";
	case FrameSubsystemType_TrxAdapter:
		return "TRX";
	case FrameSubsystemType_Router:
		return "ROUTER";
	case FrameSubsystemType_Backbone:
		return "BACKBONE";
	}
	return NULL;
}

std::string SerialFrame::GetCommandName()const
{
	char buffer[16];
	snprintf( buffer, sizeof(buffer), "CMD=%02X ", _command );
	return buffer;
}

std::string SerialFrame::ToString()const
{
	char buffer[32];
	snprintf( buffer, sizeof(buffer), "#%d ", _sequenceCounter );
	std::string s = buffer;
	const char* subsystemName = GetSubsystemName();
	if( subsystemName )
	{
		s += subsystemName;
	}else{
		snprintf(buffer, sizeof(buffer), "SUBSYS=%d", _subsystemType);
	}
	s += " " + GetCommandName() + " ";
	s += _data.ToString();
	return s;
}

bool SerialFrame::isResponseFrame() {
	return false;
}

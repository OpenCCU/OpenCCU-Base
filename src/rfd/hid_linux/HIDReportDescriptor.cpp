#include "HIDReportDescriptor.h"
#include <Logger.h>
#include <algorithm>

static unsigned char size_to_bytes(unsigned char s)
{
	if( s <= 2 )return s;
	return 4;
}

static uint32_t le_to_cpu( int count, unsigned char* data )
{
	int32_t retval = 0;
	for( int i=0; i<count; i++ )retval += data[i]>>(i*8);
	return retval;
}

CHIDReportDescriptor::CHIDReportDescriptor()
{
	_reportsHaveIdPrefix = false;
	_maxInputReportSize = 0;
	_maxOutputReportSize = 0;
	_maxFeatureReportSize = 0;
}

bool CHIDReportDescriptor::Init( libusb_device_handle* devHandle )
{
	unsigned char hidDescriptor[32];
	
	int hidDescriptorLen = libusb_control_transfer(devHandle, LIBUSB_ENDPOINT_IN + 1,
	                LIBUSB_REQUEST_GET_DESCRIPTOR, (LIBUSB_DT_HID << 8) | 0, 0,
			hidDescriptor, sizeof(hidDescriptor), 1000 );
	
	if( hidDescriptorLen < 0 )
	{
		LOG( Logger::LOG_ERROR, "Error reading HID descriptor %d", hidDescriptorLen );
		return false;
	}
	if( hidDescriptorLen < REPORT_DESCRIPTOR_SIZE_INDEX + REPORT_DESCRIPTOR_SIZE_LEN )
	{
		LOG( Logger::LOG_ERROR, "HID descriptor too short (%d bytes)", hidDescriptorLen );
		return false;
	}
	int reportDescriptorLen = le_to_cpu( 2, hidDescriptor + REPORT_DESCRIPTOR_SIZE_INDEX );
	
	unsigned char* reportDescriptor = new unsigned char[reportDescriptorLen];
	
	int retval = libusb_control_transfer(devHandle, LIBUSB_ENDPOINT_IN + 1,
	                LIBUSB_REQUEST_GET_DESCRIPTOR, (LIBUSB_DT_REPORT << 8) | 0, 0,
			reportDescriptor, reportDescriptorLen, 1000 );
	if( retval != reportDescriptorLen )
	{
		LOG( Logger::LOG_ERROR, "Error reading report descriptor %d", retval );
		delete[] reportDescriptor;
		return false;
	}

	bool success = ParseReportDescriptor( reportDescriptor, reportDescriptorLen );
	delete[] reportDescriptor;
	return success;
	
}

bool CHIDReportDescriptor::ParseReportDescriptor( unsigned char* rdesc_buf, int rdesc_size )
{
	//uint32_t    usage_page  = 0;
	//uint32_t    report_id   = 0;
	uint32_t    report_count = 0;
	uint32_t    report_size = 0;
	uint8_t    *p;

	for (p = rdesc_buf; p - rdesc_buf < rdesc_size;)
	{
		/* See 6.2.2.2 Short Items */
		uint8_t pfx     = *p++;
		uint8_t size    = pfx & 0x3;
		uint8_t bytes   = size_to_bytes(size);
		uint8_t type    = (pfx >> 2) & 0x3;
		uint8_t tag     = pfx >> 4;

		if( p + bytes > rdesc_buf + rdesc_size ){
			LOG( Logger::LOG_ERROR, "Unexpected end of descriptor at %d, tag=0x%02X, type=%d, bytes=%d", p - rdesc_buf, tag, type, bytes );
			return false;
		}
		/* If it's a main item */
		if (type == 0)
		{
			if (tag == 0xA){
				//start collection
			}else if (tag == 0xC){
				//end collection(
			}else if (tag == 0x8){
				//input
				int size = (report_count * report_size + 7) / 8;
				_maxInputReportSize = std::max( _maxInputReportSize, size );
			}else if (tag == 0x9){
				//output
				int size = (report_count * report_size + 7) / 8;
				_maxOutputReportSize = std::max( _maxOutputReportSize, size );
			}else if (tag == 0xB){
				//feature
				int size = (report_count * report_size + 7) / 8;
				_maxFeatureReportSize = std::max( _maxFeatureReportSize, size );
			}
		}
		/* Else, if it's a global item */
		else if (type == 1)
		{
			if (tag == 0x0){
				//usage page
				//usage_page = le_to_cpu(bytes, p);
			}else if (tag == 0x7){
				//report size
				report_size = le_to_cpu(bytes, p);
			}else if (tag == 0x8){
				//report id
				//report_id = le_to_cpu(bytes, p);
				_reportsHaveIdPrefix = true;
			}else if (tag == 0x9){
				//report count
				report_count = le_to_cpu(bytes, p);
			}
		}
		p += bytes;
	} 
	return true;
}

bool CHIDReportDescriptor::ReportsHaveIdPrefix()
{
	return _reportsHaveIdPrefix;
}

unsigned char CHIDReportDescriptor::GetMaxInputReportSize()
{
	return _maxInputReportSize;
}

unsigned char CHIDReportDescriptor::GetMaxOutputReportSize()
{
	return _maxOutputReportSize;
}

unsigned char CHIDReportDescriptor::GetMaxFeatureReportSize()
{
	return _maxFeatureReportSize;
}

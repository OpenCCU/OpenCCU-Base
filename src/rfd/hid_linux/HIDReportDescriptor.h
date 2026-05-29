#ifndef _HIDREPORTDESCRIPTOR_H_
#define _HIDREPORTDESCRIPTOR_H_

#include <libusb.h>

//! Implementiert rudimentäres Parsing des HID-Report-Descriptors
class CHIDReportDescriptor
{
	public:
		CHIDReportDescriptor();
		bool Init( libusb_device_handle* devHandle );
		bool ReportsHaveIdPrefix();
		unsigned char GetMaxInputReportSize();
		unsigned char GetMaxOutputReportSize();
		unsigned char GetMaxFeatureReportSize();
	private:
		bool ParseReportDescriptor( unsigned char* data, int len );
		enum{
			REPORT_DESCRIPTOR_SIZE_INDEX = 7,
			REPORT_DESCRIPTOR_SIZE_LEN = 2,
		};
		bool _reportsHaveIdPrefix;
		int _maxInputReportSize;
		int _maxOutputReportSize;
		int _maxFeatureReportSize;
};
#endif

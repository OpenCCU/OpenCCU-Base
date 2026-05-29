// HIDDevice.h: interface for the CHIDDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HIDDEVICE_H__05F2CE26_6902_4D68_80D5_11F608F4415A__INCLUDED_)
#define AFX_HIDDEVICE_H__05F2CE26_6902_4D68_80D5_11F608F4415A__INCLUDED_

#include <libusb.h>
#include "HIDReportDescriptor.h"

// Return Codes
#define HID_DEVICE_SUCCESS				0x00
#define HID_DEVICE_NOT_FOUND			0x01
#define HID_DEVICE_NOT_OPENED			0x02
#define HID_DEVICE_ALREADY_OPENED		0x03
#define	HID_DEVICE_TRANSFER_TIMEOUT		0x04
#define HID_DEVICE_TRANSFER_FAILED		0x05
#define HID_DEVICE_CANNOT_GET_HID_INFO	0x06
#define HID_DEVICE_HANDLE_ERROR			0x07
#define HID_DEVICE_INVALID_BUFFER_SIZE	0x08
#define HID_DEVICE_SYSTEM_CODE			0x09
#define HID_DEVICE_UNKNOWN_ERROR		0xFF

// Max number of USB Devices allowed
#define MAX_USB_DEVICES	64

#define DEFAULT_REPORT_INPUT_BUFFERS	0

#define MAX_SERIAL_STRING_LENGTH	256

typedef uint32_t DWORD;
typedef uint8_t BYTE;
typedef bool BOOL;
typedef uint16_t WORD;
typedef char* LPSTR;
typedef uint16_t USHORT;

//! Kapselt die Kommunikation mit HID-Geräten
class CHIDDevice
{
public:
	DWORD GetConnectedDeviceNum(WORD vid, WORD pid);
	BYTE GetSerialString(DWORD deviceIndex, WORD vid, WORD pid, LPSTR serialString, DWORD serialStringLength);
	BYTE Open(DWORD deviceIndex, WORD vid, WORD pid, WORD numInputBuffers = DEFAULT_REPORT_INPUT_BUFFERS);
	BOOL IsOpened();

	//BYTE SetFeatureReport(BYTE* buffer, DWORD bufferSize);
	//BYTE GetFeatureReport(BYTE* buffer, DWORD bufferSize);
	BYTE SetReport_Interrupt(BYTE* buffer, DWORD bufferSize, DWORD timeout);
	BYTE GetReport_Interrupt(BYTE* buffer, DWORD bufferSize, WORD numReports, DWORD* bytesReturned, DWORD timeout);
	//BYTE SetReport_Control(BYTE* buffer, DWORD bufferSize);
	//BYTE GetReport_Control(BYTE* buffer, DWORD bufferSize);

    BYTE GetSerialString(LPSTR serialString, DWORD serialStringLength);
    BYTE GetVersion(USHORT* version);

	WORD GetInputReportBufferLength();
	WORD GetOutputReportBufferLength();
	WORD GetFeatureReportBufferLength();
	WORD GetMaxReportRequest();
	BOOL FlushBuffers();

	BYTE Close();

	CHIDDevice();
	virtual ~CHIDDevice();

private:
	enum {
		EndpointOut = 0x02,
		EndpointIn = 0x83,
		InterfaceIndex = 0,
		DefaultReportIndex = 0,
		MaxReportRequest = 1
	};
	
	CHIDReportDescriptor _reportDescriptor;
	void ResetDeviceData();
	bool ReadReportDescriptor();

	libusb_device_handle* m_Handle;

	BOOL m_DeviceOpened;
	libusb_context* m_LibusbContext;

};

#endif // !defined(AFX_HIDDEVICE_H__05F2CE26_6902_4D68_80D5_11F608F4415A__INCLUDED_)

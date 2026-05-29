// HIDDevice.h: interface for the CHIDDevice class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_HIDDEVICE_H__05F2CE26_6902_4D68_80D5_11F608F4415A__INCLUDED_)
#define AFX_HIDDEVICE_H__05F2CE26_6902_4D68_80D5_11F608F4415A__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

// Inlcudes from the DDK -Make sure to also link the libraries
// "setupapi.lib" and "hid.lib" in the project settings. Also, in
// the options for Visual Studio, ensure that you have the laHID_Blinky DDK
// header directories and library directories set. Currently they are:
//
// Headers:		C:\WINDDK\3790.1830\inc\wnet
//				C:\WINDDK\3790.1830\inc\crt
//
// Libraries:	C:\WINDDK\3790.1830\lib\w2k\i386
// PARKER TO DO: add instructions for .net

extern "C" {
#include "hidsdi.h"
}
#include "setupapi.h"
#include "stdlib.h"

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

// Max number of reports that can be requested at time
#define MAX_REPORT_REQUEST_XP	512
#define MAX_REPORT_REQUEST_2K	200

#define DEFAULT_REPORT_INPUT_BUFFERS	0

#define MAX_SERIAL_STRING_LENGTH	256

//! Kapselt die Kommunikation mit HID-Geräten
class CHIDDevice
{
public:
	DWORD GetConnectedDeviceNum(WORD vid, WORD pid);
	BYTE GetSerialString(DWORD deviceIndex, WORD vid, WORD pid, LPSTR serialString, DWORD serialStringLength);
	BYTE Open(DWORD deviceIndex, WORD vid, WORD pid, WORD numInputBuffers = DEFAULT_REPORT_INPUT_BUFFERS);
	BOOL IsOpened();

	BYTE SetFeatureReport(BYTE* buffer, DWORD bufferSize);
	BYTE GetFeatureReport(BYTE* buffer, DWORD bufferSize);
	BYTE SetReport_Interrupt(BYTE* buffer, DWORD bufferSize, DWORD timeout);
	BYTE GetReport_Interrupt(BYTE* buffer, DWORD bufferSize, WORD numReports, DWORD* bytesReturned, DWORD timeout);
	BYTE SetReport_Control(BYTE* buffer, DWORD bufferSize);
	BYTE GetReport_Control(BYTE* buffer, DWORD bufferSize);

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
	void ResetDeviceData();
	BOOL GetHidDevicePath(DWORD index, char* devicePath);
	HANDLE OpenDeviceByVidPid(char* devicePath, WORD vid, WORD pid, DWORD desired_access);

	HANDLE m_Handle;
	BOOL m_DeviceOpened;

	WORD m_InputReportBufferLength;
	WORD m_OutputReportBufferLength;
	WORD m_FeatureReportBufferLength;

	DWORD m_MaxReportRequest;
};

#endif // !defined(AFX_HIDDEVICE_H__05F2CE26_6902_4D68_80D5_11F608F4415A__INCLUDED_)

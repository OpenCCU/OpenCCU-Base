#ifndef HM_RFD_HID_HIDSDI_H
#define HM_RFD_HID_HIDSDI_H

#include <windows.h>
#include "hidpi.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _HIDD_ATTRIBUTES {
	ULONG Size;
	USHORT VendorID;
	USHORT ProductID;
	USHORT VersionNumber;
} HIDD_ATTRIBUTES, *PHIDD_ATTRIBUTES;

BOOLEAN __stdcall HidD_GetAttributes(
	IN HANDLE HidDeviceObject,
	OUT PHIDD_ATTRIBUTES Attributes
);

void __stdcall HidD_GetHidGuid(
	OUT LPGUID HidGuid
);

BOOLEAN __stdcall HidD_GetPreparsedData(
	IN HANDLE HidDeviceObject,
	OUT PHIDP_PREPARSED_DATA* PreparsedData
);

BOOLEAN __stdcall HidD_FreePreparsedData(
	IN PHIDP_PREPARSED_DATA PreparsedData
);

BOOLEAN __stdcall HidD_FlushQueue(
	IN HANDLE HidDeviceObject
);

BOOLEAN __stdcall HidD_GetFeature(
	IN HANDLE HidDeviceObject,
	OUT PVOID ReportBuffer,
	IN ULONG ReportBufferLength
);

BOOLEAN __stdcall HidD_SetFeature(
	IN HANDLE HidDeviceObject,
	IN PVOID ReportBuffer,
	IN ULONG ReportBufferLength
);

BOOLEAN __stdcall HidD_GetInputReport(
	IN HANDLE HidDeviceObject,
	OUT PVOID ReportBuffer,
	IN ULONG ReportBufferLength
);

BOOLEAN __stdcall HidD_SetOutputReport(
	IN HANDLE HidDeviceObject,
	IN PVOID ReportBuffer,
	IN ULONG ReportBufferLength
);

BOOLEAN __stdcall HidD_GetNumInputBuffers(
	IN HANDLE HidDeviceObject,
	OUT PULONG NumberBuffers
);

BOOLEAN __stdcall HidD_SetNumInputBuffers(
	IN HANDLE HidDeviceObject,
	IN ULONG NumberBuffers
);

BOOLEAN __stdcall HidD_GetSerialNumberString(
	IN HANDLE HidDeviceObject,
	OUT PVOID Buffer,
	IN ULONG BufferLength
);

#ifdef __cplusplus
}
#endif

#endif

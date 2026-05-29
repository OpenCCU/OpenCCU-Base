#ifndef HM_RFD_HID_HIDPI_H
#define HM_RFD_HID_HIDPI_H

#include <windows.h>
#include "hidusage.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef LONG NTSTATUS;

typedef enum _HIDP_REPORT_TYPE {
	HidP_Input,
	HidP_Output,
	HidP_Feature
} HIDP_REPORT_TYPE;

typedef struct _HIDP_PREPARSED_DATA* PHIDP_PREPARSED_DATA;

typedef struct _HIDP_CAPS {
	USAGE Usage;
	USAGE UsagePage;
	USHORT InputReportByteLength;
	USHORT OutputReportByteLength;
	USHORT FeatureReportByteLength;
	USHORT Reserved[17];
	USHORT NumberLinkCollectionNodes;
	USHORT NumberInputButtonCaps;
	USHORT NumberInputValueCaps;
	USHORT NumberInputDataIndices;
	USHORT NumberOutputButtonCaps;
	USHORT NumberOutputValueCaps;
	USHORT NumberOutputDataIndices;
	USHORT NumberFeatureButtonCaps;
	USHORT NumberFeatureValueCaps;
	USHORT NumberFeatureDataIndices;
} HIDP_CAPS, *PHIDP_CAPS;

NTSTATUS __stdcall HidP_GetCaps(
	IN PHIDP_PREPARSED_DATA PreparsedData,
	OUT PHIDP_CAPS Capabilities
);

#ifdef __cplusplus
}
#endif

#endif

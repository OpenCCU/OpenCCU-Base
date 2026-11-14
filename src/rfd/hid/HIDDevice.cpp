// HIDDevice.cpp: implementation of the CHIDDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "HIDDevice.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHIDDevice::CHIDDevice()
{
	// Call a reset on the device data to initialize
	ResetDeviceData();
}

void CHIDDevice::ResetDeviceData()
{
	// Reset the device handle
	m_Handle = INVALID_HANDLE_VALUE;

	// Set device opened to false
	m_DeviceOpened = FALSE;

	// Set all buffer lengths to 0
	m_InputReportBufferLength = 0;
	m_OutputReportBufferLength = 0;
	m_FeatureReportBufferLength = 0;

	// Obtain the OS version
	OSVERSIONINFO osVer;
	osVer.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	GetVersionEx(&osVer);

	// Only set the max report requests if we are using 2K or later
	if ((osVer.dwPlatformId == 2) && (osVer.dwMajorVersion == 5))
	{
		if (osVer.dwMinorVersion >= 1)
		{
			// XP or later supports 512 input reports
			m_MaxReportRequest = MAX_REPORT_REQUEST_XP;
		}	
		else if (osVer.dwMinorVersion == 0)
		{
			// 2K supports 200 input reports
			m_MaxReportRequest = MAX_REPORT_REQUEST_2K;
		}
	}
	else
	{
		// Otherwise set the max reports to 0, and this will
		// only allow te default
		m_MaxReportRequest = 0;
	}
}

CHIDDevice::~CHIDDevice()
{	
	// Call close on destruction
	Close();
}

BYTE CHIDDevice::Close()
{
	// This function will close the HID Device and then calls ResetDeviceData
	// to reinitialize all of the members after the close completes

	BYTE status = HID_DEVICE_SUCCESS;

	// Check to see if the device is opened, otherwise return an error code
	if (m_DeviceOpened)
	{
		// Check that we have valid handle values, otherwise return an error code
		if ((m_Handle != INVALID_HANDLE_VALUE) && (m_Handle != NULL))
		{
			CloseHandle(m_Handle);
		}
		else
		{
			status = HID_DEVICE_HANDLE_ERROR;
		}

		// Reset the device data
		ResetDeviceData();
	}
	else
	{
		status = HID_DEVICE_NOT_OPENED;
	}

	return status;
}

BOOL CHIDDevice::GetHidDevicePath(DWORD index, char* devicePath)
{
	// This function will obtain the device path of an HID device by it's index
	// TRUE is returned if the index is found, and contains a valid device path
	// FALSE is returned if the index is not found

	BOOL status = FALSE;

	GUID		hidGuid;
	HDEVINFO	hHidDeviceInfo = NULL;

	// Obtain the HID GUID
	HidD_GetHidGuid(&hidGuid);

	// Use the HID GUID to get a handle to a list of all HID devices connected
	hHidDeviceInfo = SetupDiGetClassDevs(&hidGuid, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);

	// If the device info was NULL, return an error code
	if (hHidDeviceInfo != NULL)
	{
		SP_DEVICE_INTERFACE_DATA hidDeviceInterfaceData;
		hidDeviceInterfaceData.cbSize = sizeof(hidDeviceInterfaceData);

		// Check that the index is less than the max number of USB devices connectable
		if (index < MAX_USB_DEVICES)
		{
			// Query the device using the index to get the interface data
			BOOL hidResult = SetupDiEnumDeviceInterfaces(hHidDeviceInfo, 0, &hidGuid, index, &hidDeviceInterfaceData);

			// If a successful query was made, use it to get the detailed data of the device
			if (hidResult)
			{
				BOOL detailResult;
				DWORD length, required;
				PSP_DEVICE_INTERFACE_DETAIL_DATA hidDeviceInterfaceDetailData;

				// Obtain the length of the detailed data structure, then allocate space and retrieve it
				SetupDiGetDeviceInterfaceDetail(hHidDeviceInfo, &hidDeviceInterfaceData, NULL, 0, &length, NULL);
				hidDeviceInterfaceDetailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)malloc(length);

				if (hidDeviceInterfaceDetailData)
				{
					hidDeviceInterfaceDetailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
					detailResult = SetupDiGetDeviceInterfaceDetail(hHidDeviceInfo, &hidDeviceInterfaceData, hidDeviceInterfaceDetailData, length, &required, NULL);
					
					// If getting the device detail provides a valid result, then copy it's device
					// path into the device path argument pointer and set status to TRUE
					if (detailResult)
					{
						strcpy(devicePath, hidDeviceInterfaceDetailData->DevicePath);
						status = TRUE;
					}

					// Deallocate space for the detailed data structure
					free(hidDeviceInterfaceDetailData);
				}
			}
		}	
	}

	return status;
}

HANDLE CHIDDevice::OpenDeviceByVidPid(char* devicePath, WORD vid, WORD pid, DWORD desired_access)
{
	// This function attempts to open a device using a specific VID and PID. If it succeeds
	// then a handle is returned. If it fails, then INVALID_HANDLE_VALUE is returned

    DWORD share_mode;
    if(desired_access != FILE_READ_ATTRIBUTES)share_mode=FILE_SHARE_READ;
    else share_mode=FILE_SHARE_READ|FILE_SHARE_WRITE;

	// Open the device
	HANDLE hHidDeviceHandle = CreateFile(devicePath, desired_access, share_mode, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);

	// Check that the device opened successfully
	if (hHidDeviceHandle != INVALID_HANDLE_VALUE)
	{
		HIDD_ATTRIBUTES	hidDeviceAttributes;

		// Get the attributes of the HID device
		if (HidD_GetAttributes(hHidDeviceHandle, &hidDeviceAttributes))
		{
			// Close the handle if either the VID or the PID don't match
			if ((hidDeviceAttributes.VendorID != vid) || (hidDeviceAttributes.ProductID != pid))
			{
				CloseHandle(hHidDeviceHandle);
				hHidDeviceHandle = INVALID_HANDLE_VALUE;
			}
		}
		else
		{
			CloseHandle(hHidDeviceHandle);
			hHidDeviceHandle = INVALID_HANDLE_VALUE;
		}
	}

	return hHidDeviceHandle;
}

DWORD CHIDDevice::GetConnectedDeviceNum(WORD vid, WORD pid)
{
	// This function will return the number of devices connected with a specified VID and
	// PID, if no devices are connected, it will return a 0

	DWORD deviceNum = 0;
	char devicePath[MAX_PATH];
	DWORD i = 0;

	// Loop through to check all of the HID Device paths available
	while (GetHidDevicePath(i, devicePath))
	{
		// Attempt to open the device by VID and PID
		HANDLE hHidDeviceHandle = OpenDeviceByVidPid(devicePath, vid, pid, FILE_READ_ATTRIBUTES);

		// Check that the device is opened and the handle is valid
		if (hHidDeviceHandle != INVALID_HANDLE_VALUE)
		{
			// Increment the number of devices found then close the handle
			deviceNum++;
			CloseHandle(hHidDeviceHandle);
		}

		// Increment i for next device
		i++;
	}

	return deviceNum;
}

BYTE CHIDDevice::GetSerialString(DWORD deviceIndex, WORD vid, WORD pid, LPSTR serialString, DWORD serialStringLength)
{
	// This function will obtain the serial string of a device by it's index within it's VID
	// and PID. So if only 1 device is connected with VID 10C4, 9999, it's index is 0. If three 
	// devices are connected with VID 10C4, 9999 are connected, they would be referenced as
	// device 0, 1, and 2

	BYTE status = HID_DEVICE_NOT_FOUND;
	char devicePath[MAX_PATH];
	DWORD i = 0;
	DWORD deviceNum = 0;

	// Loop through to check all of the HID Device paths available
	while (GetHidDevicePath(i, devicePath))
	{
		// Attempt to open the device by VID and PID
		HANDLE hHidDeviceHandle = OpenDeviceByVidPid(devicePath, vid, pid, FILE_READ_ATTRIBUTES);

		// Check that the device is opened and the handle is valid
		if (hHidDeviceHandle != INVALID_HANDLE_VALUE)
		{
			// If the device index matches our current device number, then get
			// the serial string
			if (deviceNum == deviceIndex)
			{
				BYTE serialBuf[512];

				// Obtain the serial number
				if (HidD_GetSerialNumberString(hHidDeviceHandle, serialBuf, 512))
				{
					// Reset the supplied string
					memset(serialString, 0, serialStringLength);

					// Convert the string from unicode to ansi
					for (DWORD i = 0; i < serialStringLength; i++)
					{
						serialString[i] = serialBuf[i*2];
					}

					// Return success
					status = HID_DEVICE_SUCCESS;
				}
			}

			// Increment the number of devices found then close the handle
			deviceNum++;
			CloseHandle(hHidDeviceHandle);
		}

		// Increment the number of devices found
		i++;
	}

	return status;
}

BYTE CHIDDevice::GetSerialString(LPSTR serialString, DWORD serialStringLength)
{
	// This function will obtain the serial string of the previously opened device

	BYTE status = HID_DEVICE_NOT_FOUND;
	DWORD i = 0;

    // Check that the device is opened and the handle is valid
	if (IsOpened())
	{
		BYTE serialBuf[512];

		// Obtain the serial number
		if (HidD_GetSerialNumberString(m_Handle, serialBuf, 512))
		{
			// Reset the supplied string
			memset(serialString, 0, serialStringLength);

			// Convert the string from unicode to ansi
			for (DWORD i = 0; i < serialStringLength; i++)
			{
				serialString[i] = serialBuf[i*2];
			}

			// Return success
			status = HID_DEVICE_SUCCESS;
		}
	}
	return status;
}

BYTE CHIDDevice::GetVersion(USHORT* version)
{
	// This function will obtain the device version of the previously opened device

	BYTE status = HID_DEVICE_NOT_FOUND;

    // Check that the device is opened and the handle is valid
	if (IsOpened())
	{
		HIDD_ATTRIBUTES	hidDeviceAttributes;

		// Get the attributes of the HID device
		if (HidD_GetAttributes(m_Handle, &hidDeviceAttributes))
		{
            *version=hidDeviceAttributes.VersionNumber;
            status = HID_DEVICE_SUCCESS;
		}
	}
	return status;
}

BYTE CHIDDevice::Open(DWORD deviceIndex, WORD vid, WORD pid, WORD numInputBuffers)
{
	// This function will open a device by it's index and VID and PID. In addition, the number
	// of inp

	BYTE status = HID_DEVICE_SUCCESS;
	char devicePath[MAX_PATH];
	DWORD i = 0;
	DWORD deviceNum = 0;
	BOOL found = FALSE;

	// Ensure that the we don't already have an open device
	if (m_DeviceOpened)
	{
		status = HID_DEVICE_ALREADY_OPENED;
	}

	// Begin to look for the device if it is not opened
	if (status == HID_DEVICE_SUCCESS)
	{
		while (GetHidDevicePath(i, devicePath))
		{
			// Attempt to open the device by VID and PID
			HANDLE hHidDeviceHandle = OpenDeviceByVidPid(devicePath, vid, pid, GENERIC_READ | GENERIC_WRITE);

			// Check that the device is opened and the handle is valid
			if (hHidDeviceHandle != INVALID_HANDLE_VALUE)
			{
				// Set found to true to stop the loop if the device num is correct
				if (deviceNum == deviceIndex)
				{
					found = TRUE;
					
					// Set the member variables to an opened device and handle
					m_DeviceOpened = TRUE;
					m_Handle = hHidDeviceHandle;

					PHIDP_PREPARSED_DATA preparsedData;

					if (HidD_GetPreparsedData(hHidDeviceHandle, &preparsedData))
					{
						HIDP_CAPS capabilities;

						// Used the preparsed data structure to get the device capabilities
						if (HidP_GetCaps(preparsedData, &capabilities))
						{
							// Allocate memory for the input, output and feature reports
							if (capabilities.InputReportByteLength)
							{
								m_InputReportBufferLength = capabilities.InputReportByteLength;
							}

							if (capabilities.OutputReportByteLength)
							{
								m_OutputReportBufferLength = capabilities.OutputReportByteLength;
							}

							if (capabilities.FeatureReportByteLength)
							{
								m_FeatureReportBufferLength = capabilities.FeatureReportByteLength;
							}

							// See if the input buffers should be adjusted, if max report request is 0
							// then an OS not supported by ToolStick is being used, so dont modify it
							if ((m_MaxReportRequest > 0) && (numInputBuffers != DEFAULT_REPORT_INPUT_BUFFERS))
							{
								// Ensure that we are setting the input buffers to a valid setting
								if (numInputBuffers > m_MaxReportRequest) m_MaxReportRequest = numInputBuffers;

								HidD_SetNumInputBuffers(hHidDeviceHandle, numInputBuffers);
							}
							else
							{
								// Pull the current settings if default is selected
								HidD_GetNumInputBuffers(hHidDeviceHandle, (PULONG)(&m_MaxReportRequest));
							}
						}
															
						// Cleanup the preparesed data
						HidD_FreePreparsedData(preparsedData);
					}
				}

				// Increment the device number
				deviceNum++;
				
				// If the handle hasn't been found close it and continue the search
				if (!found)
				{
					CloseHandle(hHidDeviceHandle);
				}
			}

			// Increment the number of devices found
			i++;
		}

		// If the device wasnt found, return an error code
		if (!found)
		{
			status = HID_DEVICE_NOT_FOUND;
		}
	}
			
	return status;
 }

BOOL CHIDDevice::IsOpened()
{
	BOOL status = FALSE;

	// Check if a device is opened, and the handle is valid
	if (m_DeviceOpened && (m_Handle != INVALID_HANDLE_VALUE) && (m_Handle != NULL))
	{
		status = TRUE;
	}
	else
	{
		// If the device is opened, and the handle is invalid or NULL
		// reset the device opened flag, and the handle
		if (m_DeviceOpened)
		{
			ResetDeviceData();
		}

		status = FALSE;
	}

	return status;
}

BYTE CHIDDevice::SetFeatureReport(BYTE* buffer, DWORD bufferSize)
{
	BYTE status = HID_DEVICE_SUCCESS;

	// Check to see that the device is opened
	if (m_DeviceOpened)
	{
		if (!HidD_SetFeature(m_Handle, buffer, bufferSize))
		{
			status = HID_DEVICE_TRANSFER_FAILED;
		}
	}
	else
	{
		status = HID_DEVICE_NOT_OPENED;
	}

	return status;
}

BYTE CHIDDevice::GetFeatureReport(BYTE* buffer, DWORD bufferSize)
{
	BYTE reportID = buffer[0];
	BYTE status = HID_DEVICE_SUCCESS;

	// Check to see that the device is opened
	if (m_DeviceOpened)
	{
		// Clear out the report buffer, and set the head to the report ID
		memset(buffer, 0, bufferSize);
		buffer[0] = reportID;

		if (!HidD_GetFeature(m_Handle, buffer, bufferSize))
		{
			status = HID_DEVICE_TRANSFER_FAILED;
		}
	}
	else
	{
		status = HID_DEVICE_NOT_OPENED;
	}

	return status;
}

BYTE CHIDDevice::SetReport_Interrupt(BYTE* buffer, DWORD bufferSize, DWORD timeout)
{
	BYTE status = HID_DEVICE_SUCCESS;
	
	if (bufferSize <= (DWORD)(m_OutputReportBufferLength))
	{
		// Check to see that the device is opened
		if (IsOpened())
		{
			DWORD bytesWritten = 0;
			OVERLAPPED o = {0};

			o.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

			// Try to write the file
			if (!WriteFile(m_Handle, buffer, m_OutputReportBufferLength, &bytesWritten, &o))
			{
				// If the write fails, see if it is because IO is pending
				if (GetLastError() == ERROR_IO_PENDING)
				{
					//If there is still data to be written, wait on the event for 3 seconds
					DWORD waitStatus = WaitForSingleObject(o.hEvent, timeout);
					
					// If the object is signaled, then get the overlapped result, the write succeeded
					// Otherwise determine if the error was a timeout, or another error
					if (waitStatus == WAIT_OBJECT_0)
					{
						GetOverlappedResult(m_Handle, &o, &bytesWritten, FALSE);
					}
					else if (waitStatus == WAIT_TIMEOUT)
					{
						status = HID_DEVICE_TRANSFER_TIMEOUT;
						CancelIo(m_Handle);
					}
					else
					{
						status = HID_DEVICE_TRANSFER_FAILED;
						CancelIo(m_Handle);
					}
				}
				else
				{
					status = HID_DEVICE_TRANSFER_FAILED;
				}
			}

			CloseHandle(o.hEvent);
		}
		else
		{
			status = HID_DEVICE_NOT_OPENED;
		}
	}
	else
	{
		status = HID_DEVICE_INVALID_BUFFER_SIZE;
	}

	return status;
}

BYTE CHIDDevice::GetReport_Interrupt(BYTE* buffer, DWORD bufferSize, WORD numReports, DWORD* bytesReturned, DWORD timeout)
{
	BYTE status = HID_DEVICE_SUCCESS;
	DWORD bytesRead = 0;
	
	if ((bufferSize >= (DWORD)(m_InputReportBufferLength * numReports)) && (numReports <= m_MaxReportRequest))
	{
		// Clear out the report buffer, and set the head to the report ID
		memset(buffer, 0, bufferSize);

		// Check to see that the device is opened
		if (IsOpened())
		{
			OVERLAPPED o = {0};
			
			o.hEvent = CreateEvent(NULL, FALSE, FALSE, NULL);

			// Try to read input data
			if (!ReadFile(m_Handle, buffer, m_InputReportBufferLength * numReports, &bytesRead, &o))
			{
				// If the read fails, see if it is because IO is pending
				if (GetLastError() == ERROR_IO_PENDING)
				{
					// If there is still data to read, wait on the event object for 3 seconds
					DWORD waitStatus = WaitForSingleObject(o.hEvent, timeout);

					// If the object is signaled, then get the overlapped result, the read succeeded
					// Otherwise determine if the error was a timeout, or another error
					if (waitStatus == WAIT_OBJECT_0)
					{
						GetOverlappedResult(m_Handle, &o, &bytesRead, FALSE);
					}
					else if (waitStatus == WAIT_TIMEOUT)
					{
						status = HID_DEVICE_TRANSFER_TIMEOUT;
						CancelIo(m_Handle);
					}
					else
					{
						status = HID_DEVICE_TRANSFER_FAILED;
						CancelIo(m_Handle);
					}
				}
				else
				{
					status = HID_DEVICE_TRANSFER_FAILED;
				}
			}

			CloseHandle(o.hEvent);
		}
		else
		{
			status = HID_DEVICE_NOT_OPENED;
		}
	}
	else
	{
		status = HID_DEVICE_INVALID_BUFFER_SIZE;
	}

	// If the read succeeded, then send the number of bytes read back
	if (status == HID_DEVICE_SUCCESS)
	{
		*bytesReturned = bytesRead;
	}

	return status;
}

BYTE CHIDDevice::SetReport_Control(BYTE* buffer, DWORD bufferSize)
{
	BYTE status = HID_DEVICE_SUCCESS;
	DWORD lasterror;
	
	//if (bufferSize >= m_OutputReportBufferLength)
	//if (1)
	{
		// Clear out the report buffer, and set the head to the report ID
//		memset(buffer, 0, bufferSize);

		// Check to see that the device is opened
		if (IsOpened())
		{
			// Call SetOutputReport to send this report buffer over the control pipe
			if (!HidD_SetOutputReport(m_Handle, buffer, bufferSize))
			{
				status = HID_DEVICE_TRANSFER_FAILED;
				lasterror = GetLastError();
			}
		}
		else
		{
			status = HID_DEVICE_NOT_OPENED;
		}
	}
/*
	else
	{
		status = HID_DEVICE_INVALID_BUFFER_SIZE;
	}
*/
	return status;
}

BYTE CHIDDevice::GetReport_Control(BYTE* buffer, DWORD bufferSize)
{
	BYTE status = HID_DEVICE_SUCCESS;
	unsigned char reportID = buffer[0];
	if (bufferSize >= m_OutputReportBufferLength)
	{
		// Clear out the report buffer, and set the head to the report ID
		memset(buffer, 0, bufferSize);
		buffer[0] = reportID;
		// Check to see that the device is opened
		if (IsOpened())
		{
			// Call GetInputReport to get the requested report buffer over the control pipe
			if (!HidD_GetInputReport(m_Handle, buffer, m_OutputReportBufferLength))
			{
				status = HID_DEVICE_TRANSFER_FAILED;
			}
		}
		else
		{
			status = HID_DEVICE_NOT_OPENED;
		}
	}
	else
	{
		status = HID_DEVICE_INVALID_BUFFER_SIZE;
	}

	return status;
}

BOOL CHIDDevice::FlushBuffers()
{
	return HidD_FlushQueue(m_Handle);
}

WORD CHIDDevice::GetInputReportBufferLength()
{
	// Returns specified buffer length
	return m_InputReportBufferLength;
}

WORD CHIDDevice::GetOutputReportBufferLength()
{
	// Returns specified buffer length
	return m_OutputReportBufferLength;
}

WORD CHIDDevice::GetFeatureReportBufferLength()
{
	// Returns specified buffer length
	return m_FeatureReportBufferLength;
}

WORD CHIDDevice::GetMaxReportRequest()
{
	WORD maxReportRequest;

	// Returns the maximum allowable reports to request for read at a time
	if (m_MaxReportRequest > 0)
		maxReportRequest = (WORD)m_MaxReportRequest;
	else
		maxReportRequest = 32;

	return maxReportRequest;
}

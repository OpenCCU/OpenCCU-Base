// HIDDevice.cpp: implementation of the CHIDDevice class.
//
//////////////////////////////////////////////////////////////////////

#include "HIDDevice.h"
#include <Logger.h>

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHIDDevice::CHIDDevice()
{
	libusb_init( &m_LibusbContext );
	libusb_set_debug( m_LibusbContext, 3 );
	// Call a reset on the device data to initialize
	ResetDeviceData();
}

void CHIDDevice::ResetDeviceData()
{
	// Reset the device handle
	m_Handle = NULL;

	// Set device opened to false
	m_DeviceOpened = false;

}

CHIDDevice::~CHIDDevice()
{	
	// Call close on destruction
	Close();
	libusb_exit( m_LibusbContext );
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
		if (m_Handle != NULL)
		{
			libusb_release_interface( m_Handle, InterfaceIndex );
			libusb_close( m_Handle );
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

DWORD CHIDDevice::GetConnectedDeviceNum(WORD vid, WORD pid)
{
	// This function will return the number of devices connected with a specified VID and
	// PID, if no devices are connected, it will return a 0

	libusb_device **list;
	ssize_t cnt = libusb_get_device_list( m_LibusbContext, &list );
	
	if( cnt < 0 )return 0;
	
	DWORD deviceNum = 0;
	
	for( int i=0; i<cnt; i++ )
	{
		libusb_device * device = list[i];
		struct libusb_device_descriptor desc;
		if( libusb_get_device_descriptor(device, &desc) >= 0 )
		{
			if (desc.idVendor == vid && desc.idProduct == pid) {
				deviceNum++;
			}
		}
	}
	libusb_free_device_list(list, 1);
	return deviceNum;
}

BYTE CHIDDevice::GetSerialString(DWORD deviceIndex, WORD vid, WORD pid, LPSTR serialString, DWORD serialStringLength)
{
	// This function will obtain the serial string of a device by it's index within it's VID
	// and PID. So if only 1 device is connected with VID 10C4, 9999, it's index is 0. If three 
	// devices are connected with VID 10C4, 9999 are connected, they would be referenced as
	// device 0, 1, and 2

	BYTE status = HID_DEVICE_NOT_FOUND;

	libusb_device **list;
	ssize_t cnt = libusb_get_device_list( m_LibusbContext, &list );
	
	if( cnt < 0 )return 0;
	
	DWORD deviceNum = 0;
	
	for( int i=0; i<cnt; i++ )
	{
		libusb_device * device = list[i];
		struct libusb_device_descriptor desc;
		if( libusb_get_device_descriptor(device, &desc) >= 0 )
		{
			if (desc.idVendor == vid && desc.idProduct == pid) {
				if( deviceNum == deviceIndex )
				{
					libusb_device_handle* handle;
					if( libusb_open( device, &handle ) >= 0 )
					{
						libusb_get_string_descriptor_ascii( handle, desc.iSerialNumber, (unsigned char*)serialString, serialStringLength );
						// Return success
						status = HID_DEVICE_SUCCESS;
						libusb_close( handle );
						break;
					}
				}
				deviceNum++;
			}
		}
	}
	libusb_free_device_list(list, 1);
	return status;
}

BYTE CHIDDevice::GetSerialString(LPSTR serialString, DWORD serialStringLength)
{
	// This function will obtain the serial string of the previously opened device

	BYTE status = HID_DEVICE_NOT_FOUND;

    // Check that the device is opened and the handle is valid
	if (IsOpened())
	{
		struct libusb_device_descriptor desc;
		if( libusb_get_device_descriptor( libusb_get_device( m_Handle ), &desc) >= 0 )
		{
			if( libusb_get_string_descriptor_ascii( m_Handle, desc.iSerialNumber, (unsigned char*)serialString, serialStringLength ) >= 0 )
			{
				// Return success
				status = HID_DEVICE_SUCCESS;
			}
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
		struct libusb_device_descriptor desc;
		if( libusb_get_device_descriptor( libusb_get_device( m_Handle ), &desc) >= 0 )
		{
			*version = desc.bcdDevice;
			status = HID_DEVICE_SUCCESS;
		}
	}
	return status;
}

bool CHIDDevice::ReadReportDescriptor()
{
	return _reportDescriptor.Init( m_Handle );
}

BYTE CHIDDevice::Open(DWORD deviceIndex, WORD vid, WORD pid, WORD numInputBuffers)
{
	// This function will open a device by it's index and VID and PID. In addition, the number
	// of inp

	BYTE status = HID_DEVICE_SUCCESS;

	// Ensure that the we don't already have an open device
	if (m_DeviceOpened)
	{
		status = HID_DEVICE_ALREADY_OPENED;
	}

	// Begin to look for the device if it is not opened
	if (status == HID_DEVICE_SUCCESS)
	{
		m_Handle = libusb_open_device_with_vid_pid(m_LibusbContext, vid, pid);
		if( m_Handle )
		{
			int kernel_driver_active = libusb_kernel_driver_active( m_Handle, InterfaceIndex );
			if( kernel_driver_active >= 0 )
			{
				if( kernel_driver_active )
				{
					LOG( Logger::LOG_DEBUG, "Detaching device %04X/%04X from kernel driver", vid, pid );
					libusb_detach_kernel_driver( m_Handle, InterfaceIndex );
				}
				if( libusb_claim_interface( m_Handle, InterfaceIndex ) >= 0 )
				{
					if( ReadReportDescriptor() )
					{
						m_DeviceOpened = true;
					}else{
						LOG( Logger::LOG_ERROR, "Reading HID report descriptor failed" );
						status = HID_DEVICE_NOT_FOUND;
					}
				}else{
					LOG( Logger::LOG_ERROR, "Error claiming interface opening %04X/%04X", vid, pid );
					status = HID_DEVICE_NOT_FOUND;
				}
				if( status != HID_DEVICE_SUCCESS )
				{
					libusb_close( m_Handle );
					m_Handle = NULL;
				}
			}else{
				LOG( Logger::LOG_ERROR, "kernel_driver_active failed opening %04X/%04X", vid, pid );
				status = HID_DEVICE_NOT_FOUND;
				libusb_close( m_Handle );
				m_Handle = NULL;
			}
		}else{
			LOG( Logger::LOG_INFO, "Device not found opening %04X/%04X", vid, pid );
			status = HID_DEVICE_NOT_FOUND;
		}
	}
	return status;
 }

BOOL CHIDDevice::IsOpened()
{
	BOOL status = false;

	// Check if a device is opened, and the handle is valid
	if (m_DeviceOpened && (m_Handle != NULL))
	{
		status = true;
	}
	else
	{
		// If the device is opened, and the handle is invalid or NULL
		// reset the device opened flag, and the handle
		if (m_DeviceOpened)
		{
			ResetDeviceData();
		}

		status = false;
	}

	return status;
}

BYTE CHIDDevice::SetReport_Interrupt(BYTE* buffer, DWORD bufferSize, DWORD timeout)
{
	BYTE status = HID_DEVICE_SUCCESS;
	
	if (bufferSize <= GetOutputReportBufferLength())
	{
		if( !_reportDescriptor.ReportsHaveIdPrefix() )
		{
			//skip the first byte indicating the report type
			bufferSize--;
			buffer++;
		}
	
		// Check to see that the device is opened
		if (IsOpened())
		{
			int transferred = 0;
			while( transferred < (int)bufferSize )
			{
				int oldTransferred = transferred;
				int result = libusb_interrupt_transfer( m_Handle, EndpointOut, buffer, bufferSize, &transferred, timeout );
				if( result == LIBUSB_ERROR_TIMEOUT )
				{
					if ( transferred > oldTransferred )continue;
					status = HID_DEVICE_TRANSFER_TIMEOUT;
					LOG( Logger::LOG_WARNING, "Transfer timeout after %d bytes sending %d bytes", transferred, bufferSize );
					break;
				}
				if( result < 0 )
				{
					LOG( Logger::LOG_WARNING, "Transfer failed sending %d bytes", bufferSize );
					status = HID_DEVICE_TRANSFER_FAILED;
				}
				//LOG( Logger::LOG_DEBUG, "%d bytes sent", transferred );
				break;
			}
		}else{
			status = HID_DEVICE_NOT_OPENED;
		}
	}else{
		LOG( Logger::LOG_ERROR, "HID output report too big");
	}
	return status;
}

BYTE CHIDDevice::GetReport_Interrupt(BYTE* buffer, DWORD bufferSize, WORD numReports, DWORD* bytesReturned, DWORD timeout)
{
	BYTE status = HID_DEVICE_SUCCESS;
	
	int reportSize = GetInputReportBufferLength();
	*bytesReturned = 0;
	if ((int)bufferSize >= reportSize )
	{
		if( !_reportDescriptor.ReportsHaveIdPrefix() )
		{
			//set first byte for report id to maintain a consistent interface
			*buffer = 0;
			buffer++;
			(*bytesReturned)++;
			reportSize--;
		}
		// Check to see that the device is opened
		if (IsOpened())
		{
			int transferred = 0;
			if( !timeout )timeout = 1;
			int result = libusb_interrupt_transfer( m_Handle, EndpointIn, buffer, reportSize, &transferred, timeout );
			if( result >= 0 )
			{
				(*bytesReturned) += transferred;
				//LOG( Logger::LOG_DEBUG, "%d bytes received", transferred );
			}else{
				if( result == LIBUSB_ERROR_TIMEOUT )
				{
					status = HID_DEVICE_TRANSFER_TIMEOUT;
				}else{
					LOG( Logger::LOG_WARNING, "Transfer failed receiving %d bytes: %d", reportSize, result );
					status = HID_DEVICE_TRANSFER_FAILED;
				}
			}
		}else{
			status = HID_DEVICE_NOT_OPENED;
		}
	}else{
		LOG( Logger::LOG_ERROR, "HID input buffer too small for maximum report size");
	}
	return status;
}

WORD CHIDDevice::GetInputReportBufferLength()
{
	return _reportDescriptor.GetMaxInputReportSize() + 1;
}

WORD CHIDDevice::GetOutputReportBufferLength()
{
	return _reportDescriptor.GetMaxOutputReportSize() + 1;
}

WORD CHIDDevice::GetMaxReportRequest()
{
	return MaxReportRequest;
}

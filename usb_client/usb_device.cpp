// usb_device.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "usb_device.h"

#ifdef _WIN32
#include "usb_device_windows.h"
#else
#include "usb_device_linux.h"
#endif

USBDevice::USBDevice()
{
}

USBDevice::~USBDevice()
{
}

USBDeviceIdentifier::~USBDeviceIdentifier()
{
}

std::auto_ptr<USBDevice> USBDevice::create(const USBDeviceIdentifier* deviceID, const std::string& serialNumber)
{
    // create platform specific implementation
#ifdef _WIN32
    return std::auto_ptr<USBDevice>(new USBDeviceWindows(deviceID, serialNumber));
#else
    return std::auto_ptr<USBDevice>(new USBDeviceLinux(deviceID, serialNumber));
#endif
}

// End of file usb_device.cpp

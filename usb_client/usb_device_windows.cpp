// usb_device_windows.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "usb_device_windows.h"
#include <cstring>
#include <boost/scoped_array.hpp>

#include <delayimp.h>
#pragma comment(lib, "delayimp")
#include <setupapi.h>

namespace
{
    const int DEFAULT_CONFIGURATION_INDEX = 0;
    const size_t MAX_URB_SIZE = 1048576;
    const size_t MAX_IN_FLIGHT = 8; // 8MB

    /**
     * Ensure WinUSB is loaded
     *
     * WinUSB can be lazy loaded instead of directly linked to - this allows
     * use of client programs on systems where WinUSB isn't installed, but the
     * user doesn't need USB aspects of that client
     */
    void ensureWinUSBLoaded()
    {
        // see if it's already loaded or linked as a non-delayed load
        if (GetModuleHandle("WINUSB") == NULL)
        {
            // try to load the dll
            // The traditional try catch using /EHa doesn't seem to work
            // with VC2010. This may be due to the compiler flags that are getting set?
            EXCEPTION_POINTERS * eps = 0;
            __try
            {
                if (!SUCCEEDED(__HrLoadAllImportsForDll("WINUSB.DLL")))
                    throw USBException("Failed to load winusb.dll");
            }
            __except(eps = GetExceptionInformation(), ERROR_MOD_NOT_FOUND)
            {
                throw USBException("Failed to load winusb.dll");
            }
        }
    }

    /**
     * Get the path to a device instance
     */
    std::string GetDevicePath(HDEVINFO deviceInfo,
                              SP_DEVICE_INTERFACE_DATA& deviceInterfaceData)
    {
        // find out how big device interface detail is
        ULONG requiredLength=0;
        if (SetupDiGetDeviceInterfaceDetail(deviceInfo,
                                            &deviceInterfaceData,
                                            NULL, 0,
                                            &requiredLength,
                                            NULL))
            return std::string();

        if (GetLastError() != ERROR_INSUFFICIENT_BUFFER)
            return false;

        // allocate buffer
        PSP_DEVICE_INTERFACE_DETAIL_DATA detailData = (PSP_DEVICE_INTERFACE_DETAIL_DATA)
            LocalAlloc(LMEM_FIXED, requiredLength);

        if (detailData == NULL)
            return std::string();

        detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        ULONG length = requiredLength;

        // get device detail
        if (!SetupDiGetDeviceInterfaceDetail(deviceInfo,
                                             &deviceInterfaceData,
                                             detailData,
                                             length,
                                             &requiredLength,
                                             0))
        {
            LocalFree(detailData);
            return std::string();
        }

        // extract path
        std::string path = detailData->DevicePath;

        LocalFree(detailData);

        return path;
    }

    /**
     * Get device paths for devices matching a GUID
     */
    std::vector<std::string> getDevicesForGUID(const GUID* guid)
    {
        HDEVINFO deviceInfo = SetupDiGetClassDevs(guid,
                                                  NULL, NULL,
                                                  DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
        if (deviceInfo == INVALID_HANDLE_VALUE)
            throw USBException("Failed to get USB devices");

        std::vector<std::string> devicePaths;

        for (unsigned int device = 0; ; ++device)
        {
            // enumerate device interfaces
            SP_DEVICE_INTERFACE_DATA interfaceData;
            interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
            if (!SetupDiEnumDeviceInterfaces(deviceInfo,
                                             NULL,
                                             guid,
                                             device,
                                             &interfaceData))
            {
                // call failed - either no more devices or error
                if (GetLastError() != ERROR_NO_MORE_ITEMS)
                {
                    SetupDiDestroyDeviceInfoList(deviceInfo);
                    throw USBException("Failed to get USB device");
                }
                // otherwise all done
                break;
            }
            else
            {
                // process device interface
                std::string devicePath = GetDevicePath(deviceInfo, interfaceData);
                if (devicePath.empty())
                {
                    SetupDiDestroyDeviceInfoList(deviceInfo);
                    throw USBException("Failed to get USB device path");
                }

                devicePaths.push_back(devicePath);
            }
        }

        SetupDiDestroyDeviceInfoList(deviceInfo);

        return devicePaths;
    }


    int getUsbString(WINUSB_INTERFACE_HANDLE deviceHandle,
                     uint8_t index, uint16_t langId,
                     std::string& str)
    {
        const size_t MAX_DESC = 256;
        UCHAR buf[MAX_DESC];
        ULONG bytesRead;

        memset(buf, 0, MAX_DESC);

        // lookup first supported language from descriptor 0
        if (langId == 0)
        {
            if (!WinUsb_GetDescriptor(deviceHandle,
                                      USB_STRING_DESCRIPTOR_TYPE,
                                      0,
                                      0,
                                      buf,
                                      MAX_DESC,
                                      &bytesRead))
                return false;

            langId = buf[2] | (buf[3] << 8);
        }

        // get descriptor content
        if (!WinUsb_GetDescriptor(deviceHandle,
                                  USB_STRING_DESCRIPTOR_TYPE,
                                  index,
                                  langId,
                                  buf,
                                  MAX_DESC,
                                  &bytesRead))
            return false;
        else
        {
            // descriptor returned has data length (bytes) in first byte, index in
            // second, then unicode data (16 bits)
            // extract the length and convert to ascii (discarding high bytes)
            size_t strLen = (buf[0] - 2) / 2;
            str.resize(strLen);
            unsigned short* src = (unsigned short*)&buf[2];
            for (size_t i = 0; i < strLen; ++i, ++src)
                str[i] = (unsigned char)(*src & 0xFF);

            return true;
        }
    }

    HANDLE openDevice(const std::string& devicePath)
    {
        return CreateFile(devicePath.c_str(),
                          GENERIC_WRITE | GENERIC_READ,
                          FILE_SHARE_WRITE | FILE_SHARE_READ,
                          NULL,
                          OPEN_EXISTING,
                          FILE_ATTRIBUTE_NORMAL | FILE_FLAG_OVERLAPPED,
                          NULL);
    }


    bool openWinUSB(HANDLE handle, WINUSB_INTERFACE_HANDLE& deviceHandle)
    {
        bool ok = true;
        try
        {
            ok = (WinUsb_Initialize(handle, &deviceHandle) == TRUE);
        }
        catch (...)
        {
            // if delayed loading of winusb.dll has been specified at link time, then
            // an exception will be thrown at this point if the user hasn't installed
            // the WinUsb runtime (i.e. hasn't installed a driver).
            ok = false;
        }

        return ok;
    }

    /*
     * Get the serial number of the device specified by a device path
     */
    std::string getDeviceSerialNumber(const std::string& devicePath)
    {
        ensureWinUSBLoaded();

        // open device
        HANDLE handle = openDevice(devicePath);
        if (handle == INVALID_HANDLE_VALUE)
            throw USBException("Failed to open " + devicePath);

        // init win usb
        WINUSB_INTERFACE_HANDLE deviceHandle;
        bool ok = openWinUSB(handle, deviceHandle);

        if (!ok)
        {
            ::CloseHandle(handle);
            throw USBException("Failed to initialise WinUSB for " + devicePath);
        }

        // Get the device descriptor
        USB_DEVICE_DESCRIPTOR deviceDescriptor;
        ULONG bytesRead;
        ok = (WinUsb_GetDescriptor(deviceHandle, USB_DEVICE_DESCRIPTOR_TYPE,
                                   0, 0,
                                   (PUCHAR)&deviceDescriptor, sizeof(USB_DEVICE_DESCRIPTOR),
                                   &bytesRead) == TRUE);
        if (!ok || bytesRead != sizeof(USB_DEVICE_DESCRIPTOR))
        {
            WinUsb_Free(deviceHandle);
            ::CloseHandle(handle);
            throw USBException("Failed to get USB descriptor for " + devicePath);
        }

        // get serial number if present
        std::string serialNumberString = "";
        if (deviceDescriptor.iSerialNumber != 0)
        {
            if (!getUsbString(deviceHandle, deviceDescriptor.iSerialNumber, 0, serialNumberString))
            {
                WinUsb_Free(deviceHandle);
                ::CloseHandle(handle);
                throw USBException("Failed to get serial number for " + devicePath);
            }
        }

        WinUsb_Free(deviceHandle);
        ::CloseHandle(handle);

        return serialNumberString;
    }

    /**
     * Get the path for a device with the specified guid and serial number
     *
     * Return first device found if empty serial number
     */
    std::string getDevicePath(const GUID* guid, const std::string& serialNumber)
    {
        std::vector<std::string> devicePaths = getDevicesForGUID(guid);
        if (devicePaths.empty())
            throw USBException("No USB device found");

        if (serialNumber == "")
            return devicePaths.front();
        else
        {
            for (std::vector<std::string>::const_iterator devPath = devicePaths.begin();
                 devPath != devicePaths.end();
                 ++devPath)
            {
                std::string devSerial = getDeviceSerialNumber(*devPath);
                if (devSerial == serialNumber)
                    return *devPath;
            }
            throw USBException("USB device with serial " + serialNumber + " not found");
        }
    }
}

USBDeviceWindows::USBDeviceWindows(const USBDeviceIdentifier* deviceID, const std::string& serialNumber)
    : m_devicePath(getDevicePath(deviceID->guid, serialNumber)),
      m_serialNumber(serialNumber),
      m_interfaceIndex(deviceID->interfaceNumber),
      m_altSetting(0),
      m_handle(INVALID_HANDLE_VALUE),
      m_deviceHandle(INVALID_HANDLE_VALUE),
      m_interfaceHandle(INVALID_HANDLE_VALUE),
      m_nextToken(0),
      m_numInFlightTransfers(0),
      m_unplugged(false)
{
}

USBDeviceWindows::~USBDeviceWindows()
{
}

size_t USBDeviceWindows::asyncTransferSize() const
{
    return MAX_URB_SIZE;
}

size_t USBDeviceWindows::asyncTransferCount() const
{
    return MAX_IN_FLIGHT;
}

void USBDeviceWindows::connect()
{
    boost::mutex::scoped_lock lock(m_lock);

    if (m_handle != INVALID_HANDLE_VALUE ||
        m_interfaceHandle != INVALID_HANDLE_VALUE)
        // already open
        throw USBException("USB device already connected");

    ensureWinUSBLoaded();

    // open device
    m_handle = openDevice(m_devicePath);
    if (m_handle == INVALID_HANDLE_VALUE)
        throw USBException("Failed to open " + m_devicePath);

    // init win usb
    bool ok = openWinUSB(m_handle, m_deviceHandle);

    if (!ok || m_deviceHandle == INVALID_HANDLE_VALUE)
    {
        doDisconnect();
        throw USBException("Error initialising WinUsb");
    }

    if (m_interfaceIndex == 0)
    {
        // use device handle directly
        m_interfaceHandle = m_deviceHandle;
    }
    else
    {
        // need to get interface handle
        ok = (WinUsb_GetAssociatedInterface(m_deviceHandle, m_interfaceIndex, &m_interfaceHandle) == TRUE);
        if (!ok || m_interfaceHandle == INVALID_HANDLE_VALUE)
        {
            doDisconnect();
            throw USBException("Error opening USB interface");
        }
    }

    // get endpoint information
    UCHAR altSetting;
    ok = (WinUsb_GetCurrentAlternateSetting(m_interfaceHandle, &altSetting) == TRUE);
    if (ok)
        m_epInfo = examineEndpoints(altSetting);

    // reset bulk endpoints to clear toggle bits
    for (std::vector<USBEPInfo>::const_iterator e = m_epInfo.begin();
         e != m_epInfo.end() && ok;
         ++e)
    {
        if (e->type == USBEPInfo::EP_TYPE_BULK)
            ok = (WinUsb_ResetPipe(m_interfaceHandle, e->addr) == TRUE);
    }
}


void USBDeviceWindows::disconnect()
{
    boost::mutex::scoped_lock lock(m_lock);

    doDisconnect();
}


void USBDeviceWindows::doDisconnect()
{
    // close interface
    if (m_interfaceHandle != INVALID_HANDLE_VALUE)
    {
        if (m_interfaceIndex != 0)
            WinUsb_Free(m_interfaceHandle);
        m_interfaceHandle = INVALID_HANDLE_VALUE;
    }

    // close WinUSB layer
    if (m_deviceHandle != INVALID_HANDLE_VALUE)
    {
        WinUsb_Free(m_deviceHandle);
        m_deviceHandle = INVALID_HANDLE_VALUE;
    }

    // close device handle
    if (m_handle != INVALID_HANDLE_VALUE)
    {
        ::CloseHandle(m_handle);
        m_handle = INVALID_HANDLE_VALUE;
    }
}


std::vector<USBEPInfo> USBDeviceWindows::examineEndpoints(unsigned altSetting)
{
    bool ok = true;

    USB_INTERFACE_DESCRIPTOR ifaceDescriptor;
    if (!WinUsb_QueryInterfaceSettings(m_interfaceHandle, altSetting, &ifaceDescriptor))
        throw USBException("Failed to get endpoint information");

    std::vector<USBEPInfo> epInfo;
    for (int i = 0; i < ifaceDescriptor.bNumEndpoints && ok; ++i)
    {
        WINUSB_PIPE_INFORMATION pipeInfo;
        ok = (WinUsb_QueryPipe(m_interfaceHandle, altSetting, (UCHAR)i,
                               &pipeInfo) == TRUE);
        if (ok)
        {
            USBEPInfo ep;
            ep.addr = pipeInfo.PipeId;
            switch (pipeInfo.PipeType)
            {
            case UsbdPipeTypeControl:     ep.type = USBEPInfo::EP_TYPE_CONTROL; break;
            case UsbdPipeTypeIsochronous: ep.type = USBEPInfo::EP_TYPE_ISOCHRONOUS; break;
            case UsbdPipeTypeBulk:        ep.type = USBEPInfo::EP_TYPE_BULK; break;
            case UsbdPipeTypeInterrupt:   ep.type = USBEPInfo::EP_TYPE_INTERRUPT; break;
            }
            epInfo.push_back(ep);
        }
    }

    return epInfo;
}

std::vector<USBEPInfo> USBDeviceWindows::getEndPoints()
{
    boost::mutex::scoped_lock lock(m_lock);

    return m_epInfo;
}


int USBDeviceWindows::controlTransfer(uint8_t reqType, uint8_t request,
                                      uint16_t value, uint16_t index,
                                      void* data, uint16_t size, int timeout)
{
    boost::mutex::scoped_lock lock(m_lock);

    WINUSB_SETUP_PACKET packet;
    packet.RequestType = reqType;
    packet.Request = request;
    packet.Value = value;
    packet.Index = index;
    packet.Length = size;

    ULONG txSize;
    if (!WinUsb_ControlTransfer(m_interfaceHandle,
                                packet,
                                reinterpret_cast<UCHAR *>(data),
                                size,
                                &txSize,
                                0))
        throw USBException("Control transfer failed");

    // number of bytes received
    return txSize;
}


int USBDeviceWindows::submitReadTransfer(int endpoint, void* data, size_t size)
{
    boost::mutex::scoped_lock lock(m_lock);

    TransferPtr transfer = submitTransfer(endpoint,
                                          data, size);
    return transfer->token;
}


int USBDeviceWindows::submitWriteTransfer(int endpoint, const void* data, size_t size)
{
    boost::mutex::scoped_lock lock(m_lock);

    TransferPtr transfer = submitTransfer(endpoint,
                                          const_cast<void*>(data), size);
    return transfer->token;
}


size_t USBDeviceWindows::pendingTransfers()
{
    boost::mutex::scoped_lock lock(m_lock);

    return m_numInFlightTransfers + m_completedTransfers.size();
}


void USBDeviceWindows::cancelTransfers()
{
    boost::mutex::scoped_lock lock(m_lock);

    // cancel each pending operation
    for (EPTransferPtrDequeMap::iterator epInFlight = m_inFlightTransfers.begin();
         epInFlight != m_inFlightTransfers.end();
         ++epInFlight)
    {
        for (TransferPtrDeque::iterator t = epInFlight->second.begin();
            t != epInFlight->second.end();
            ++t)
        {
            if ((*t)->status == Transfer_IN_PROGRESS)
                ::CancelIoEx(m_handle, &((*t)->overlapped));
        }
    }

    // client will call complete transfer to wait for cancellation
}

int USBDeviceWindows::completeTransfer(Transfer_Status* status, size_t* used)
{
    boost::mutex::scoped_lock lock(m_lock);

    if (m_completedTransfers.empty() && m_numInFlightTransfers > 0)
        completeOneTransfer();

    if (m_completedTransfers.empty())
    {
        // nothing ready
        return -1;
    }

    TransferPtr transfer = m_completedTransfers.front();
    m_completedTransfers.pop();

    int token = transfer->token;

    if (status)
        *status = transfer->status;

    if (used)
        *used = transfer->used;

    transfer.reset();

    return token;
}


USBDeviceWindows::TransferPtr USBDeviceWindows::submitTransfer(uint8_t address,
                                                               void* data,
                                                               size_t len)
{
    // create libusb transfers
    if (len > MAX_URB_SIZE)
        throw USBException("Invalid transfer size");

    TransferPtr transfer(new Transfer(m_nextToken++));

    DWORD bytesRead = 0;
    BOOL res = false;
    if (USB_ENDPOINT_DIRECTION_IN(address))
    {
        res = WinUsb_ReadPipe(m_interfaceHandle, address,
                              reinterpret_cast<UCHAR*>(data), (ULONG)len, &bytesRead,
                              &transfer->overlapped);
    }
    else
    {
        res = WinUsb_WritePipe(m_interfaceHandle, address,
                               reinterpret_cast<UCHAR*>(data), (ULONG)len, &bytesRead,
                               &transfer->overlapped);
    }

    if (res)
    {
        // completed immediately
        transfer->status = Transfer_SUCCESS;
        transfer->used = bytesRead;
    }
    else
    {
        // examine error code to see if deferred or error
        switch (GetLastError())
        {
        case ERROR_IO_PENDING:
            // an async read began
            break;

        default:
            // not pending, therefore a real error to be reported
            throw USBException("Failed to submit transfer");
        }
    }

    m_inFlightTransfers[address].push_back(transfer);
    ++m_numInFlightTransfers;

    return transfer;
}


void USBDeviceWindows::completeOneTransfer()
{
    // build list of handles to wait on - this will contain the handles for
    // first pending transfer for each endpoint with pending transfers
    boost::scoped_array<HANDLE> handles(new HANDLE[m_inFlightTransfers.size()]);
    // used to map completed object index back to endpoint
    boost::scoped_array<int> addresses(new int[m_inFlightTransfers.size()]);
    int numHandles = 0;
    for (EPTransferPtrDequeMap::const_iterator epQ = m_inFlightTransfers.begin();
         epQ != m_inFlightTransfers.end();
         ++epQ)
    {
        if (!epQ->second.empty())
        {
            handles[numHandles] = epQ->second.front()->overlapped.hEvent;
            addresses[numHandles] = epQ->first;
            numHandles++;
        }
    }

    // wait for one of the handles to complete
    m_lock.unlock();
    DWORD dwWaitResult = WaitForMultipleObjects(numHandles, handles.get(), FALSE, INFINITE);
    m_lock.lock();

    if (dwWaitResult >= WAIT_OBJECT_0 && dwWaitResult < (WAIT_OBJECT_0 + numHandles))
    {
        // Get the status of Overlapped RW event

        // map the result to an endpoint address
        int objIndex = dwWaitResult - WAIT_OBJECT_0;
        int address = addresses[objIndex];

        // get the front of the queue for that address
        TransferPtr t = m_inFlightTransfers[address].front();

        // get the result of the transfer
        DWORD nBytes;
        if (!WinUsb_GetOverlappedResult(m_interfaceHandle, &t->overlapped, &nBytes, FALSE))
        {
            // failed - examine the error number to get that failure reason
            DWORD err = GetLastError();
            if (err = ERROR_OPERATION_ABORTED)
                t->status = Transfer_CANCELLED;
            else if (checkForDisconnection())
                // error may be generated when USB device (any device, not just this one) is unplugged)
                t->status = Transfer_ERROR;
            else
                // ignore: may be other device being unplugged
                t->status = Transfer_IN_PROGRESS;
        }
        else
        {
            // successful transfer
            t->status = Transfer_SUCCESS;
            t->used = nBytes;
        }

        // move to completed queue
        if (t->status != Transfer_IN_PROGRESS)
        {
            m_completedTransfers.push(t);
            m_inFlightTransfers[address].pop_front();
            m_numInFlightTransfers--;
        }
    }
    else
    {
        throw USBException("Failed to complete transfer");
    }
}


bool USBDeviceWindows::checkForDisconnection()
{
    // check if device is still available

    if (m_unplugged)
        // we already know it's gone
        return true;

    UCHAR tmp;
    if (!WinUsb_GetCurrentAlternateSetting(m_interfaceHandle, &tmp))
    {
        // call failed - assume this device has been disconnected
        m_unplugged = true;
    }

    if (m_unplugged)
    {
        // device has been unplugged, close it down
        doDisconnect();
        return true;
    }
    else
    {
        // device is still there and responding
        return false;
    }
}


// create a transfer, using RAII for event handle
USBDeviceWindows::Transfer::Transfer(int _token)
    : token(_token),
      status(USBDevice::Transfer_IN_PROGRESS),
      used(0)
{
    memset(&overlapped, 0, sizeof(OVERLAPPED));
    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (overlapped.hEvent == NULL)
        throw USBException("Failed to initialise USB transfer");
}

USBDeviceWindows::Transfer::~Transfer()
{
    ::CloseHandle(overlapped.hEvent);
}

// End of file usb_device_windows.cpp

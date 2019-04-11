// usb_device_windows.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef USB_DEVICE_WINDOWS_H
#define USB_DEVICE_WINDOWS_H

#include "usb_device.h"

#include <vector>
#include <deque>
#include <queue>
#include <map>
#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>
#include <boost/thread/mutex.hpp>

#include <windows.h>
#include <winusb.h>

/**
 * Implementation of USBDevice for Windows
 *
 * Uses WinUSB driver
 */
class USBDeviceWindows : public USBDevice
{
public:
    USBDeviceWindows(const USBDeviceIdentifier* deviceID, const std::string& serialNumber);
    virtual ~USBDeviceWindows();

    virtual size_t asyncTransferSize() const;
    virtual size_t asyncTransferCount() const;

    virtual void connect();
    virtual void disconnect();

    virtual std::vector<USBEPInfo> getEndPoints();

    virtual int controlTransfer(uint8_t reqType, uint8_t request,
                                uint16_t value, uint16_t index,
                                void* data, uint16_t size, int timeout);

    virtual int submitReadTransfer(int endpoint, void* data, size_t size);
    virtual int submitWriteTransfer(int endpoint, const void* data, size_t size);
    virtual size_t pendingTransfers();
    virtual void cancelTransfers();
    virtual int completeTransfer(Transfer_Status* status, size_t* used);

private:
    /*
     * Transfer information
     */
    struct Transfer : private boost::noncopyable
    {
        Transfer(int _token);
        ~Transfer();

        int token; // token allocated to transfer
        OVERLAPPED overlapped; // used to wait on completion
        Transfer_Status status; // status
        size_t used; // number of bytes transferred
    };
    typedef boost::shared_ptr<Transfer> TransferPtr;
    typedef std::deque<TransferPtr> TransferPtrDeque;
    typedef std::map<int, TransferPtrDeque> EPTransferPtrDequeMap;

    void doDisconnect();

    std::vector<USBEPInfo> examineEndpoints(unsigned altSetting);

    TransferPtr submitTransfer(uint8_t address,
                               void* data,
                               size_t len);

    void completeOneTransfer();
    bool checkForDisconnection();

    int getUsbString(WINUSB_INTERFACE_HANDLE deviceHandle,
                     uint8_t index, uint16_t langId,
                     std::string& str);

    boost::mutex m_lock;

    std::string             m_devicePath;
    std::string             m_serialNumber;
    int                     m_interfaceIndex;
    int                     m_altSetting;

    HANDLE                  m_handle;
    WINUSB_INTERFACE_HANDLE m_deviceHandle;
    WINUSB_INTERFACE_HANDLE m_interfaceHandle;

    std::vector<USBEPInfo>  m_epInfo;

    int                     m_nextToken;
    EPTransferPtrDequeMap   m_inFlightTransfers;
    int                     m_numInFlightTransfers;
    std::queue<TransferPtr> m_completedTransfers;
    bool                    m_unplugged;
};

#endif // USB_DEVICE_WINDOWS_H

// End of file usb_device_windows.h

// streaming_trace_usb.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef STREAMING_TRACE_USB_H
#define STREAMING_TRACE_USB_H

#include "streaming_trace_base.h"

#include "usb_device.h"

/**
 * Base implementation of streaming trace over USB
 *
 * Uses uddabs library to collect data from USB endpoints
 *
 * Implementations must define their vendor/product IDs and endpoints used for trace data
 */
class StreamingTraceUSB : public StreamingTraceBase
{
public:
    StreamingTraceUSB();
    virtual ~StreamingTraceUSB();

protected:
    virtual void doConnect();
    virtual void doDisconnect();
    virtual bool isConnected();

    virtual void submitBuffer(int sink, Buffer& buf);
    virtual void doCancelPendingBuffers(int sink);
    virtual bool waitForBuffer(int sink, Buffer& buf);

    virtual std::vector<const USBDeviceIdentifier*> getDeviceIDs() = 0;
    virtual std::string getTargetIdentifier() = 0;
    virtual std::vector<SinkInfo> discoverSinks() = 0;

    virtual void usbSetup();
    virtual void usbTeardown();

    virtual bool completeUsbTransfer(int token, USBDevice::Transfer_Status status, size_t used);

    std::auto_ptr<USBDevice> m_usb;
    std::queue<int> m_dataTransferTokens;
};


#endif // STREAMING_TRACE_USB_H

// End of file streaming_trace_usb.h

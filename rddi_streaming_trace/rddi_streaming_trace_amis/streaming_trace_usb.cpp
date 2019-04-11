// streaming_trace_usb.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "streaming_trace_usb.h"

#include "st_error.h"
#include "usb_device.h"

StreamingTraceUSB::StreamingTraceUSB()
    : m_usb(0)
{
}


StreamingTraceUSB::~StreamingTraceUSB()
{
}


void StreamingTraceUSB::doConnect()
{
    std::vector<const USBDeviceIdentifier*> deviceIDs = getDeviceIDs();

    if (deviceIDs.empty())
        throw StreamingTraceException(RDDI_STREAMING_TRACE_NO_DEVICE, "No device identifiers defined");

    // open USB
    m_usb = USBDevice::create(deviceIDs[0], getTargetIdentifier());

    m_usb->connect();

    usbSetup();
}


void StreamingTraceUSB::doDisconnect()
{
    usbTeardown();

    if (m_usb.get())
    {
        m_usb->disconnect();
        m_usb.reset(0);
    }
}


// called with lock held
bool StreamingTraceUSB::isConnected()
{
    return (m_usb.get() != 0);
}


void StreamingTraceUSB::usbSetup()
{
    // no action here - implementations may override for device specific setup
    // (e.g. alternate interface selection, control transfers etc)
}


void StreamingTraceUSB::usbTeardown()
{
    // no action here - implementations may override for device specific teardown
    // (e.g. alternate interface selection, control transfers etc)
}


void StreamingTraceUSB::submitBuffer(int sink, Buffer& buf)
{
    SinkStatePtr& sinkState = m_sinkState[sink];
    unsigned int endpoint = sinkState->transportID;
    int token = m_usb->submitReadTransfer(endpoint,
                                          buf.pEventBuffer->pBuf,
                                          buf.pEventBuffer->size);
    m_dataTransferTokens.push(token);
}


void StreamingTraceUSB::doCancelPendingBuffers(int sink)
{
    m_usb->cancelTransfers();
}


bool StreamingTraceUSB::waitForBuffer(int sink, Buffer& buf)
{
    USBDevice::Transfer_Status status;
    size_t used;
    // release lock during blocking operation
    m_lock.unlock();
    int token = m_usb->completeTransfer(&status, &used);
    m_lock.lock();
    if (token == -1)
        return false;

    int expDataToken = m_dataTransferTokens.front();

    if (token == expDataToken)
    {
        m_dataTransferTokens.pop();

        RDDIStreamingTraceEventBuffer* buffer = buf.pEventBuffer;

        switch (status)
        {
        case USBDevice::Transfer_SUCCESS:
            buffer->type = RDDI_STREAMING_TRACE_EVENT_TYPE_DATA;
            buffer->used = used;
            break;

        case USBDevice::Transfer_CANCELLED:
            // return empty data packet
            buffer->type = RDDI_STREAMING_TRACE_EVENT_TYPE_DATA;
            buffer->used = 0;
            break;

        case USBDevice::Transfer_ERROR:
        default:
            // TODO: encode error code / message in buffer
            buffer->type = RDDI_STREAMING_TRACE_EVENT_TYPE_ERROR;
            buffer->used = 0;
            break;
        }
        return true;
    }
    else
    {
        // pass to implementation to handle implementation defined messages
        return completeUsbTransfer(token, status, used);
    }
}

bool StreamingTraceUSB::completeUsbTransfer(int token, USBDevice::Transfer_Status status, size_t used)
{
    // handler for implementation defined USB transfers
    // default behaviour is to fail
    return false;
}


// End of file streaming_trace_usb.cpp

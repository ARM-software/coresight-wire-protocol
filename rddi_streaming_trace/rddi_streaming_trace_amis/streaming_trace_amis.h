// streaming_trace_amis.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef STREAMING_TRACE_AMIS_H
#define STREAMING_TRACE_AMIS_H

#include "streaming_trace_usb.h"

#include <vector>

#include <boost/property_tree/ptree.hpp>

#include "coresight_trace.h"

/**
 * Implementation of streaming trace from AMIS PoC FPGA
 */
class StreamingTraceAMIS : public StreamingTraceUSB
{
public:
    StreamingTraceAMIS(const std::string& usbID, const std::string& id);
    ~StreamingTraceAMIS();

    void setBufferSize(size_t bufferSize) { m_bufferSize = bufferSize; }
    void setBufferWM(size_t bufferWM) { m_bufferWM = bufferWM; }

    void setSinkMetadata(const boost::optional<boost::property_tree::ptree&>&  sinkMeta);

private:
    // trace stream endpoint addresses
    struct EndpointInfo
    {
        int addr;
        int attachedSink;
    };
    typedef std::vector<EndpointInfo> EndpointInfoSeq;

    virtual std::vector<const USBDeviceIdentifier*> getDeviceIDs();
    virtual std::vector<SinkInfo> discoverSinks();
    virtual std::string getTargetIdentifier();

    virtual void usbSetup();
    virtual void usbTeardown();

    virtual void attachDevice(int sink);
    virtual void detachDevice(int sink);
    virtual void startDevice(int sink);
    virtual void stopDevice(int sink);
    virtual void doFlush(int sink);
    virtual bool completeUsbTransfer(int token, USBDevice::Transfer_Status status, size_t used);

    void submitControlChannelRequest();

    EndpointInfoSeq::iterator epForSink(int sink);
    int endpointSetParam(uint16_t addr, uint8_t request, uint16_t value);

    unsigned m_vid;
    unsigned m_pid;
    unsigned m_interface;
    std::string m_targetID;
    std::auto_ptr<USBDeviceIdentifier> m_usbID;

    size_t m_bufferSize;
    size_t m_bufferWM;

    EndpointInfoSeq m_traceEndpoints;

    int m_controlEP;
    int m_controlToken;
    cs_trace_ctl_msg m_controlMsg;

    std::map<std::string, std::string> m_sinkMeta;
};

#endif // STREAMING_TRACE_AMIS_H

// End of file streaming_trace_amis.h

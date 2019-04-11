// streaming_trace_amis.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "streaming_trace_amis.h"

#include "st_error.h"

#include "usb_device.h"

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/property_tree/json_parser.hpp>
#include <boost/algorithm/string.hpp>

#ifdef _WIN32
#include "initguid.h"
#endif

#include <sstream>
#include <iomanip>
#include <stdexcept>

namespace
{
    unsigned hex_to_uint(const std::string& s)
    {
        unsigned v;
        std::istringstream str(s);
        str >> std::hex >> v;
        return v;
    }

    unsigned dec_to_uint(const std::string& s)
    {
        unsigned v;
        std::istringstream str(s);
        str >> std::dec >> v;
        return v;
    }

    /**
     * Parse a string of form vid:pid[:interface]
     */
    void parse_usbid(const std::string& usbID, unsigned& vid, unsigned& pid, unsigned& interfaceID)
    {
        std::string::size_type colon1Pos = usbID.find(':');
        if (colon1Pos == std::string::npos)
            throw std::runtime_error("Invalid USB identifier: " + usbID + ", expected VID:PID[:interface]");
        std::string::size_type colon2Pos = usbID.find(':', colon1Pos+1);
        std::string sVID = usbID.substr(0, colon1Pos);
        std::string sPID = usbID.substr(colon1Pos+1, colon2Pos-colon1Pos-1);
        vid = hex_to_uint(sVID);
        pid = hex_to_uint(sPID);

        // extract optional interface number
        if (colon2Pos != std::string::npos)
        {
            std::string sInterface = usbID.substr(colon2Pos+1);
            interfaceID = dec_to_uint(sInterface);
        }
        else
        {
            interfaceID = 0;
        }
    }

    // ETR parameters: 1Mb buffer, interrupt when 128k full
    const unsigned DEFAULT_ETR_BUFFER_SIZE = 1024 * 1024;
    const unsigned DEFAULT_ETR_WATERMARK = DEFAULT_ETR_BUFFER_SIZE - 128 * 1024;
    const unsigned ETR_TX_TIMEOUT = 100;

#ifdef WIN32
    DEFINE_GUID(AMISFPGAGuidConstant, 0x4A52C629, 0xCCE2, 0x4BCB, 0x9A, 0x34, 0xB9, 0x9D, 0x5D, 0xC9, 0xBD, 0x45);
    GUID AMISFPGAGuid = AMISFPGAGuidConstant;
#endif
}

StreamingTraceAMIS::StreamingTraceAMIS(const std::string& usbID, const std::string& targetID)
    : m_targetID(targetID),
      m_bufferSize(DEFAULT_ETR_BUFFER_SIZE),
      m_bufferWM(DEFAULT_ETR_WATERMARK),
      m_controlToken(-1)
{

#ifdef WIN32
    // extract VID, PID and optional interface from usbID
    m_interface = 0;
    USBDeviceIdentifier* USBID = new USBDeviceIdentifier(&AMISFPGAGuid, m_interface);
#else
    // extract VID, PID and optional interface from usbID
    parse_usbid(usbID, m_vid, m_pid, m_interface);
    USBDeviceIdentifier* USBID = new USBDeviceIdentifier(m_vid, m_pid, m_interface);
#endif

    m_usbID.reset(USBID);
}


StreamingTraceAMIS::~StreamingTraceAMIS()
{

}


std::vector<const USBDeviceIdentifier*> StreamingTraceAMIS::getDeviceIDs()
{
    std::vector<const USBDeviceIdentifier*> devIDs;
    devIDs.push_back(m_usbID.get());
    return devIDs;
}


std::vector<StreamingTraceBase::SinkInfo> StreamingTraceAMIS::discoverSinks()
{
    std::vector<SinkInfo> sinks;
    char nameBuf[256];

    // use ctrl transfers to get all ETR names
    // call with increasing index until empty name is returned
    for (int i = 0; ; ++i)
    {
        try
        {
            int res = m_usb->controlTransfer(USBDevice::CONTROL_DIR_IN | USBDevice::CONTROL_TYPE_VENDOR | USBDevice::CONTROL_RECIPIENT_INTERFACE,
                                             USB_CSTRACE_INTF_REQ_STREAM_TMC_INFO,
                                             i, m_interface,
                                             (uint8_t*)nameBuf, sizeof(nameBuf)-1, 1000);
            if (res <= 0)
                // all ETRs discovered
                break;

            // ensure null terminated
            nameBuf[sizeof(nameBuf)-1] = '\0';
            nameBuf[res] = '\0';

            std::string etrName(nameBuf);
            if (etrName == "")
                // empty name indicates all ETRs discovered
                break;

            SinkInfo sink;
            sink.details.name = etrName;
            sink.details.metadata = (m_sinkMeta.find(etrName) == m_sinkMeta.end() ? "" : m_sinkMeta.at(etrName));
            sink.details.dataBufferCount = m_usb->asyncTransferCount();
            sink.details.dataBufferSize = m_usb->asyncTransferSize();
            sink.details.eventBufferCount = 1;
            sink.details.eventBufferSize = 256;
            sinks.push_back(sink);
        }
        catch (const USBException&)
        {
            break;
        }
    }

    return sinks;
}

std::string StreamingTraceAMIS::getTargetIdentifier()
{
    return m_targetID;
}



void StreamingTraceAMIS::usbSetup()
{
    // get available endpoints
    m_traceEndpoints.clear();
    std::vector<USBEPInfo> epInfo = m_usb->getEndPoints();
    for (std::vector<USBEPInfo>::const_iterator ep = epInfo.begin();
         ep != epInfo.end();
         ++ep)
    {
        if ((ep->addr & USBEPInfo::EP_DIR_MASK) == USBEPInfo::EP_DIR_IN &&
            ep->type == USBEPInfo::EP_TYPE_BULK)
        {
            EndpointInfo e;
            e.addr = ep->addr;
            e.attachedSink = -1;
            m_traceEndpoints.push_back(e);
        }
        else if ((ep->addr & USBEPInfo::EP_DIR_MASK) == USBEPInfo::EP_DIR_IN &&
                 ep->type == USBEPInfo::EP_TYPE_INTERRUPT)
        {
            m_controlEP = ep->addr;
        }
    }
}


void StreamingTraceAMIS::usbTeardown()
{
}

void StreamingTraceAMIS::attachDevice(int sink)
{
    // validate buffer parameters
    if (m_bufferSize % 4096 != 0)
        throw USBException("Buffer size must be a multiple of 4096");
    if (m_bufferSize > 0xFFFF000)
        throw USBException("Buffer size must be less than 0xFFFF000");
    if (m_bufferWM % 4096 != 0)
        throw USBException("Buffer watermark must be a multiple of 4096");
    if (m_bufferWM >= m_bufferSize)
        throw USBException("Buffer watermark must be less than buffer size");

    // find free endpoint and attach to ETR
    std::vector<EndpointInfo>::iterator attachEP = m_traceEndpoints.end();
    for (std::vector<EndpointInfo>::iterator e = m_traceEndpoints.begin();
         e != m_traceEndpoints.end();
         ++e)
    {
        if (e->attachedSink == -1)
        {
            attachEP = e;
            break;
        }
    }

    if (attachEP == m_traceEndpoints.end())
        throw USBException("No free trace stream for ETR");

    int res = endpointSetParam(attachEP->addr, USB_CSTRACE_EP_REQ_SET_TMC, sink);
    if (res < 0)
        throw USBException("Failed to set ETR endpoint");

    res = endpointSetParam(attachEP->addr, USB_CSTRACE_EP_REQ_SET_TX_TIMEOUT, ETR_TX_TIMEOUT);
    if (res < 0)
        throw USBException("Failed to set ETR endpoint timeout");

    uint16_t bufferBlocks = (uint16_t)((m_bufferSize / 4096) & 0xFFFF);
    res = endpointSetParam(attachEP->addr, USB_CSTRACE_EP_REQ_SET_BUF_SIZE, bufferBlocks);
    if (res < 0)
        throw USBException("Failed to set ETR buffer size");

    uint16_t wmBlocks = (uint16_t)((m_bufferWM / 4096) & 0xFFFF);
    res = endpointSetParam(attachEP->addr, USB_CSTRACE_EP_REQ_SET_WATER_MARK, wmBlocks);
    if (res < 0)
        throw USBException("Failed to set ETR buffer size");

    res = endpointSetParam(attachEP->addr, USB_CSTRACE_EP_REQ_ATTACH_STREAM, 0);
    if (res < 0)
        throw USBException("Failed to attach ETR");

    submitControlChannelRequest();

    // successfully attached : update state
    attachEP->attachedSink = sink;

    SinkStatePtr& sinkState = m_sinkState[sink];
    sinkState->transportID = attachEP->addr;
}

StreamingTraceAMIS::EndpointInfoSeq::iterator StreamingTraceAMIS::epForSink(int sink)
{
    int epAddr = -1;
    for (std::vector<EndpointInfo>::iterator e = m_traceEndpoints.begin();
         e != m_traceEndpoints.end();
         ++e)
    {
        if (e->attachedSink == sink)
            return e;
    }

    return m_traceEndpoints.end();
}

void StreamingTraceAMIS::detachDevice(int sink)
{
    EndpointInfoSeq::iterator attachedEP = epForSink(sink);

    if (attachedEP != m_traceEndpoints.end())
    {
        int res = endpointSetParam(attachedEP->addr, USB_CSTRACE_EP_REQ_DETACH_STREAM, 0);
        if (res < 0)
        {
            // TODO: warn but continue
        }
        attachedEP->attachedSink = -1;
    }

    // control channel request will be cancelled by doCancelPendingBuffers()
    // need to complete pending transfers to free resources
    while (m_usb->pendingTransfers() > 0)
    {
        USBDevice::Transfer_Status status;
        size_t used;
        int token = m_usb->completeTransfer(&status, &used);
        if (token == -1)
            break;
    }
}


void StreamingTraceAMIS::startDevice(int sink)
{
    EndpointInfoSeq::iterator ep = epForSink(sink);

    if (ep == m_traceEndpoints.end())
        throw USBException("No endpoint attached to sink");

    if (m_controlToken = -1)
        submitControlChannelRequest();

    int res = endpointSetParam(ep->addr, USB_CSTRACE_EP_REQ_START_STREAM, 0);
    if (res < 0)
        throw USBException("Failed to start ETR");
}


void StreamingTraceAMIS::stopDevice(int sink)
{
}


void StreamingTraceAMIS::doFlush(int sink)
{
    EndpointInfoSeq::iterator ep = epForSink(sink);

    if (ep == m_traceEndpoints.end())
        throw USBException("No endpoint attached to sink");

    // Stop the trace capture on the target
    // this will flush the ETR and initiate the stop sequence
    // when all data is received, CS_STREAM_END_SESSION will be sent
    int res = endpointSetParam(ep->addr, USB_CSTRACE_EP_REQ_STOP_STREAM, 0);
    if (res < 0)
        throw USBException("Failed to stop ETR");
}


bool StreamingTraceAMIS::completeUsbTransfer(int token, USBDevice::Transfer_Status status, size_t used)
{
    if (token == m_controlToken)
    {
        if (status == USBDevice::Transfer_SUCCESS &&
            used >= sizeof(m_controlMsg))
        {
            int sink = m_controlMsg.streamIdx;

            switch (m_controlMsg.status)
            {
            case CS_STREAM_END_SESSION:
                doSendStateEvent(sink, RDDI_STREAMING_TRACE_EVENT_TYPE_END_OF_DATA);
                break;

            case CS_STREAM_DETACHED:
                // TODO: indicate detached to detachDevice()
                break;

            case CS_STREAM_BUSY:
            case CS_STREAM_ATTACHED:
            case CS_STREAM_DETACHING:
            case CS_STREAM_PREPARE_SESSION:
                // no action on these
                break;
            }

            submitControlChannelRequest();
        }
        else
            m_controlToken = -1;
    }

    return false;
}

void StreamingTraceAMIS::submitControlChannelRequest()
{
    m_controlToken = m_usb->submitReadTransfer(m_controlEP, (uint8_t*)&m_controlMsg, sizeof(m_controlMsg));
}

int StreamingTraceAMIS::endpointSetParam(uint16_t addr, uint8_t request, uint16_t value)
{
    return m_usb->controlTransfer(USBDevice::CONTROL_DIR_OUT | USBDevice::CONTROL_TYPE_VENDOR | USBDevice::CONTROL_RECIPIENT_ENDPOINT,
                                  request, value, addr,
                                  0, 0, 1000);
}

void StreamingTraceAMIS::setSinkMetadata(const boost::optional<boost::property_tree::ptree&>&  sinkMeta)
{
    if (sinkMeta)
    {
        for (boost::property_tree::ptree::iterator sink = sinkMeta->begin(); sink != sinkMeta->end(); ++sink)
        {
            std::string sinkName = sink->second.get<std::string>("<xmlattr>.name");
            boost::optional<boost::property_tree::ptree&> sinkAttrs = sink->second.get_child_optional("<xmlattr>");
            if (sinkAttrs)
            {
                sinkAttrs->erase("name");
                std::ostringstream ss;
                boost::property_tree::json_parser::write_json(ss, sinkAttrs.get());
                m_sinkMeta.insert(std::pair<std::string, std::string>(sinkName, ss.str()));
            }
        }
    }
}

// factory method to create instance
StreamingTraceBase* CreateStreamingTrace(const std::string &xmlFile)
{
    try
    {
        boost::property_tree::ptree tree;
        boost::property_tree::read_xml(xmlFile, tree);
        std::string targetType = tree.get<std::string>("config.target.<xmlattr>.type", "amis");
        std::string targetUSBID = tree.get<std::string>("config.target.<xmlattr>.usbid", "05c0:0002");
        std::string targetAddr = tree.get<std::string>("config.target.<xmlattr>.address", "");
        if (!boost::iequals(targetType, "amis"))
            throw StreamingTraceException(RDDI_BADARG, "Unsupported target type " + targetType);

        StreamingTraceAMIS* st = new StreamingTraceAMIS(targetUSBID, targetAddr);

        size_t bufferSize = tree.get<size_t>("config.target.<xmlattr>.bufsz", 0);
        if (bufferSize != 0)
            st->setBufferSize(bufferSize);
        size_t bufferWM = tree.get<size_t>("config.target.<xmlattr>.bufwm", 0);
        if (bufferWM != 0)
            st->setBufferWM(bufferWM);

        st->setSinkMetadata(tree.get_child_optional("config.sinks"));

        return st;
    }
    catch (const std::exception& e)
    {
        throw StreamingTraceException(RDDI_PARSE_FAILED, std::string("Failed to parse config file: ") + e.what());
    }
}

// End of file streaming_trace_amis.cpp

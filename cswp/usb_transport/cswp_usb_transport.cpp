// cswp_usb_transport.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "cswp_client.h"

#include "usb_device.h"

#include <stdexcept>
#include <cstring>

#ifdef _WIN32
#include "initguid.h"
namespace
{
    DEFINE_GUID(AMISFPGACSWPGuidConstant, 0x4A52C629, 0xCCE2, 0x4BCB, 0x9A, 0x34, 0xB9, 0x9D, 0x5D, 0xC9, 0xBD, 0x46);
    GUID AMISFPGACSWPGuid = AMISFPGACSWPGuidConstant;
}
#endif

class CSWPUSBClient
{
public:
    CSWPUSBClient(const char* serialNumber);
    ~CSWPUSBClient();

    void connect();
    void disconnect();

    int send(const void* data, size_t size);
    int receive(void* data, size_t size, size_t* used);

private:
    std::string m_serialNumber;

    std::auto_ptr<USBDevice> m_usb;
    int m_epCmd;
    int m_epRsp;
};

static int cswp_usb_connect(cswp_client_t* client, cswp_client_transport_t* transport)
{
    try
    {
        CSWPUSBClient* usbClient = reinterpret_cast<CSWPUSBClient*>(transport->priv);
        usbClient->connect();
    }
    catch (const std::exception& e)
    {
        return cswp_client_error(client, CSWP_COMMS, e.what());
    }

    return CSWP_SUCCESS;
}

static int cswp_usb_disconnect(cswp_client_t* client, cswp_client_transport_t* transport)
{
    if (transport->priv)
    {
        // use auto ptr to ensure client is destroyed on exit
        std::auto_ptr<CSWPUSBClient> usbClient(reinterpret_cast<CSWPUSBClient*>(transport->priv));
        transport->priv = NULL;

        try
        {
            usbClient->disconnect();
        }
        catch (const std::exception& e)
        {
            return cswp_client_error(client, CSWP_COMMS, e.what());
        }
    }

    return CSWP_SUCCESS;
}

static int cswp_usb_send(cswp_client_t* client, cswp_client_transport_t* transport, const void* data, size_t size)
{
    CSWPUSBClient* usbClient = reinterpret_cast<CSWPUSBClient*>(transport->priv);

    try
    {
        return usbClient->send(data, size);
    }
    catch (const std::exception& e)
    {
        return cswp_client_error(client, CSWP_COMMS, e.what());
    }
}

static int cswp_usb_receive(cswp_client_t* client, cswp_client_transport_t* transport, void* data, size_t size, size_t* used)
{
    CSWPUSBClient* usbClient = reinterpret_cast<CSWPUSBClient*>(transport->priv);

    try
    {
        return usbClient->receive(data, size, used);
    }
    catch (const std::exception& e)
    {
        return cswp_client_error(client, CSWP_COMMS, e.what());
    }
}

void cswp_client_usb_transport_init(cswp_client_transport_t* transport,
                                    const char* serialNumber)
{
    transport->connect = cswp_usb_connect;
    transport->disconnect = cswp_usb_disconnect;
    transport->send = cswp_usb_send;
    transport->receive = cswp_usb_receive;

    transport->priv = new CSWPUSBClient(serialNumber);
}

CSWPUSBClient::CSWPUSBClient(const char* serialNumber)
    : m_serialNumber(serialNumber),
      m_epCmd(-1),
      m_epRsp(-1)
{
}


CSWPUSBClient::~CSWPUSBClient()
{
}

void CSWPUSBClient::connect()
{
#ifdef _WIN32
    USBDeviceIdentifier USBID(&AMISFPGACSWPGuid, 0);
#else
    USBDeviceIdentifier USBID(0x05c0, 0x0002, 1);
#endif
    m_usb = USBDevice::create(&USBID, m_serialNumber);

    m_usb->connect();

    // identify endpoints
    std::vector<USBEPInfo> epInfo = m_usb->getEndPoints();
    for (std::vector<USBEPInfo>::const_iterator ep = epInfo.begin();
         ep != epInfo.end();
         ++ep)
    {
        if ((ep->addr & USBEPInfo::EP_DIR_MASK) == USBEPInfo::EP_DIR_OUT &&
            ep->type == USBEPInfo::EP_TYPE_BULK)
        {
            m_epCmd = ep->addr;
        }
        else if ((ep->addr & USBEPInfo::EP_DIR_MASK) == USBEPInfo::EP_DIR_IN &&
                 ep->type == USBEPInfo::EP_TYPE_BULK)
        {
            m_epRsp = ep->addr;
        }
    }

    if (m_epCmd == -1)
        throw std::runtime_error("Failed to find command endpoint");
    if (m_epRsp == -1)
        throw std::runtime_error("Failed to find response endpoint");
}


void CSWPUSBClient::disconnect()
{
    m_usb->disconnect();
}


int CSWPUSBClient::send(const void* data, size_t size)
{
    int res = CSWP_SUCCESS;

    int cmdToken = m_usb->submitWriteTransfer(m_epCmd, data, size);

    while (1)
    {
        USBDevice::Transfer_Status status;
        size_t used;
        int token = m_usb->completeTransfer(&status, &used);

        if (token == cmdToken)
        {
            if (status != USBDevice::Transfer_SUCCESS || used < size)
                throw std::runtime_error("Failed to send command");
            return CSWP_SUCCESS;
        }
    }

    return CSWP_COMMS;
}

int CSWPUSBClient::receive(void* data, size_t size, size_t* used)
{
    // TODO: run on other thread for async responses
    int rspToken = m_usb->submitReadTransfer(m_epRsp, data, size);

    // wait for command and response to complete
    while (1)
    {
        USBDevice::Transfer_Status status;
        size_t txUsed;
        int token = m_usb->completeTransfer(&status, &txUsed);

        if (token == rspToken)
        {
            if (status != USBDevice::Transfer_SUCCESS)
                throw std::runtime_error("Failed to receive response");
            *used = txUsed;
            return CSWP_SUCCESS;
        }

    }

    return CSWP_COMMS;
}

// cswp_tcp_transport.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include <memory>

#ifdef _WIN32
#include <winsock2.h>
#include <comdef.h>

#include "boost/format.hpp"
#endif

#include "transport_exception.h"
#include "cswp_client.h"
#include "tcp_device.h"

class CSWPTCPClient
{
public:
    CSWPTCPClient(const char* addr, int port);
    ~CSWPTCPClient();

    void connect();
    void disconnect();

    int send(const void* data, size_t size);
    int receive(void* data, size_t size, size_t* used);

private:
    const char* m_addr;
    int m_port;

    std::auto_ptr<TCPDevice> m_tcp;
};

static int cswp_tcp_connect(cswp_client_t* client, cswp_client_transport_t* transport)
{
    try
    {
        CSWPTCPClient* tcpClient = reinterpret_cast<CSWPTCPClient*>(transport->priv);
        tcpClient->connect();
    }
    catch (const std::exception& e)
    {
        return cswp_client_error(client, CSWP_COMMS, e.what());
    }

    return CSWP_SUCCESS;
}

static int cswp_tcp_disconnect(cswp_client_t* client, cswp_client_transport_t* transport)
{
    if (transport->priv)
    {
        // use auto ptr to ensure client is destroyed on exit
        std::auto_ptr<CSWPTCPClient> tcpClient(reinterpret_cast<CSWPTCPClient*>(transport->priv));
        transport->priv = NULL;

        try
        {
            tcpClient->disconnect();
        }
        catch (const std::exception& e)
        {
            return cswp_client_error(client, CSWP_COMMS, e.what());
        }
    }

    return CSWP_SUCCESS;
}

static int cswp_tcp_send(cswp_client_t* client, cswp_client_transport_t* transport, const void* data, size_t size)
{
    CSWPTCPClient* tcpClient = reinterpret_cast<CSWPTCPClient*>(transport->priv);

    try
    {
        return tcpClient->send(data, size);
    }
    catch (const std::exception& e)
    {
        return cswp_client_error(client, CSWP_COMMS, e.what());
    }
}

static int cswp_tcp_receive(cswp_client_t* client, cswp_client_transport_t* transport, void* data, size_t size, size_t* used)
{
    CSWPTCPClient* tcpClient = reinterpret_cast<CSWPTCPClient*>(transport->priv);

    try
    {
        return tcpClient->receive(data, size, used);
    }
    catch (const std::exception& e)
    {
        return cswp_client_error(client, CSWP_COMMS, e.what());
    }
}

void cswp_client_tcp_transport_init(cswp_client_transport_t* transport,
                                    const char* addr,
                                    int port)
{
    transport->connect = cswp_tcp_connect;
    transport->disconnect = cswp_tcp_disconnect;
    transport->send = cswp_tcp_send;
    transport->receive = cswp_tcp_receive;

    transport->priv = new CSWPTCPClient(addr, port);
}

CSWPTCPClient::CSWPTCPClient(const char* addr, const int port)
    : m_addr(addr),
      m_port(port)
{
#ifdef _WIN32
    const int reqWinsockVer = 2;
    WSADATA wsaData = {0};

    int err = WSAStartup(MAKEWORD(reqWinsockVer, 0), &wsaData);
    if (err)
        throw TransportException((boost::format("Error in WSAStartup=%1%: %2%") % err % _com_error(err).ErrorMessage()).str());

    if (LOBYTE(wsaData.wVersion) < reqWinsockVer)
    {
        WSACleanup();
        throw TransportException("Required winsock version not available");
    }
#endif
}


CSWPTCPClient::~CSWPTCPClient()
{
#ifdef _WIN32
    WSACleanup();
#endif
}

void CSWPTCPClient::connect()
{
    m_tcp = std::auto_ptr<TCPDevice>(new TCPDevice(m_addr, m_port));
}


void CSWPTCPClient::disconnect()
{
    m_tcp->disconnect();
}


int CSWPTCPClient::send(const void* data, size_t size)
{
    if (!data)
        return CSWP_BAD_ARGS;

    m_tcp->write(data, size);
    return CSWP_SUCCESS;
}

int CSWPTCPClient::receive(void* data, size_t maxSize, size_t* used)
{
    if (!used || !data)
        return CSWP_BAD_ARGS;

    *used = m_tcp->read(data, maxSize);
    return CSWP_SUCCESS;
}


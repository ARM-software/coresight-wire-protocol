// tcp_device.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include <cstdio>
#include <cstring>
#include <cerrno>

#ifdef _WIN32
#include <winsock2.h>
#include <ws2tcpip.h>
#include <basetsd.h>
#include <comdef.h>

typedef SSIZE_T ssize_t;

#define close closesocket

#define SOCKERR WSAGetLastError()
#define GETSOCKERR_MSG(x) _com_error(x).ErrorMessage()

#else // linux
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

#define SOCKERR errno
#define GETSOCKERR_MSG(x) strerror(x)

static const int INVALID_SOCKET = -1;
#endif

#include "boost/format.hpp"

#include "common_tcp.h"
#include "tcp_device.h"
#include "transport_exception.h"

using boost::format;

static void throwEx(char* fn, int err)
{
    throw TransportException((format("Error during %1%, system error code=%2%: %3%") % fn % err % GETSOCKERR_MSG(err)).str());
}

class AddrInfo
{
public:
    AddrInfo(const char* addr, int port)
    {
        struct addrinfo hints = {0};

        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_STREAM;

        char p[31] = {0};
        sprintf(p, "%d", port);

        int err = getaddrinfo(addr, p, &hints, &m_res);
        if (err)
            throw TransportException((format("Error during getaddrinfo(), system error code=%1%: %2%") % err % gai_strerror(err)).str());
    }

    ~AddrInfo()
    {
        freeaddrinfo(m_res);
    }

    struct addrinfo* m_res;
};

TCPDevice::TCPDevice(const char* addr, int port)
    : m_sockfd(INVALID_SOCKET)
{
    // validate
    struct in_addr ip = {0};
    if (!inet_pton(AF_INET, addr, &ip))
        throw TransportException("Invalid IPv4 address for TCPDevice");

    if (port <= 0 || port > 65535)
        throw TransportException("Invalid network port for TCPDevice");

    // connect
    AddrInfo ai(addr, port);
    struct addrinfo* res = ai.m_res;

    m_sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (m_sockfd == INVALID_SOCKET)
        throwEx("socket", SOCKERR);

    if (connect(m_sockfd, res->ai_addr, res->ai_addrlen))
    {
        int err = SOCKERR;
        close(m_sockfd);
        throwEx("connect", err);
    }
}

void TCPDevice::write(const void* data, size_t sz)
{
    if (cswp_write_msg_tcp(m_sockfd, data, sz) == -1)
        throwEx("write", SOCKERR);
}

size_t TCPDevice::read(void* data, size_t sz)
{
    ssize_t bytesRead = cswp_read_msg_tcp(m_sockfd, data, sz);
    if (bytesRead == -1)
        throwEx("read", SOCKERR);
    else if (bytesRead == 0)
        throw TransportException("Error during read, connection was shut down on other end");

    return static_cast<size_t>(bytesRead);
}

void TCPDevice::disconnect()
{
    close(m_sockfd);
    m_sockfd = INVALID_SOCKET;
}

TCPDevice::~TCPDevice()
{
    disconnect();
}

// End of file tcp_device.cpp


// tcp_client_test.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include "doctest.h"

#include "fff.h"
DEFINE_FFF_GLOBALS;

#include <cstdlib>
#include <climits>
#include <cerrno>

#ifdef _WIN32
#include <windows.h>
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;

extern "C"
{
FAKE_VALUE_FUNC(int, socket, int, int, int);
FAKE_VALUE_FUNC(int, connect, int, const void*, int);
FAKE_VALUE_FUNC(int, recv, int, char*, int, int);
FAKE_VALUE_FUNC(int, send, int, const char*, int, int);
FAKE_VALUE_FUNC(int, closesocket, int);
FAKE_VALUE_FUNC(int, getaddrinfo, PCSTR, PCSTR, const void*, void**);
FAKE_VOID_FUNC(freeaddrinfo, void*);
FAKE_VALUE_FUNC(int, inet_pton, int, PCSTR, PVOID);
FAKE_VOID_FUNC(WSASetLastError, int);
FAKE_VALUE_FUNC(int, WSAGetLastError);
}

#define read_fake recv_fake
#define write_fake send_fake
#define read_reset recv_reset
#define write_reset send_reset

struct ai_fake
{
    int ai_family;
    int ai_socktype;
    int ai_protocol;
};

static ai_fake aiFake;
int getaddrinfo_nice_obj(PCSTR a, PCSTR b, const void* c, void** d)
{
    *d = &aiFake;
    return 0;
}

#else // linux

extern "C"
{
FAKE_VALUE_FUNC(int, socket, int, int, int);
FAKE_VALUE_FUNC(int, connect, int, void*, socklen_t);
FAKE_VALUE_FUNC(ssize_t, read, int, void*, size_t);
FAKE_VALUE_FUNC(ssize_t, write, int, const void*, size_t);
}
#endif

#include "tcp_device.h"


TEST_CASE("TCPDevice::TCPDevice - bad args")
{
    REQUIRE_THROWS(TCPDevice("I'm not an IP address", 1234));
    REQUIRE_THROWS(TCPDevice("300.300.300.300", 1234));
    REQUIRE_THROWS(TCPDevice("10.10.10.10", 0));
    REQUIRE_THROWS(TCPDevice("10.10.10.10", -1));
    REQUIRE_THROWS(TCPDevice("10.10.10.10", INT32_MAX));
}


TEST_CASE("TCPDevice::TCPDevice - bad socket")
{
    socket_fake.return_val = -1;
    REQUIRE_THROWS(TCPDevice("127.0.0.1", 8888));
    RESET_FAKE(socket);
}


TEST_CASE("TCPDevice::TCPDevice - nobody listening")
{
    connect_fake.return_val = -1;
    REQUIRE_THROWS(TCPDevice("127.0.0.1", 8888));
}


TEST_CASE("TCPDevice::TCPDevice")
{
#ifdef _WIN32
    inet_pton_fake.return_val = 1;
    getaddrinfo_fake.custom_fake = getaddrinfo_nice_obj;
#endif
    uint32_t len = 8;
    void* buf = (void*)(&len);
    connect_fake.return_val = 0;
    TCPDevice tcp("127.0.0.1", 8888);

    read_fake.return_val = 4;
    tcp.read(buf, len);

    read_fake.return_val = 0;
    REQUIRE_THROWS(tcp.read(buf, len));

    read_fake.return_val = -1;
    REQUIRE_THROWS(tcp.read(NULL, len));

    write_fake.return_val = 4;
    tcp.write(buf, len);

    write_fake.return_val = -1;
    REQUIRE_THROWS(tcp.write(NULL, len));
}


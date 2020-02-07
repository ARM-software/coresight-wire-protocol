// tcp_device.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef TCP_DEVICE_H
#define TCP_DEVICE_H

#include <cstdlib>

/**
 * TCP device interface
 */
class TCPDevice
{
public:
    /**
     * Create a TCP device and open a connection
     *
     * An IPv4 address and network port will be given to attach to a specific instance of a
     * device.
     *
     * @param addr IPv4 address of debug target agent
     * @param Network port used by debug target agent
     */
    TCPDevice(const char* addr, int port);

    virtual ~TCPDevice();

    /**
     * Close the connection to the device
     */
    void disconnect();

    void write(const void*, size_t);
    size_t read(void* data, size_t sz);

private:
    int m_sockfd;
};

#endif // TCP_DEVICE_H

// End of file tcp_device.h


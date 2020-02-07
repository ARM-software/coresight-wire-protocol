// common_tcp.c
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#define _DEFAULT_SOURCE

#include <stdint.h>
#include <errno.h>

#ifdef _WIN32
#include <winsock2.h>
#include <basetsd.h>
typedef SSIZE_T ssize_t;

static_assert(-1 == SOCKET_ERROR, "ssize_t can't store SOCKET_ERROR");

static ssize_t read(int fd, void* vptr, size_t n)
{
    WSASetLastError(0);
    return recv(fd, vptr, n, 0);
}

static ssize_t write(int fd, void* vptr, size_t n)
{
    WSASetLastError(0);
    return send(fd, vptr, n, 0);
}

#else // linux
#include <unistd.h>
#endif

#include "common_tcp.h"

typedef uint32_t CSWP_MSG_LEN;

static uint32_t cswp_common_tcp_get_uint32(uint8_t* buf)
{
    uint32_t v = buf[0];
    v |= buf[1] << 8;
    v |= buf[2] << 16;
    v |= buf[3] << 24;
    return v;
}

ssize_t cswp_readn(int fd, void* vptr, size_t n)
{
    errno = 0;
    size_t nleft = n;
    ssize_t nread = 0;
    char* ptr = vptr;

    while (nleft > 0)
    {
        if ((nread = read(fd, ptr, nleft)) < 0)
        {
            if (errno == EINTR)
                nread = 0;      /* and call read() again */
            else
                return -1;
        }
        else if (nread == 0)
            break;              /* EOF */

        nleft -= nread;
        ptr += nread;
    }
    return (n - nleft);         /* return >= 0 */
}

ssize_t cswp_read_msg_tcp(int fd, void* buf, size_t maxSz)
{
    size_t hdrLen = sizeof(CSWP_MSG_LEN);
    ssize_t nread = cswp_readn(fd, buf, hdrLen);
    if (nread == -1)
        return -1;
    else if (nread != hdrLen)
        return 0;

    uint32_t msgLen = cswp_common_tcp_get_uint32(buf);

    /* This will be validated during CSWP msg parsing */
    if (msgLen > maxSz)
        msgLen = maxSz;

    nread = cswp_readn(fd, (CSWP_MSG_LEN*)buf + 1, msgLen - hdrLen);
    if (nread == -1)
        return -1;
    else if (nread != msgLen - hdrLen)
        return 0;

    return nread + hdrLen;
}

ssize_t cswp_write_msg_tcp(int fd, const void* vptr, size_t n)
{
    errno = 0;
    size_t nleft = n;
    ssize_t nwritten = 0;
    const char* ptr = vptr;

    while (nleft > 0)
    {
        if ((nwritten = write(fd, ptr, nleft)) <= 0)
        {
            if (nwritten < 0 && errno == EINTR)
                nwritten = 0;   /* and call write() again */
            else
                return -1;    /* error */
        }

        nleft -= nwritten;
        ptr += nwritten;
    }
    return n;
}


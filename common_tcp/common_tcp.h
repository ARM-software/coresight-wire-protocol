// common_tcp.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef COMMON_TCP_H
#define COMMON_TCP_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <stdlib.h>

/* These functions assume buf to be a 32-bit aligned buffer */
/* They return -1 on error and set errno. Otherwise, return num bytes r/w */
ssize_t cswp_read_msg_tcp(int fd, void* vptr, size_t n);
ssize_t cswp_write_msg_tcp(int fd, const void* vptr, size_t sz);

#ifdef __cplusplus
}
#endif

#endif


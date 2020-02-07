// cswp_tcp_transport.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef CSWP_TCP_TRANSPORT_H
#define CSWP_TCP_TRANSPORT_H

#include "cswp_client.h"

void cswp_client_tcp_transport_init(cswp_client_transport_t* transport,
                                    const char* addr,
                                    int port);

#endif // CSWP_TCP_TRANSPORT_H


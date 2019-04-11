// cswp_impl.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef CSWP_IMPL_H
#define CSWP_IMPL_H

#include "cswp_server_types.h"

/*
 * Implementation defined functions required by server
 */
extern const cswp_server_impl_t cswpServerImpl;

/*
 * Logging functions also used in main file
 */
extern int verbose;
#define V_INFO 1
#define V_DEBUG 2
#define V_TRACE 3

void setup_logging(int level, const char* filename);
void close_logging();
void vlog(int level, const char* msg, ...);

#endif // CSWP_IMPL_H

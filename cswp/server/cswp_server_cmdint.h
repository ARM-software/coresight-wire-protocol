// cswp_server_cmdint.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/**
 * @file cswp_server_cmdint.h
 * @brief CSWP server command handler
 */

#ifndef CSWP_SERVER_CMDINT_H
#define CSWP_SERVER_CMDINT_H

#include "cswp_types.h"
#include "cswp_server_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Handle a CSWP command
 *
 * @param state The server state
 * @param cmd The buffer containing the command
 * @param rsp The buffer to encode the response to
 */
int cswp_handle_command(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp);

#ifdef __cplusplus
}
#endif

#endif /* CSWP_SERVER_CMDINT_H */

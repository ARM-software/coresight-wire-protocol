// cswp_server_impl.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/**
 * @file cswp_server_impl.h
 * @brief CSWP server implementation
 */

#ifndef CSWP_SERVER_IMPL_H
#define CSWP_SERVER_IMPL_H

#include "cswp_server_types.h"

#include <stdlib.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise CSWP server
 *
 * @param state The server state
 */
void cswp_server_init(cswp_server_state_t* state);

/**
 * Shutdown CSWP server
 *
 * Ensure resources are released
 *
 * @param state The server state
 */
void cswp_server_term(cswp_server_state_t* state);

/**
 * Clear all internal structures used to handle devices in CSWP server
 *
 * @param state The server state
 */

void cswp_server_clear_devices(cswp_server_state_t* state);

/**
 * Setup all internal structures used to handle devices in CSWP server
 *
 * @param state The server state
 * @param deviceCount The number of devices
 */
void cswp_server_init_devices(cswp_server_state_t* state, unsigned deviceCount);

/**
 * Set information for a specific device
 *
 * @param state The server state
 * @param index index of the device
 * @param deviceName Name of the device
 * @param deviceType Type of the device
 */
int cswp_server_set_device(cswp_server_state_t* state, unsigned index, const char* deviceName, const char* deviceType);

/**
 * Open a device
 *
 * @param state The server state
 * @param deviceNo The device index to open
 * @param deviceInfo Buffer to receive device information. May be NULL if
 *                   not required
 * @param deviceInfoSz Size of deviceInfo buffer
 */
int cswp_server_device_open(cswp_server_state_t* state, unsigned deviceNo, char* deviceInfo, size_t deviceInfoSz);

/**
 * Close a device
 *
 * @param state The server state
 * @param deviceNo The device index to close
 */
int cswp_server_device_close(cswp_server_state_t* state, unsigned deviceNo);

/**
 * Set a configuration item
 *
 * @param state The server state
 * @param deviceNo The device index
 * @param name Name of the config item to set
 * @param value Value to set the config item to
 */
int cswp_server_set_config(cswp_server_state_t* state, unsigned deviceNo, const char* name, const char* value);

/**
 * Get a configuration item
 *
 * @param state The server state
 * @param deviceNo The device index
 * @param name Name of the config item to set
 * @param value Receives config item value
 * @param valueSize Config item value buffer size
 */
int cswp_server_get_config(cswp_server_state_t* state, unsigned deviceNo, const char* name, char* value, size_t valueSize);

/**
 * Get device capabilities
 *
 * @param state The server state
 * @param deviceNo The device index
 * @param capabilities Device capabilities field
 * @param capabilitiesData Capability specific data
 */
int cswp_server_get_device_capabilities(cswp_server_state_t* state, unsigned deviceNo, varint_t* capabilities, varint_t* capabilitiesData);

/**
 * Build register list for a device
 *
 * Detect all the rergisters available and add it to the register list
 *
 * @param state The server state
 * @param deviceNo The device index
 */
int cswp_server_reg_list_build(cswp_server_state_t* state, unsigned deviceNo);

/**
 * Read register from a device
 *
 * @param state The server state
 * @param deviceNo The device index
 * @param regID Register IDs to read
 * @param value Receives register value
 */
int cswp_server_reg_read(cswp_server_state_t* state, unsigned deviceNo, unsigned regID, uint32_t* value);

/**
 * Write register of a device
 *
 * @param state The server state
 * @param deviceNo The device index
 * @param regID Register IDs to write
 * @param value Register value to write
 */
int cswp_server_reg_write(cswp_server_state_t* state, unsigned deviceNo, unsigned regID, uint32_t value);

/**
 * Read memory from a device
 *
 * @param state The server state
 * @param deviceNo The device index
 * @param address The address to read from
 * @param size The number of bytes to read
 * @param accessSize The access size to use
 * @param flags Flags
 * @param pData Receives the data
 */
int cswp_server_mem_read(cswp_server_state_t* state, unsigned deviceNo,
                         uint64_t address, size_t size,
                         cswp_access_size_t accessSize, unsigned flags, uint8_t* pData);

/**
 * Write memory to a device
 *
 * @param state The server state
 * @param deviceNo The device index
 * @param address The address to read from
 * @param size The number of bytes to read
 * @param accessSize The access size to use
 * @param flags Flags
 * @param pData The data to write
 */
int cswp_server_mem_write(cswp_server_state_t* state, unsigned deviceNo,
                          uint64_t address, size_t size,
                          cswp_access_size_t accessSize, unsigned flags, const uint8_t* pData);

/**
 * Poll memory from a device until value is matched
 *
 * @param state The server state
 * @param deviceNo The device index
 * @param address The address to read from
 * @param size The number of bytes to read
 * @param accessSize The access size to use
 * @param flags Flags
 * @param tries Number of tries before failing
 * @param interval Microsecond delay between each read (0 indicates none)
 * @param pMask The mask used when comparing to value
 * @param pValue Value to compare against
 * @param pData Receives the last data read
 */
int cswp_server_mem_poll(cswp_server_state_t* state, unsigned deviceNo,
                         uint64_t address, size_t size,
                         cswp_access_size_t accessSize, unsigned flags,
                         unsigned tries, unsigned interval,
                         const uint8_t* pMask, const uint8_t* pValue,
                         uint8_t* pData);

#ifdef __cplusplus
}
#endif

#endif /* CSWP_SERVER_IMPL_H */

/* End of file cswp_server_impl.h */

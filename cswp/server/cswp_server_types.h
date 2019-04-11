// cswp_server_types.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/**
 * @file cswp_server_types.h
 * @brief CSWP server types
 */

#ifndef CSWP_SERVER_TYPES_H
#define CSWP_SERVER_TYPES_H

#include "cswp_types.h"

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Per-device information
 */
typedef struct
{
    /**
     * The number of registers provided by the device
     */
    unsigned int registerCount;

    /**
     * Array of register information
     */
    cswp_register_info_t* registerInfo;
} cswp_device_info_t;

struct _cswp_server_state_t;

/**
 * Functions provided by implementation
 */
typedef struct
{
    /**
     * Initialise server
     *
     * @param state The server state
     */
    int (*init)(struct _cswp_server_state_t* state);

    /**
     * Terminate server
     *
     * @param state The server state
     */
    int (*term)(struct _cswp_server_state_t* state);

    /**
     * Initialise device list
     */
    int (*init_devices)(struct _cswp_server_state_t* state, unsigned int deviceCount);

    /**
     * Clear device list
     */
    int (*clear_devices)(struct _cswp_server_state_t* state);

    /**
     * Add a device
     *
     * Called from cswp_server_set_device()
     *
     * @param state The server state
     * @param deviceIndex The device number
     * @param deviceType The device type
     */
     int (*device_add)(struct _cswp_server_state_t* state, unsigned deviceIndex, const char* deviceType);

    /**
     * Open a device
     *
     * @param state The server state
     * @param deviceIndex The device number
     */
    int (*device_open)(struct _cswp_server_state_t* state, unsigned deviceIndex);

    /**
     * Close a device
     *
     * @param state The server state
     * @param deviceIndex The device number
     */
    int (*device_close)(struct _cswp_server_state_t* state, unsigned deviceIndex);

    /**
     * Set a config item
     *
     * @param state The server state
     * @param deviceIndex The device number
     * @param name The config item name
     * @param value The config item value
     */
    int (*set_config)(struct _cswp_server_state_t* state, unsigned deviceIndex, const char* name, const char* value);

    /**
     * Get a config item
     *
     * @param state The server state
     * @param deviceIndex The device number
     * @param name The config item name
     * @param value Receives the config item value
     * @param valueSize Size of value buffer
     */
    int (*get_config)(struct _cswp_server_state_t* state, unsigned deviceIndex, const char* name, char* value, size_t valueSize);

    /**
     * Get capabilities
     *
     * @param state The server state
     * @param deviceIndex The device number
     * @param capabilities Device capabilities bitfield
     * @param capabilitiesData Capability specific data
     */
    int (*get_device_capabilities)(struct _cswp_server_state_t* state, unsigned deviceIndex,  varint_t* capabilities, varint_t* capabilitiesData);

    /**
     * Build register list
     *
     * Update the register list for the device
     *
     * @param state The server state
     * @param deviceIndex The device number
     */
    int (*register_list_build)(struct _cswp_server_state_t* state, unsigned deviceIndex);

    /**
     * Read a register
     *
     * @param state The server state
     * @param deviceIndex The device number
     * @param registerID Register ID to read
     * @param value Receives register value
     */
    int (*register_read)(struct _cswp_server_state_t* state, unsigned deviceIndex, int registerID, uint32_t* value);

    /**
     * Write a register
     *
     * @param state The server state
     * @param deviceIndex The device number
     * @param registerID Register ID to write
     * @param value Register value
     */
    int (*register_write)(struct _cswp_server_state_t* state, unsigned deviceIndex, int registerID, uint32_t value);

    /**
     * Read memory
     *
     * @param state The server state
     * @param deviceIndex The device number
     * @param address The address to read from
     * @param size The number of bytes to read
     * @param accessSize The access size to use
     * @param flags Flags
     * @param pData Receives the data
     */
    int (*mem_read)(struct _cswp_server_state_t* state, unsigned deviceIndex,
                    uint64_t address, size_t size,
                    cswp_access_size_t accessSize, unsigned flags, uint8_t* pData);

    /**
     * Write memory
     *
     * @param state The server state
     * @param deviceIndex The device number
     * @param address The address to read from
     * @param size The number of bytes to read
     * @param accessSize The access size to use
     * @param flags Flags
     * @param pData The data to write
     */
    int (*mem_write)(struct _cswp_server_state_t* state, unsigned deviceIndex,
                     uint64_t address, size_t size,
                     cswp_access_size_t accessSize, unsigned flags, const uint8_t* pData);

    /**
     * Poll memory
     *
     * @param state The server state
     * @param deviceIndex The device number
     * @param address The address to read from
     * @param size The number of bytes to read
     * @param accessSize The access size to use
     * @param flags Flags
     * @param tries Number of attempts to poll
     * @param interval Delay (us) between attempts
     * @param pMask The mask to apply when polling
     * @param pValue The value to poll for
     * @param pData The last value read
     */
    int (*mem_poll)(struct _cswp_server_state_t* state, int deviceIndex,
                    uint64_t address, size_t size,
                    cswp_access_size_t accessSize, unsigned flags,
                    unsigned tries, unsigned interval,
                    const uint8_t* pMask, const uint8_t* pValue,
                    uint8_t* pData);

    /**
     * Log function
     *
     * @param state The server state
     * @param level The log level
     * @param msg Message format string
     */
    void (*log)(struct _cswp_server_state_t* state, cswp_log_level_t level, const char* msg, ...);
} cswp_server_impl_t;

/**
 * Server state
 */
typedef struct _cswp_server_state_t
{
    /**
     * Number of devices
     */
    unsigned int deviceCount;

    /**
     * Array of device names
     */
    const char** deviceNames;

    /**
     * Array of device type
     */
    const char** deviceTypes;

    /**
     * Array of device information
     */
    cswp_device_info_t* deviceInfo;

    /**
     * Implementation defined functions
     */
    const cswp_server_impl_t* impl;

    /**
     * System description file
     * NULL if not supported
     */
    uint8_t*  systemDescription;

    /**
     * System description file size
     */
    unsigned int systemDescriptionSize;

    /**
     * System description file format
     */
    unsigned int systemDescriptionFormat;

    /**
     * Private data for the implementation
     */
    void* priv;
} cswp_server_state_t;

#ifdef __cplusplus
}
#endif

#endif /* CSWP_SERVER_TYPES_H */

/* End of file cswp_server_types.h */

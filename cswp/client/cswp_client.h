// cswp_client.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/**
 * @file cswp_client.h
 * @brief CSWP client command/response encoding and decoding
 *
 * All buffers passed across this interface are owned by the caller - this
 * library does not allocate any memory that is passed back to the caller.
 * The caller should allocate buffers of sufficent size before calling
 * functions in this interface.  If a buffer is not large enough,
 * CSWP_OUTPUT_BUFFER_OVERFLOW will be returned.
 */

#ifndef CSWP_CLIENT_H
#define CSWP_CLIENT_H

#include "cswp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

struct _cswp_client_t;

/**
 * Client transport functions
 */
typedef struct _cswp_client_transport_t
{
    /**
     * Called from cswp_init() to connect the client transport
     */
    int (*connect)(struct _cswp_client_t* client, struct _cswp_client_transport_t* transport);

    /**
     * Called from cswp_term() to disconnect the client transport
     */
    int (*disconnect)(struct _cswp_client_t* client, struct _cswp_client_transport_t* transport);

    /**
     * Send data over the client transport
     */
    int (*send)(struct _cswp_client_t* client, struct _cswp_client_transport_t* transport, const void* data, size_t size);

    /**
     * Receive data over the client transport
     */
    int (*receive)(struct _cswp_client_t* client, struct _cswp_client_transport_t* transport, void* data, size_t size, size_t* used);

    /**
     * Private data for transport
     */
    void *priv;
} cswp_client_transport_t;


/**
 * Client state
 */
typedef struct _cswp_client_t
{
    /**
     * Error message buffer
     */
    char* errorMsg;

    /**
     * Private data for client implementation
     */
    void* priv;
} cswp_client_t;

/**
 * Initialise CSWP client
 *
 * @param client Pointer to cswp_client_t
 * @param transport Pointer to cswp_client_transport_t to use for this session
 */
int cswp_client_init(cswp_client_t* client,
                     cswp_client_transport_t* transport);

/**
 * Cleanup CSWP client
 *
 * @param client Pointer to cswp_client_t
 */
int cswp_client_term(cswp_client_t* client);

/**
 * Report an error
 *
 * For use by transport layer
 */
int cswp_client_error(cswp_client_t* client, int errorCode, const char* fmt, ...);

/**
 * Open CSWP connection to target
 *
 * @param client Pointer to cswp_client_t
 * @param clientID Client identifier string
 * @param serverProtocolVersion Receives server protocol version
 * @param serverID Buffer to receive server identifier string
 * @param serverIDSize Size of serverID buffer
 * @param serverVersion Receives server version number
 */
int cswp_init(cswp_client_t* client,
              const char*    clientID,
              unsigned*      serverProtocolVersion,
              char*          serverID,
              size_t         serverIDSize,
              unsigned*      serverVersion);

/**
 * Close CSWP connection to target
 *
 * @param client Pointer to cswp_client_t
 */
int cswp_term(cswp_client_t* client);

/**
 * Begin batch of commands
 *
 * Response data will not be valid until cswp_batch_end() is called
 *
 * @param client Pointer to cswp_client_t
 * @param abortOnError 0: continue on error, Non-zero: abort on error
 */
int cswp_batch_begin(cswp_client_t* client, int abortOnError);

/**
 * Complete batch of commands
 *
 * @param client Pointer to cswp_client_t
 * @param opsCompleted Receives number of operations completed
 */
int cswp_batch_end(cswp_client_t* client, unsigned* opsCompleted);

/**
 * Send client information to server
 *
 * This allows client operations to be included in server logs
 *
 * @param client Pointer to cswp_client_t
 * @param message Message to send
 */
int cswp_client_info(cswp_client_t* client,
                     const char* message);

/**
 * Set device list
 *
 * @param client Pointer to cswp_client_t
 * @param deviceCount Number of devices
 * @param deviceList Array of device strings
 * @param deviceTypes Array of type strings
 */
int cswp_set_devices(cswp_client_t* client,
                     unsigned deviceCount,
                     const char** deviceList,
                     const char** deviceTypes);

/**
 * Get device list
 *
 * @param client Pointer to cswp_client_t
 * @param deviceCount Receives number of devices
 * @param deviceList Array of char arrays to receive device strings
 * @param deviceListSize Number of entries in deviceList
 * @param deviceListEntrySize Size of each entry in deviceList
 * @param deviceTypes Array of char arrays to receive device type
 * @param deviceTypeSize Number of entries in deviceTypes
 * @param deviceTypeEntrySize Size of each entry in deviceTypes
 */
int cswp_get_devices(cswp_client_t* client,
                     unsigned* deviceCount,
                     char** deviceList,
                     size_t deviceListSize,
                     size_t deviceListEntrySize,
                     char** deviceTypes,
                     size_t deviceTypeSize,
                     size_t deviceTypeEntrySize);

/**
 * Get system description
 *
 * Return the SDF file describing the system.
 *
 * @param client Pointer to cswp_client_t
 * @param descriptionFormat 0-SDF file 1-SDF file compressend with gzip
 * @param descriptionSize size of the returned SDF file
 * @param descriptionDataBuffer buffer for SDF file
 * @param bufferSize Size of descriptionDataBuffer buffer
 */

int cswp_get_system_description(cswp_client_t* client,
                                unsigned* descriptionFormat,
                                unsigned* descriptionSize,
                                uint8_t* descriptionDataBuffer,
                                size_t bufferSize);

/**
 * Open a device
 *
 * Device numbers are 0 based.  Devices must be opened before any other
 * device operation (e.g. register or memory access) is performed
 *
 * @param client Pointer to cswp_client_t
 * @param deviceNo The device index to open
 * @param deviceInfo Buffer to receive device information. May be NULL if
 *                   not required
 * @param deviceInfoSize Size of deviceInfo buffer
 */
int cswp_device_open(cswp_client_t* client,
                     unsigned deviceNo,
                     char* deviceInfo,
                     size_t deviceInfoSize);

/**
 * Close a device
 *
 * @param client Pointer to cswp_client_t
 * @param deviceNo The device index
 */
int cswp_device_close(cswp_client_t* client,
                      unsigned deviceNo);

/**
 * Set a configuration item
 *
 * @param client Pointer to cswp_client_t
 * @param deviceNo The device index
 * @param name Name of the config item to set
 * @param value Value to set the config item to
 */
int cswp_set_config(cswp_client_t* client,
                    varint_t deviceNo,
                    const char* name,
                    const char* value);

/**
 * Get a configuration item
 *
 * @param client Pointer to cswp_client_t
 * @param deviceNo The device index
 * @param name Name of the config item to set
 * @param value Receives config item value
 * @param valueSize Config item value buffer size
 */
int cswp_get_config(cswp_client_t* client,
                    varint_t deviceNo,
                    const char* name,
                    char* value,
                    size_t valueSize);

/**
 * Get device capabilities
 *
 * @param client Pointer to cswp_client_t
 * @param deviceNo The device index
 * @param capabilities Device capabilities field
 * @param capabilityData Capability specific data
 */
int cswp_get_device_capabilities(cswp_client_t* client,
                                 varint_t deviceNo,
                                 unsigned* capabilities,
                                 unsigned* capabilityData);
/**
 * Get registers for a device
 *
 * Callers supply an array of cswp_register_info_t to receive the register
 * information and a char array to receive strings.  cswp_register_info_t
 * has strings for register names and descriptions - these strings will
 * be stored in strBuf, separated by NULL and the pointers in
 * cswp_register_info_t will point to the location of each string in strBuf
 *
 * @param client Pointer to cswp_client_t
 * @param deviceNo The device index
 * @param registerCount Receives the number of registers
 * @param registerInfo Array of cswp_register_info_t
 * @param registerInfoSize Number of entries in registerInfo
 * @param strBuf Receives string data
 * @param strBufSize Size of strBuf
 */
int cswp_device_reg_list(cswp_client_t* client,
                         unsigned deviceNo,
                         unsigned* registerCount,
                         cswp_register_info_t* registerInfo,
                         size_t registerInfoSize,
                         char *strBuf,
                         size_t strBufSize);

/**
 * Read registers from a device
 *
 * @param client Pointer to cswp_client_t
 * @param deviceNo The device index
 * @param registerCount The number of registers to read
 * @param registerIDs Array of register IDs to read
 * @param registerValues Receives register values
 * @param registerValuesSize Size of registerValues
 */
int cswp_device_reg_read(cswp_client_t* client,
                         unsigned deviceNo,
                         size_t registerCount,
                         const unsigned* registerIDs,
                         uint32_t* registerValues,
                         size_t registerValuesSize);

/**
 * Write registers of a device
 *
 * @param client Pointer to cswp_client_t
 * @param deviceNo The device index
 * @param registerCount The number of registers to write
 * @param registerIDs Array of register IDs to write
 * @param registerValues Register values to write
 * @param registerValuesSize Size of registerValues
 */
int cswp_device_reg_write(cswp_client_t* client,
                          unsigned deviceNo,
                          size_t registerCount,
                          const unsigned* registerIDs,
                          const uint32_t* registerValues,
                          size_t registerValuesSize);

/**
 * Read memory from a device
 *
 * @param client Pointer to cswp_client_t
 * @param deviceNo The device index
 * @param address The address to read from
 * @param size The number of bytes to read
 * @param accessSize The access size to use
 * @param flags Flags
 * @param buf Receives the data
 * @param bytesRead Number of bytes read
 */
int cswp_device_mem_read(cswp_client_t* client,
                         unsigned deviceNo,
                         uint64_t address,
                         size_t size,
                         cswp_access_size_t accessSize,
                         unsigned flags,
                         uint8_t* buf,
                         size_t* bytesRead);

/**
 * Write memory to a device
 *
 * @param client Pointer to cswp_client_t
 * @param deviceNo The device index
 * @param address The address to read from
 * @param size The number of bytes to read
 * @param accessSize The access size to use
 * @param flags Flags
 * @param buf The data to write
 */
int cswp_device_mem_write(cswp_client_t* client,
                          unsigned deviceNo,
                          uint64_t address,
                          size_t size,
                          cswp_access_size_t accessSize,
                          unsigned flags,
                          const uint8_t* buf);

/**
 * Poll memory from a device until value is matched
 *
 * Repeatedly reads from a memory location until a target value is read.
 * A mask is applied to the value read.  If the CSWP_MEM_POLL_MATCH_NE flag
 * is set, then the location is read until the read value does not match the
 * target value.  Otherwise, the location is read until the read value does
 * match the target value.
 *
 * Clients may specifify a delay between each read attempt.  The delay is
 * specified in microseconds.  If an implementation cannot measure time to
 * this precision, it may reduce the number of tries in order to keep the
 * total time constant.
 *
 * @param client Pointer to cswp_client_t
 * @param deviceNo The device index
 * @param address The address to read from
 * @param size The number of bytes to read
 * @param accessSize The access size to use
 * @param flags Flags
 * @param mask The mask used when comparing to value
 * @param value Value to compare against
 * @param tries Number of tries before failing
 * @param interval Microsecond delay between each read (0 indicates none)
 * @param buf Receives the last data read
 * @param bytesRead Number of bytes read
 */
int cswp_device_mem_poll(cswp_client_t* client,
                         unsigned deviceNo,
                         uint64_t address,
                         size_t size,
                         cswp_access_size_t accessSize,
                         unsigned flags,
                         unsigned tries,
                         unsigned interval,
                         const uint8_t* mask,
                         const uint8_t* value,
                         uint8_t* buf,
                         size_t* bytesRead);

#ifdef __cplusplus
}
#endif


#endif /* CSWP_CLIENT_H */

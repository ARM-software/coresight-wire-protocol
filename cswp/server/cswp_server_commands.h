// cswp_server_commands.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/**
 * @file cswp_server_commands.h
 * @brief CSWP server command/response encoding and decoding
 */

#ifndef CSWP_SERVER_COMMANDS_H
#define CSWP_SERVER_COMMANDS_H

#include "cswp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Decode a command header
 *
 * @param buf The buffer to decode from
 * @param messageType Receives the message type identifier (See cswp_commands_t)
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_decode_command_header(CSWP_BUFFER* buf,
                               varint_t* messageType);

/**
 * Encode a response header
 *
 * @param buf The buffer to encode to
 * @param messageType The message type identifier (See cswp_commands_t)
 * @param errorCode The error code (See cswp_result_t)
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_response_header(CSWP_BUFFER* buf,
                                varint_t messageType,
                                varint_t errorCode);

/**
 * Encode an error response
 *
 * @param buf The buffer to encode to
 * @param messageType The message type identifier (See cswp_commands_t)
 * @param errorCode The error code (See cswp_result_t)
 * @param errorMessage Error details string
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_error_response(CSWP_BUFFER* buf,
                               varint_t messageType,
                               varint_t errorCode,
                               const char* errorMessage);

/**
 * Decode a CSWP_INIT command
 *
 * @param buf The buffer to decode from
 * @param clientProtocolVersion Receives the maximum protocol version supported by the client
 * @param clientID Receives the client identifier string
 * @param clientIDSize The size of the clientID buffer
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_decode_init_command_body(CSWP_BUFFER* buf,
                                  varint_t* clientProtocolVersion,
                                  char* clientID,
                                  size_t clientIDSize);

/**
 * Encode a CSWP_INIT response
 *
 * @param buf The buffer to encode to
 * @param serverProtocolVersion Server protocol version
 * @param serverID Server ID string
 * @param serverVersion Server version
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_init_response(CSWP_BUFFER* buf,
                              varint_t serverProtocolVersion,
                              const char* serverID,
                              varint_t serverVersion);

/**
 * Encode a CSWP_TERM response
 *
 * @param buf The buffer to encode to
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_term_response(CSWP_BUFFER* buf);

/**
 * Decode a CSWP_CLIENT_INFO command
 *
 * @param buf The buffer to encode to
 * @param message Receives message from the client
 * @param messageSize size of message buffer
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_decode_client_info_command_body(CSWP_BUFFER* buf,
                                         char* message,
                                         size_t messageSize);

/**
 * Encode a CSWP_CLIENT_INFO response
 *
 * @param buf The buffer to encode to
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_client_info_response(CSWP_BUFFER* buf);

/**
 * Decode a CSWP_SET_DEVICES command
 *
 * The server should then read the device ID strings with
 * deviceCount calls to
 *    cswp_buffer_get_string(buf, deviceIDBuf, deviceIDSize);
 *
 * @param buf The buffer to decode from
 * @param deviceCount The number of entries in deviceList
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_decode_set_devices_command_body(CSWP_BUFFER* buf,
                                         varint_t* deviceCount);

/**
 * Encode a CSWP_SET_DEVICES response
 *
 * @param buf The buffer to encode to
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_set_devices_response(CSWP_BUFFER* buf);

/**
 * Encode a CSWP_GET_DEVICES response
  *
 * @param buf The buffer to encode to
 * @param deviceCount Device count
 * @param deviceList Array of device specification strings
 * @param deviceTypes Array of device type strings
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_get_devices_response(CSWP_BUFFER* buf,
                                     varint_t deviceCount,
                                     const char** deviceList,
                                     const char** deviceTypes);

/**
 * Encode a CSWP_GET_SYSTEM_DESCRIPTION response
  *
 * @param buf The buffer to encode to
 * @param systemDescriptionFormat 0-SDF format 1-SDF compreesed with gzip format  
 * @param systemDescriptionSize Size of systemDescrptionData buffer
 * @param systemDescriptionData System description data
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_get_system_description_response(CSWP_BUFFER* buf,
                                                varint_t systemDescriptionFormat,
                                                varint_t systemDescriptionSize,
                                                uint8_t* systemDescriptionData);

/**
 * Decode a CSWP_DEVICE_OPEN command
 *
 * @param buf The buffer to decode from
 * @param deviceNo Receives the device number
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_decode_device_open_command_body(CSWP_BUFFER* buf,
                                         varint_t* deviceNo);

/**
 * Encode a CSWP_DEVICE_OPEN response
 *
 * @param buf The buffer to decode to
 * @param deviceInfo Device information string
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_device_open_response(CSWP_BUFFER* buf,
                                     const char* deviceInfo);

/**
 * Decode a CSWP_DEVICE_CLOSE command
 *
 * @param buf The buffer to decode from
 * @param deviceNo Receives the device number
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_decode_device_close_command_body(CSWP_BUFFER* buf,
                                          varint_t* deviceNo);

/**
 * Encode a CSWP_DEVICE_CLOSE response
 *
 * @param buf The buffer to encode to
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_device_close_response(CSWP_BUFFER* buf);

/**
 * Decode a CSWP_SET_CONFIG command
 *
 * @param buf The buffer to encode to
 * @param deviceNo The device number
 * @param name Receive the name of the config item to set
 * @param nameSize Size of name buffer
 * @param value Receive the value to set the config item to
 * @param valueSize Size of value buffer
 */
int cswp_decode_set_config_command_body(CSWP_BUFFER* buf,
                                        varint_t* deviceNo,
                                        char* name,
                                        size_t nameSize,
                                        char* value,
                                        size_t valueSize);

/**
 * Encode a CSWP_SET_CONFIG response
 *
 * @param buf The buffer to encode to
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_set_config_response(CSWP_BUFFER* buf);

/**
 * Decode a CSWP_GET_CONFIG command
 *
 * @param buf The buffer to encode to
 * @param deviceNo The device number
 * @param name Receive the name of the config item to get
 * @param nameSize Size of name buffer
 */
int cswp_decode_get_config_command_body(CSWP_BUFFER* buf,
                                        varint_t* deviceNo,
                                        char* name,
                                        size_t nameSize);

/**
 * Encode a CSWP_GET_CONFIG response
 *
 * @param buf The buffer to decode from
 * @param value Config item value
 */
int cswp_encode_get_config_response(CSWP_BUFFER* buf,
                                    const char* value);


/**
 * Decode a CSWP_GET_DEVICE_CAPABILITIES command
 *
 * @param buf The buffer to encode to
 * @param deviceNo The device number
 */
int cswp_decode_get_device_capabilities_command_body(CSWP_BUFFER* buf,
                                                     varint_t* deviceNo);


/**
 * Decode a CSWP_GET_DEVICE_CAPABILITIES command
 *
 * @param buf The buffer to encode to
 * @param capabilities Device capabilities field
 * @param capabilitiesData Capability specific data
 */
int cswp_encode_get_device_capabilities_response(CSWP_BUFFER* buf,
                                                 varint_t capabilities,
                                                 varint_t capabilitiesData);
/**
 * Decode a CSWP_REG_LIST command
 *
 * @param buf The buffer to decode from
 * @param deviceNo Receive the device number
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_decode_reg_list_command_body(CSWP_BUFFER* buf,
                                      varint_t* deviceNo);

/**
 * Encode a CSWP_REG_LIST response
 *
 * The server should then add register information
 * by making registerCount calls to cswp_encode_reg_info()
 *
 * @param buf The buffer to encode to
 * @param registerCount Number of registers
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_reg_list_response(CSWP_BUFFER* buf,
                                  varint_t registerCount);

/**
 * Encode register info
 *
 * @param buf The buffer to encode to
 * @param id Register ID number
 * @param name Register name
 * @param size Register size
 * @param displayName Register name
 * @param description Register description
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_reg_info(CSWP_BUFFER* buf,
                         varint_t id,
                         const char* name,
                         varint_t size,
                         const char* displayName,
                         const char* description);

/**
 * Decode a CSWP_REG_READ command
 *
 * The server should then read the register IDs by making
 * count calls to:
 *   cswp_buffer_get_varint(buf, &regID);
 *
 * @param buf The buffer to encode to
 * @param deviceNo Receives the device number
 * @param count Receives the number of registers to read
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_decode_reg_read_command_body(CSWP_BUFFER* buf,
                                      varint_t* deviceNo,
                                      varint_t* count);

/**
 * Encode a CSWP_REG_READ response
 *
 * @param buf The buffer to encode to
 * @param count Number of register values
 * @param registerValues Array of register values
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_reg_read_response(CSWP_BUFFER* buf,
                                  varint_t count,
                                  const uint32_t* registerValues);

/**
 * Decode a CSWP_REG_WRITE command
 *
 * The caller should then read the register IDs and values with
 * count calls to:
 *   cswp_buffer_get_varint(buf, registerID);
 *   cswp_buffer_get_uint32(buf, registerValue);
 *
 * @param buf The buffer to decode from
 * @param deviceNo Receives the device number
 * @param count Receives the number of registers to write
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_decode_reg_write_command_body(CSWP_BUFFER* buf,
                                       varint_t* deviceNo,
                                       varint_t* count);

/**
 * Encode a CSWP_REG_WRITE response
 *
 * @param buf The buffer to encode to
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_reg_write_response(CSWP_BUFFER* buf);

/**
 * Decode a CSWP_MEM_READ command
 *
 * @param buf The buffer to decode from
 * @param deviceNo Receives the device number
 * @param address Receives the address to read from
 * @param size Receives the number of bytes to read
 * @param accessSize Receives the access size (cswp_access_size_t) to use
 * @param flags Receives flags
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_decode_mem_read_command_body(CSWP_BUFFER* buf,
                                      varint_t* deviceNo,
                                      uint64_t* address,
                                      varint_t* size,
                                      varint_t* accessSize,
                                      varint_t* flags);

/**
 * Encode a CSWP_MEM_READ response
 *
 * @param buf The buffer to encode to
 * @param count The number of bytes read
 * @param data The data read
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_mem_read_response(CSWP_BUFFER* buf,
                                  varint_t count,
                                  const uint8_t* data);

/**
 * Decode a CSWP_MEM_WRITE command
 *
 * The server should then obtain a pointer to the data
 * with a call to:
 *   cswp_buffer_get_direct(buf, &pData, count);
 *
 * @param buf The buffer to decode from
 * @param deviceNo Receives the device number
 * @param address Receives the address to read from
 * @param size Receives the number of bytes to read
 * @param accessSize Receives the access size (cswp_access_size_t) to use
 * @param flags Receives flags
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_decode_mem_write_command_body(CSWP_BUFFER* buf,
                                       varint_t* deviceNo,
                                       uint64_t* address,
                                       varint_t* size,
                                       varint_t* accessSize,
                                       varint_t* flags);

/**
 * Encode a CSWP_MEM_WRITE response
 *
 * @param buf The buffer to encode to
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_mem_write_response(CSWP_BUFFER* buf);

/**
 * Decode a CSWP_MEM_POLL command
 *
 * The server should then obtain a pointer to the mask & value
 * with a call to:
 *   cswp_buffer_get_direct(buf, &pMask, count);
 *   cswp_buffer_get_direct(buf, &pValue, count);
 *
 * @param buf The buffer to decode from
 * @param deviceNo Receives the device number
 * @param address Receives the address to read from
 * @param size Receives the number of bytes to read
 * @param accessSize Receives the access size (cswp_access_size_t) to use
 * @param flags Receives flags
 * @param tries Receives tries
 * @param interval Receives interval
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_decode_mem_poll_command_body(CSWP_BUFFER* buf,
                                      varint_t* deviceNo,
                                      uint64_t* address,
                                      varint_t* size,
                                      varint_t* accessSize,
                                      varint_t* flags,
                                      varint_t* tries,
                                      varint_t* interval);

/**
 * Encode a CSWP_MEM_POLL response
 *
 * @param buf The buffer to encode to
 * @param count The number of bytes read
 * @param data The data read
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_mem_poll_response(CSWP_BUFFER* buf,
                                  varint_t count,
                                  const uint8_t* data);

/**
 * Encode a CSWP_ASYNC_MESSAGE message
 *
 * @param buf The buffer to encode to
 * @param errorCode The error code
 * @param deviceNo The device number
 * @param level The message level
 * @param message The message contents
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_encode_async_message(CSWP_BUFFER* buf,
                              varint_t errorCode,
                              varint_t deviceNo,
                              varint_t level,
                              const char* message);

#ifdef __cplusplus
}
#endif

#endif /* CSWP_SERVER_COMMANDS_H */

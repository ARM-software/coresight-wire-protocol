// cswp_client_commands.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/**
 * @file cswp_client_commands.h
 * @brief CSWP client command/response encoding and decoding
 */

#ifndef CSWP_CLIENT_COMMANDS_H
#define CSWP_CLIENT_COMMANDS_H

#include "cswp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Encode a command header
 *
 * @param buf The buffer to encode to
 * @param messageType The message type identifier (See cswp_commands_t)
 */
int cswp_encode_command_header(CSWP_BUFFER* buf,
                               varint_t messageType);

/**
 * Decode a response header
 *
 * @param buf The buffer to decode from
 * @param messageType Receives the message type identifier (See cswp_commands_t)
 * @param errorCode Receives the error code (See cswp_result_t)
 */
int cswp_decode_response_header(CSWP_BUFFER* buf,
                                varint_t* messageType,
                                varint_t* errorCode);

/**
 * Decode an error response
 *
 * @param buf The buffer to decode from
 * @param errorMessage Buffer to receives the error message
 * @param errorMessageSize Size of the error message buffer
 */
int cswp_decode_error_response_body(CSWP_BUFFER* buf,
                                    char* errorMessage,
                                    size_t errorMessageSize);

/**
 * Encode a CSWP_INIT command
 *
 * @param buf The buffer to encode to
 * @param clientProtocolVersion The maximum protocol version supported by this client
 * @param clientID Client identifier string
 */
int cswp_encode_init_command(CSWP_BUFFER* buf,
                             varint_t clientProtocolVersion,
                             const char* clientID);

/**
 * Decode a CSWP_INIT response
 *
 * @param buf The buffer to decode from
 * @param serverProtocolVersion Receives server protocol version
 * @param serverID Receives server ID string
 * @param serverIDSize Size of serverID buffer
 * @param serverVersion Receives server version
 */
int cswp_decode_init_response_body(CSWP_BUFFER* buf,
                                   varint_t* serverProtocolVersion,
                                   char* serverID,
                                   size_t serverIDSize,
                                   varint_t* serverVersion);

/**
 * Encode a CSWP_TERM command
 *
 * @param buf The buffer to encode to
 */
int cswp_encode_term_command(CSWP_BUFFER* buf);

/**
 * Encode a CSWP_CLIENT_INFO command
 *
 * @param buf The buffer to encode to
 * @param message Message to send to the client
 */
int cswp_encode_client_info_command(CSWP_BUFFER* buf,
                                    const char* message);

/**
 * Encode a CSWP_SET_DEVICES command
 *
 * @param buf The buffer to encode to
 * @param deviceCount The number of entries in deviceList
 * @param deviceList Array of device specification strings
 * @param deviceType Array of device type strings
 */
int cswp_encode_set_devices_command(CSWP_BUFFER* buf,
                                    varint_t deviceCount,
                                    const char** deviceList,
                                    const char** deviceType);

/**
 * Encode a CSWP_GET_DEVICES command
 *
 * @param buf The buffer to encode to
 */
int cswp_encode_get_devices_command(CSWP_BUFFER* buf);

/**
 * Decode a CSWP_GET_DEVICES response
 *
 * The client should then read the device ID strings with
 * deviceCount calls to
 *    cswp_buffer_get_string(buf, deviceIDBuf, deviceIDSize);
 *
 * @param buf The buffer to decode from
 * @param deviceCount Receives device count
 */
int cswp_decode_get_devices_response_body(CSWP_BUFFER* buf,
                                          varint_t* deviceCount);

/**
 * Encode a CSWP_GET_SYSTEM_DESCRIPTION command
 *
 * @param buf The buffer to encode to
 */
int cswp_encode_get_system_description_command(CSWP_BUFFER* buf);

/**
 * Decode a CSWP_GET_SYSTEM_DESCRIPTION response
 *
 * @param buf The buffer to decode from
 * @param systemDescriptionFormat 0-SDF format 1-SDF compreesed with gzip format
 * @param systemDescriptionSize Size of systemDescrptionData buffer
 * @param systemDescriptionData System description data
 * @param systemDescriptionDataSize size of the systemDescriptionData buffer
 * @return Error code: CSWP_SUCCESS on success, or other cswp_result_t on error
 */
int cswp_decode_get_system_description_response_body(CSWP_BUFFER* buf,
                                                     varint_t* systemDescriptionFormat,
                                                     varint_t* systemDescriptionSize,
                                                     uint8_t* systemDescriptionData,
                                                     size_t systemDescriptionDataSize);

/**
 * Encode a CSWP_DEVICE_OPEN command
 *
 * @param buf The buffer to encode to
 * @param deviceNo The device number
 */
int cswp_encode_device_open_command(CSWP_BUFFER* buf,
                                    varint_t deviceNo);

/**
 * Decode a CSWP_DEVICE_OPEN response
 *
 * @param buf The buffer to decode from
 * @param deviceInfo Receives device information string
 * @param deviceInfoSize Size of deviceInfo buffer
 */
int cswp_decode_device_open_response_body(CSWP_BUFFER* buf,
                                          char* deviceInfo,
                                          size_t deviceInfoSize);

/**
 * Encode a CSWP_DEVICE_CLOSE command
 *
 * @param buf The buffer to encode to
 * @param deviceNo The device number
 */
int cswp_encode_device_close_command(CSWP_BUFFER* buf,
                                     varint_t deviceNo);

/**
 * Encode a CSWP_SET_CONFIG command
 *
 * @param buf The buffer to encode to
 * @param deviceNo The device number
 * @param name Name of the config item to set
 * @param value Value to set the config item to
 */
int cswp_encode_set_config_command(CSWP_BUFFER* buf,
                                   varint_t deviceNo,
                                   const char* name,
                                   const char* value);

/**
 * Encode a CSWP_GET_CONFIG command
 *
 * @param buf The buffer to encode to
 * @param deviceNo The device number
 * @param name Name of the config item to get
 */
int cswp_encode_get_config_command(CSWP_BUFFER* buf,
                                   varint_t deviceNo,
                                   const char* name);

/**
 * Decode a CSWP_GET_CONFIG response
 *
 * @param buf The buffer to decode from
 * @param value Receives config item value
 * @param valueSize Config item value buffer size
 */
int cswp_decode_get_config_response_body(CSWP_BUFFER* buf,
                                         char* value,
                                         size_t valueSize);

/**
 * Encode a CSWP_GET_DEVICE_CAPABILITIES command
 *
 * @param buf The buffer to encode to
 * @param deviceNo The device number
 */
int cswp_encode_get_device_capabilities_command(CSWP_BUFFER* buf,
                                                varint_t deviceNo);

/**
 * Decode a CSWP_GET_DEVICE_CAPABILITIES response
 *
 * @param buf The buffer to decode from
 * @param capabilities Device capabilities field
 * @param capabilityData Capability specific data
 */
int cswp_decode_get_device_capabilities_response_body(CSWP_BUFFER* buf,
                                                      varint_t* capabilities,
                                                      varint_t* capabilityData);

/**
 * Encode a CSWP_REG_LIST command
 *
 * @param buf The buffer to encode to
 * @param deviceNo The device number
 */
int cswp_encode_reg_list_command(CSWP_BUFFER* buf,
                                 varint_t deviceNo);

/**
 * Decode a CSWP_REG_LIST response
 *
 * The client should then get register information
 * by making registerCount calls to cswp_decode_reg_info()
 *
 * @param buf The buffer to decode from
 * @param registerCount Receives number of registers
 */
int cswp_decode_reg_list_response_body(CSWP_BUFFER* buf,
                                       varint_t* registerCount);

/**
 * Decode register info
 *
 * @param buf The buffer to decode from
 * @param id Receives register ID number
 * @param name Receives register name
 * @param nameSize Size of name buffer
 * @param size Receives register size
 * @param displayName Receives register display name
 * @param displayNameSize Size of displayName buffer
 * @param description Receives register description
 * @param descriptionSize Size of description buffer
 */
int cswp_decode_reg_info(CSWP_BUFFER* buf,
                         varint_t* id,
                         char* name,
                         size_t nameSize,
                         varint_t* size,
                         char* displayName,
                         size_t displayNameSize,
                         char* description,
                         size_t descriptionSize);

/**
 * Encode a CSWP_REG_READ command
 *
 * @param buf The buffer to encode to
 * @param deviceNo The device number
 * @param count The number of registers to read
 * @param registerIDs Array of register IDs
 */
int cswp_encode_reg_read_command(CSWP_BUFFER* buf,
                                 varint_t deviceNo,
                                 varint_t count,
                                 const varint_t* registerIDs);

/**
 * Decode a CSWP_REG_READ response
 *
 * The client should then read register values with count
 * calls to:
 *   cswp_buffer_get_uint32(buf, registerValue);
 *
 * @param buf The buffer to decode from
 * @param count Receives number of register values
 */
int cswp_decode_reg_read_response_body(CSWP_BUFFER* buf,
                                       varint_t* count);

/**
 * Encode a CSWP_REG_WRITE command
 *
 * The caller should then encode the register IDs and values with
 * count calls to:
 *   cswp_buffer_put_varint(buf, registerID);
 *   cswp_buffer_put_uint32(buf, registerValue);
 *
 * @param buf The buffer to encode to
 * @param deviceNo The device number
 * @param count The number of registers to write
 */
int cswp_encode_reg_write_command(CSWP_BUFFER* buf,
                                  varint_t deviceNo,
                                  varint_t count);

/**
 * Encode a CSWP_MEM_READ command
 *
 * @param buf The buffer to encode to
 * @param deviceNo The device number
 * @param address The address to read from
 * @param size The number of bytes to read
 * @param accessSize The access size (cswp_access_size_t) to use
 * @param flags Flags
 */
int cswp_encode_mem_read_command(CSWP_BUFFER* buf,
                                 varint_t deviceNo,
                                 uint64_t address,
                                 varint_t size,
                                 varint_t accessSize,
                                 varint_t flags);

/**
 * Decode a CSWP_MEM_READ response
 *
 * The client should then obtain a pointer to the read data
 * with a call to:
 *   cswp_buffer_get_direct(buf, &pData, count);
 *
 * @param buf The buffer to decode from
 * @param count Receives the number of bytes read
 */
int cswp_decode_mem_read_response_body(CSWP_BUFFER* buf,
                                       varint_t* count);

/**
 * Encode a CSWP_MEM_WRITE command
 *
 * @param buf The buffer to encode to
 * @param deviceNo The device number
 * @param address The address to read from
 * @param size The number of bytes to read
 * @param accessSize The access size (cswp_access_size_t) to use
 * @param flags Flags
 * @param data The data to write
 */
int cswp_encode_mem_write_command(CSWP_BUFFER* buf,
                                  varint_t deviceNo,
                                  uint64_t address,
                                  varint_t size,
                                  varint_t accessSize,
                                  varint_t flags,
                                  const uint8_t* data);

/**
 * Encode a CSWP_MEM_POLL command
 *
 * @param buf The buffer to encode to
 * @param deviceNo The device number
 * @param address The address to read from
 * @param size The number of bytes to read
 * @param accessSize The access size (cswp_access_size_t) to use
 * @param flags Flags
 * @param tries Number of tries
 * @param delay Delay between each try
 * @param mask Mask used in compare
 * @param value Value to compare to
 */
int cswp_encode_mem_poll_command(CSWP_BUFFER* buf,
                                 varint_t deviceNo,
                                 uint64_t address,
                                 varint_t size,
                                 varint_t accessSize,
                                 varint_t flags,
                                 varint_t tries,
                                 varint_t delay,
                                 const uint8_t* mask,
                                 const uint8_t* value);

/**
 * Decode a CSWP_MEM_POLL response
 *
 * The client should then obtain a pointer to the read data
 * with a call to:
 *   cswp_buffer_get_direct(buf, &pData, count);
 *
 * @param buf The buffer to decode from
 * @param count Receives the number of bytes read
 */
int cswp_decode_mem_poll_response_body(CSWP_BUFFER* buf,
                                       varint_t* count);

/**
 * Decode a CSWP_ASYNC_MESSAGE message
 *
 * @param buf The buffer to decode from
 * @param deviceNo Receives the device number
 * @param level Receives the message level
 * @param message Receives the message contents
 * @param messageSize Size of the message buffer
 */
int cswp_decode_async_message_body(CSWP_BUFFER* buf,
                                   varint_t* deviceNo,
                                   varint_t* level,
                                   char* message,
                                   size_t messageSize);

#ifdef __cplusplus
}
#endif

#endif /* CSWP_CLIENT_COMMANDS_H */

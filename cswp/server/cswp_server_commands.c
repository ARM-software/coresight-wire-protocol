// cswp_server_commands.c
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "cswp_server_commands.h"
#include "cswp_buffer.h"

#define __CSWP_CHECK(x) if ((res = (x)) != CSWP_SUCCESS) return res;


int cswp_decode_command_header(CSWP_BUFFER* buf,
                               varint_t* messageType)
{
    int res = CSWP_SUCCESS;

    __CSWP_CHECK(cswp_buffer_get_varint(buf, messageType));

    return res;
}


int cswp_encode_response_header(CSWP_BUFFER* buf,
                                varint_t messageType,
                                varint_t errorCode)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_put_varint(buf, messageType));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, errorCode));
    return res;
}


int cswp_encode_error_response(CSWP_BUFFER* buf,
                               varint_t messageType,
                               varint_t errorCode,
                               const char* errorMessage)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, messageType, errorCode));
    __CSWP_CHECK(cswp_buffer_put_string(buf, errorMessage));
    return res;
}


int cswp_decode_init_command_body(CSWP_BUFFER* buf,
                                  varint_t* clientProtocolVersion,
                                  char* clientID,
                                  size_t clientIDSize)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, clientProtocolVersion));
    __CSWP_CHECK(cswp_buffer_get_string(buf, clientID, clientIDSize));
    return res;
}


int cswp_encode_init_response(CSWP_BUFFER* buf,
                              varint_t serverProtocolVersion,
                              const char* serverID,
                              varint_t serverVersion)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_INIT, 0));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, serverProtocolVersion));
    __CSWP_CHECK(cswp_buffer_put_string(buf, serverID));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, serverVersion));
    return res;
}


int cswp_encode_term_response(CSWP_BUFFER* buf)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_TERM, 0));
    return res;
}


int cswp_decode_client_info_command_body(CSWP_BUFFER* buf,
                                         char* message,
                                         size_t messageSize)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_string(buf, message, messageSize));
    return res;
}


int cswp_encode_client_info_response(CSWP_BUFFER* buf)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_CLIENT_INFO, 0));
    return res;
}


int cswp_decode_set_devices_command_body(CSWP_BUFFER* buf,
                                         varint_t* deviceCount)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, deviceCount));
    return res;
}


int cswp_encode_set_devices_response(CSWP_BUFFER* buf)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_SET_DEVICES, 0));
    return res;
}


int cswp_encode_get_devices_response(CSWP_BUFFER* buf,
                                     varint_t deviceCount,
                                     const char** deviceList,
                                     const char** deviceTypes)
{
    int res = CSWP_SUCCESS;
    int i;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_GET_DEVICES, 0));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, deviceCount));
    for (i = 0; i < deviceCount; ++i)
    {
        __CSWP_CHECK(cswp_buffer_put_string(buf, deviceList[i]));
        __CSWP_CHECK(cswp_buffer_put_string(buf, deviceTypes[i]));
    }
    return res;
}


int cswp_encode_get_system_description_response(CSWP_BUFFER* buf,
                                                varint_t systemDescriptionFormat,
                                                varint_t systemDescriptionSize,
                                                uint8_t* systemDescriptionData)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_GET_SYSTEM_DESCRIPTION, 0));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, systemDescriptionFormat));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, systemDescriptionSize));
    __CSWP_CHECK(cswp_buffer_put_data(buf, systemDescriptionData, systemDescriptionSize));
    return res;
}


int cswp_decode_device_open_command_body(CSWP_BUFFER* buf,
                                         varint_t* deviceNo)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, deviceNo));
    return res;
}


int cswp_encode_device_open_response(CSWP_BUFFER* buf,
                                     const char* deviceInfo)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_DEVICE_OPEN, 0));
    __CSWP_CHECK(cswp_buffer_put_string(buf, deviceInfo));
    return res;
}


int cswp_decode_device_close_command_body(CSWP_BUFFER* buf,
                                          varint_t* deviceNo)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, deviceNo));
    return res;
}


int cswp_encode_device_close_response(CSWP_BUFFER* buf)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_DEVICE_CLOSE, 0));
    return res;
}


int cswp_decode_set_config_command_body(CSWP_BUFFER* buf,
                                        varint_t* deviceNo,
                                        char* name,
                                        size_t nameSize,
                                        char* value,
                                        size_t valueSize)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, deviceNo));
    __CSWP_CHECK(cswp_buffer_get_string(buf, name, nameSize));
    __CSWP_CHECK(cswp_buffer_get_string(buf, value, valueSize));
    return res;
}


int cswp_encode_set_config_response(CSWP_BUFFER* buf)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_SET_CONFIG, 0));
    return res;
}


int cswp_decode_get_config_command_body(CSWP_BUFFER* buf,
                                        varint_t* deviceNo,
                                        char* name,
                                        size_t nameSize)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, deviceNo));
    __CSWP_CHECK(cswp_buffer_get_string(buf, name, nameSize));
    return res;
}

int cswp_encode_get_config_response(CSWP_BUFFER* buf,
                                    const char* value)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_GET_CONFIG, 0));
    __CSWP_CHECK(cswp_buffer_put_string(buf, value));
    return res;
}


int cswp_decode_get_device_capabilities_command_body(CSWP_BUFFER* buf,
                                                     varint_t* deviceNo)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, deviceNo));
    return res;
}


int cswp_encode_get_device_capabilities_response(CSWP_BUFFER* buf,
                                                 varint_t capabilities,
                                                 varint_t capabilitiesData)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_GET_DEVICE_CAPABILITIES, 0));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, capabilities));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, capabilitiesData));
    return res;
}


int cswp_decode_reg_list_command_body(CSWP_BUFFER* buf,
                                      varint_t* deviceNo)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, deviceNo));
    return res;
}


int cswp_encode_reg_list_response(CSWP_BUFFER* buf,
                                  varint_t registerCount)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_REG_LIST, 0));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, registerCount));
    return res;
}


int cswp_encode_reg_info(CSWP_BUFFER* buf,
                         varint_t id,
                         const char* name,
                         varint_t size,
                         const char* displayName,
                         const char* description)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_put_varint(buf, id));
    __CSWP_CHECK(cswp_buffer_put_string(buf, name));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, size));
    __CSWP_CHECK(cswp_buffer_put_string(buf, displayName));
    __CSWP_CHECK(cswp_buffer_put_string(buf, description));
    return res;
}


int cswp_decode_reg_read_command_body(CSWP_BUFFER* buf,
                                      varint_t* deviceNo,
                                      varint_t* count)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, deviceNo));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, count));
    return res;
}


int cswp_encode_reg_read_response(CSWP_BUFFER* buf,
                                  varint_t count,
                                  const uint32_t* registerValues)
{
    int res = CSWP_SUCCESS;
    int i;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_REG_READ, 0));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, count));
    for (i = 0; i < count; ++i)
        __CSWP_CHECK(cswp_buffer_put_uint32(buf, registerValues[i]));
    return res;
}


int cswp_decode_reg_write_command_body(CSWP_BUFFER* buf,
                                       varint_t* deviceNo,
                                       varint_t* count)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, deviceNo));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, count));
    return res;
}


int cswp_encode_reg_write_response(CSWP_BUFFER* buf)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_REG_WRITE, 0));
    return res;
}


int cswp_decode_mem_read_command_body(CSWP_BUFFER* buf,
                                      varint_t* deviceNo,
                                      uint64_t* address,
                                      varint_t* size,
                                      varint_t* accessSize,
                                      varint_t* flags)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, deviceNo));
    __CSWP_CHECK(cswp_buffer_get_uint64(buf, address));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, size));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, accessSize));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, flags));
    return res;
}


int cswp_encode_mem_read_response(CSWP_BUFFER* buf,
                                  varint_t count,
                                  const uint8_t* data)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_MEM_READ, 0));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, count));
    __CSWP_CHECK(cswp_buffer_put_data(buf, data, count));
    return res;
}


int cswp_decode_mem_write_command_body(CSWP_BUFFER* buf,
                                       varint_t* deviceNo,
                                       uint64_t* address,
                                       varint_t* size,
                                       varint_t* accessSize,
                                       varint_t* flags)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, deviceNo));
    __CSWP_CHECK(cswp_buffer_get_uint64(buf, address));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, size));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, accessSize));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, flags));
    return res;
}


int cswp_encode_mem_write_response(CSWP_BUFFER* buf)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_MEM_WRITE, 0));
    return res;
}


int cswp_decode_mem_poll_command_body(CSWP_BUFFER* buf,
                                      varint_t* deviceNo,
                                      uint64_t* address,
                                      varint_t* size,
                                      varint_t* accessSize,
                                      varint_t* flags,
                                      varint_t* tries,
                                      varint_t* interval)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, deviceNo));
    __CSWP_CHECK(cswp_buffer_get_uint64(buf, address));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, size));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, accessSize));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, flags));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, tries));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, interval));
    return res;
}


int cswp_encode_mem_poll_response(CSWP_BUFFER* buf,
                                  varint_t count,
                                  const uint8_t* data)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_MEM_POLL, 0));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, count));
    __CSWP_CHECK(cswp_buffer_put_data(buf, data, count));
    return res;
}


int cswp_encode_async_message(CSWP_BUFFER* buf,
                              varint_t errorCode,
                              varint_t deviceNo,
                              varint_t level,
                              const char* message)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_response_header(buf, CSWP_ASYNC_MESSAGE, errorCode));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, deviceNo));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, level));
    __CSWP_CHECK(cswp_buffer_put_string(buf, message));
    return res;
}

/* end of file cswp_commands.c */

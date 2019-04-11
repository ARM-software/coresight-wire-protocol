// cswp_client_commands.c
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "cswp_client_commands.h"
#include "cswp_buffer.h"

#define __CSWP_CHECK(x) if ((res = (x)) != CSWP_SUCCESS) return res;

static int cswp_get_optional_string(CSWP_BUFFER* buf, char* str, size_t strSz)
{
    int res = CSWP_SUCCESS;

    if (strSz == 0 || str == NULL) {
        varint_t sz;
        __CSWP_CHECK(cswp_buffer_get_varint(buf, &sz));
        cswp_buffer_skip(buf, sz);
    } else {
        __CSWP_CHECK(cswp_buffer_get_string(buf, str, strSz));
    }

    return res;
}

int cswp_encode_command_header(CSWP_BUFFER* buf,
                               varint_t messageType)
{
    int res = CSWP_SUCCESS;

    __CSWP_CHECK(cswp_buffer_put_varint(buf, messageType));

    return res;
}


int cswp_decode_response_header(CSWP_BUFFER* buf,
                                varint_t* messageType,
                                varint_t* errorCode)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, messageType));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, errorCode));
    return res;
}


int cswp_decode_error_response_body(CSWP_BUFFER* buf,
                                    char* errorMessage,
                                    size_t errorMessageSize)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_string(buf, errorMessage, errorMessageSize));
    return res;
}


int cswp_encode_init_command(CSWP_BUFFER* buf,
                             varint_t clientProtocolVersion,
                             const char* clientID)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_INIT));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, clientProtocolVersion));
    __CSWP_CHECK(cswp_buffer_put_string(buf, clientID));
    return res;
}


int cswp_decode_init_response_body(CSWP_BUFFER* buf,
                                   varint_t* serverProtocolVersion,
                                   char* serverID,
                                   size_t serverIDSize,
                                   varint_t* serverVersion)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, serverProtocolVersion));
    __CSWP_CHECK(cswp_get_optional_string(buf, serverID, serverIDSize));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, serverVersion));
    return res;
}


int cswp_encode_term_command(CSWP_BUFFER* buf)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_TERM));
    return res;
}


int cswp_encode_client_info_command(CSWP_BUFFER* buf,
                                    const char* message)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_CLIENT_INFO));
    __CSWP_CHECK(cswp_buffer_put_string(buf, message));
    return res;
}


int cswp_encode_set_devices_command(CSWP_BUFFER* buf,
                                    varint_t deviceCount,
                                    const char** deviceList,
                                    const char** deviceType)
{
    int res = CSWP_SUCCESS;
    int i;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_SET_DEVICES));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, deviceCount));
    for (i = 0; i < deviceCount; ++i)
    {
        __CSWP_CHECK(cswp_buffer_put_string(buf, deviceList[i]));
        __CSWP_CHECK(cswp_buffer_put_string(buf, deviceType[i]));
    }
    return res;
}


int cswp_encode_get_devices_command(CSWP_BUFFER* buf)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_GET_DEVICES));
    return res;
}


int cswp_decode_get_devices_response_body(CSWP_BUFFER* buf,
                                          varint_t* deviceCount)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, deviceCount));
    return res;
}


int cswp_encode_get_system_description_command(CSWP_BUFFER* buf)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_GET_SYSTEM_DESCRIPTION));
    return res;
}


int cswp_decode_get_system_description_response_body(CSWP_BUFFER* buf,
                                                     varint_t* systemDescriptionFormat,
                                                     varint_t* systemDescriptionSize,
                                                     uint8_t* systemDescriptionData,
                                                     size_t systemDescriptionDataSize)
{
    int res = CSWP_SUCCESS;
    size_t bufferSize;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, systemDescriptionFormat));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, systemDescriptionSize));
    bufferSize = systemDescriptionDataSize < *systemDescriptionSize ? systemDescriptionDataSize : *systemDescriptionSize;
    __CSWP_CHECK(cswp_buffer_get_data(buf, systemDescriptionData, bufferSize));
    return res;
}


int cswp_encode_device_open_command(CSWP_BUFFER* buf,
                                    varint_t deviceNo)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_DEVICE_OPEN));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, deviceNo));
    return res;
}

int cswp_decode_device_open_response_body(CSWP_BUFFER* buf,
                                          char* deviceInfo,
                                          size_t deviceInfoSize)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_get_optional_string(buf, deviceInfo, deviceInfoSize));
    return res;
}


int cswp_encode_device_close_command(CSWP_BUFFER* buf,
                                     varint_t deviceNo)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_DEVICE_CLOSE));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, deviceNo));
    return res;
}


int cswp_encode_set_config_command(CSWP_BUFFER* buf,
                                   varint_t deviceNo,
                                   const char* name,
                                   const char* value)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_SET_CONFIG));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, deviceNo));
    __CSWP_CHECK(cswp_buffer_put_string(buf, name));
    __CSWP_CHECK(cswp_buffer_put_string(buf, value));
    return res;
}


int cswp_encode_get_config_command(CSWP_BUFFER* buf,
                                   varint_t deviceNo,
                                   const char* name)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_GET_CONFIG));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, deviceNo));
    __CSWP_CHECK(cswp_buffer_put_string(buf, name));
    return res;
}


int cswp_decode_get_config_response_body(CSWP_BUFFER* buf,
                                         char* value,
                                         size_t valueSize)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_string(buf, value, valueSize));
    return res;
}


int cswp_encode_get_device_capabilities_command(CSWP_BUFFER* buf,
                                                varint_t deviceNo)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_GET_DEVICE_CAPABILITIES));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, deviceNo));
    return res;
}


int cswp_decode_get_device_capabilities_response_body(CSWP_BUFFER* buf,
                                                      varint_t* capabilities,
                                                      varint_t* capabilityData)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, capabilities));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, capabilityData));
    return res;
}

int cswp_encode_reg_list_command(CSWP_BUFFER* buf,
                                 varint_t deviceNo)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_REG_LIST));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, deviceNo));
    return res;
}


int cswp_decode_reg_list_response_body(CSWP_BUFFER* buf,
                                       varint_t* registerCount)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, registerCount));
    return res;
}


int cswp_decode_reg_info(CSWP_BUFFER* buf,
                         varint_t* id,
                         char* name,
                         size_t nameSize,
                         varint_t* size,
                         char* displayName,
                         size_t displayNameSize,
                         char* description,
                         size_t descriptionSize)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, id));
    __CSWP_CHECK(cswp_buffer_get_string(buf, name, nameSize));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, size));
    __CSWP_CHECK(cswp_buffer_get_string(buf, displayName, displayNameSize));
    __CSWP_CHECK(cswp_buffer_get_string(buf, description, descriptionSize));
    return res;
}


int cswp_encode_reg_read_command(CSWP_BUFFER* buf,
                                 varint_t deviceNo,
                                 varint_t count,
                                 const varint_t* registerIDs)
{
    int res = CSWP_SUCCESS;
    int i;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_REG_READ));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, deviceNo));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, count));
    for (i = 0; i < count; ++i)
        __CSWP_CHECK(cswp_buffer_put_varint(buf, registerIDs[i]));
    return res;
}


int cswp_decode_reg_read_response_body(CSWP_BUFFER* buf,
                                       varint_t* count)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, count));
    return res;
}


int cswp_encode_reg_write_command(CSWP_BUFFER* buf,
                                  varint_t deviceNo,
                                  varint_t count)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_REG_WRITE));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, deviceNo));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, count));
    return res;
}


int cswp_encode_mem_read_command(CSWP_BUFFER* buf,
                                 varint_t deviceNo,
                                 uint64_t address,
                                 varint_t size,
                                 varint_t accessSize,
                                 varint_t flags)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_MEM_READ));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, deviceNo));
    __CSWP_CHECK(cswp_buffer_put_uint64(buf, address));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, size));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, accessSize));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, flags));
    return res;
}


int cswp_decode_mem_read_response_body(CSWP_BUFFER* buf,
                                       varint_t* count)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, count));
    return res;
}


int cswp_encode_mem_write_command(CSWP_BUFFER* buf,
                                  varint_t deviceNo,
                                  uint64_t address,
                                  varint_t size,
                                  varint_t accessSize,
                                  varint_t flags,
                                  const uint8_t* data)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_MEM_WRITE));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, deviceNo));
    __CSWP_CHECK(cswp_buffer_put_uint64(buf, address));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, size));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, accessSize));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, flags));
    __CSWP_CHECK(cswp_buffer_put_data(buf, data, size));
    return res;
}


int cswp_encode_mem_poll_command(CSWP_BUFFER* buf,
                                 varint_t deviceNo,
                                 uint64_t address,
                                 varint_t size,
                                 varint_t accessSize,
                                 varint_t flags,
                                 varint_t tries,
                                 varint_t delay,
                                 const uint8_t* mask,
                                 const uint8_t* value)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_encode_command_header(buf, CSWP_MEM_POLL));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, deviceNo));
    __CSWP_CHECK(cswp_buffer_put_uint64(buf, address));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, size));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, accessSize));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, flags));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, tries));
    __CSWP_CHECK(cswp_buffer_put_varint(buf, delay));
    __CSWP_CHECK(cswp_buffer_put_data(buf, mask, size));
    __CSWP_CHECK(cswp_buffer_put_data(buf, value, size));
    return res;
}


int cswp_decode_mem_poll_response_body(CSWP_BUFFER* buf,
                                       varint_t* count)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, count));
    return res;
}


int cswp_decode_async_message_body(CSWP_BUFFER* buf,
                                   varint_t* deviceNo,
                                   varint_t* level,
                                   char* message,
                                   size_t messageSize)
{
    int res = CSWP_SUCCESS;
    __CSWP_CHECK(cswp_buffer_get_varint(buf, deviceNo));
    __CSWP_CHECK(cswp_buffer_get_varint(buf, level));
    __CSWP_CHECK(cswp_buffer_get_string(buf, message, messageSize));
    return res;
}

/* end of file cswp_commands.c */

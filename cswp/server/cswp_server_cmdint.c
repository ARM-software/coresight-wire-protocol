// cswp_server_cmdint.c
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "cswp_server_cmdint.h"
#include "cswp_server_commands.h"
#include "cswp_server_impl.h"
#include "cswp_buffer.h"

#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#ifdef _WIN32
#define snprintf _snprintf
#endif

/* Server identifier / version info */
const unsigned SERVER_PROTOCOL_VERSION = CSWP_PROTOCOL_v1;
const char*    SERVER_ID               = "AMIS PoC CSWP Server";
const unsigned SERVER_VERISION         = 0x0100;

#ifdef DEBUG
#define __CSWP_LOG_MAX CSWP_LOG_DEBUG
#else
#define __CSWP_LOG_MAX CSWP_LOG_INFO
#endif

/* use macros to compile out debug level */
#define CSWP_LOG(state, level, ...)                                     \
    do { if (level <= __CSWP_LOG_MAX &&                                 \
             state && state->impl && state->impl->log)                  \
            state->impl->log(state, level, __VA_ARGS__);                \
    } while (0)


static int cswp_error(cswp_server_state_t* state,
                      CSWP_BUFFER* rsp, varint_t messageType, int res,
                      const char* msgFmt, ...)
{
    va_list args;
    char buf[1024];
    int bytesWritten;

    va_start(args, msgFmt);

    buf[0] = buf[sizeof(buf)-1] = '\0';

    /* TODO: map error code to string */
    bytesWritten = snprintf(buf, sizeof(buf), "Error %d: ", res);
    if (bytesWritten > 0 && bytesWritten < sizeof(buf))
    {
        vsnprintf(buf + bytesWritten, sizeof(buf) - bytesWritten, msgFmt, args);
    }
    va_end(args);

    /* Log */
    CSWP_LOG(state, CSWP_LOG_ERROR, buf);

    cswp_encode_error_response(rsp, messageType, res, buf);

    return res;
}


static int cswp_init(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;
    varint_t protocolVersion;
    char clientID[256];

    res = cswp_decode_init_command_body(cmd, &protocolVersion, clientID, sizeof(clientID));
    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_INIT, res, "Failed to decode CSWP_INIT command");
    }
    else
    {
        CSWP_LOG(state, CSWP_LOG_INFO, "Client %s connected: protocol version: %d", clientID, protocolVersion);

        cswp_server_init(state);
        res = cswp_encode_init_response(rsp, SERVER_PROTOCOL_VERSION,
                                        SERVER_ID, SERVER_VERISION);
        if (res != CSWP_SUCCESS)
        {
            cswp_error(state, rsp, CSWP_INIT, res, "Failed to encode CSWP_INIT response");
        }
    }

    return res;
}


static int cswp_term(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;

    CSWP_LOG(state, CSWP_LOG_INFO, "Client disconnected");

    cswp_server_term(state);

    res = cswp_encode_term_response(rsp);
    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_TERM, res, "Failed to encode CSWP_TERM response");
    }

    return res;
}


static int cswp_client_info(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;
    char msgBuf[1024];

    res = cswp_decode_client_info_command_body(cmd, msgBuf, sizeof(msgBuf));
    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_CLIENT_INFO, res, "Failed to decode CSWP_CLIENT_INFO command");
    }
    else
    {
        /* Add client message to server log */
        CSWP_LOG(state, CSWP_LOG_INFO, "Client info: %s", msgBuf);

        res = cswp_encode_client_info_response(rsp);
        if (res != CSWP_SUCCESS)
        {
            cswp_error(state, rsp, CSWP_CLIENT_INFO, res, "Failed to encode CSWP_CLIENT_INFO response");
        }
    }

    return res;
}

static int cswp_set_devices(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;
    varint_t deviceCount;
    unsigned i;

    res = cswp_decode_set_devices_command_body(cmd, &deviceCount);
    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_SET_DEVICES, res, "Failed to decode CSWP_SET_DEVICES command");
    }
    else
    {
        cswp_server_clear_devices(state);
        cswp_server_init_devices(state, deviceCount);
        for (i = 0; i < deviceCount; ++i)
        {
            char deviceID[256];
            char deviceType[256];

            res = cswp_buffer_get_string(cmd, deviceID, sizeof(deviceID));
            if (res != CSWP_SUCCESS)
            {
                cswp_error(state, rsp, CSWP_SET_DEVICES, res, "Failed to decode CSWP_SET_DEVICES command - Cannot get device name");
                break;
            }

            res = cswp_buffer_get_string(cmd, deviceType, sizeof(deviceType));
            if (res != CSWP_SUCCESS)
            {
                cswp_error(state, rsp, CSWP_SET_DEVICES, res, "Failed to decode CSWP_SET_DEVICES command - Cannot get device type");
                break;
            }

            /* add to device list */
            CSWP_LOG(state, CSWP_LOG_INFO, "Device %d: %s", i, deviceID);
            res = cswp_server_set_device(state, i, deviceID, deviceType);
            if (res != CSWP_SUCCESS)
            {
                cswp_error(state, rsp, CSWP_SET_DEVICES, res, "Failed to add device %s", deviceID);
                break;
            }
        }

        if (res == CSWP_SUCCESS)
        {
            res = cswp_encode_set_devices_response(rsp);
            if (res != CSWP_SUCCESS)
            {
                cswp_error(state, rsp, CSWP_SET_DEVICES, res, "Failed to encode CSWP_SET_DEVICES response");
            }
        }
    }

    return res;
}


static int cswp_get_devices(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;

    /* Reply with information from server state */
    res = cswp_encode_get_devices_response(rsp, state->deviceCount, state->deviceNames, state->deviceTypes);
    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_GET_DEVICES, res, "Failed to encode CSWP_GET_DEVICES response");
    }

    return res;
}


static int cswp_get_system_description(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;

    if (state->systemDescription == NULL)
    {
        res = CSWP_UNSUPPORTED;
        cswp_error(state, rsp, CSWP_GET_SYSTEM_DESCRIPTION, CSWP_UNSUPPORTED, "Failed to get system description");
    }
    else
    {
        res = cswp_encode_get_system_description_response(rsp, state->systemDescriptionFormat, state->systemDescriptionSize, state->systemDescription);
        if (res != CSWP_SUCCESS)
        {
            cswp_error(state, rsp, CSWP_GET_SYSTEM_DESCRIPTION, res, "Failed to encode CSWP_GET_SYSTEM_DESCRIPTION response");
        }

    }

    return res;
}


static int cswp_device_open(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;
    varint_t deviceNo;
    char deviceInfo[256];

    res = cswp_decode_device_open_command_body(cmd, &deviceNo);
    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_DEVICE_OPEN, res, "Failed to decode CSWP_DEVICE_OPEN command");
    }
    else
    {
        CSWP_LOG(state, CSWP_LOG_INFO, "Open device %d", deviceNo);

        if (deviceNo >= state->deviceCount)
        {
            res = cswp_error(state, rsp, CSWP_DEVICE_OPEN, CSWP_INVALID_DEVICE, "Invalid device %u", deviceNo);
        }
        else
        {
            /* device init */
            res = cswp_server_device_open(state, deviceNo, deviceInfo, sizeof(deviceInfo));
            if (res != CSWP_SUCCESS)
            {
                cswp_error(state, rsp, CSWP_DEVICE_OPEN, res, "Failed to open device %u", deviceNo);
            }
            else
            {
                res = cswp_encode_device_open_response(rsp, deviceInfo);
                if (res != CSWP_SUCCESS)
                {
                    cswp_error(state, rsp, CSWP_DEVICE_OPEN, res, "Failed to encode CSWP_DEVICE_OPEN response");
                }
            }
        }
    }

    return res;
}


static int cswp_device_close(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;
    varint_t deviceNo;

    res = cswp_decode_device_close_command_body(cmd, &deviceNo);
    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_DEVICE_CLOSE, res, "Failed to decode CSWP_DEVICE_CLOSE command");
    }
    else
    {
        if (deviceNo >= state->deviceCount)
        {
            res = cswp_error(state, rsp, CSWP_DEVICE_OPEN, CSWP_INVALID_DEVICE, "Invalid device %u", deviceNo);
        }
        else
        {
            CSWP_LOG(state, CSWP_LOG_INFO, "Close device %d", deviceNo);

            res = cswp_server_device_close(state, deviceNo);
            if (res != CSWP_SUCCESS)
            {
                cswp_error(state, rsp, CSWP_DEVICE_CLOSE, res, "Failed to close device %u", deviceNo);
            }
            else
            {
                res = cswp_encode_device_close_response(rsp);
                if (res != CSWP_SUCCESS)
                {
                    cswp_error(state, rsp, CSWP_DEVICE_CLOSE, res, "Failed to encode CSWP_DEVICE_CLOSE response");
                }
            }
        }
    }

    return res;
}


static int cswp_set_config(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;
    varint_t deviceNo;
    char name[256];
    char value[256];

    res = cswp_decode_set_config_command_body(cmd, &deviceNo, name, sizeof(name), value, sizeof(value));
    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_SET_CONFIG, res, "Failed to decode CSWP_SET_CONFIG command");
    }
    else
    {
        CSWP_LOG(state, CSWP_LOG_INFO, "Set config device %d: %s = %s", deviceNo, name, value);

        if (deviceNo >= state->deviceCount)
        {
            res = cswp_error(state, rsp, CSWP_SET_CONFIG, CSWP_INVALID_DEVICE, "Invalid device %u", deviceNo);
        }
        else
        {
            res = cswp_server_set_config(state, deviceNo, name, value);
            if (res != CSWP_SUCCESS)
            {
                cswp_error(state, rsp, CSWP_SET_CONFIG, res, "Failed to set config item on device %u", deviceNo);
            }
            else
            {
                res = cswp_encode_set_config_response(rsp);
                if (res != CSWP_SUCCESS)
                {
                    cswp_error(state, rsp, CSWP_SET_CONFIG, res, "Failed to encode CSWP_SET_CONFIG response");
                }
            }
        }
    }

    return res;
}


static int cswp_get_config(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;
    varint_t deviceNo;
    char name[256];
    char value[256];

    res = cswp_decode_get_config_command_body(cmd, &deviceNo, name, sizeof(name));
    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_GET_CONFIG, res, "Failed to decode CSWP_GET_CONFIG command");
    }
    else
    {
        CSWP_LOG(state, CSWP_LOG_INFO, "Get config device %d: %s", deviceNo, name);

        if (deviceNo >= state->deviceCount)
        {
            res = cswp_error(state, rsp, CSWP_GET_CONFIG, CSWP_INVALID_DEVICE, "Invalid device %u", deviceNo);
        }
        else
        {
            res = cswp_server_get_config(state, deviceNo, name, value, sizeof(value));
            if (res != CSWP_SUCCESS)
            {
                cswp_error(state, rsp, CSWP_GET_CONFIG, res, "Failed to get config item on device %u", deviceNo);
            }
            else
            {
                res = cswp_encode_get_config_response(rsp, value);
                if (res != CSWP_SUCCESS)
                {
                    cswp_error(state, rsp, CSWP_GET_CONFIG, res, "Failed to encode CSWP_GET_CONFIG response");
                }
            }
        }
    }

    return res;
}


static int cswp_get_device_capabilities(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;
    varint_t deviceNo, capabilities, capabilitiesData;

    res = cswp_decode_get_device_capabilities_command_body(cmd, &deviceNo);
    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_GET_DEVICE_CAPABILITIES, res, "Failed to decode CSWP_GET_DEVICE_CAPABILITIES command");
    }
    else
    {
        CSWP_LOG(state, CSWP_LOG_INFO, "Get capabilities for device %d");

        if (deviceNo >= state->deviceCount)
        {
            res = cswp_error(state, rsp, CSWP_GET_DEVICE_CAPABILITIES, CSWP_INVALID_DEVICE, "Invalid device %u", deviceNo);
        }
        else
        {
            res = cswp_server_get_device_capabilities(state, deviceNo, &capabilities, &capabilitiesData);
            if (res != CSWP_SUCCESS)
            {
                cswp_error(state, rsp, CSWP_GET_DEVICE_CAPABILITIES, res, "Failed to get capabilities on device %u", deviceNo);
            }
            else
            {
                res = cswp_encode_get_device_capabilities_response(rsp, capabilities, capabilitiesData);
                if (res != CSWP_SUCCESS)
                {
                    cswp_error(state, rsp, CSWP_GET_DEVICE_CAPABILITIES, res, "Failed to encode CSWP_GET_DEVICE_CAPABILITIES response");
                }
            }
        }
    }

    return res;
}

static int cswp_reg_list(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;
    varint_t deviceNo;
    unsigned regCount;
    unsigned i;

    res = cswp_decode_reg_list_command_body(cmd, &deviceNo);
    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_REG_LIST, res, "Failed to decode CSWP_REG_LIST command");
    }
    else
    {
        if (deviceNo >= state->deviceCount)
        {
            res = cswp_error(state, rsp, CSWP_DEVICE_OPEN, CSWP_INVALID_DEVICE, "Invalid device %u", deviceNo);
        }
        else
        {
            const cswp_device_info_t* deviceInfo = &state->deviceInfo[deviceNo];

            res = cswp_server_reg_list_build(state, deviceNo);

            regCount = deviceInfo->registerCount;
            res = cswp_encode_reg_list_response(rsp, regCount);
            for (i = 0; i < regCount && res == CSWP_SUCCESS; ++i)
            {
                const cswp_register_info_t* regInfo = &deviceInfo->registerInfo[i];
                res = cswp_encode_reg_info(rsp,
                                           regInfo->id,
                                           regInfo->name,
                                           regInfo->size,
                                           regInfo->displayName,
                                           regInfo->description);
            }

            if (res != CSWP_SUCCESS)
            {
                cswp_error(state, rsp, CSWP_REG_LIST, res, "Failed to encode CSWP_REG_LIST response");
            }
        }
    }

    return res;
}


static int cswp_reg_read(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;
    varint_t deviceNo;
    varint_t regCount;
    varint_t regID;
    unsigned r;
    uint32_t* regValues = NULL;

    res = cswp_decode_reg_read_command_body(cmd, &deviceNo, &regCount);
    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_REG_READ, res, "Failed to decode CSWP_REG_READ command");
    }
    else
    {
        if (deviceNo >= state->deviceCount)
        {
            res = cswp_error(state, rsp, CSWP_REG_READ, CSWP_INVALID_DEVICE, "Invalid device %u", deviceNo);
        }
        else
        {
            regValues = malloc(regCount * sizeof(uint32_t));

            for (r = 0; r < regCount && res == CSWP_SUCCESS; ++r)
            {
                res = cswp_buffer_get_varint(cmd, &regID);
                if (res == CSWP_SUCCESS)
                {
                    CSWP_LOG(state, CSWP_LOG_INFO, "Read reg %u", regID);
                    res = cswp_server_reg_read(state, deviceNo, regID, &regValues[r]);
                }
                if (res != CSWP_SUCCESS)
                {
                    res = cswp_error(state, rsp, CSWP_REG_READ, res, "Failed to read register %u", regID);
                }
            }

            if (res == CSWP_SUCCESS)
            {
                res = cswp_encode_reg_read_response(rsp, regCount, regValues);
                if (res != CSWP_SUCCESS)
                {
                    cswp_error(state, rsp, CSWP_REG_READ, res, "Failed to encode CSWP_REG_READ response");
                }
            }
        }

        free(regValues);
    }

    return res;
}


static int cswp_reg_write(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;
    varint_t deviceNo;
    varint_t regCount;
    varint_t regID;
    uint32_t regVal;
    unsigned r;

    res = cswp_decode_reg_write_command_body(cmd, &deviceNo, &regCount);
    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_REG_WRITE, res, "Failed to decode CSWP_REG_WRITE command");
    }
    else
    {
        if (deviceNo >= state->deviceCount)
        {
            res = cswp_error(state, rsp, CSWP_DEVICE_OPEN, CSWP_INVALID_DEVICE, "Invalid device %u", deviceNo);
        }
        else
        {
            for (r = 0; r < regCount && res == CSWP_SUCCESS; ++r)
            {
                res = cswp_buffer_get_varint(cmd, &regID);
                if (res == CSWP_SUCCESS)
                    res = cswp_buffer_get_uint32(cmd, &regVal);

                if (res == CSWP_SUCCESS)
                {
                    CSWP_LOG(state, CSWP_LOG_INFO, "Write reg %u = 0x%08X", regID, regVal);
                    res = cswp_server_reg_write(state, deviceNo, regID, regVal);
                }

                if (res != CSWP_SUCCESS)
                {
                    res = cswp_error(state, rsp, CSWP_REG_WRITE, res, "Failed to write register %u", regID);
                }
            }
        }

        if (res == CSWP_SUCCESS)
        {
            res = cswp_encode_reg_write_response(rsp);
            if (res != CSWP_SUCCESS)
            {
                cswp_error(state, rsp, CSWP_REG_WRITE, res, "Failed to encode CSWP_REG_WRITE response");
            }
        }
    }

    return res;
}


static int cswp_mem_read(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;
    varint_t deviceNo;
    uint64_t address;
    varint_t size;
    varint_t accessSize;
    varint_t flags;
    uint8_t* readBuf = NULL;

    res = cswp_decode_mem_read_command_body(cmd, &deviceNo,
                                            &address, &size,
                                            &accessSize, &flags);
    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_MEM_READ, res, "Failed to decode CSWP_MEM_READ command");
    }
    else
    {
        if (deviceNo >= state->deviceCount)
        {
            res = cswp_error(state, rsp, CSWP_DEVICE_OPEN, CSWP_INVALID_DEVICE, "Invalid device %u", deviceNo);
        }
        else
        {
            CSWP_LOG(state, CSWP_LOG_INFO, "Mem read: %d: 0x%08X%08X ..+0x%X, acc=0x%X, flags=0x%X",
                     deviceNo, address >> 32, address & 0xFFFFFFFFL, size, accessSize, flags);

            readBuf = malloc(size);
            res = cswp_server_mem_read(state, deviceNo, address, size, accessSize, flags, readBuf);
            if (res != CSWP_SUCCESS)
            {
                res = cswp_error(state, rsp, CSWP_MEM_READ, res, "Failed to read memory %d: 0x%08X%08X ..+0x%X, acc=0x%X, flags=0x%X",
                                 deviceNo, address >> 32, address & 0xFFFFFFFFL, size, accessSize, flags);
            }
        }

        if (res == CSWP_SUCCESS)
        {
            res = cswp_encode_mem_read_response(rsp, size, readBuf);

            if (res != CSWP_SUCCESS)
            {
                cswp_error(state, rsp, CSWP_MEM_READ, res, "Failed to encode CSWP_MEM_READ response");
            }
        }

        if (readBuf != NULL)
            free(readBuf);
    }

    return res;
}


static int cswp_mem_write(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;
    varint_t deviceNo;
    uint64_t address;
    varint_t size;
    varint_t accessSize;
    varint_t flags;
    void* writeBuf;

    res = cswp_decode_mem_write_command_body(cmd, &deviceNo,
                                             &address, &size,
                                             &accessSize, &flags);
    if (res == CSWP_SUCCESS)
        res = cswp_buffer_get_direct(cmd, &writeBuf, size);

    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_MEM_WRITE, res, "Failed to decode CSWP_MEM_WRITE command");
    }
    else
    {
        if (deviceNo >= state->deviceCount)
        {
            res = cswp_error(state, rsp, CSWP_DEVICE_OPEN, CSWP_INVALID_DEVICE, "Invalid device %u", deviceNo);
        }
        else
        {
            CSWP_LOG(state, CSWP_LOG_INFO, "Mem write: %d: 0x%08X%08X ..+0x%X, acc=0x%X, flags=0x%X",
                     deviceNo, address >> 32, address & 0xFFFFFFFFL, size, accessSize, flags);

            res = cswp_server_mem_write(state, deviceNo, address, size, accessSize, flags, writeBuf);
            if (res != CSWP_SUCCESS)
            {
                res = cswp_error(state, rsp, CSWP_MEM_WRITE, res, "Failed to write memory %d: 0x%08X%08X ..+0x%X, acc=0x%X, flags=0x%X",
                                 deviceNo, address >> 32, address & 0xFFFFFFFFL, size, accessSize, flags);
            }
        }

        if (res == CSWP_SUCCESS)
        {
            res = cswp_encode_mem_write_response(rsp);
            if (res != CSWP_SUCCESS)
            {
                cswp_error(state, rsp, CSWP_MEM_WRITE, res, "Failed to encode CSWP_MEM_WRITE response");
            }
        }
    }

    return res;
}


static int cswp_mem_poll(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;
    varint_t deviceNo;
    uint64_t address;
    varint_t size;
    varint_t accessSize;
    varint_t flags;
    varint_t tries;
    varint_t interval;
    void* maskBuf;
    void* valueBuf;
    uint8_t* readBuf = NULL;

    res = cswp_decode_mem_poll_command_body(cmd, &deviceNo,
                                            &address, &size,
                                            &accessSize, &flags,
                                            &tries, &interval);
    if (res == CSWP_SUCCESS)
        res = cswp_buffer_get_direct(cmd, &maskBuf, size);
    if (res == CSWP_SUCCESS)
        res = cswp_buffer_get_direct(cmd, &valueBuf, size);

    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_MEM_POLL, res, "Failed to decode CSWP_MEM_POLL command");
    }
    else
    {
        if (deviceNo >= state->deviceCount)
        {
            res = cswp_error(state, rsp, CSWP_DEVICE_OPEN, CSWP_INVALID_DEVICE, "Invalid device %u", deviceNo);
        }
        else
        {
            CSWP_LOG(state, CSWP_LOG_INFO, "Mem poll: %d: 0x%08X%08X ..+0x%X, acc=0x%X, flags=0x%X",
                     deviceNo, address >> 32, address & 0xFFFFFFFFL, size, accessSize, flags);

            readBuf = malloc(size);
            res = cswp_server_mem_poll(state, deviceNo, address, size, accessSize, flags, tries, interval, maskBuf, valueBuf, readBuf);
            if (res != CSWP_SUCCESS)
            {
                res = cswp_error(state, rsp, CSWP_MEM_POLL, res, "Failed to poll memory %d: 0x%08X%08X ..+0x%X, acc=0x%X, flags=0x%X",
                                 deviceNo, address >> 32, address & 0xFFFFFFFFL, size, accessSize, flags);
            }
        }

        if (res == CSWP_SUCCESS)
        {
            res = cswp_encode_mem_poll_response(rsp, size, readBuf);
            if (res != CSWP_SUCCESS)
            {
                cswp_error(state, rsp, CSWP_MEM_POLL, res, "Failed to encode CSWP_MEM_POLL response");
            }
        }

        if (readBuf != NULL)
            free(readBuf);
    }

    return res;
}


static int cswp_dispatch_command(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp, varint_t messageType)
{
    int res;

    res = CSWP_UNSUPPORTED;

    switch (messageType)
    {
    case CSWP_INIT:
        res = cswp_init(state, cmd, rsp);
        break;

    case CSWP_TERM:
        res = cswp_term(state, cmd, rsp);
        break;

    case CSWP_CLIENT_INFO:
        res = cswp_client_info(state, cmd, rsp);
        break;

    case CSWP_SET_DEVICES:
        res = cswp_set_devices(state, cmd, rsp);
        break;

    case CSWP_GET_DEVICES:
        res = cswp_get_devices(state, cmd, rsp);
        break;

    case CSWP_GET_SYSTEM_DESCRIPTION:
        res = cswp_get_system_description(state, cmd, rsp);
        break;

    case CSWP_DEVICE_OPEN:
        res = cswp_device_open(state, cmd, rsp);
        break;

    case CSWP_DEVICE_CLOSE:
        res = cswp_device_close(state, cmd, rsp);
        break;

    case CSWP_SET_CONFIG:
        res = cswp_set_config(state, cmd, rsp);
        break;

    case CSWP_GET_CONFIG:
        res = cswp_get_config(state, cmd, rsp);
        break;

    case CSWP_GET_DEVICE_CAPABILITIES:
        res = cswp_get_device_capabilities(state, cmd, rsp);
        break;

    case CSWP_REG_LIST:
        res = cswp_reg_list(state, cmd, rsp);
        break;

    case CSWP_REG_READ:
        res = cswp_reg_read(state, cmd, rsp);
        break;

    case CSWP_REG_WRITE:
        res = cswp_reg_write(state, cmd, rsp);
        break;

    case CSWP_MEM_READ:
        res = cswp_mem_read(state, cmd, rsp);
        break;

    case CSWP_MEM_WRITE:
        res = cswp_mem_write(state, cmd, rsp);
        break;

    case CSWP_MEM_POLL:
        res = cswp_mem_poll(state, cmd, rsp);
        break;

    case CSWP_ASYNC_MESSAGE:
        break;

        /* No support for any other command (including impl defined */
    default:
        cswp_error(state, rsp, messageType, res, "Unknown message type %d", messageType);
        break;
    }

    return res;
}

int cswp_handle_command(cswp_server_state_t* state, CSWP_BUFFER* cmd, CSWP_BUFFER* rsp)
{
    int res;
    varint_t messageType;

    if (state == NULL)
        return CSWP_NOT_INITIALIZED;

    res = cswp_decode_command_header(cmd, &messageType);
    if (res != CSWP_SUCCESS)
    {
        cswp_error(state, rsp, CSWP_NONE, res, "Failed to decode command header");
    }
    else
    {
        res = cswp_dispatch_command(state, cmd, rsp, messageType);
    }

    return res;
}

/* end of file cswp_server_cmdint.c */

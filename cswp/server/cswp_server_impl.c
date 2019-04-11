// cswp_server_impl.c
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "cswp_server_impl.h"
#include "cswp_types.h"

#include <string.h>
#include <stdio.h>
#ifdef _WIN32
#define snprintf _snprintf
#endif

void cswp_server_init(cswp_server_state_t* state)
{
    state->deviceCount = 0;
    state->deviceNames = NULL;
    state->deviceTypes = NULL;
    state->deviceInfo = NULL;

    if (state->impl && state->impl->init)
        state->impl->init(state);
}


void cswp_server_term(cswp_server_state_t* state)
{
    if (state->impl && state->impl->term)
        state->impl->term(state);

    cswp_server_clear_devices(state);
}


static void cswp_server_clear_device_info(cswp_device_info_t* devInfo)
{
    int i;

    if (devInfo->registerInfo)
    {
        for (i = 0; i < devInfo->registerCount; ++i)
        {
            free((void*)devInfo->registerInfo[i].name);
            free((void*)devInfo->registerInfo[i].displayName);
            free((void*)devInfo->registerInfo[i].description);
        }
        free(devInfo->registerInfo);
        devInfo->registerInfo = 0;
    }
}


void cswp_server_clear_devices(cswp_server_state_t* state)
{
    unsigned int i;

    if (state->impl && state->impl->clear_devices)
        state->impl->clear_devices(state);

    for (i = 0; i < state->deviceCount; ++i)
    {
        cswp_server_clear_device_info(&state->deviceInfo[i]);
        free((void*)state->deviceNames[i]);
        free((void*)state->deviceTypes[i]);
    }
    free((void*)state->deviceNames);
    free((void*)state->deviceTypes);
    free(state->deviceInfo);
    state->deviceCount = 0;
    state->deviceNames = NULL;
    state->deviceTypes = NULL;
    state->deviceInfo = NULL;
}


void cswp_server_init_devices(cswp_server_state_t* state, unsigned deviceCount)
{
    state->deviceCount = deviceCount;
    state->deviceNames = calloc(deviceCount, sizeof(const char*));
    state->deviceTypes = calloc(deviceCount, sizeof(const char*));
    state->deviceInfo = calloc(deviceCount, sizeof(cswp_device_info_t));

    if (state->impl && state->impl->init_devices)
        state->impl->init_devices(state, deviceCount);
}


int cswp_server_set_device(cswp_server_state_t* state, unsigned index, const char* deviceName, const char* deviceType)
{
    int res = CSWP_SUCCESS;

    if (index >= state->deviceCount)
        return CSWP_INVALID_DEVICE;
    state->deviceNames[index] = strdup(deviceName);
    state->deviceTypes[index] = strdup(deviceType);

    if (state->impl && state->impl->device_add)
        res = state->impl->device_add(state, index, deviceType);

    return res;
}


int cswp_server_device_open(cswp_server_state_t* state, unsigned deviceNo, char* deviceInfo, size_t deviceInfoSz)
{
    int res = CSWP_SUCCESS;
    cswp_device_info_t* devInfo;

    snprintf(deviceInfo, deviceInfoSz, "Device %d info", deviceNo);
    deviceInfo[deviceInfoSz-1] = '\0';

    devInfo = &state->deviceInfo[deviceNo];
    devInfo->registerCount = 0;
    devInfo->registerInfo = NULL;

    if (state->impl && state->impl->device_open)
        res = state->impl->device_open(state, deviceNo);

    return res;
}


int cswp_server_device_close(cswp_server_state_t* state, unsigned deviceNo)
{
    int res = CSWP_SUCCESS;
    cswp_device_info_t* deviceInfo;

    if (state->impl && state->impl->device_close)
        res = state->impl->device_close(state, deviceNo);

    deviceInfo = &state->deviceInfo[deviceNo];
    cswp_server_clear_device_info(deviceInfo);

    return res;
}


int cswp_server_set_config(cswp_server_state_t* state, unsigned deviceNo, const char* name, const char* value)
{
    /* Check implementation supports set config */
    if (!state->impl || !state->impl->set_config)
        return CSWP_UNSUPPORTED;

    return state->impl->set_config(state, deviceNo, name, value);
}


int cswp_server_get_config(cswp_server_state_t* state, unsigned deviceNo, const char* name, char* value, size_t valueSize)
{
    /* Check implementation supports get config */
    if (!state->impl || !state->impl->get_config)
        return CSWP_UNSUPPORTED;

    return state->impl->get_config(state, deviceNo, name, value, valueSize);
}

int cswp_server_get_device_capabilities(cswp_server_state_t* state, unsigned deviceNo, varint_t* capabilities, varint_t* capabilitiesData)
{
    /* Check implementation supports get_device_capabilities */
    if (!state->impl || !state->impl->get_device_capabilities)
        return CSWP_UNSUPPORTED;

    return state->impl->get_device_capabilities(state, deviceNo, capabilities, capabilitiesData);
}

int cswp_server_reg_list_build(cswp_server_state_t* state, unsigned deviceNo)
{
    /* Check implementation supports register list discovery */
    if (!state->impl || !state->impl->register_list_build)
        return CSWP_UNSUPPORTED;

    return state->impl->register_list_build(state, deviceNo);
}


int cswp_server_reg_read(cswp_server_state_t* state, unsigned deviceNo, unsigned regID, uint32_t* value)
{
    /* Check implementation supports register read */
    if (!state->impl || !state->impl->register_read)
        return CSWP_UNSUPPORTED;

    return state->impl->register_read(state, deviceNo, regID, value);
}


int cswp_server_reg_write(cswp_server_state_t* state, unsigned deviceNo, unsigned regID, uint32_t value)
{
    /* Check implementation supports register write */
    if (!state->impl || !state->impl->register_write)
        return CSWP_UNSUPPORTED;

    return state->impl->register_write(state, deviceNo, regID, value);
}


int cswp_server_mem_read(cswp_server_state_t* state, unsigned deviceNo,
                         uint64_t address, size_t size,
                         cswp_access_size_t accessSize, unsigned flags, uint8_t* pData)
{
    /* Check implementation supports memory read */
    if (!state->impl || !state->impl->mem_read)
        return CSWP_UNSUPPORTED;

    return state->impl->mem_read(state, deviceNo, address, size, accessSize, flags, pData);
}


int cswp_server_mem_write(cswp_server_state_t* state, unsigned deviceNo,
                          uint64_t address, size_t size,
                          cswp_access_size_t accessSize, unsigned flags, const uint8_t* pData)
{
    /* Check implementation supports memory write */
    if (!state->impl || !state->impl->mem_write)
        return CSWP_UNSUPPORTED;

    return state->impl->mem_write(state, deviceNo, address, size, accessSize, flags, pData);
}


int cswp_server_mem_poll(cswp_server_state_t* state, unsigned deviceNo,
                         uint64_t address, size_t size,
                         cswp_access_size_t accessSize, unsigned flags,
                         unsigned tries, unsigned interval,
                         const uint8_t* pMask, const uint8_t* pValue,
                         uint8_t* pData)
{
    /* Check implementation supports memory poll */
    if (!state->impl || !state->impl->mem_poll)
        return CSWP_UNSUPPORTED;

    return state->impl->mem_poll(state, deviceNo, address, size, accessSize, flags,
                                 tries, interval,
                                 pMask, pValue, pData);
}

/* End of file cswp_server_impl.c */

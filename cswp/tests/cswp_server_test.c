// cswp_server_test.c
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "cswp_server_cmdint.h"
#include "cswp_client.h"
#include "cswp_client_commands.h"
#include "cswp_server_commands.h"
#include "cswp_server_impl.h"
#include "cswp_server_types.h"
#include "cswp_test.h"

#include <string.h>
#include <stdio.h>

typedef struct
{
    cswp_server_state_t* serverState;
    CSWP_BUFFER* cmd;
    CSWP_BUFFER* rsp;
} cswp_test_client_priv_t;

static int test_transport_connect(cswp_client_t* client, cswp_client_transport_t* transport)
{
    cswp_test_client_priv_t* priv = (cswp_test_client_priv_t*)transport->priv;
    priv->cmd = cswp_buffer_alloc(16384);
    priv->rsp = cswp_buffer_alloc(16384);

    return CSWP_SUCCESS;
}

static int test_transport_disconnect(cswp_client_t* client, cswp_client_transport_t* transport)
{
    cswp_test_client_priv_t* priv = (cswp_test_client_priv_t*)transport->priv;
    cswp_buffer_free(priv->cmd);
    cswp_buffer_free(priv->rsp);

    return CSWP_SUCCESS;
}

static int test_transport_send(cswp_client_t* client, cswp_client_transport_t* transport, const void* data, size_t size)
{
    cswp_test_client_priv_t* priv = (cswp_test_client_priv_t*)transport->priv;
    cswp_buffer_clear(priv->cmd);
    memcpy(priv->cmd->buf, data, size);
    priv->cmd->pos = priv->cmd->used = size;
    return CSWP_SUCCESS;
}

static int test_transport_receive(cswp_client_t* client, cswp_client_transport_t* transport, void* data, size_t size, size_t* used)
{
    cswp_test_client_priv_t* priv = (cswp_test_client_priv_t*)transport->priv;
    int res = CSWP_SUCCESS;
    uint32_t cmdSize;
    varint_t numCmds;
    uint8_t abortOnError;
    unsigned c;
    uint8_t* pLen;

    /* check command size */
    cswp_buffer_seek(priv->cmd, 0);
    cswp_buffer_get_uint32(priv->cmd, &cmdSize);
    if (cmdSize != priv->cmd->used)
        return CSWP_COMMS;

    /* Read number of sub commands */
    cswp_buffer_get_varint(priv->cmd, &numCmds);
    cswp_buffer_get_uint8(priv->cmd, &abortOnError);

    /* Write reply header */
    /*   reserve space for reply size */
    cswp_buffer_clear(priv->rsp);
    priv->rsp->pos = 4;
    priv->rsp->used = 4;
    /*   num responses */
    cswp_buffer_put_varint(priv->rsp, numCmds);

    for (c = 0; c < numCmds && priv->cmd->pos < priv->cmd->used; ++c)
    {
        res = cswp_handle_command(priv->serverState, priv->cmd, priv->rsp);
        if (res != CSWP_SUCCESS && abortOnError)
            break;
    }

    if (res == CSWP_SUCCESS)
    {
        CHECK_EQUAL(c, numCmds);
        CHECK_EQUAL(priv->cmd->pos, priv->cmd->used);
    }

    /* Generate cancelled errors for subsequent commands if abort on error */
    if (abortOnError)
    {
        for (; c < numCmds; ++c)
            cswp_encode_error_response(priv->rsp, 0, CSWP_CANCELLED,
                                       "Cancelled");
    }

    /* insert message lenfth */
    pLen = priv->rsp->buf;
    *pLen++ = (priv->rsp->used & 0xFF);
    *pLen++ = ((priv->rsp->used >> 8) & 0xFF);
    *pLen++ = ((priv->rsp->used >> 16) & 0xFF);
    *pLen++ = ((priv->rsp->used >> 24) & 0xFF);

    if (priv->rsp->used > size)
        return CSWP_OUTPUT_BUFFER_OVERFLOW;

    memcpy(data, priv->rsp->buf, priv->rsp->used);
    *used = priv->rsp->used;

    // command errors are encoded in response
    return CSWP_SUCCESS;
}

cswp_client_transport_t testClientTransport = {
    /*.connect = */ test_transport_connect,
    /*.disconnect = */ test_transport_disconnect,
    /*.send = */ test_transport_send,
    /*.receive = */ test_transport_receive,
    /*.priv = */ NULL,
};

static char testCfg[2][16];
static uint32_t testRegs[10];
static uint8_t testMem[16];

static int test_impl_init(cswp_server_state_t* state)
{
    return CSWP_SUCCESS;
}

static int test_impl_term(cswp_server_state_t* state)
{
    return CSWP_SUCCESS;
}

static int test_impl_device_add(cswp_server_state_t* state, unsigned deviceIndex, const char* deviceType)
{
    return CSWP_SUCCESS;
}

static int test_impl_device_open(cswp_server_state_t* state, unsigned deviceIndex)
{
    cswp_device_info_t* devInfo = &state->deviceInfo[deviceIndex];

    if (deviceIndex == 0)
    {
        int i;
        char buf[256];

        devInfo->registerCount = 10;
        devInfo->registerInfo = malloc(devInfo->registerCount * sizeof(cswp_register_info_t));

        for (i = 0; i < devInfo->registerCount; ++i)
        {
            devInfo->registerInfo[i].id = i;
            sprintf(buf, "R_%d", i);
            devInfo->registerInfo[i].name = strdup(buf);
            devInfo->registerInfo[i].size = 1;
            sprintf(buf, "R %d", i);
            devInfo->registerInfo[i].displayName = strdup(buf);
            sprintf(buf, "Register %d", i);
            devInfo->registerInfo[i].description = strdup(buf);
        }
    }

    return CSWP_SUCCESS;
}

static int test_impl_set_config(cswp_server_state_t* state, unsigned deviceIndex, const char* name, const char* value)
{
    if (deviceIndex != 0)
        return CSWP_UNSUPPORTED;

    if (strcmp("config_1", name) == 0)
        strcpy(testCfg[0], value);
    else if (strcmp("config_2", name) == 0)
        strcpy(testCfg[1], value);
    else
        return CSWP_BAD_ARGS;

    return CSWP_SUCCESS;
}

static int test_impl_get_config(cswp_server_state_t* state, unsigned deviceIndex, const char* name, char* value, size_t valueSize)
{
    if (deviceIndex != 0)
        return CSWP_UNSUPPORTED;

    if (strcmp("config_1", name) == 0)
    {
        if (valueSize < strlen(testCfg[0]))
            return CSWP_BAD_ARGS;
        strcpy(value, testCfg[0]);
    }
    else if (strcmp("config_2", name) == 0)
    {
        if (valueSize < strlen(testCfg[1]))
            return CSWP_BAD_ARGS;
        strcpy(value, testCfg[1]);
    }
    else
        return CSWP_BAD_ARGS;

    return CSWP_SUCCESS;
}

static int test_impl_get_device_capabilities(struct _cswp_server_state_t* state, unsigned deviceIndex,  varint_t* capabilities, varint_t* capabilitiesData)
{
    *capabilitiesData = 0;
    if (strcmp("mem-ap.v2", state->deviceTypes[deviceIndex]) == 0)
      *capabilities = CSWP_CAP_REG | CSWP_CAP_MEM | CSWP_CAP_MEM_POLL;
    else if (strcmp("mem-ap.v1", state->deviceTypes[deviceIndex]) == 0)
      *capabilities = CSWP_CAP_REG | CSWP_CAP_MEM | CSWP_CAP_MEM_POLL;
    else if (strcmp("memory", state->deviceTypes[deviceIndex]) == 0)
      *capabilities = CSWP_CAP_MEM | CSWP_CAP_MEM_POLL;
    else if (strcmp("dap.v6", state->deviceTypes[deviceIndex]) == 0)
      *capabilities = CSWP_CAP_REG;
    else if (strcmp("dap.v5", state->deviceTypes[deviceIndex]) == 0)
      *capabilities = CSWP_CAP_REG;
    else if (strcmp("jtag.ap", state->deviceTypes[deviceIndex]) == 0)
      *capabilities = CSWP_CAP_REG;
    else if (strcmp("cscomp", state->deviceTypes[deviceIndex]) == 0)
      *capabilities = CSWP_CAP_REG;
    else if (strcmp("linux.cscomp", state->deviceTypes[deviceIndex]) == 0)
      *capabilities = CSWP_CAP_REG;
    else
      *capabilities = CSWP_CAP_REG | CSWP_CAP_MEM | CSWP_CAP_MEM_POLL;

    return CSWP_SUCCESS;
}

static int test_impl_reg_read(struct _cswp_server_state_t* state, unsigned deviceIndex, int registerID, uint32_t* value)
{
    if (deviceIndex != 0)
        return CSWP_UNSUPPORTED;

    if (registerID > 10)
        return CSWP_BAD_ARGS;

    *value = testRegs[registerID];

    return CSWP_SUCCESS;
}

static int test_impl_reg_write(struct _cswp_server_state_t* state, unsigned deviceIndex, int registerID, uint32_t value)
{
    if (deviceIndex != 0)
        return CSWP_UNSUPPORTED;

    if (registerID > 10)
        return CSWP_BAD_ARGS;

    testRegs[registerID] = value;

    return CSWP_SUCCESS;
}

static int test_impl_mem_read(struct _cswp_server_state_t* state, unsigned deviceIndex,
                uint64_t address, size_t size,
                cswp_access_size_t accessSize, unsigned flags, uint8_t* pData)
{
    if (deviceIndex != 0)
        return CSWP_UNSUPPORTED;

    if (address > sizeof(testMem) || (address+size) > sizeof(testMem))
        return CSWP_BAD_ARGS;

    memcpy(pData, testMem+address, size);

    return CSWP_SUCCESS;
}


static int test_impl_mem_write(struct _cswp_server_state_t* state, unsigned deviceIndex,
                 uint64_t address, size_t size,
                 cswp_access_size_t accessSize, unsigned flags, const uint8_t* pData)
{
    if (deviceIndex != 0)
        return CSWP_UNSUPPORTED;

    if (address > sizeof(testMem) || (address+size) > sizeof(testMem))
        return CSWP_BAD_ARGS;

    memcpy(testMem+address, pData, size);

    return CSWP_SUCCESS;
}

static int test_impl_mem_poll(struct _cswp_server_state_t* state, int deviceIndex,
                              uint64_t address, size_t size,
                              cswp_access_size_t accessSize, unsigned flags,
                              unsigned tries, unsigned interval,
                              const uint8_t* pMask, const uint8_t* pValue,
                              uint8_t* pData)
{
    if (deviceIndex != 0)
        return CSWP_UNSUPPORTED;

    if (address > sizeof(testMem) || (address+size) > sizeof(testMem))
        return CSWP_BAD_ARGS;

    memcpy(pData, testMem+address, size);

    return CSWP_SUCCESS;
}

const cswp_server_impl_t testImpl = {
    /*.init = */ test_impl_init,
    /*.term = */ test_impl_term,
    /*.init_devices = */ NULL,
    /*.clear_devices = */ NULL,
    /*.device_add = */ test_impl_device_add,
    /*.device_open = */ test_impl_device_open,
    /*.device_close = */ NULL,
    /*.set_config = */ test_impl_set_config,
    /*.get_config = */ test_impl_get_config,
    /*.get_device_capabilities = */ test_impl_get_device_capabilities,
    /*.register_list_build = */ NULL,
    /*.register_read = */ test_impl_reg_read,
    /*.register_write = */ test_impl_reg_write,
    /*.mem_read = */ test_impl_mem_read,
    /*.mem_write = */ test_impl_mem_write,
    /*.mem_poll = */ test_impl_mem_poll,
};

static void test_init_term()
{
    int res;
    cswp_client_t client;
    cswp_server_state_t state;
    char ID[256];
    unsigned protoVer, svrVer;

    state.impl = &testImpl;
    testClientTransport.priv = calloc(1, sizeof(cswp_test_client_priv_t));
    ((cswp_test_client_priv_t*)testClientTransport.priv)->serverState = &state;
    cswp_client_init(&client, &testClientTransport);
    res = cswp_init(&client,
                    "Test client",
                    &protoVer, ID, sizeof(ID), &svrVer);

    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(1, protoVer);
    CHECK_EQUAL(0, strcmp("AMIS PoC CSWP Server", ID));
    CHECK_EQUAL(0x100, svrVer);

    CHECK_EQUAL(0, state.deviceCount);

    res = cswp_term(&client);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    res = cswp_client_term(&client);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    free(testClientTransport.priv);
}

static void do_init(cswp_client_t* client, cswp_client_transport_t* transport)
{
    int res;
    cswp_server_state_t* state;

    state = calloc(1, sizeof(cswp_server_state_t));
    state->impl = &testImpl;

    transport->priv = calloc(1, sizeof(cswp_test_client_priv_t));
    ((cswp_test_client_priv_t*)transport->priv)->serverState = state;

    cswp_client_init(client, transport);
    res = cswp_init(client,
                    "Test client",
                    NULL, NULL, 0, NULL);

    CHECK_EQUAL(CSWP_SUCCESS, res);
}

static void do_term(cswp_client_t* client, cswp_client_transport_t* transport)
{
    int res;
    cswp_test_client_priv_t* priv = (cswp_test_client_priv_t*)transport->priv;

    res = cswp_term(client);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    res = cswp_client_term(client);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    free(priv->serverState);
    free(priv);
}


static void test_client_info()
{
    cswp_client_t client;
    int res;

    do_init(&client, &testClientTransport);

    res = cswp_client_info(&client, "A message from the client");
    CHECK_EQUAL(CSWP_SUCCESS, res);

    do_term(&client, &testClientTransport);
}


static void test_set_get_devices()
{
    cswp_client_t client;
    cswp_test_client_priv_t* priv;
    int res;
    const char* devices[] = {
        "A device",
        "Another device",
        "And another"
    };
    const char* types[] = {
        "Type 1",
        "Type 2",
        "Type 3 or Type 4"
    };
    unsigned getDeviceCount;
    char getDeviceBufs[4][256];
    char* getDeviceBuf[] = {
        getDeviceBufs[0],
        getDeviceBufs[1],
        getDeviceBufs[2],
        getDeviceBufs[3],
    };
    char getDeviceTypeBufs[3][32];
    char* getDeviceTypeBuf[] = {
        getDeviceTypeBufs[0],
        getDeviceTypeBufs[1],
        getDeviceTypeBufs[2]
    };
    do_init(&client, &testClientTransport);
    priv = (cswp_test_client_priv_t*)testClientTransport.priv;

    res = cswp_set_devices(&client, 3, devices, types);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    CHECK_EQUAL(3, priv->serverState->deviceCount);
    CHECK_EQUAL(0, strcmp("A device", priv->serverState->deviceNames[0]));
    CHECK_EQUAL(0, strcmp("Another device", priv->serverState->deviceNames[1]));
    CHECK_EQUAL(0, strcmp("And another", priv->serverState->deviceNames[2]));
    CHECK_EQUAL(0, strcmp("Type 1", priv->serverState->deviceTypes[0]));
    CHECK_EQUAL(0, strcmp("Type 2", priv->serverState->deviceTypes[1]));
    CHECK_EQUAL(0, strcmp("Type 3 or Type 4", priv->serverState->deviceTypes[2]));

    /* TODO: device info */

    res = cswp_get_devices(&client, &getDeviceCount, getDeviceBuf, 4, 256, getDeviceTypeBuf, 3, 32);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    CHECK_EQUAL(3, getDeviceCount);
    CHECK_EQUAL(0, strcmp("A device", getDeviceBuf[0]));
    CHECK_EQUAL(0, strcmp("Another device", getDeviceBuf[1]));
    CHECK_EQUAL(0, strcmp("And another", getDeviceBuf[2]));
    CHECK_EQUAL(0, strcmp("Type 1", getDeviceTypeBuf[0]));
    CHECK_EQUAL(0, strcmp("Type 2", getDeviceTypeBuf[1]));
    CHECK_EQUAL(0, strcmp("Type 3 or Type 4", getDeviceTypeBuf[2]));

    do_term(&client, &testClientTransport);
}


static void test_get_system_description()
{
    int res;
    cswp_client_t client;
    unsigned descriptionFormat, descriptionSize;
    uint8_t descriptionDataBuffer[10];

    do_init(&client, &testClientTransport);

    ((cswp_test_client_priv_t*)testClientTransport.priv)->serverState->systemDescription = NULL;
    res = cswp_get_system_description(&client,
                                      &descriptionFormat,
                                      &descriptionSize,
                                      descriptionDataBuffer,
                                      10);
    CHECK_EQUAL(CSWP_UNSUPPORTED, res);

    uint8_t desc[8] = {0x1, 0x2, 0x3, 0x4, 0x5, 0x6, 0x7, 0x8};
    ((cswp_test_client_priv_t*)testClientTransport.priv)->serverState->systemDescription = desc;
    ((cswp_test_client_priv_t*)testClientTransport.priv)->serverState->systemDescriptionSize = 8;
    ((cswp_test_client_priv_t*)testClientTransport.priv)->serverState->systemDescriptionFormat = 0;
    res = cswp_get_system_description(&client,
                                      &descriptionFormat,
                                      &descriptionSize,
                                      descriptionDataBuffer,
                                      10);
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(0, descriptionFormat);
    CHECK_EQUAL(8, descriptionSize);
    CHECK_EQUAL(0, memcmp(descriptionDataBuffer, desc, 8*sizeof(uint8_t)));

    do_term(&client, &testClientTransport);
}


static void do_setup_devices(cswp_client_t* client)
{
    int res;
    const char* devices[] = {
        "Device 1",
        "Device 2",
    };
    const char* types[] = {
        "Type 1",
        "Type 2",
    };

    res = cswp_set_devices(client, 2, devices, types);
    CHECK_EQUAL(CSWP_SUCCESS, res);
}


static void test_dev_open_close()
{
    cswp_client_t client;
    int res;
    char devInfo[256];

    do_init(&client, &testClientTransport);
    do_setup_devices(&client);

    res = cswp_device_open(&client, 0, devInfo, sizeof(devInfo));
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(0, strcmp("Device 0 info", devInfo));

    res = cswp_device_open(&client, 1, devInfo, sizeof(devInfo));
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(0, strcmp("Device 1 info", devInfo));

    res = cswp_device_close(&client, 0);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    res = cswp_device_close(&client, 1);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    do_term(&client, &testClientTransport);
}


static void do_open_device(cswp_client_t* client, int deviceNo)
{
    int res;
    res = cswp_device_open(client, deviceNo, NULL, 0);
    CHECK_EQUAL(CSWP_SUCCESS, res);
}


static void test_config()
{
    cswp_client_t client;
    int res;
    char value[1024];

    do_init(&client, &testClientTransport);
    do_setup_devices(&client);
    do_open_device(&client, 0);

    res = cswp_set_config(&client, 0, "config_1", "foo");
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(0, strcmp("foo", testCfg[0]));

    res = cswp_set_config(&client, 0, "config_2", "bar");
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(0, strcmp("bar", testCfg[1]));

    res = cswp_set_config(&client, 1, "config_1", "bar");
    CHECK_EQUAL(CSWP_UNSUPPORTED, res);

    res = cswp_set_config(&client, 0, "config_3", "bar");
    CHECK_EQUAL(CSWP_BAD_ARGS, res);

    res = cswp_get_config(&client, 0, "config_1", value, sizeof(value));
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(0, strcmp("foo", value));

    res = cswp_get_config(&client, 0, "config_2", value, sizeof(value));
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(0, strcmp("bar", value));

    do_term(&client, &testClientTransport);
}


static void test_get_device_capabilities()
{
    cswp_client_t client;
    int res;
    const char* devices[] = {
        "Device 1",
        "Device 2",
    };
    const char* types[] = {
        "mem-ap.v2",
        "dap.v6",
    };
    unsigned capabilities, capabilityData;

    do_init(&client, &testClientTransport);

    // Set devices
    res = cswp_set_devices(&client, 2, devices, types);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    res = cswp_get_device_capabilities(&client, 0, &capabilities, &capabilityData);
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(CSWP_CAP_REG | CSWP_CAP_MEM | CSWP_CAP_MEM_POLL, capabilities);
    CHECK_EQUAL(0, capabilityData);

    res = cswp_get_device_capabilities(&client, 1, &capabilities, &capabilityData);
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(CSWP_CAP_REG, capabilities);
    CHECK_EQUAL(0, capabilityData);

    do_term(&client, &testClientTransport);
}


static void test_reg_list()
{
    cswp_client_t client;
    int res;
    unsigned regCount;
    unsigned int i;
    cswp_register_info_t registerInfo[20];
    char strbuf[1024];

    do_init(&client, &testClientTransport);
    do_setup_devices(&client);
    do_open_device(&client, 0);
    do_open_device(&client, 1);

    res = cswp_device_reg_list(&client, 0, &regCount,
                               registerInfo, 20,
                               strbuf, 1024);
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(10, regCount);

    for (i = 0; i < 10; ++i)
    {
        char expName[256];
        char expDisplayName[256];
        char expDescription[256];
        sprintf(expName, "R_%d", i);
        sprintf(expDisplayName, "R %d", i);
        sprintf(expDescription, "Register %d", i);
        CHECK_EQUAL(i, registerInfo[i].id);
        CHECK_EQUAL(0, strcmp(expName, registerInfo[i].name));
        CHECK_EQUAL(1, registerInfo[i].size);
        CHECK_EQUAL(0, strcmp(expDisplayName, registerInfo[i].displayName));
        CHECK_EQUAL(0, strcmp(expDescription, registerInfo[i].description));
    }

    res = cswp_device_reg_list(&client, 1, &regCount,
                               registerInfo, 20,
                               strbuf, 1024);
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(0, regCount);

    do_term(&client, &testClientTransport);
}


static void test_reg_access()
{
    cswp_client_t client;
    int res;
    unsigned regIDs[3];
    uint32_t regVals[3];

    do_init(&client, &testClientTransport);
    do_setup_devices(&client);
    do_open_device(&client, 0);

    memset(testRegs, 0, sizeof(testRegs));
    testRegs[1] = 0xDEADBEEF;
    testRegs[2] = 0x80000000;
    testRegs[6] = 0x12345678;

    regIDs[0] = 1;
    regIDs[1] = 6;
    regIDs[2] = 2;

    res = cswp_device_reg_read(&client, 0, 3, regIDs, regVals, 3);
    CHECK_EQUAL(CSWP_SUCCESS, res);
    /* values returned in order of read */
    CHECK_EQUAL(0xDEADBEEF, regVals[0]);
    CHECK_EQUAL(0x12345678, regVals[1]);
    CHECK_EQUAL(0x80000000, regVals[2]);

    regIDs[0] = 4;
    regIDs[1] = 9;
    regIDs[2] = 0;
    regVals[0] = 0x0BADF00D;
    regVals[1] = 0xAA55AA55;
    regVals[2] = 0x00000001;

    res = cswp_device_reg_write(&client, 0, 3, regIDs, regVals, 3);
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(0x0BADF00D, testRegs[4]);
    CHECK_EQUAL(0xAA55AA55, testRegs[9]);
    CHECK_EQUAL(0x00000001, testRegs[0]);

    do_term(&client, &testClientTransport);
}


static void test_mem_access()
{
    cswp_client_t client;
    uint8_t readBuf[16];
    int res;
    size_t bytesRead;

    do_init(&client, &testClientTransport);
    do_setup_devices(&client);
    do_open_device(&client, 0);

    memcpy(testMem, "Hello world", 12);

    res = cswp_device_mem_read(&client, 0, 0, 12, CSWP_ACCESS_SIZE_DEF, 0, readBuf, &bytesRead);
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(0, memcmp(readBuf, "Hello world", 12));
    CHECK_EQUAL(12, bytesRead);

    res = cswp_device_mem_read(&client, 0, 4, 8, CSWP_ACCESS_SIZE_DEF, 0, readBuf, &bytesRead);
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(0, memcmp(readBuf, "o world", 8));
    CHECK_EQUAL(8, bytesRead);

    res = cswp_device_mem_write(&client, 0, 0, 14, CSWP_ACCESS_SIZE_DEF, 0, (uint8_t*)"Goodbye world");
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(0, memcmp(testMem, "Goodbye world", 14));

    res = cswp_device_mem_write(&client, 0, 4, 6, CSWP_ACCESS_SIZE_DEF, 0, (uint8_t*)"night");
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(0, memcmp(testMem, "Goodnight", 10));

    res = cswp_device_mem_poll(&client, 0, 0, 4, CSWP_ACCESS_SIZE_DEF, 0, 10, 100,
                               (uint8_t*)"\x7F\x3F\x1F\x0F", (uint8_t*)"nigh", readBuf, &bytesRead);
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(0, memcmp(testMem, "Good", 4));
    CHECK_EQUAL(4, bytesRead);

    do_term(&client, &testClientTransport);
}


static void test_batch()
{
    cswp_client_t client;
    cswp_test_client_priv_t* testPriv;
    int res;
    unsigned regIDs[3];
    uint32_t regVals1[3];
    uint32_t regVals2[3];
    uint32_t regVals3[3];

    do_init(&client, &testClientTransport);
    do_setup_devices(&client);
    do_open_device(&client, 0);

    testPriv = (cswp_test_client_priv_t*)testClientTransport.priv;

    cswp_buffer_clear(testPriv->cmd);

    /* Empty batch */
    res = cswp_batch_begin(&client, 0);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    /* nothing sent to transport yet */
    CHECK_EQUAL(0, testPriv->cmd->used);

    /* Complete batch */
    uint32_t opsComplete = 0;
    res = cswp_batch_end(&client, &opsComplete);
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(0, opsComplete);

    /* nothing sent to transport: no commands */
    CHECK_EQUAL(0, testPriv->cmd->used);

    memset(testRegs, 0, sizeof(testRegs));
    testRegs[1] = 0xDEADBEEF;
    testRegs[2] = 0x80000000;
    testRegs[6] = 0x12345678;

    /* Batch of register accesses */
    cswp_buffer_clear(testPriv->cmd);
    res = cswp_batch_begin(&client, 0);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    /* read once */
    regIDs[0] = 1;
    regIDs[1] = 6;
    regIDs[2] = 2;
    res = cswp_device_reg_read(&client, 0, 3, regIDs, regVals1, 3);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    /* write 2 regs*/
    regVals2[0] = 0x0BADF00D;
    regVals2[1] = 0x55AA;
    res = cswp_device_reg_write(&client, 0, 2, regIDs, regVals2, 2);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    /* read again */
    res = cswp_device_reg_read(&client, 0, 3, regIDs, regVals3, 3);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    /* nothing sent to transport yet */
    CHECK_EQUAL(0, testPriv->cmd->used);

    /* Complete batch */
    res = cswp_batch_end(&client, &opsComplete);
    CHECK_EQUAL(CSWP_SUCCESS, res);
    CHECK_EQUAL(3, opsComplete);

    CHECK_EQUAL(0xDEADBEEF, regVals1[0]);
    CHECK_EQUAL(0x12345678, regVals1[1]);
    CHECK_EQUAL(0x80000000, regVals1[2]);

    CHECK_EQUAL(0x0BADF00D, regVals3[0]);
    CHECK_EQUAL(0x000055AA, regVals3[1]);
    CHECK_EQUAL(0x80000000, regVals3[2]);

    /* all sent to transport now */
    CHECK_EQUAL(34, testPriv->cmd->used);

    /* failing batch commands: continue on error */
    cswp_buffer_clear(testPriv->cmd);
    res = cswp_batch_begin(&client, 0);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    memset(regVals1, 0, sizeof(regVals1));
    memset(regVals2, 0, sizeof(regVals2));
    memset(regVals3, 0, sizeof(regVals3));

    /* read regs on device 0 */
    res = cswp_device_reg_read(&client, 0, 3, regIDs, regVals1, 3);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    /* read regs on device 1 (invalid) */
    res = cswp_device_reg_read(&client, 1, 3, regIDs, regVals2, 3);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    /* read device 0 again */
    res = cswp_device_reg_read(&client, 0, 3, regIDs, regVals3, 3);
    CHECK_EQUAL(CSWP_SUCCESS, res);

    /* nothing sent to transport yet */
    CHECK_EQUAL(0, testPriv->cmd->used);

    /* Complete batch */
    res = cswp_batch_end(&client, &opsComplete);
    CHECK_EQUAL(CSWP_UNSUPPORTED, res);
    CHECK_EQUAL(1, opsComplete);

    /* 1st command was executed */
    CHECK_EQUAL(0x0BADF00D, regVals1[0]);
    CHECK_EQUAL(0x000055AA, regVals1[1]);
    CHECK_EQUAL(0x80000000, regVals1[2]);

    /* others weren't */
    CHECK_EQUAL(0, regVals2[0]);
    CHECK_EQUAL(0, regVals2[1]);
    CHECK_EQUAL(0, regVals2[2]);
    CHECK_EQUAL(0, regVals3[0]);
    CHECK_EQUAL(0, regVals3[1]);
    CHECK_EQUAL(0, regVals3[2]);

    do_term(&client, &testClientTransport);
}


void test_server()
{
    test_init_term();
    test_client_info();
    test_set_get_devices();
    test_get_system_description();
    test_dev_open_close();
    test_config();
    test_get_device_capabilities();
    test_reg_list();
    test_reg_access();
    test_mem_access();

    test_batch();
}

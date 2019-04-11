// cswp_client.c
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "cswp_client.h"
#include "cswp_client_commands.h"
#include "cswp_buffer.h"

#include <string.h>
#include <stdio.h>
#include <stdarg.h>

#define BUFFER_SIZE 32768
#define ERROR_MESSAGE_SIZE 1024

/* Header is:
 * uint32 size
 * varint command count (allow 10 bytes)
 * uint8 error behaviour
 */
#define CSWP_REQ_HEADER_SIZE (4+10+1)

/*
 * In order to support batch messages, command handling is split into two
 * parts - the request is encoded into the command buffer and a callback can
 * be registered to handle the response when it is received.
 *
 * After encoding the command, cswp_client_process() is called - when not in
 * batch mode the command will be executed immediately, but in batch mode the
 * command will not be executed until cswp_batch_end() is called
 *
 * After each block of commands is executed, the list of pending responses is
 * processed
 */

typedef int (*complete_func)(cswp_client_t* client, void* replyData);

/**
 * Data required to process a response
 */
typedef struct _pending_response_t
{
    /** Expected response message type */
    cswp_commands_t type;

    /** Function to call to process response */
    complete_func complete;

    /** Argument to pass to completion response */
    void* replyData;

    /** Pointer to next response */
    struct _pending_response_t* next;
} pending_response_t;


/**
 * Batch mode and whether to continue / abort on error
 */
typedef enum
{
    BATCH_NONE,
    BATCH_CONTINUE,
    BATCH_ABORT
} batch_mode_t;

/**
 * Private data for CSWP client
 */
typedef struct _cswp_client_priv_t
{
    /** Transport interface */
    cswp_client_transport_t* transport;

    /** Header buffers */
    CSWP_BUFFER* hdr;
    /** Request buffer */
    CSWP_BUFFER* cmd;
    /** Response buffer */
    CSWP_BUFFER* rsp;

    /** Batch mode */
    batch_mode_t batch_mode;
    /** Number of command in batch request */
    int num_cmds;

    /** Expected response sequence */
    pending_response_t* pending_responses;
} cswp_client_priv_t;


/*
 * Add an expected response
 */
static void cswp_client_push_request(cswp_client_t* client,
                                     cswp_commands_t type,
                                     complete_func complete,
                                     void* replyData)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    pending_response_t** pPending;
    pending_response_t* pending_response;

    // go to end of list
    pPending = &priv->pending_responses;
    while (*pPending != NULL)
        pPending = &((*pPending)->next);

    // add to end of list
    pending_response = (pending_response_t*)calloc(sizeof(pending_response_t), 1);
    pending_response->type = type;
    pending_response->complete = complete;
    pending_response->replyData = replyData;
    *pPending = pending_response;

    ++priv->num_cmds;
}

/*
 * Initialise client
 */
int cswp_client_init(cswp_client_t* client,
                     cswp_client_transport_t* transport)
{
    cswp_client_priv_t* priv;

    /* Allocate buffer for error message */
    client->errorMsg = calloc(1, ERROR_MESSAGE_SIZE);

    /* Allocate and initialise private data */
    priv = calloc(sizeof(cswp_client_priv_t), 1);
    priv->transport = transport;
    priv->hdr = cswp_buffer_alloc(CSWP_REQ_HEADER_SIZE);
    priv->cmd = cswp_buffer_alloc(BUFFER_SIZE);
    priv->rsp = cswp_buffer_alloc(BUFFER_SIZE);
    priv->batch_mode = BATCH_NONE;
    client->priv = priv;

    return CSWP_SUCCESS;
}


int cswp_client_term(cswp_client_t* client)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;

    /* Cleanup error message */
    free(client->errorMsg);
    client->errorMsg = NULL;

    /* Cleanup private data */
    if (priv)
    {
        cswp_buffer_free(priv->hdr);
        cswp_buffer_free(priv->cmd);
        cswp_buffer_free(priv->rsp);

        free(client->priv);
        client->priv = NULL;
    }

    return CSWP_SUCCESS;
}

/*
 * Prepare the request buffer to write command data
 */
static int cswp_client_prepare_cmd(cswp_client_t* client)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;

    if (priv->batch_mode == BATCH_NONE)
    {
        /* reset buffer, reserving space for message header */
        priv->cmd->pos = CSWP_REQ_HEADER_SIZE;
        priv->cmd->used = CSWP_REQ_HEADER_SIZE;
        priv->pending_responses = NULL;
        priv->num_cmds = 0;
    }

    return CSWP_SUCCESS;
}


/*
 * Process a server response
 */
static int cswp_client_process_response(cswp_client_t* client,
                                        pending_response_t* pendingRsp)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    varint_t msgType, errCode;
    int res;

    /* Examine the header */
    res = cswp_decode_response_header(priv->rsp, &msgType, &errCode);
    /* Check the message type is as expected */
    if (res == CSWP_SUCCESS && msgType != pendingRsp->type)
    {
        res = cswp_client_error(client, CSWP_COMMS, "Unexpected response: 0x%lX", msgType);
    }
    /* Check the command completed successfully */
    if (res == CSWP_SUCCESS && errCode != CSWP_SUCCESS)
    {
        res = errCode;
        cswp_decode_error_response_body(priv->rsp, client->errorMsg, ERROR_MESSAGE_SIZE);
    }
    if (res == CSWP_SUCCESS)
    {
        /* Call any message specific response handler */
        if (pendingRsp->complete)
            res = pendingRsp->complete(client, pendingRsp->replyData);
    }

    return res;
}

/*
 * Send request and receive response
 */
static int cswp_client_transact(cswp_client_t* client, unsigned* opsCompleted)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res;
    uint32_t reqSize, rspSize;
    size_t reqOffset;
    varint_t numRsps;
    uint8_t* pBuf;
    uint8_t *pHdr;
    pending_response_t* pendingRsp;

    /* encode message header */
    cswp_buffer_clear(priv->hdr);
    cswp_buffer_put_varint(priv->hdr, priv->num_cmds);
    cswp_buffer_put_uint8(priv->hdr, priv->batch_mode);

    /* Insert header before message body */
    /*   Calculate position where header will start */
    reqOffset = CSWP_REQ_HEADER_SIZE - 4 - priv->hdr->used;
    reqSize = priv->cmd->used - reqOffset;
    pBuf = priv->cmd->buf + reqOffset;
    pHdr = pBuf;
    /*   Inject message length */
    *pHdr++ = (reqSize & 0xFF);
    *pHdr++ = ((reqSize >> 8) & 0xFF);
    *pHdr++ = ((reqSize >> 16) & 0xFF);
    *pHdr++ = ((reqSize >> 24) & 0xFF);
    /*    Copy header */
    memcpy(pHdr, priv->hdr->buf, priv->hdr->used);

    if (opsCompleted)
        *opsCompleted = 0;

    /* Send to server */
    res = priv->transport->send(client, priv->transport, pBuf, reqSize);

    /* Get response */
    if (res == CSWP_SUCCESS)
        res = priv->transport->receive(client, priv->transport, priv->rsp->buf, priv->rsp->size, &priv->rsp->used);

    if (res == CSWP_SUCCESS)
    {
        /* Decode header */
        cswp_buffer_seek(priv->rsp, 0);
        cswp_buffer_get_uint32(priv->rsp, &rspSize);
        /* check reply length matches data received */
        if (rspSize > priv->rsp->used)
            res = cswp_client_error(client, CSWP_COMMS, "Incomplete response received.  Received %d bytes, expected %d",
                                    priv->rsp->used, rspSize);
    }

    if (res == CSWP_SUCCESS)
    {
        /* Check all responses received */
        cswp_buffer_get_varint(priv->rsp, &numRsps);
        if (numRsps != priv->num_cmds)
            res = cswp_client_error(client, CSWP_COMMS, "Incomplete response received.  Received %d responses, expected %d",
                                    numRsps, priv->num_cmds);
    }

    if (res == CSWP_SUCCESS)
    {
        /* process each response */
        pendingRsp = priv->pending_responses;
        while (pendingRsp != NULL && res == CSWP_SUCCESS)
        {
            res = cswp_client_process_response(client, pendingRsp);

            if (opsCompleted && res == CSWP_SUCCESS)
                (*opsCompleted)++;

            pendingRsp = pendingRsp->next;
        }
        /* TODO: continue processing on error? */
    }

    /* cleanup response list */
    pendingRsp = priv->pending_responses;
    while (pendingRsp != NULL)
    {
        pending_response_t* d = pendingRsp;
        if (pendingRsp->replyData)
            free(pendingRsp->replyData);
        pendingRsp = pendingRsp->next;
        free(d);
    }
    priv->pending_responses = NULL;

    return res;
}

/*
 * Process a request
 *
 * Unless in batch mode, call cswp_client_transact()
 */
static int cswp_client_process(cswp_client_t* client)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res = CSWP_SUCCESS;

    if (priv->batch_mode == BATCH_NONE && priv->num_cmds > 0)
        res = cswp_client_transact(client, NULL);

    return res;
}

/*
 * Write an error message into client error message buffer
 */
int cswp_client_error(cswp_client_t* client, int errorCode, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    vsnprintf(client->errorMsg, ERROR_MESSAGE_SIZE, fmt, args);
    va_end(args);
    return errorCode;
}

/**
 * Reply data for CSWP_INIT command
 */
struct reply_data_init {
    /** Server protocol version returned by CSWP server */
    unsigned* serverProtocolVersion;
    /** Buffer for server ID returned by CSWP server */
    char* serverID;
    /** Size of serverID buffer */
    size_t serverIDSize;
    /** Server version returned by CSWP server */
    unsigned* serverVersion;
};

/*
 * Completion function for CSWP_INIT
 */
static int cswp_init_complete(cswp_client_t* client,
                              void* replyData)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    struct reply_data_init* initReply = (struct reply_data_init*)replyData;
    varint_t protoVer, svrVer;
    int res;

    res = cswp_decode_init_response_body(priv->rsp, &protoVer, initReply->serverID, initReply->serverIDSize, &svrVer);
    if (res == CSWP_SUCCESS)
    {
        if (initReply->serverProtocolVersion)
            *initReply->serverProtocolVersion = protoVer;
        if (initReply->serverVersion)
            *initReply->serverVersion = svrVer;
    }

    return res;
}

int cswp_init(cswp_client_t* client,
              const char*    clientID,
              unsigned*      serverProtocolVersion,
              char*          serverID,
              size_t         serverIDSize,
              unsigned*      serverVersion)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res = CSWP_SUCCESS;

    if (priv->transport->connect)
        res = priv->transport->connect(client, priv->transport);
    if (res == CSWP_SUCCESS)
    {
        cswp_client_prepare_cmd(client);
        res = cswp_encode_init_command(priv->cmd, CSWP_PROTOCOL_v1, clientID);
    }
    if (res == CSWP_SUCCESS)
    {
        struct reply_data_init* initReply = calloc(1, sizeof(struct reply_data_init));
        initReply->serverProtocolVersion = serverProtocolVersion;
        initReply->serverID = serverID;
        initReply->serverIDSize = serverIDSize;
        initReply->serverVersion = serverVersion;
        cswp_client_push_request(client, CSWP_INIT, cswp_init_complete, initReply);
    }
    if (res == CSWP_SUCCESS)
        res = cswp_client_process(client);

    return res;
}


int cswp_term(cswp_client_t* client)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res;

    cswp_client_prepare_cmd(client);
    res = cswp_encode_term_command(priv->cmd);
    if (res == CSWP_SUCCESS)
        cswp_client_push_request(client, CSWP_TERM, NULL, 0);
    if (res == CSWP_SUCCESS)
        res = cswp_client_process(client);

    if (priv->transport->disconnect)
        priv->transport->disconnect(client, priv->transport);

    return res;
}


int cswp_batch_begin(cswp_client_t* client, int abortOnError)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;

    /* Clear buffer */
    cswp_client_prepare_cmd(client);

    if (abortOnError)
        priv->batch_mode = BATCH_ABORT;
    else
        priv->batch_mode = BATCH_CONTINUE;

    return CSWP_SUCCESS;
}


int cswp_batch_end(cswp_client_t* client, unsigned *opsCompleted)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res = CSWP_SUCCESS;

    /* Process any pending operations */
    if (priv->num_cmds > 0)
        res = cswp_client_transact(client, opsCompleted);

    priv->batch_mode = BATCH_NONE;

    return res;
}


int cswp_client_info(cswp_client_t* client,
                     const char* message)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res;

    cswp_client_prepare_cmd(client);
    res = cswp_encode_client_info_command(priv->cmd, message);
    if (res == CSWP_SUCCESS)
        cswp_client_push_request(client, CSWP_CLIENT_INFO, NULL, 0);
    if (res == CSWP_SUCCESS)
        res = cswp_client_process(client);

    return res;
}


int cswp_set_devices(cswp_client_t* client,
                     unsigned deviceCount,
                     const char** deviceList,
                     const char** deviceTypes)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res;

    cswp_client_prepare_cmd(client);
    res = cswp_encode_set_devices_command(priv->cmd, deviceCount, deviceList, deviceTypes);
    if (res == CSWP_SUCCESS)
        cswp_client_push_request(client, CSWP_SET_DEVICES, NULL, 0);
    if (res == CSWP_SUCCESS)
        res = cswp_client_process(client);

    return res;
}

/**
 * Reply data for CSWP_GET_DEVICES command
 */
struct reply_data_get_devices {
    /** Number of devices returned by CSWP server*/
    unsigned* deviceCount;
    /** List of devices returned by CSWP server*/
    char** deviceList;
    /** Number of entries in deviceList */
    size_t deviceListSize;
    /** Number of byte for each entry in deviceList */
    size_t deviceListEntrySize;
    /** List of device types returned by CSWP server */
    char** deviceTypes;
    /** Number of entries in deviceTypes */
    size_t deviceTypeSize;
    /** Number of byte for each entry in deviceTypes */
    size_t deviceTypeEntrySize;
};

/*
 * Completion function for CSWP_GET_DEVICES
 */
static int cswp_get_devices_complete(cswp_client_t* client,
                                     void* replyData)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    struct reply_data_get_devices* getDevicesReplyData = (struct reply_data_get_devices*)replyData;
    varint_t devCount;
    int res;
    int i;

    res = cswp_decode_get_devices_response_body(priv->rsp, &devCount);
    if (res == CSWP_SUCCESS)
    {
        *getDevicesReplyData->deviceCount = devCount;
        if (getDevicesReplyData->deviceListSize < devCount)
        {
            res = cswp_client_error(client, CSWP_OUTPUT_BUFFER_OVERFLOW, "Device list too small");
        }
        else if (getDevicesReplyData->deviceTypeSize < devCount)
        {
            res = cswp_client_error(client, CSWP_OUTPUT_BUFFER_OVERFLOW, "Device type list too small");
        }
        else
            for (i = 0; i < devCount && res == CSWP_SUCCESS; ++i)
            {
                res = cswp_buffer_get_string(priv->rsp, getDevicesReplyData->deviceList[i], getDevicesReplyData->deviceListEntrySize);
                res = cswp_buffer_get_string(priv->rsp, getDevicesReplyData->deviceTypes[i], getDevicesReplyData->deviceTypeEntrySize);
            }
    }

    return res;
}

int cswp_get_devices(cswp_client_t* client,
                     unsigned* deviceCount,
                     char** deviceList,
                     size_t deviceListSize,
                     size_t deviceListEntrySize,
                     char** deviceTypes,
                     size_t deviceTypeSize,
                     size_t deviceTypeEntrySize)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res;

    cswp_client_prepare_cmd(client);
    res = cswp_encode_get_devices_command(priv->cmd);
    if (res == CSWP_SUCCESS)
    {
        struct reply_data_get_devices* replyData = calloc(1, sizeof(struct reply_data_get_devices));
        replyData->deviceCount = deviceCount;
        replyData->deviceList = deviceList;
        replyData->deviceListSize = deviceListSize;
        replyData->deviceListEntrySize = deviceListEntrySize;
        replyData->deviceTypes = deviceTypes;
        replyData->deviceTypeSize = deviceTypeSize;
        replyData->deviceTypeEntrySize = deviceTypeEntrySize;
        cswp_client_push_request(client, CSWP_GET_DEVICES, cswp_get_devices_complete, replyData);
    }
    if (res == CSWP_SUCCESS)
        res = cswp_client_process(client);

    return res;
}

/**
 * Reply data for CSWP_GET_SYSTEM_DESCRIPTION command
 */
struct reply_data_get_system_description {
    /** Format of the description */
    unsigned* descriptionFormat;
    /** Size of the description */
    unsigned* descriptionSize;
    /** Buffer for the description data */
    uint8_t* descriptionDataBuffer;
    /** Size of the provided buffer */
    size_t bufferSize;
};

/*
 * Completion function for CSWP_GET_SYSTEM_DESCRIPTION
 */
static int cswp_get_system_description_complete(cswp_client_t* client,
                                                void* replyData)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    struct reply_data_get_system_description* getSystemDescriptionReplyData = (struct reply_data_get_system_description*)replyData;
    varint_t systemDescriptionFormat, systemDescriptionSize;
    int res;

    res = cswp_decode_get_system_description_response_body(priv->rsp,
                                                           &systemDescriptionFormat,
                                                           &systemDescriptionSize,
                                                           getSystemDescriptionReplyData->descriptionDataBuffer,
                                                           getSystemDescriptionReplyData->bufferSize);
    if (res == CSWP_SUCCESS)
    {
        *getSystemDescriptionReplyData->descriptionFormat = systemDescriptionFormat;
        *getSystemDescriptionReplyData->descriptionSize = systemDescriptionSize;
        if (getSystemDescriptionReplyData->bufferSize < systemDescriptionSize)
        {
            res = cswp_client_error(client, CSWP_OUTPUT_BUFFER_OVERFLOW, "System description buffer too small");
        }
    }

    return res;
}

int cswp_get_system_description(cswp_client_t* client,
                                unsigned* descriptionFormat,
                                unsigned* descriptionSize,
                                uint8_t* descriptionDataBuffer,
                                size_t bufferSize)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res;

    cswp_client_prepare_cmd(client);
    res = cswp_encode_get_system_description_command(priv->cmd);
    if (res == CSWP_SUCCESS)
    {
        struct reply_data_get_system_description* replyData = calloc(1, sizeof(struct reply_data_get_system_description));
        replyData->descriptionFormat = descriptionFormat;
        replyData->descriptionSize = descriptionSize;
        replyData->descriptionDataBuffer = descriptionDataBuffer;
        replyData->bufferSize = bufferSize;
        cswp_client_push_request(client, CSWP_GET_SYSTEM_DESCRIPTION, cswp_get_system_description_complete, replyData);
    }
    if (res == CSWP_SUCCESS)
        res = cswp_client_process(client);

    return res;
}

/**
 * Reply data for CSWP_DEVICE_OPEN command
 */
struct reply_data_device_open {
    /** Device info returned by CSWP server */
    char* deviceInfo;
    /** Size of the deviceInfo buffer */
    size_t deviceInfoSize;
};

/*
 * Completion function for CSWP_DEVICE_OPEN
 */
static int cswp_device_open_complete(cswp_client_t* client,
                                     void* replyData)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    struct reply_data_device_open* deviceOpenReplyData = (struct reply_data_device_open*)replyData;
    int res;

    res = cswp_decode_device_open_response_body(priv->rsp,
                                                deviceOpenReplyData->deviceInfo,
                                                deviceOpenReplyData->deviceInfoSize);

    return res;
}


int cswp_device_open(cswp_client_t* client,
                     unsigned deviceNo,
                     char* deviceInfo,
                     size_t deviceInfoSize)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res;

    cswp_client_prepare_cmd(client);
    res = cswp_encode_device_open_command(priv->cmd, deviceNo);
    if (res == CSWP_SUCCESS)
    {
        struct reply_data_device_open* replyData = calloc(1, sizeof(struct reply_data_device_open));
        replyData->deviceInfo = deviceInfo;
        replyData->deviceInfoSize = deviceInfoSize;
        cswp_client_push_request(client, CSWP_DEVICE_OPEN, cswp_device_open_complete, replyData);
    }
    if (res == CSWP_SUCCESS)
        res = cswp_client_process(client);

    return res;
}


int cswp_device_close(cswp_client_t* client,
                      unsigned deviceNo)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res;

    cswp_client_prepare_cmd(client);
    res = cswp_encode_device_close_command(priv->cmd, deviceNo);
    if (res == CSWP_SUCCESS)
        cswp_client_push_request(client, CSWP_DEVICE_CLOSE, NULL, 0);
    if (res == CSWP_SUCCESS)
        res = cswp_client_process(client);

    return res;
}

int cswp_set_config(cswp_client_t* client,
                    varint_t deviceNo,
                    const char* name,
                    const char* value)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res;

    cswp_client_prepare_cmd(client);
    res = cswp_encode_set_config_command(priv->cmd, deviceNo, name, value);
    if (res == CSWP_SUCCESS)
        cswp_client_push_request(client, CSWP_SET_CONFIG, NULL, 0);
    if (res == CSWP_SUCCESS)
        res = cswp_client_process(client);

    return res;
}

/**
 * Reply data for CSWP_GET_CONFIG command
 */
struct reply_data_get_config {
    /** Buffer for configuration item value */
    char* value;
    /** Size of value buffer */
    size_t valueSize;
};

static int cswp_get_config_complete(cswp_client_t* client,
                                    void* replyData)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    struct reply_data_get_config* getConfigReplyData = (struct reply_data_get_config*)replyData;
    int res;

    res = cswp_decode_get_config_response_body(priv->rsp,
                                               getConfigReplyData->value,
                                               getConfigReplyData->valueSize);

    return res;
}

int cswp_get_config(cswp_client_t* client,
                    varint_t deviceNo,
                    const char* name,
                    char* value,
                    size_t valueSize)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res;

    cswp_client_prepare_cmd(client);
    res = cswp_encode_get_config_command(priv->cmd, deviceNo, name);
    if (res == CSWP_SUCCESS)
    {
        struct reply_data_get_config* replyData = calloc(1, sizeof(struct reply_data_get_config));
        replyData->value = value;
        replyData->valueSize = valueSize;
        cswp_client_push_request(client, CSWP_GET_CONFIG, cswp_get_config_complete, replyData);
        res = cswp_client_process(client);
    }

    return res;
}

/**
 * Reply data for CSWP_GET_DEVICE_CAPABILITIES command
 */
struct reply_data_get_device_capabilities {
    /** Capabilities returned by the CSWP server */
    unsigned* capabilities;
    /** Capability data returned by the CSWP server */
    unsigned* capabilityData;
};

static int cswp_get_device_capabilities_complete(cswp_client_t* client,
                                                 void* replyData)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    struct reply_data_get_device_capabilities* getDeviceCapabilitiesReplyData = (struct reply_data_get_device_capabilities*)replyData;
    int res;
    varint_t capabilities, capabilityData;

    res = cswp_decode_get_device_capabilities_response_body(priv->rsp,
                                                            &capabilities,
                                                            &capabilityData);
    if (res == CSWP_SUCCESS)
    {
        *getDeviceCapabilitiesReplyData->capabilities = capabilities;
        *getDeviceCapabilitiesReplyData->capabilityData = capabilityData;
    }

    return res;
}

int cswp_get_device_capabilities(cswp_client_t* client,
                                 varint_t deviceNo,
                                 unsigned* capabilities,
                                 unsigned* capabilityData)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res;

    cswp_client_prepare_cmd(client);
    res = cswp_encode_get_device_capabilities_command(priv->cmd, deviceNo);
    if (res == CSWP_SUCCESS)
    {
        struct reply_data_get_device_capabilities* replyData = calloc(1, sizeof(struct reply_data_get_device_capabilities));
        replyData->capabilities = capabilities;
        replyData->capabilityData = capabilityData;
        cswp_client_push_request(client, CSWP_GET_DEVICE_CAPABILITIES, cswp_get_device_capabilities_complete, replyData);
        res = cswp_client_process(client);
    }
    return res;
}

/**
 * Reply data for CSWP_REG_LIST command
 */
struct reply_data_reg_list {
    /** Number of registers returned by CSWP server */
    unsigned* registerCount;
    /** Register info returned by CSWP server */
    cswp_register_info_t* registerInfo;
    /** Size of the registerInfo buffer */
    size_t registerInfoSize;
    /** String buffer */
    char *strBuf;
    /** Size of buffer */
    size_t strBufSize;
};

/*
 * Completion function for CSWP_REG_LIST
 */
static int cswp_device_reg_list_complete(cswp_client_t* client,
                                         void* replyData)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    struct reply_data_reg_list* regListReplyData = (struct reply_data_reg_list*)replyData;
    varint_t regCount;
    int res;
    int i;

    res = cswp_decode_reg_list_response_body(priv->rsp, &regCount);
    if (res == CSWP_SUCCESS)
    {
        *regListReplyData->registerCount = regCount;
        if (regListReplyData->registerInfoSize < regCount)
            res = cswp_client_error(client, CSWP_OUTPUT_BUFFER_OVERFLOW, "registerInfo too small");
    }
    if (res == CSWP_SUCCESS)
    {
        /* get register info */

        const size_t nameSize = 256;
        const size_t descSize = 1024;
        char* regName = malloc(nameSize);
        char* displayName = malloc(nameSize);
        char* description = malloc(descSize);
        for (i = 0; i < regCount && res == CSWP_SUCCESS; ++i)
        {
            varint_t regID, regSize;
            res = cswp_decode_reg_info(priv->rsp, &regID,
                                       regName, nameSize,
                                       &regSize,
                                       displayName, nameSize,
                                       description, descSize);
            if (res == CSWP_SUCCESS)
            {
				size_t len;
                regListReplyData->registerInfo[i].id = regID;
                regListReplyData->registerInfo[i].size = regSize;

                len = strlen(regName);
                if (len > regListReplyData->strBufSize)
                {
                    res = cswp_client_error(client, CSWP_OUTPUT_BUFFER_OVERFLOW, "strBuf too small");
                }
                else
                {
                    strcpy(regListReplyData->strBuf, regName);
                    regListReplyData->registerInfo[i].name = regListReplyData->strBuf;
                    regListReplyData->strBuf += len+1;
                    regListReplyData->strBufSize -= len+1;
                }

                len = strlen(displayName);
                if (len > regListReplyData->strBufSize)
                {
                    res = cswp_client_error(client, CSWP_OUTPUT_BUFFER_OVERFLOW, "strBuf too small");
                }
                else
                {
                    strcpy(regListReplyData->strBuf, displayName);
                    regListReplyData->registerInfo[i].displayName = regListReplyData->strBuf;
                    regListReplyData->strBuf += len+1;
                    regListReplyData->strBufSize -= len+1;
                }

                len = strlen(description);
                if (len > regListReplyData->strBufSize)
                {
                    res = cswp_client_error(client, CSWP_OUTPUT_BUFFER_OVERFLOW, "strBuf too small");
                }
                else
                {
                    strcpy(regListReplyData->strBuf, description);
                    regListReplyData->registerInfo[i].description = regListReplyData->strBuf;
                    regListReplyData->strBuf += len+1;
                    regListReplyData->strBufSize -= len+1;
                }
            }
        }
        free(regName);
        free(displayName);
        free(description);
    }

    return res;
}

int cswp_device_reg_list(cswp_client_t* client,
                         unsigned deviceNo,
                         unsigned* registerCount,
                         cswp_register_info_t* registerInfo,
                         size_t registerInfoSize,
                         char *strBuf,
                         size_t strBufSize)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res;

    cswp_client_prepare_cmd(client);
    res = cswp_encode_reg_list_command(priv->cmd, deviceNo);
    if (res == CSWP_SUCCESS)
    {
        struct reply_data_reg_list* replyData = calloc(1, sizeof(struct reply_data_reg_list));
        replyData->registerCount = registerCount;
        replyData->registerInfo = registerInfo;
        replyData->registerInfoSize = registerInfoSize;
        replyData->strBuf = strBuf;
        replyData->strBufSize = strBufSize;
        cswp_client_push_request(client, CSWP_REG_LIST, cswp_device_reg_list_complete, replyData);
    }
    if (res == CSWP_SUCCESS)
        res = cswp_client_process(client);

    return res;
}

/**
 * Reply data for CSWP_REG_READ command
 */
struct reply_data_reg_read {
    /** Buffer for register value */
    uint32_t* registerValues;
    /** Size of the provided buffer */
    size_t registerValuesSize;
};

/*
 * Completion function for CSWP_REG_READ
 */
static int cswp_device_reg_read_complete(cswp_client_t* client,
                                         void* replyData)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    struct reply_data_reg_read* regReadReplyData = (struct reply_data_reg_read*)replyData;
    int res;
    varint_t count;
    int i;

    res = cswp_decode_reg_read_response_body(priv->rsp, &count);
    if (res == CSWP_SUCCESS)
    {
        if (regReadReplyData->registerValuesSize < count)
            res = cswp_client_error(client, CSWP_OUTPUT_BUFFER_OVERFLOW, "registerValues too small");
        else
            for (i = 0; i < count && res == CSWP_SUCCESS; ++i)
                res = cswp_buffer_get_uint32(priv->rsp, &regReadReplyData->registerValues[i]);
    }

    return res;
}

int cswp_device_reg_read(cswp_client_t* client,
                         unsigned deviceNo,
                         size_t registerCount,
                         const unsigned* registerIDs,
                         uint32_t* registerValues,
                         size_t registerValuesSize)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    varint_t *regIDs;
    int res;
    int i;

    regIDs = malloc(registerCount * sizeof(varint_t));
    for (i = 0; i < registerCount; ++i)
        regIDs[i] = registerIDs[i];
    cswp_client_prepare_cmd(client);
    res = cswp_encode_reg_read_command(priv->cmd, deviceNo, registerCount, regIDs);
    free(regIDs);

    if (res == CSWP_SUCCESS)
    {
        struct reply_data_reg_read* replyData = calloc(1, sizeof(struct reply_data_reg_read));
        replyData->registerValues = registerValues;
        replyData->registerValuesSize = registerValuesSize;
        cswp_client_push_request(client, CSWP_REG_READ, cswp_device_reg_read_complete, replyData);
        res = cswp_client_process(client);
    }

    return res;
}


int cswp_device_reg_write(cswp_client_t* client,
                          unsigned deviceNo,
                          size_t registerCount,
                          const unsigned* registerIDs,
                          const uint32_t* registerValues,
                          size_t registerValuesSize)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int i;
    int res;

    cswp_client_prepare_cmd(client);
    res = cswp_encode_reg_write_command(priv->cmd, deviceNo, registerCount);
    for (i = 0; i < registerCount; ++i)
    {
        cswp_buffer_put_varint(priv->cmd, registerIDs[i]);
        cswp_buffer_put_uint32(priv->cmd, registerValues[i]);
    }
    if (res == CSWP_SUCCESS)
    {
        cswp_client_push_request(client, CSWP_REG_WRITE, NULL, 0);
        res = cswp_client_process(client);
    }

    return res;
}


/**
 * Reply data for CSWP_MEM_READ command
 */
struct reply_data_mem_read {
    /** Buffer for read data */
    uint8_t* buf;
    /** Number of byte in the provided buffer */
    size_t* bytesRead;
};

/*
 * Completion function for CSWP_MEM_READ
 */
static int cswp_device_mem_read_complete(cswp_client_t* client, void* replyData)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    struct reply_data_mem_read* memReadReplyData = (struct reply_data_mem_read*)replyData;
    int res;
    varint_t bytesRead;
    void* pData;

    res = cswp_decode_mem_read_response_body(priv->rsp, &bytesRead);
    if (res == CSWP_SUCCESS)
    {
        cswp_buffer_get_direct(priv->rsp, &pData, bytesRead);
        memcpy(memReadReplyData->buf, pData, bytesRead);
        *memReadReplyData->bytesRead = bytesRead;
    }

    return res;
}

int cswp_device_mem_read(cswp_client_t* client,
                         unsigned deviceNo,
                         uint64_t address,
                         size_t size,
                         cswp_access_size_t accessSize,
                         unsigned flags,
                         uint8_t* buf,
                         size_t* bytesRead)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res;

    cswp_client_prepare_cmd(client);
    res = cswp_encode_mem_read_command(priv->cmd, deviceNo, address, size, accessSize, flags);
    if (res == CSWP_SUCCESS)
    {
        struct reply_data_mem_read* replyData = calloc(1, sizeof(struct reply_data_mem_read));
        replyData->buf = buf;
        replyData->bytesRead = bytesRead;
        cswp_client_push_request(client, CSWP_MEM_READ, cswp_device_mem_read_complete, replyData);
        res = cswp_client_process(client);
    }

    return res;
}


int cswp_device_mem_write(cswp_client_t* client,
                          unsigned deviceNo,
                          uint64_t address,
                          size_t size,
                          cswp_access_size_t accessSize,
                          unsigned flags,
                          const uint8_t* pData)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res;

    cswp_client_prepare_cmd(client);
    res = cswp_encode_mem_write_command(priv->cmd, deviceNo, address, size, accessSize, flags, pData);
    if (res == CSWP_SUCCESS)
        cswp_client_push_request(client, CSWP_MEM_WRITE, NULL, 0);
    if (res == CSWP_SUCCESS)
        res = cswp_client_process(client);

    return res;
}


/**
 * Reply data for CSWP_MEM_POLL command
 */
struct reply_data_mem_poll {
    /** Buffer for read data */
    uint8_t* buf;
    /** Size of the provided buffer */
    size_t* bytesRead;
};

/*
 * Completion function for CSWP_MEM_POLL
 */
static int cswp_device_mem_poll_complete(cswp_client_t* client, void* replyData)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    struct reply_data_mem_poll* memPollReplyData = (struct reply_data_mem_poll*)replyData;
    int res;
    varint_t bytesRead;
    void* pData;

    res = cswp_decode_mem_poll_response_body(priv->rsp, &bytesRead);
    if (res == CSWP_SUCCESS)
    {
        cswp_buffer_get_direct(priv->rsp, &pData, bytesRead);
        if (memPollReplyData->buf)
            memcpy(memPollReplyData->buf, pData, bytesRead);
        if (memPollReplyData->bytesRead)
            *memPollReplyData->bytesRead = bytesRead;
    }

    return res;
}

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
                         size_t* bytesRead)
{
    cswp_client_priv_t* priv = (cswp_client_priv_t*)client->priv;
    int res;

    cswp_client_prepare_cmd(client);
    res = cswp_encode_mem_poll_command(priv->cmd, deviceNo,
                                       address, size, accessSize, flags,
                                       tries, interval, mask, value);
    if (res == CSWP_SUCCESS)
    {
        struct reply_data_mem_poll* replyData = calloc(1, sizeof(struct reply_data_mem_poll));
        replyData->buf = buf;
        replyData->bytesRead = bytesRead;
        cswp_client_push_request(client, CSWP_MEM_POLL, cswp_device_mem_poll_complete, replyData);
        res = cswp_client_process(client);
    }

    return res;
}

/* end of file cswp_client.c */

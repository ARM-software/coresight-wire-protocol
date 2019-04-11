/*
 * rddi_streaming_trace.h - Streaming trace header for RDDI
 * Copyright (C) 2016-2018 Arm Limited (or its affiliates). All rights reserved.
 */

/**  \file

     \brief This file describes the Streaming trace functionality of the RDDI module.
*/

#ifndef RDDI_STREAMING_TRACE_H
#define RDDI_STREAMING_TRACE_H

#include "rddi.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS
/* 0x02XX - streaming trace errors */

/** not connected */
#define RDDI_STREAMING_TRACE_NO_CONNECTION      0x0201

/** no event available */
#define RDDI_STREAMING_TRACE_NO_EVENT           0x0202

/** a comms error occurred between host and trace unit */
#define RDDI_STREAMING_TRACE_COMMSERROR         0x0203

/** timeout waiting for data */
#define RDDI_STREAMING_TRACE_TIMEOUT            0x0204

/** no device found */
#define RDDI_STREAMING_TRACE_NO_DEVICE          0x0205

/** invalid sink */
#define RDDI_STREAMING_TRACE_INVALID_SINK       0x0206

/** sink not attached */
#define RDDI_STREAMING_TRACE_SINK_NOT_ATTACHED  0x0207

/** sink not started */
#define RDDI_STREAMING_TRACE_SINK_NOT_STARTED   0x0208

/** connect failed */
#define RDDI_STREAMING_TRACE_CONNECT_FAILED     0x0209

/** sink start failed */
#define RDDI_STREAMING_TRACE_SINK_START_FAILED  0x020A

/** sink stop failed */
#define RDDI_STREAMING_TRACE_SINK_STOP_FAILED   0x020B

/** streaming trace data has overflowed */
#define RDDI_STREAMING_TRACE_DATA_OVERFLOW      0x020C

/** there are potential problems with the integrity of the streaming trace data */
#define RDDI_STREAMING_TRACE_DATA_INTEGRITY     0x020D

/** auxiliary probe flash programming error */
#define RDDI_STREAMING_TRACE_AUX_PROBE_PROG     0x020E

/** set streaming trace mode failed */
#define RDDI_STREAMING_TRACE_SET_MODE_FAILED    0x020F

#endif

/**
 * Description of a trace sink
 *
 * Used by StreamingTrace_GetSinkDetails() to get information
 * about a trace sink.
 *
 * The client should allocate buffers for pName and pMetadata and set the
 * nameLen and metadataLen fields to the buffer size.  The recommended minimum
 * buffer size for pName is 256 bytes and 4096 bytes for pMetadata
 */
typedef struct
{
    char* pName;              /*!< client supplied buffer to receive name */
    size_t nameLen;           /*!< size of buffer to receive name */
    char* pMetadata;          /*!< client supplied buffer to receive JSON metadata */
    size_t metadataLen;       /*!< size of buffer to receive JSON metadata */
    size_t dataBufferSize;    /*!< recommended data buffer size */
    size_t dataBufferCount;   /*!< recommended number of data buffers */
    size_t eventBufferSize;   /*!< recommended event buffer size */
    size_t eventBufferCount;  /*!< recommended number of event buffers */
} RDDIStreamingTraceSinkDetails;

/**
 * Types of event that can be returned to a client
 */
typedef enum
{
    RDDI_STREAMING_TRACE_EVENT_TYPE_NONE,  /*!< pBuf does not contain a valid event */
    RDDI_STREAMING_TRACE_EVENT_TYPE_EVENT, /*!< pBuf contains a JSON event notification */
    RDDI_STREAMING_TRACE_EVENT_TYPE_ERROR, /*!< pBuf contains a JSON error notification */
    RDDI_STREAMING_TRACE_EVENT_TYPE_DATA,  /*!< pBuf contains trace data */
    RDDI_STREAMING_TRACE_EVENT_TYPE_END_OF_DATA, /*!< indicates that StreamingTrace_Flush() has completed */
    /* Values between CUSTOM_S and CUSTOM_E indicate that pBuf contains
       transport specific data */
    RDDI_STREAMING_TRACE_EVENT_TYPE_CUSTOM_S = 0x10000000,
    RDDI_STREAMING_TRACE_EVENT_TYPE_CUSTOM_E = 0x7FFFFFFF,
} RDDIStreamingTraceEventType;

/**
 * Buffer to receive events from a trace sink
 *
 * Events can be trace data, information messages, error messages or
 * custom events.
 *
 * Clients should allocate the buffer (pBuf) and set size to indicate
 * the size of the allocated buffer.
 *
 * When the buffer is returned to the client by StreamingTrace_WaitForEvent(),
 * type will be set to indicate the event type and used will be set with the
 * number of bytes used in the buffer
 */
typedef struct
{
    RDDIStreamingTraceEventType type; /*!< Event type */
    uint8* pBuf;                      /*!< Client supplied buffer */
    size_t size;                      /*!< Size of client supplied buffer */
    size_t used;                      /*!< Bytes used in buffer */
} RDDIStreamingTraceEventBuffer;

/**
 * Initialise a connection to the streaming trace service.
 * This function uses the address and configuration details from the
 * configuration file already opened by the ConfigInfo_OpenFile() function to
 * provide a fully configured connection.
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @return RDDI_SUCCESS on success, other RDDI_STREAMING_TRACE_xxxx on error
 */
RDDI int StreamingTrace_Connect(
    RDDIHandle handle);

/**
 * Disconnect from the streaming trace service
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @return RDDI_SUCCESS on success, other RDDI_STREAMING_TRACE_xxxx on error
 */
RDDI int StreamingTrace_Disconnect(
    RDDIHandle handle);

/**
 * Get the number of sinks available
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[out] pSinkCount The number of sinks available
 * @return RDDI_SUCCESS on success, other RDDI_STREAMING_TRACE_xxxx on error
 */
RDDI int StreamingTrace_GetSinkCount(
    RDDIHandle handle,
    int *pSinkCount);

/**
 * Get details for a trace sink.
 *
 * The implementation may provide metadata for the sink in JSON format. The
 * implementation should return an empty string if no metadata is provided. If
 * the reported sink name is different to the respective device name used by
 * the debug interface, the metadata value "base_address" can be used to match
 * the sink to the debug device.
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] sink The sink ID
 * @param[out] pSinkDetails Pointer to RDDIStreamingTraceSinkDetails instance
 *     to receive the sink details.  The client should allocate buffers for
 *     pName and pMetadata and set the nameLen and metadataLen fields to
 *     the buffer size.  The recommended minimum buffer size for pName is 256
 *     bytes and 4096 bytes for pMetadata.  If necessary, the strings are
 *     truncated to fit in the buffers and RDDI_BUFFER_OVERFLOW is returned.
 * @return RDDI_SUCCESS on success, RDDI_BUFFER_OVERFLOW on buffer overflow or
 *     other RDDI_xxxx on error
 */
RDDI int StreamingTrace_GetSinkDetails(
    RDDIHandle handle,
    int sink,
    RDDIStreamingTraceSinkDetails* pSinkDetails);

/**
 * Get configuration item value for a trace sink
 *
 * There is one pre-defined configuration item: "CONFIG_ITEMS". This is a
 * read-only config item that contains a newline separated list of the names
 * of all the configuration items for the selected sink. This can be used to
 * retrieve all the configuration items for the device without prior knowledge
 * of their existence.
 *
 * The value is NULL terminated.  The specified size of the buffer includes
 * the NULL terminator. If the value is too large to fit in the
 * supplied buffer, the buffer is NULL terminated at the last character and
 * RDDI_BUFFER_OVERFLOW is returned.
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] sink The sink ID
 * @param[in] pName The config item name
 * @param[out] pValue Buffer to receive the config item value
 * @param[in] valueLen The size of the config item value buffer
 * @return RDDI_SUCCESS on success, RDDI_BUFFER_OVERFLOW on buffer overflow,
 *     other RDDI_STREAMING_TRACE_xxxx on error
 */
RDDI int StreamingTrace_GetConfigItem(
    RDDIHandle handle,
    int sink,
    const char *pName,
    char *pValue,
    size_t valueLen);

/**
 * Set configuration item value for a trace sink
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] sink The sink ID
 * @param[in] pName The config item name
 * @param[in] pValue The config item name
 * @return RDDI_SUCCESS on success, other RDDI_STREAMING_TRACE_xxxx on error
 */
RDDI int StreamingTrace_SetConfigItem(
    RDDIHandle handle,
    int sink,
    const char *pName,
    const char *pValue);

/**
 * Attaches to a trace sink so that we can start to receive trace events.
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] sink The sink ID
 * @return RDDI_SUCCESS on success, other RDDI_STREAMING_TRACE_xxxx on error
 */
RDDI int StreamingTrace_Attach(
    RDDIHandle handle,
    int sink);

/**
 * Detaches from a trace sink. On return, all event tokens should be
 * considered invalid and all event buffers are considered to be owned by
 * the client.
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] sink The sink ID
 * @return RDDI_SUCCESS on success, other RDDI_STREAMING_TRACE_xxxx on error
 */
RDDI int StreamingTrace_Detach(
    RDDIHandle handle,
    int sink);

/**
 * Passes an event buffer to the trace system (this API) so that it may (at
 * some unspecified future time) be filled with trace data and then returned
 * to the client via either a StreamingTrace_WaitForEvent() call or a
 * StreamingTrace_Detach() call. Whilst a buffer is owned by the trace system
 * the buffer should not be modified by the client.
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] sink The sink ID
 * @param[in] bufferType The type of the buffer
 *     (RDDI_STREAMING_TRACE_EVENT_TYPE_DATA or RDDI_STREAMING_TRACE_EVENT_TYPE_EVENT)
 * @param[in] pEventBuffer a pointer to the event data buffer
 * @param[out] pEventToken this call returns a token which is
 *     associated with the event buffer whilst this API owns the
 *     event buffer
 * @return RDDI_SUCCESS on success, other RDDI_STREAMING_TRACE_xxxx on error
 */
RDDI int StreamingTrace_SubmitEventBuffer(
    RDDIHandle handle,
    int sink,
    int bufferType,
    RDDIStreamingTraceEventBuffer* pEventBuffer,
    int* pEventToken);

/**
 * This call allows the client to poll for the next trace event from a sink.
 *
 * A successful call will return ownership of the event buffer associated with
 * the returned token back to the client. Upon a successful return, the token
 * is no longer associated with the event buffer.
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] sink The sink ID
 * @param[out] pEventToken Receives the completed token
 * @param[in] msTimeout the number of ms to wait for an event to occur
 * @return RDDI_SUCCESS on success, RDDI_STREAMING_TRACE_TIMEOUT on timeout,
 *     other RDDI_STREAMING_TRACE_xxxx on error
 */
RDDI int StreamingTrace_WaitForEvent(
    RDDIHandle handle,
    int sink,
    int* pEventToken,
    int msTimeout);

/**
 * Starts processing trace data/events for the trace sink. Note that this
 * does not control the actual trace sink itself - just the processing of
 * trace data/events associated with the sink.
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] sink The sink ID
 * @return RDDI_SUCCESS on success, other RDDI_STREAMING_TRACE_xxxx on error
 */
RDDI int StreamingTrace_Start(
    RDDIHandle handle,
    int sink);

/**
 * Signals the end of trace capture.  The capture device should stop
 * collecting more trace data, but should continue to send any already
 * collected trace data stored in internal buffers.  The capture device should
 * send an event with type RDDI_STREAMING_TRACE_EVENT_TYPE_END to indicate
 * that all data has been sent. The client will continue to call
 * StreamingTrace_WaitForEvent() and StreamingTrace_SubmitEventBuffer() to
 * collect this data until the end of data event is received.
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] sink The sink ID
 * @return RDDI_SUCCESS on success, other RDDI_STREAMING_TRACE_xxxx on error
 */
RDDI int StreamingTrace_Flush(
    RDDIHandle handle,
    int sink);

/**
 * Stops processing trace data/events for the trace sink. Note that this
 * does not control the actual trace sink itself - just the processing of
 * trace data/events associated with the sink.
 *
 * Any pending data / events will be delivered to the client.  All buffers
 * will be returned to the client via StreamingTrace_WaitForEvent()
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] sink The sink ID
 * @return RDDI_SUCCESS on success, other RDDI_STREAMING_TRACE_xxxx on error
 */
RDDI int StreamingTrace_Stop(
    RDDIHandle handle,
    int sink);

/**
    Perform an RDDI implementation specific transaction.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] pDataIn data supplied by client for this transaction
    \param[in] dataInLen length in bytes of the data supplied by the client for
    this transaction
    \param[out] pDataOut Client-supplied buffer to take the results of the transaction
    \param[in] dataOutLen length in bytes of the data buffer supplied by the
    client for the results of this transaction
    \param[out] dataOutUsedLen The number of bytes of transaction data returned.
 */
RDDI int StreamingTrace_Transaction(
    RDDIHandle handle,
    const uint8* pDataIn,
    size_t dataInLen,
    uint8 *pDataOut,
    size_t dataOutLen,
    size_t* dataOutUsedLen);

/**
 * Request the capture device to stream blocks of data from its store up to the client.
 * If the store is byte-addressable, this corresponds to a block size of 1, with the
 * parameters being equivalent to startByte and numBytes.
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] sink The sink ID
 * @param[in] startBlock The position in the trace hardware store in blocks
 * @param[in] numBlocks The number of blocks to be streamed
 * @return RDDI_SUCCESS on success, other RDDI_STREAMING_TRACE_xxxx on error
 *
 * This function can be used for capture devices that have a store which trace has been
 * captured into (as opposed to capture devices which stream trace to the client during
 * trace capture). After trace capture has stopped, it may be used in order to fetch
 * the stored trace.
 *
 * Prior to calling this function, buffers should be submitted using
 * StreamingTrace_SubmitEventBuffer calls. After calling this function, the capture
 * device will stream the requested data to the client in a similar manner to the
 * behaviour of a live-streaming device following a StreamingTrace_Start call. The
 * client should receive the filled buffers via StreamingTrace_WaitForEvent calls,
 * resubmitting each buffer after its contents have been read.
 *
 * Once all requested data has been streamed, the capture device should send an event
 * with type RDDI_STREAMING_TRACE_EVENT_TYPE_END (note that StreamingTrace_Flush should
 * not be called). After the client receives this event, it should call
 * StreamingTrace_Stop, and should then continue to call StreamingTrace_WaitForEvent to
 * retrieve the remaining unused buffers which were previously submitted.
 */
RDDI int StreamingTrace_RequestStreamBlocks(
    RDDIHandle handle,
    int sink,
    uint64 startBlock,
    uint64 numBlocks);

#endif /* RDDI_STREAMING_TRACE_H */
/* end of file rddi_streaming_trace.h */

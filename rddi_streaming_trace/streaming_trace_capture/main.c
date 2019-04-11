// main.c
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/**
 * Example for streaming trace collection
 */

#include "rddi.h"
#include "rddi_configinfo.h"
#include "rddi_streaming_trace.h"

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
#include <windows.h>
#else
#include <signal.h>
#endif

#ifndef _WIN32
#include <sys/time.h>

static long get_current_time(void)
{
    struct timeval tv;

    if (gettimeofday(&tv, 0) != 0)
        return 0;
    return tv.tv_sec*1000 + tv.tv_usec/1000 + ((tv.tv_usec >= 500) ? 1 : 0);
}

#else

static long get_current_time()
{
    return GetTickCount();
}

#endif

void report_error(int res, const char* msg, ...);
int discover_sinks(int handle, RDDIStreamingTraceSinkDetails** sinkDetails, int* numSinks);
void describe_sinks(const RDDIStreamingTraceSinkDetails* sinkDetails, int numSinks);
void cleanup_sink_details(RDDIStreamingTraceSinkDetails* sinkDetails, int numSinks);
int collect_trace(int handle, int sink, const RDDIStreamingTraceSinkDetails* sinkDetails, FILE* outFile, size_t maxCaptureSize);
int find_buffer(int token, int* bufferTokens, size_t numBuffers);

volatile int exitFlag = 0;

#define FLUSH_TIMEOUT (10 * 1000)

int main(int argc, char *argv[])
{
    const char* configFile;
    size_t maxCapture = 0;
    FILE* outFile = NULL;
    RDDIHandle handle;
    int res;
    int numSinks;
    RDDIStreamingTraceSinkDetails* sinkDetails;

    if (argc < 2)
    {
        fprintf(stderr, "usage: amis_trace_capture config_file [out_file (0 indicates none)]] [max_capture_size]\n");
        return -2;
    }

    configFile = argv[1];

    if (argc > 2)
    {
        if (strcmp("0", argv[2]) != 0)
            outFile = fopen(argv[2], "wb");
    }

    if (argc > 3)
        maxCapture = strtoul(argv[3], 0, 0);

    /* Open and configure streaming trace interface */
    res = RDDI_Open(&handle, 0);
    if (res != RDDI_SUCCESS)
    {
        report_error(res, "Failed to open Streaming Trace interface");
        return -1;
    }

    res = ConfigInfo_OpenFile(handle, configFile);
    if (res != RDDI_SUCCESS)
    {
        report_error(res, "Failed to configure Streaming Trace interface");
        RDDI_Close(handle);
        return -1;
    }

    /* Open streaming trace interface */
    res = StreamingTrace_Connect(handle);
    if (res != RDDI_SUCCESS)
    {
        report_error(res, "Failed to connect to streaming trace");
        RDDI_Close(handle);
        return -1;
    }

    /* Discover sink details */
    res = discover_sinks(handle, &sinkDetails, &numSinks);
    if (res == RDDI_SUCCESS)
    {
        /* Show sink details */
        describe_sinks(sinkDetails, numSinks);

        /* Collect trace */
        if (numSinks > 0)
        {
            int sink = 0;
            printf("Collecting trace from sink %d\n", sink);
            collect_trace(handle, sink, &sinkDetails[sink], outFile, maxCapture);
        }

        cleanup_sink_details(sinkDetails, numSinks);
    }

    /* Disconnect */
    res = StreamingTrace_Disconnect(handle);
    if (res != RDDI_SUCCESS)
        report_error(res, "Failed to disconnect from streaming trace");

    res = RDDI_Close(handle);
    if (res != RDDI_SUCCESS)
        report_error(res, "Failed to close streaming trace");

    if (outFile != NULL)
        fclose(outFile);

    return 0;
}


/*
 * Print error message and error description
 */
void report_error(int res, const char* msg, ...)
{
    int lastErr;
    char errDetails[256];

    va_list args;
    va_start(args, msg);
    vfprintf(stderr, msg, args);
    va_end(args);

    RDDI_GetLastError(&lastErr, errDetails, sizeof(errDetails));
    fprintf(stderr, ": %s (%d)\n", errDetails, res);
}


/*
 * Discover the sinks provided by the streaming trace interface
 */
int discover_sinks(int handle, RDDIStreamingTraceSinkDetails** sinkDetails, int* numSinks)
{
    const size_t nameSize = 256;
    const size_t metadataSize = 4096;
    int res;
    int i;

    /* Get the number of sinks */
    res = StreamingTrace_GetSinkCount(handle, numSinks);
    if (res != RDDI_SUCCESS)
    {
        report_error(res, "Failed to get streaming trace sink count");
        return res;
    }

    /* Allocate array of sink details */
    *sinkDetails = (RDDIStreamingTraceSinkDetails*)malloc(*numSinks * sizeof(RDDIStreamingTraceSinkDetails));

    /* Get the details for each sink */
    for (i = 0; i < *numSinks; ++i)
    {
        RDDIStreamingTraceSinkDetails* details = &(*sinkDetails)[i];
        /* Allocate name & metadata buffers */
        details->pName = (char *)malloc(nameSize);
        details->nameLen = nameSize;
        details->pMetadata = (char *)malloc(metadataSize);
        details->metadataLen = metadataSize;

        res = StreamingTrace_GetSinkDetails(handle, i, details);
        if (res != RDDI_SUCCESS)
        {
            report_error(res, "Failed to get streaming trace sink details for sink %d", i);
            return res;
        }

        /* Ensure name & metadata are null terminated */
        details->pName[nameSize-1] = '\0';
        details->pMetadata[metadataSize-1] = '\0';
    }

    return res;
}

/*
 * Print details of each sink
 */
void describe_sinks(const RDDIStreamingTraceSinkDetails* sinkDetails, int numSinks)
{
    int i;
    for (i = 0; i < numSinks; ++i)
    {
        const RDDIStreamingTraceSinkDetails* details = &sinkDetails[i];
        printf("Sink %d: name: %s, data buffers: %lu x %lu, event buffers: %lu x %lu, metadata: %s\n",
               i, details->pName,
               details->dataBufferCount, details->dataBufferSize,
               details->eventBufferCount, details->eventBufferSize,
               details->pMetadata);
    }
}

/*
 * Cleanup the sink information
 */
void cleanup_sink_details(RDDIStreamingTraceSinkDetails* sinkDetails, int numSinks)
{
    int i;
    for (i = 0; i < numSinks; ++i)
    {
        free(sinkDetails[i].pName);
        free(sinkDetails[i].pMetadata);
    }
    free(sinkDetails);
}

#ifdef _WIN32
BOOL CtrlHandler(DWORD ctrlType)
{
    switch (ctrlType)
    {
    case CTRL_C_EVENT:
    case CTRL_BREAK_EVENT:
        exitFlag++;
        return TRUE;

    default:
        return FALSE;
    }
}
#else
static void exit_handler(int sig)
{
   exitFlag++;
}
#endif

/*
 * Collect streaming trace
 */
int collect_trace(int handle, int sink, const RDDIStreamingTraceSinkDetails* sinkDetails, FILE* outFile, size_t maxCaptureSize)
{
    int res;
    int numDataBuffers, numEventBuffers, numBuffers, pendingBuffers;
    RDDIStreamingTraceEventBuffer* buffers;
    RDDIStreamingTraceEventBuffer* buffer;
    int* bufferTokens;
    int i;
    size_t bytesCaptured;
    int flushed;
    int token;
    int foundBuffer;
    long t0=-1, t1, elapsed;
    long tFlush=-1;

    /* Attach to the sink and start processing */
    res = StreamingTrace_Attach(handle, sink);
    if (res != RDDI_SUCCESS)
    {
        report_error(res, "Failed to attach to sink");
        return res;
    }

    /* Allocate capture buffers */
    numDataBuffers = (int)sinkDetails->dataBufferCount;
    numEventBuffers = (int)sinkDetails->eventBufferCount;
    numBuffers = numDataBuffers + numEventBuffers;
    buffers = malloc(numBuffers * sizeof(RDDIStreamingTraceEventBuffer));
    bufferTokens = malloc(numBuffers * sizeof(int));
    for (i = 0; i < numDataBuffers; ++i)
    {
        buffers[i].pBuf = (uint8*)malloc(sinkDetails->dataBufferSize);
        buffers[i].size = sinkDetails->dataBufferSize;
        bufferTokens[i] = -1;
    }
    for (i = numDataBuffers; i < numBuffers; ++i)
    {
        buffers[i].pBuf = (uint8*)malloc(sinkDetails->eventBufferSize);
        buffers[i].size = sinkDetails->eventBufferSize;
        bufferTokens[i] = -1;
    }

    /* install handler for SIGINT/Ctrl-C */
#ifdef _WIN32
    SetConsoleCtrlHandler((PHANDLER_ROUTINE)CtrlHandler, TRUE);
#else
    signal(SIGINT, exit_handler);
#endif

    exitFlag = 0;

    res = StreamingTrace_Start(handle, sink);
    if (res != RDDI_SUCCESS)
    {
        report_error(res, "Failed to start sink");
        StreamingTrace_Detach(handle, sink);
        return res;
    }

    /* Submit buffers */
    for (i = 0; i < numBuffers; ++i)
    {
        int type = i < numDataBuffers ? RDDI_STREAMING_TRACE_EVENT_TYPE_DATA : RDDI_STREAMING_TRACE_EVENT_TYPE_EVENT;
        res = StreamingTrace_SubmitEventBuffer(handle, sink, type, &buffers[i], &bufferTokens[i]);
        if (res != RDDI_SUCCESS)
        {
            report_error(res, "Failed to submit event buffer %d", i);
            StreamingTrace_Stop(handle, sink);
            StreamingTrace_Detach(handle, sink);
            return res;
        }
    }
    pendingBuffers = numBuffers;

    t0 = get_current_time();

    /* Capture data */
    bytesCaptured = 0;
    flushed = 0;
    while (exitFlag <= 1)
    {
        /* check for exit criteria */
        if (maxCaptureSize != 0 && bytesCaptured >= maxCaptureSize)
            exitFlag = 1;

        if (exitFlag == 1 && !flushed)
        {
            /* Flush to stop capture */
            printf("Stopping capture\n");
            res = StreamingTrace_Flush(handle, sink);
            if (res != RDDI_SUCCESS)
                report_error(res, "Failed to flush sink");
            flushed = 1;
            tFlush = get_current_time();
            /* Continue to receive and re-submit buffers until end of trace event */
        }
        else if (flushed)
        {
            if (get_current_time() > tFlush+FLUSH_TIMEOUT)
            {
                printf("Flush timed out\n");
                break;
            }
        }

        /* Wait for completion of a buffer */
        res = StreamingTrace_WaitForEvent(handle, sink, &token, 1000);
        if (res == RDDI_STREAMING_TRACE_TIMEOUT)
        {
            continue;
        }
        else if (res != RDDI_SUCCESS)
        {
            report_error(res, "Failed to complete buffer");
            break;
        }

        --pendingBuffers;

        /* find the buffer associated with this token */
        foundBuffer = find_buffer(token, bufferTokens, numBuffers);
        if (foundBuffer == -1)
        {
            fprintf(stderr, "Unexpected token %d", token);
            break;
        }

        /* Process buffer */
        buffer = &buffers[foundBuffer];
        printf("Completed buffer: token: %d, index=%d, state=%d, used=%lu\n",
               token, foundBuffer,
               buffer->type, buffer->used);
        /* process trace data */
        if (outFile != NULL &&
            (maxCaptureSize == 0 || bytesCaptured < maxCaptureSize))
        {
            size_t bytesToWrite = buffer->used;
            if (maxCaptureSize != 0 && bytesToWrite > (maxCaptureSize-bytesCaptured))
                bytesToWrite = maxCaptureSize-bytesCaptured;
            fwrite(buffer->pBuf, 1, bytesToWrite, outFile);
        }

        if (buffer->type == RDDI_STREAMING_TRACE_EVENT_TYPE_DATA)
            bytesCaptured += buffer->used;
        else if (buffer->type == RDDI_STREAMING_TRACE_EVENT_TYPE_END_OF_DATA)
        {
            printf("All data received\n");
            break;
        }

        /* Re-submit buffer */
        res = StreamingTrace_SubmitEventBuffer(handle, sink,
                                               foundBuffer < numDataBuffers ? RDDI_STREAMING_TRACE_EVENT_TYPE_DATA : RDDI_STREAMING_TRACE_EVENT_TYPE_EVENT,
                                               buffer, &bufferTokens[foundBuffer]);
        if (res != RDDI_SUCCESS)
        {
            report_error(res, "Failed to submit event buffer %d", foundBuffer);
            break;
        }
        ++pendingBuffers;
    }

    /* Stop capture */
    t1 = get_current_time();
    printf("Capture complete\n");
    res = StreamingTrace_Stop(handle, sink);
    if (res != RDDI_SUCCESS)
        report_error(res, "Failed to stop sink");

    /* Wait for remaining buffers to be returned */
    while (pendingBuffers > 0)
    {
        /* Wait for completion of a buffer */
        int token;
        res = StreamingTrace_WaitForEvent(handle, sink, &token, 10000);
        if (res != RDDI_SUCCESS)
        {
            report_error(res, "Failed to complete buffer");
            break;
        }

        --pendingBuffers;
    }

    /* restore handler for SIGINT/Ctrl-C */
#ifdef _WIN32
    SetConsoleCtrlHandler(NULL, FALSE);
#else
    signal(SIGINT, SIG_DFL);
#endif

    res = StreamingTrace_Detach(handle, sink);
    if (res != RDDI_SUCCESS)
        report_error(res, "Failed to detach from sink");

    /* free buffers */
    for (i = 0; i < numBuffers; ++i)
        free(buffers[i].pBuf);
    free(buffers);
    free(bufferTokens);

    elapsed = t1-t0;
    printf("Collected %ld bytes in %ldms: %.2f kb/s\n",
           bytesCaptured, elapsed, (bytesCaptured / 1024.0) / (elapsed / 1000.0));

    /* Return error code */
    return res;
}

int find_buffer(int token, int* bufferTokens, size_t numBuffers)
{
    int foundBuffer = -1;
    int i;
    for (i = 0; i < numBuffers; ++i)
        if (token == bufferTokens[i])
            foundBuffer = i;
    return foundBuffer;
}

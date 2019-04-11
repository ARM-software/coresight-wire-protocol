// streaming_trace_base.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef STREAMING_TRACE_BASE_H
#define STREAMING_TRACE_BASE_H

#include "rddi_streaming_trace.h"

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/thread/condition.hpp>
#include <boost/shared_ptr.hpp>

#include <string>
#include <memory>
#include <deque>
#include <queue>
#include <vector>


/**
 * Details for a trace sink
 */
struct SinkDetails
{
    std::string name;
    std::string metadata;
    size_t dataBufferCount;
    size_t dataBufferSize;
    size_t eventBufferCount;
    size_t eventBufferSize;
};

/**
 * Base class for streaming trace implementations
 *
 * Provides the client API functions, managing the discovery and description of
 * trace sinks, handling of event buffers etc.
 *
 * Implementations must provide the low level functions to discover trace
 * sinks and submit and receive data buffers
 */
class StreamingTraceBase
{
public:
    StreamingTraceBase();
    virtual ~StreamingTraceBase();

    /**
     * Connect to the streaming trace interface
     */
    void Connect();

    /**
     * Disconnect from the streaming trace interface
     */
    void Disconnect();

    /**
     * Get the number of sinks on this interface
     */
    int GetSinkCount();

    /**
     * Get details of a given sink
     */
    SinkDetails GetSinkDetails(int sink);

    /**
     * Get config item value
     */
    std::string GetConfigItem(int sink, const char* name);

    /**
     * Set config item value
     */
    void SetConfigItem(int sink, const char* name, const char* value);

    /**
     * Attach to a trace sink
     */
    void Attach(int sink);

    /**
     * Detach from a trace sink
     */
    void Detach(int sink);

    /**
     * Submit a buffer to receive data / events from a trace sink
     *
     * Buffers are not submitted to the lower layers until the sink has been started
     */
    void SubmitEventBuffer(int sink,
                           int bufferType,
                           RDDIStreamingTraceEventBuffer* pEventBuffer,
                           int* pEventToken);

    /**
     * Wait for the next buffer from a trace sink
     */
    int WaitForEvent(int sink, int msTimeout);

    /**
     * Start data collection from a sink
     *
     * Any buffers previously submitted are passed to the lower layers
     */
    void Start(int sink);

    /**
     * Stop data collection from a sink
     */
    void Stop(int sink);

    /**
     * Flush any unfilled buffers
     *
     * Implementations should return control of any
     * pending buffers to the client
     */
    void Flush(int sink);


    void SendStateEvent(int sink, RDDIStreamingTraceEventType eventType);

protected:
    /**
     * Associates a token to a client supplied buffer
     */
    struct Buffer
    {
        int token;
        RDDIStreamingTraceEventBuffer* pEventBuffer;
    };
    typedef std::deque<Buffer> BufferQueue;

    /**
     * Information describing a sink
     */
    struct SinkInfo
    {
        SinkDetails details;
    };

    /**
     * The run time state of a sink
     */
    struct SinkState
    {
        enum Status
        {
            DETACHED,
            ATTACHED,
            ACTIVE
        };

        SinkState() : status(DETACHED) { }

        Status status;
        unsigned int transportID;     // identifier for transport layer
        BufferQueue pendingBuffers;   // buffers submitted to transport layer
        BufferQueue queuedBuffers;    // buffers not yet submitted to transport layer
        BufferQueue eventBuffers;     // buffers for status events
        boost::condition_variable pendingBuffersCond;
        std::queue<int> completedEventTokens;
        boost::condition_variable completedEventCond;
    };
    typedef boost::shared_ptr<SinkState> SinkStatePtr;

    // Functions required in implementations

    /**
     * Discover the available sinks
     */
    virtual std::vector<SinkInfo> discoverSinks() = 0;

    /**
     * Open implementation specific connection
     */
    virtual void doConnect() = 0;

    /**
     * Close implementation specific connection
     */
    virtual void doDisconnect() = 0;

    /**
     * Check implementation specific connection
     */
    virtual bool isConnected() = 0;

    /**
     * Implementation specific actions when attaching
     */
    virtual void attachDevice(int sink);

    /**
     * Implementation specific actions when detaching
     */
    virtual void detachDevice(int sink);

    /**
     * Submit buffer to transport layer
     */
    virtual void submitBuffer(int sink, Buffer& buf) = 0;

    /**
     * Cancel buffer in transport layer
     */
    virtual void doCancelPendingBuffers(int sink) = 0;

    /**
     * Implementation specific start
     */
    virtual void startDevice(int sink);

    /**
     * Implementation specific stop
     */
    virtual void stopDevice(int sink);

    /**
     * Flush transport layer
     */
    virtual void doFlush(int sink) = 0;

    /**
     * Complete buffer in transport layer
     *
     * @return false if no buffer available
     */
    virtual bool waitForBuffer(int sink, Buffer& buf) = 0;

    /**
     * Guard function to check connection is open
     */
    void CheckConnected();

    /**
     * Submit buffers to transport layer
     */
    void submitBuffers(int sink);

    void dataThread(int sink);

    bool WaitForDataEvent(int sink);

    void doSendStateEvent(int sink, RDDIStreamingTraceEventType eventType);

    boost::mutex m_lock;

    std::vector<SinkInfo> m_sinkInfo;
    std::vector<SinkStatePtr> m_sinkState;
    int m_nextToken;

    std::auto_ptr<boost::thread> m_dataThread;
};

#endif // STREAMING_TRACE_BASE_H

// End of file streaming_trace_base.h

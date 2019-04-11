// streaming_trace_base.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "streaming_trace_base.h"

#include "st_error.h"

StreamingTraceBase::StreamingTraceBase()
    : m_nextToken(0)
{
}

StreamingTraceBase::~StreamingTraceBase()
{
}

void StreamingTraceBase::Connect()
{
    boost::mutex::scoped_lock lock(m_lock);

    doConnect();

    m_sinkInfo = discoverSinks();

    // need at least one event for stop
    for (std::vector<SinkInfo>::iterator sink = m_sinkInfo.begin();
         sink != m_sinkInfo.end();
         ++sink)
        if (sink->details.eventBufferCount == 0)
        {
            sink->details.eventBufferCount = 1;
            sink->details.eventBufferSize = 256;
        }

    m_sinkState.resize(m_sinkInfo.size());
    for (std::vector<SinkStatePtr>::iterator sinkState = m_sinkState.begin();
         sinkState != m_sinkState.end();
         ++sinkState)
        (*sinkState).reset(new SinkState());
}


void StreamingTraceBase::Disconnect()
{
    boost::mutex::scoped_lock lock(m_lock);

    doDisconnect();

    m_sinkInfo.clear();
    m_sinkState.clear();
}


int StreamingTraceBase::GetSinkCount()
{
    boost::mutex::scoped_lock lock(m_lock);

    CheckConnected();

    return m_sinkInfo.size();
}


SinkDetails StreamingTraceBase::GetSinkDetails(int sink)
{
    boost::mutex::scoped_lock lock(m_lock);

    CheckConnected();

    if (sink >= (int)m_sinkInfo.size())
        throw StreamingTraceException(RDDI_STREAMING_TRACE_INVALID_SINK, "Invalid sink");

    return m_sinkInfo[sink].details;
}


std::string StreamingTraceBase::GetConfigItem(int sink, const char* name)
{
    boost::mutex::scoped_lock lock(m_lock);

    CheckConnected();

    if (sink >= (int)m_sinkInfo.size())
        throw StreamingTraceException(RDDI_STREAMING_TRACE_INVALID_SINK, "Invalid sink");

    return "";
}


void StreamingTraceBase::SetConfigItem(int sink, const char* name, const char* value)
{
    boost::mutex::scoped_lock lock(m_lock);

    CheckConnected();

    if (sink >= (int)m_sinkInfo.size())
        throw StreamingTraceException(RDDI_STREAMING_TRACE_INVALID_SINK, "Invalid sink");
}


void StreamingTraceBase::Attach(int sink)
{
    boost::mutex::scoped_lock lock(m_lock);

    CheckConnected();

    if (sink >= (int)m_sinkInfo.size())
        throw StreamingTraceException(RDDI_STREAMING_TRACE_INVALID_SINK, "Invalid sink");

    SinkStatePtr& sinkState = m_sinkState[sink];
    if (sinkState->status == SinkState::DETACHED)
        sinkState->status = SinkState::ATTACHED;

    attachDevice(sink);

    m_dataThread.reset(new boost::thread(boost::bind(&StreamingTraceBase::dataThread, this, sink)));
}


void StreamingTraceBase::attachDevice(int sink)
{
    // no action here - implementations may override for device specific actions
}


void StreamingTraceBase::Detach(int sink)
{
    boost::mutex::scoped_lock lock(m_lock);

    CheckConnected();

    if (sink >= (int)m_sinkInfo.size())
        throw StreamingTraceException(RDDI_STREAMING_TRACE_INVALID_SINK, "Invalid sink");

    SinkStatePtr& sinkState = m_sinkState[sink];

    sinkState->status = SinkState::DETACHED;
    sinkState->pendingBuffersCond.notify_all();

    // wait for data thread to exit
    if (m_dataThread.get())
    {
        lock.unlock();
        m_dataThread->join();
        lock.lock();
        m_dataThread.reset(0);
    }

    // cancel in progress transactions
    doCancelPendingBuffers(sink);

    // client won't call WaitForEvent, so wait for and complete any
    // pending buffers now
    while (!sinkState->pendingBuffers.empty())
    {
        if (!waitForBuffer(sink, sinkState->pendingBuffers.front()))
            break;
        sinkState->pendingBuffers.pop_front();
    }

    detachDevice(sink);

    sinkState->queuedBuffers.clear();
    sinkState->pendingBuffers.clear();
    sinkState->eventBuffers.clear();
}


void StreamingTraceBase::detachDevice(int sink)
{
    // no action here - implementations may override for device specific actions
}


void StreamingTraceBase::SubmitEventBuffer(int sink,
                                           int bufferType,
                                           RDDIStreamingTraceEventBuffer* pEventBuffer,
                                           int* pEventToken)
{
    boost::mutex::scoped_lock lock(m_lock);

    CheckConnected();

    if (sink >= (int)m_sinkInfo.size())
        throw StreamingTraceException(RDDI_STREAMING_TRACE_INVALID_SINK, "Invalid sink");

    SinkStatePtr& sinkState = m_sinkState[sink];

    if (sinkState->status == SinkState::DETACHED)
        throw StreamingTraceException(RDDI_STREAMING_TRACE_SINK_NOT_ATTACHED, "Sink not attached");

    // allocate token
    int token = m_nextToken;
    ++m_nextToken;

    // reset used/state
    pEventBuffer->type = RDDI_STREAMING_TRACE_EVENT_TYPE_NONE;
    pEventBuffer->used = 0;

    // add buffer to queue
    Buffer buf = { token, pEventBuffer };

    if (bufferType == RDDI_STREAMING_TRACE_EVENT_TYPE_DATA)
    {
        sinkState->queuedBuffers.push_back(buf);

        // submit to transport layer if started and sufficient space
        if (sinkState->status == SinkState::ACTIVE)
            submitBuffers(sink);
    }
    else if (bufferType == RDDI_STREAMING_TRACE_EVENT_TYPE_EVENT)
    {
        sinkState->eventBuffers.push_back(buf);
    }
    else
        throw StreamingTraceException(RDDI_STREAMING_TRACE_EVENT_TYPE_ERROR, "Invalid buffer type");

    *pEventToken = token;
}


int StreamingTraceBase::WaitForEvent(int sink, int msTimeout)
{
    boost::mutex::scoped_lock lock(m_lock);

    if (sink >= (int)m_sinkInfo.size())
        throw StreamingTraceException(RDDI_STREAMING_TRACE_INVALID_SINK, "Invalid sink");

    SinkStatePtr& sinkState = m_sinkState[sink];

    // wait for token to be added to completed token queue - either from data
    // thread or client event
    boost::system_time deadline = boost::get_system_time() + boost::posix_time::milliseconds(msTimeout);
    while (sinkState->completedEventTokens.empty())
        if (!sinkState->completedEventCond.timed_wait(lock, deadline))
            break;

    if (sinkState->completedEventTokens.empty())
        throw StreamingTraceException(RDDI_STREAMING_TRACE_TIMEOUT, "Timeout waiting for event");

    int token = sinkState->completedEventTokens.front();
    sinkState->completedEventTokens.pop();
    return token;
}

void StreamingTraceBase::dataThread(int sink)
{
    try
    {
        while (WaitForDataEvent(sink))
            ;
    }
    catch (...)
    {
        // exit thread cleanly
    }
}

bool StreamingTraceBase::WaitForDataEvent(int sink)
{
    boost::mutex::scoped_lock lock(m_lock);

    SinkStatePtr& sinkState = m_sinkState[sink];

    // wait for a buffer to be submitted
    while (sinkState->pendingBuffers.empty() && isConnected() && sinkState->status != SinkState::DETACHED)
        sinkState->pendingBuffersCond.wait(lock);

    if (!isConnected() || sinkState->status == SinkState::DETACHED)
        // indicate thread exit
        return false;

    Buffer& buf = sinkState->pendingBuffers.front();

    bool gotBuffer = waitForBuffer(sink, buf);
    if (gotBuffer)
    {
        int completedToken = buf.token;

        // remove from pending buffers
        // don't need to track buffer pointer any longer
        sinkState->pendingBuffers.pop_front();

        // submit next buffer
        if (sinkState->status == SinkState::ACTIVE)
            submitBuffers(sink);

        sinkState->completedEventTokens.push(completedToken);
        sinkState->completedEventCond.notify_one();
    }

    return true;
}


void StreamingTraceBase::Start(int sink)
{
    boost::mutex::scoped_lock lock(m_lock);

    CheckConnected();

    if (sink >= (int)m_sinkInfo.size())
        throw StreamingTraceException(RDDI_STREAMING_TRACE_INVALID_SINK, "Invalid sink");

    SinkStatePtr& sinkState = m_sinkState[sink];

    if (sinkState->status == SinkState::DETACHED)
        throw StreamingTraceException(RDDI_STREAMING_TRACE_SINK_NOT_ATTACHED, "Sink not attached");

    sinkState->status = SinkState::ACTIVE;

    // submit buffers to transport layer
    submitBuffers(sink);

    startDevice(sink);
}


void StreamingTraceBase::startDevice(int sink)
{
    // no action here - implementations may override for device specific actions
}


void StreamingTraceBase::Stop(int sink)
{
    boost::mutex::scoped_lock lock(m_lock);

    CheckConnected();

    if (sink >= (int)m_sinkInfo.size())
        throw StreamingTraceException(RDDI_STREAMING_TRACE_INVALID_SINK, "Invalid sink");

    SinkStatePtr& sinkState = m_sinkState[sink];

    if (sinkState->status != SinkState::ACTIVE)
        throw StreamingTraceException(RDDI_STREAMING_TRACE_SINK_NOT_STARTED, "Sink not started");

    stopDevice(sink);

    sinkState->status = SinkState::ATTACHED;

    // cancel in progress transactions
    doCancelPendingBuffers(sink);

    // complete event buffers
    while (!sinkState->eventBuffers.empty())
    {
        Buffer& buf = sinkState->eventBuffers.front();
        buf.pEventBuffer->type = RDDI_STREAMING_TRACE_EVENT_TYPE_NONE;
        buf.pEventBuffer->used = 0;
        int token = buf.token;
        sinkState->eventBuffers.pop_front();
        sinkState->completedEventTokens.push(token);
    }
    if (!sinkState->completedEventTokens.empty())
        sinkState->completedEventCond.notify_one();

    // buffers will be returned to client via WaitForEvent()
}


void StreamingTraceBase::stopDevice(int sink)
{
    // no action here - implementations may override for device specific actions
}


void StreamingTraceBase::Flush(int sink)
{
    boost::mutex::scoped_lock lock(m_lock);

    CheckConnected();

    if (sink >= (int)m_sinkInfo.size())
        throw StreamingTraceException(RDDI_STREAMING_TRACE_INVALID_SINK, "Invalid sink");

    SinkStatePtr& sinkState = m_sinkState[sink];

    if (sinkState->status == SinkState::DETACHED)
        throw StreamingTraceException(RDDI_STREAMING_TRACE_SINK_NOT_ATTACHED, "Sink not attached");

    doFlush(sink);

    // buffers will be returned to client via WaitForEvent() - which may
    // submit more buffers
}

//
// Add an event directly to the complete queue
//
void StreamingTraceBase::SendStateEvent(int sink, RDDIStreamingTraceEventType eventType)
{
    boost::mutex::scoped_lock lock(m_lock);

    if (sink >= (int)m_sinkInfo.size())
        throw StreamingTraceException(RDDI_STREAMING_TRACE_INVALID_SINK, "Invalid sink");

    doSendStateEvent(sink, eventType);
}

void StreamingTraceBase::doSendStateEvent(int sink, RDDIStreamingTraceEventType eventType)
{
    SinkStatePtr& sinkState = m_sinkState[sink];

    // complete event buffer if available
    if (!sinkState->eventBuffers.empty())
    {
        Buffer& buf = sinkState->eventBuffers.front();
        buf.pEventBuffer->type = eventType;
        int token = buf.token;
        sinkState->eventBuffers.pop_front();
        sinkState->completedEventTokens.push(token);
        sinkState->completedEventCond.notify_one();
    }
}

// called with lock held
void StreamingTraceBase::CheckConnected()
{
    if (!isConnected())
        throw StreamingTraceException(RDDI_STREAMING_TRACE_NO_CONNECTION, "Streaming trace is not connected");
}


// called with lock held
void StreamingTraceBase::submitBuffers(int sink)
{
    // submit buffers to implementation.  The implementation indicates how
    // many pending buffers it can handle with SinkInfo.bufferCount

    SinkStatePtr& sinkState = m_sinkState[sink];
    SinkInfo& sinkInfo = m_sinkInfo[sink];
    while (sinkState->pendingBuffers.size() < sinkInfo.details.dataBufferCount &&
           !sinkState->queuedBuffers.empty())
    {
        // submit the next buffer to transport
        Buffer buf = sinkState->queuedBuffers.front();
        submitBuffer(sink, buf);

        // move to pending queue
        sinkState->queuedBuffers.pop_front();
        sinkState->pendingBuffers.push_back(buf);
        sinkState->pendingBuffersCond.notify_one();
    }
}

// End of file streaming_trace_base.cpp

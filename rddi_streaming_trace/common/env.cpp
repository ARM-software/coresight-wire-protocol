// env.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/*!
 *  \file dummy_env.cpp
 *  \brief DummyEnv implementation
 */
#include "env.h"

#include "util.h"
#include "streaming_trace_base.h"
#include "rddi.h"

#include <string>
#include <fstream>
#include <iterator>
#include <ios>
#include <algorithm>
#include <utility>
#include <cstring>

using namespace std;

// Because we constuct the Env in 2 stages, we have 2 implementations...
class Env::Impl
{
public:
    virtual ~Impl() {}

    virtual void StreamingTrace_Connect() = 0;
    virtual void StreamingTrace_Disconnect() = 0;
    virtual void StreamingTrace_GetSinkCount(int *pSinkCount) = 0;
    virtual void StreamingTrace_GetSinkDetails(int sink, RDDIStreamingTraceSinkDetails* pDetails) = 0;
    virtual void StreamingTrace_GetConfigItem(int sink, const char *pName, char *pValue, size_t valueLen) = 0;
    virtual void StreamingTrace_SetConfigItem(int sink, const char *pName, const char *pValue) = 0;
    virtual void StreamingTrace_Attach(int sink) = 0;
    virtual void StreamingTrace_Detach(int sink) = 0;
    virtual void StreamingTrace_SubmitEventBuffer(int sink,
                                                  int bufferType,
                                                  RDDIStreamingTraceEventBuffer* pEventBuffer,
                                                  int* pEventToken) = 0;
    virtual void StreamingTrace_WaitForEvent(int sink, int* pEventToken, int msTimeout) = 0;
    virtual void StreamingTrace_Start(int sink) = 0;
    virtual void StreamingTrace_Stop(int sink) = 0;
    virtual void StreamingTrace_Flush(int sink) = 0;
};

// NullImplementation is a placeholder to return not configured errors if the environment has not yet
// been fully configured
class NullImplementation : public Env::Impl
{
public:
    virtual void StreamingTrace_Connect()
    {
        Fail();
    }

    virtual void StreamingTrace_Disconnect()
    {
        Fail();
    }

    virtual void StreamingTrace_GetSinkCount(int *pSinkCount)
    {
        Fail();
    }

    virtual void StreamingTrace_GetSinkDetails(int sink, RDDIStreamingTraceSinkDetails* pDetails)
    {
        Fail();
    }

    virtual void StreamingTrace_GetConfigItem(int sink, const char *pName, char *pValue, size_t valueLen)
    {
        Fail();
    }

    virtual void StreamingTrace_SetConfigItem(int sink, const char *pName, const char *pValue)
    {
        Fail();
    }

    virtual void StreamingTrace_Attach(int sink)
    {
        Fail();
    }

    virtual void StreamingTrace_Detach(int sink)
    {
        Fail();
    }

    virtual void StreamingTrace_SubmitEventBuffer(int sink,
                                                  int bufferType,
                                                  RDDIStreamingTraceEventBuffer* pEventBuffer,
                                                  int* pEventToken)
    {
        Fail();
    }

    virtual void StreamingTrace_WaitForEvent(int sink, int* pEventToken, int msTimeout)
    {
        Fail();
    }

    virtual void StreamingTrace_Start(int sink)
    {
        Fail();
    }

    virtual void StreamingTrace_Stop(int sink)
    {
        Fail();
    }

    virtual void StreamingTrace_Flush(int sink)
    {
        Fail();
    }

private:
    void Fail() const
    {
        throw StreamingTraceException(RDDI_NO_CONFIG, "Not configured");
    }
};

// factory function provided by implementation
extern StreamingTraceBase* CreateStreamingTrace(const std::string &xmlFile);

// RealImplementation does the real work with the mmap()-ed core files and fake registers.
class RealImplementation : public Env::Impl
{
public:
    RealImplementation(const std::string &xmlFile)
    {
        m_StreamingTrace.reset(CreateStreamingTrace(xmlFile));
    }

    virtual void StreamingTrace_Connect()
    {
        m_StreamingTrace->Connect();
    }

    virtual void StreamingTrace_Disconnect()
    {
        m_StreamingTrace->Disconnect();
    }

    virtual void StreamingTrace_GetSinkCount(int *pSinkCount)
    {
        CheckNotNull(pSinkCount);

        *pSinkCount = m_StreamingTrace->GetSinkCount();
    }

    virtual void StreamingTrace_GetSinkDetails(int sink, RDDIStreamingTraceSinkDetails* pDetails)
    {
        CheckNotNull(pDetails);
        SinkDetails details = m_StreamingTrace->GetSinkDetails(sink);
        bool nameOK = FillStringParam(details.name, pDetails->pName, pDetails->nameLen, 0);
        bool metadataOK = FillStringParam(details.metadata, pDetails->pMetadata, pDetails->metadataLen, 0);
        pDetails->dataBufferCount = details.dataBufferCount;
        pDetails->dataBufferSize = details.dataBufferSize;
        pDetails->eventBufferCount = details.eventBufferCount;
        pDetails->eventBufferSize = details.eventBufferSize;

        if (!nameOK || !metadataOK)
            throw StreamingTraceException(RDDI_BUFFER_OVERFLOW, "Buffer overflow");
    }

    virtual void StreamingTrace_GetConfigItem(int sink, const char *pName, char *pValue, size_t valueLen)
    {
        CheckNotNull(pName);
        CheckNotNull(pValue);
        std::string val = m_StreamingTrace->GetConfigItem(sink, pName);
        bool ok = FillStringParam(val, pValue, valueLen, 0);
        if (!ok)
            throw StreamingTraceException(RDDI_BUFFER_OVERFLOW, "Buffer overflow");
    }

    virtual void StreamingTrace_SetConfigItem(int sink, const char *pName, const char *pValue)
    {
        CheckNotNull(pName);
        CheckNotNull(pValue);
        m_StreamingTrace->SetConfigItem(sink, pName, pValue);
    }

    virtual void StreamingTrace_Attach(int sink)
    {
        m_StreamingTrace->Attach(sink);
    }

    virtual void StreamingTrace_Detach(int sink)
    {
        m_StreamingTrace->Detach(sink);
    }

    virtual void StreamingTrace_SubmitEventBuffer(int sink,
                                                  int bufferType,
                                                  RDDIStreamingTraceEventBuffer* pEventBuffer,
                                                  int* pEventToken)
    {
        m_StreamingTrace->SubmitEventBuffer(sink, bufferType, pEventBuffer, pEventToken);
    }

    virtual void StreamingTrace_WaitForEvent(int sink, int* pEventToken, int msTimeout)
    {
        CheckNotNull(pEventToken);
        *pEventToken = m_StreamingTrace->WaitForEvent(sink, msTimeout);
    }

    virtual void StreamingTrace_Start(int sink)
    {
        m_StreamingTrace->Start(sink);
    }

    virtual void StreamingTrace_Stop(int sink)
    {
        m_StreamingTrace->Stop(sink);
    }

    virtual void StreamingTrace_Flush(int sink)
    {
        m_StreamingTrace->Flush(sink);
    }

private:
    std::auto_ptr<StreamingTraceBase> m_StreamingTrace;
};


Env::Env()
    : m_impl(new NullImplementation())
{
}

Env::~Env()
{
}

void Env::Construct(const string& xmlFile)
{
}

void Env::Configure(const char *pFilename)
{
    m_impl.reset(new RealImplementation(pFilename));
}

void Env::StreamingTrace_Connect()
{
    m_impl->StreamingTrace_Connect();
}

void Env::StreamingTrace_Disconnect()
{
    m_impl->StreamingTrace_Disconnect();
}

void Env::StreamingTrace_GetSinkCount(int *pSinkCount)
{
    m_impl->StreamingTrace_GetSinkCount(pSinkCount);
}

void Env::StreamingTrace_GetSinkDetails(int sink, RDDIStreamingTraceSinkDetails* pDetails)
{
    m_impl->StreamingTrace_GetSinkDetails(sink, pDetails);
}

void Env::StreamingTrace_GetConfigItem(int sink, const char *pName, char *pValue, size_t valueLen)
{
    m_impl->StreamingTrace_GetConfigItem(sink, pName, pValue, valueLen);
}

void Env::StreamingTrace_SetConfigItem(int sink, const char *pName, const char *pValue)
{
    m_impl->StreamingTrace_SetConfigItem(sink, pName, pValue);
}

void Env::StreamingTrace_Attach(int sink)
{
    m_impl->StreamingTrace_Attach(sink);
}

void Env::StreamingTrace_Detach(int sink)
{
    m_impl->StreamingTrace_Detach(sink);
}

void Env::StreamingTrace_SubmitEventBuffer(int sink,
                                           int bufferType,
                                           RDDIStreamingTraceEventBuffer* pEventBuffer,
                                           int* pEventToken)
{
    m_impl->StreamingTrace_SubmitEventBuffer(sink, bufferType, pEventBuffer, pEventToken);
}

void Env::StreamingTrace_WaitForEvent(int sink, int* pEventToken, int msTimeout)
{
    m_impl->StreamingTrace_WaitForEvent(sink, pEventToken, msTimeout);
}

void Env::StreamingTrace_Start(int sink)
{
    m_impl->StreamingTrace_Start(sink);
}

void Env::StreamingTrace_Stop(int sink)
{
    m_impl->StreamingTrace_Stop(sink);
}

void Env::StreamingTrace_Flush(int sink)
{
    m_impl->StreamingTrace_Flush(sink);
}


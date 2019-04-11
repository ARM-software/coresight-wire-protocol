// env.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/*! \file dummy_env.h
 *  \brief Implement functions for the dummy environment.
 */
#ifndef INC_ENV_H
#define INC_ENV_H

#include "rddi_streaming_trace.h"

#include <boost/shared_ptr.hpp>
#include <boost/noncopyable.hpp>

class Env : boost::noncopyable
{
public:
    Env();
    ~Env();

    void Configure(const char *pFilename);

    int LastErrorCode() const;
    std::string LastErrorText() const;

    void StreamingTrace_Connect();
    void StreamingTrace_Disconnect();
    void StreamingTrace_GetSinkCount(int *pSinkCount);
    void StreamingTrace_GetSinkDetails(int sink, RDDIStreamingTraceSinkDetails* pDetails);
    void StreamingTrace_GetConfigItem(int sink, const char *pName, char *pValue, size_t valueLen);
    void StreamingTrace_SetConfigItem(int sink, const char *pName, const char *pValue);
    void StreamingTrace_Attach(int sink);
    void StreamingTrace_Detach(int sink);
    void StreamingTrace_SubmitEventBuffer(int sink,
                                          int bufferType,
                                          RDDIStreamingTraceEventBuffer* pEventBuffer,
                                          int* pEventToken);
    void StreamingTrace_WaitForEvent(int sink, int* pEventToken, int msTimeout);
    void StreamingTrace_Start(int sink);
    void StreamingTrace_Stop(int sink);
    void StreamingTrace_Flush(int sink);

    class Impl;

private:
    void Construct(const std::string& xmlFile);

    typedef boost::shared_ptr<Impl> ImplPtr;
    ImplPtr m_impl;
};


#endif // INC_ENV_H

// api_stubs.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/*!
 *  \file rddi_stubs.cpp
 *  \brief Implementation of functions exported by RDDI dll
 */

#include "rddi.h"
#include "rddi_streaming_trace.h"

#include "env_map.h"
#include "env.h"
#include "st_error.h"
#include "last_error.h"
#include "util.h"
#include "buildinfo.h"

#include <boost/shared_ptr.hpp>
#include <boost/current_function.hpp>

#include <stdexcept>
#include <map>
#include <cstring>
// #include <ostream>
#include <sstream>

using namespace std;
using namespace boost;
using namespace Err;
// using namespace Util;
using EnvironmentMap::EnvPtr;

// default visibilty is hidden on linux builds
#ifndef WIN32
#undef RDDI
#ifdef __cplusplus
  #define RDDI extern "C" __attribute__ ((visibility ("default")))
#else
  #define RDDI __attribute__ ((visibility ("default")))
#endif
#endif

void StoreError(const StreamingTraceException& ex)
{
    StoreError(ex.code(), ex.what());
}

StreamingTraceException MakeRddiExFromCurrentException()
{
    try
    {
        throw;
    }
    catch (StreamingTraceException& e)
    {
        return e;
    }
    catch (const std::invalid_argument& e)
    {
        return StreamingTraceException(RDDI_BADARG, e.what());
    }
    catch (const std::bad_alloc& e)
    {
        ostringstream ost;
        ost << "Unable to allocate memory. Details: " << e.what();
        return StreamingTraceException(RDDI_INTERNAL_ERROR, ost.str());
    }
    // add any more standard exceptions here

    // anything else ends up as an internal error - as it shouldn't happen
    catch (const std::exception& e)
    {
        return StreamingTraceException(RDDI_INTERNAL_ERROR, e.what());
    }
    catch (...)
    {
        return StreamingTraceException(RDDI_INTERNAL_ERROR, "Caught Unknown Exception");
    }
}


#define TRAP_EXCEPTIONS(block)                                          \
    try                                                                 \
    {                                                                   \
        block;                                                          \
        StoreError(StreamingTraceException(RDDI_SUCCESS, ""));          \
    }                                                                   \
    catch (...)                                                         \
    {                                                                   \
        StoreError(MakeRddiExFromCurrentException());                   \
    }


// generic handing of VERSION that doesn't rely on it being a valid numeric expression
// i.e. handles the 1.0.1 case as well as the 1.0 or 1.1 cases.
// lovely nested macros to handle stringisation and macro substitution ordering issues.
#define STRVER(x) #x
#define STRVER2(y) STRVER(y)

void Version(int* major, int* minor, int* build, 
             char *name, unsigned nameLen, unsigned* nameUsed)
{
    if (major == NULL)
        throw StreamingTraceException(RDDI_BADARG, "Major not set");
    if (minor == NULL)
        throw StreamingTraceException(RDDI_BADARG, "Minor not set");
    if (build == NULL)
        throw StreamingTraceException(RDDI_BADARG, "Build not set");

    istringstream version(STRVER2(VERSION));

    int tmp_major = 0;
    int tmp_minor = 0;
    char dot;

    version >> tmp_major >> dot >> tmp_minor;

    *major = tmp_major;
    *minor = tmp_minor;
    *build = BUILD_NUMBER;

    unsigned int length = strlen(NAME);
    if (nameUsed != NULL)
        *nameUsed = length + 1;
    if ((name != NULL) && (nameLen > 0))
    {
        if (nameLen <= length)
            throw StreamingTraceException(RDDI_BUFFER_OVERFLOW, "Buffer overflow");
        strcpy(name, NAME);
    }
}


RDDI int RDDI_Open(RDDIHandle *pHandle, const void *pDetails)
{
    TRAP_EXCEPTIONS(
        CheckNotNull(pHandle);
        *pHandle = EnvironmentMap::Store(EnvPtr(new Env));
    );

    return LastErrorCode();
}

RDDI int RDDI_Close(RDDIHandle handle)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Remove(handle));
    return LastErrorCode();
}

RDDI int RDDI_GetVersion(RDDIHandle, int *pMajor, int *pMinor, int *pBuild, char *pDescription, size_t descriptionLen)
{
    TRAP_EXCEPTIONS(
        Version(pMajor, pMinor, pBuild, pDescription, descriptionLen, 0);
        );
    return LastErrorCode();
}

RDDI void RDDI_SetLogCallback(
    RDDIHandle handle,
    RDDILogCallback pfn,
    void* context,
    int maxLogLevel)
{
    // Not yet implemented
}

RDDI int RDDI_GetLastError(int *pError, char *pDetails, size_t detailsLen)
{
    try
    {
        // report code
        CheckNotNull(pError);
        *pError = LastErrorCode();

        if (detailsLen != 0u)
        {
            // report text
            CheckNotNull(pDetails);
            string errorText(LastErrorText());
            *pDetails = '\0';
            strncat(pDetails, errorText.c_str(), detailsLen - 1);

            if (detailsLen <= errorText.length())
            {
                throw StreamingTraceException(RDDI_BUFFER_OVERFLOW, "Error buffer overflow");
            }
        }
        return RDDI_SUCCESS;
    }
    catch (const StreamingTraceException& e)
    {
        return e.code();
    }
    catch (...)
    {
        return RDDI_INTERNAL_ERROR;
    }
}

RDDI int ConfigInfo_OpenFile(RDDIHandle handle, const char *pFilename)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->Configure(pFilename));
    return LastErrorCode();
}

RDDI int StreamingTrace_Connect(RDDIHandle handle)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->StreamingTrace_Connect());
    return LastErrorCode();
}

RDDI int StreamingTrace_Disconnect(RDDIHandle handle)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->StreamingTrace_Disconnect());
    return LastErrorCode();
}

RDDI int StreamingTrace_GetSinkCount(
    RDDIHandle handle,
    int *pSinkCount)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->StreamingTrace_GetSinkCount(pSinkCount));
    return LastErrorCode();
}

RDDI int StreamingTrace_GetSinkDetails(
    RDDIHandle handle,
    int sink,
    RDDIStreamingTraceSinkDetails* pDetails)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->StreamingTrace_GetSinkDetails(sink, pDetails));
    return LastErrorCode();
}

RDDI int StreamingTrace_GetConfigItem(
    RDDIHandle handle,
    int sink,
    const char *pName,
    char *pValue,
    size_t valueLen)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->StreamingTrace_GetConfigItem(sink, pName, pValue, valueLen));
    return LastErrorCode();
}

RDDI int StreamingTrace_SetConfigItem(
    RDDIHandle handle,
    int sink,
    const char *pName,
    const char *pValue)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->StreamingTrace_SetConfigItem(sink, pName, pValue));
    return LastErrorCode();
}

RDDI int StreamingTrace_Attach(
    RDDIHandle handle,
    int sink)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->StreamingTrace_Attach(sink));
    return LastErrorCode();
}

RDDI int StreamingTrace_Detach(
    RDDIHandle handle,
    int sink)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->StreamingTrace_Detach(sink));
    return LastErrorCode();
}

RDDI int StreamingTrace_SubmitEventBuffer(
    RDDIHandle handle,
    int sink,
    int bufferType,
    RDDIStreamingTraceEventBuffer* pEventBuffer,
    int* pEventToken)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->StreamingTrace_SubmitEventBuffer(sink, bufferType, pEventBuffer, pEventToken));
    return LastErrorCode();
}

RDDI int StreamingTrace_WaitForEvent(
    RDDIHandle handle,
    int sink,
    int* pEventToken,
    int msTimeout)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->StreamingTrace_WaitForEvent(sink, pEventToken, msTimeout));
    return LastErrorCode();
}

RDDI int StreamingTrace_Start(
    RDDIHandle handle,
    int sink)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->StreamingTrace_Start(sink));
    return LastErrorCode();
}

RDDI int StreamingTrace_Stop(
    RDDIHandle handle,
    int sink)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->StreamingTrace_Stop(sink));
    return LastErrorCode();
}

RDDI int StreamingTrace_Flush(
    RDDIHandle handle,
    int sink)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->StreamingTrace_Flush(sink));
    return LastErrorCode();
}


// util.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef UTIL_H
#define UTIL_H

#include "st_error.h"
#include "rddi_streaming_trace.h"

//! ensure pointer argument not null
template <class T>
inline void CheckNotNull(T* p)
{
    if (p == 0)
    {
        throw StreamingTraceException(RDDI_BADARG, "Bad argument to command. (Null pointer)");
    }
}

bool FillStringParam(const std::string& src, char* dest, size_t len, size_t* used);

#endif // UTIL_H

// End of file util.h

// util.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "util.h"
#include <cstring>

bool FillStringParam(const std::string& src, char* dest, size_t len, size_t* used)
{
    const size_t Length(src.length());

    if (used != 0)
    {
        *used = static_cast<unsigned>(Length + 1); // room for NUL
    }

    if (dest != 0)
    {
        if (len == 0u)
        {
            throw StreamingTraceException(RDDI_BADARG, "Zero buffer length");
        }

        // Copy at most len characters including NUL
        *dest = '\0';
        strncat(dest, src.c_str(), len - 1);

        return len > Length;
    }
    else
    {
        return true;
    }
}

// End of file util.cpp

// utils.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/*! \file utils.h
 *  \brief Misc utilities.
 */

#include <cstddef>
#include <cstring>
#include <cassert>
#include <string>
#include <sstream>
#include "rddi.h"
#include "rddi_ex.h"
#include "utils.h"
#include "buildinfo.h"

#define STRVER(x) #x
#define STRVER2(y) STRVER(y)

bool Utils::FillStringParam(const std::string& src, char* dest, size_t len, 
                            size_t* used)
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
            throw RddiEx(RDDI_BADARG, "Zero buffer length");
        }

        // Copy at most len characters including NULL
        *dest = '\0';
        strncat(dest, src.c_str(), len - 1);

        return len > Length;
    }
    else
    {
        return true;
    }
}

void Utils::Version(int* major, int* minor, int* build, 
                    char *name, unsigned nameLen, unsigned* nameUsed)
{
    if (major == NULL)
        throw RddiEx(RDDI_BADARG);
    if (minor == NULL)
        throw RddiEx(RDDI_BADARG);
    if (build == NULL)
        throw RddiEx(RDDI_BADARG);

    std::istringstream version(STRVER2(VERSION));

    int tmp_major = 0;
    int tmp_minor = 0;
    char dot;

    version >> tmp_major >> dot >> tmp_minor;

    FillOptionalParam(tmp_major, major);
    FillOptionalParam(tmp_minor, minor);
    FillOptionalParam(REVISION, build);

    unsigned int length = (unsigned int) strlen(NAME);
    if (nameUsed != NULL)
        *nameUsed = length + 1;
    if ((name != NULL) && (nameLen > 0))
    {
        if (nameLen <= length)
            throw RddiEx(RDDI_BUFFER_OVERFLOW);
        strcpy(name, NAME);
    }
}

void Utils::RddiError::StoreErr(const RddiEx& rex)
{
    assert(rex.what() != 0);
    unsigned code(rex.code());
    std::string msg(rex.what());

    // If there's no text to report to the user, try to get away with the 
    // default text for the error code.
    Utils::RddiError::StoreError(code, msg.empty() ? GetErrorText(code) : msg);
}

RddiEx Utils::RddiError::MakeRddiExFromCurrentException()
{
    try
    {
        throw;
    }
    catch (RddiEx& e)
    {
        return e;
    }
    catch (const std::invalid_argument& e)
    {
        return RddiEx(RDDI_BADARG, e.what());
    }
    catch (const std::bad_alloc& e)
    {
        std::ostringstream ost;
        ost << "Unable to allocate memory. Details: " << e.what();
        return RddiEx(RDDI_OUTOFMEM, ost.str());
    }
    // add any more standard exceptions here

    // anything else ends up as an internal error - as it shouldn't happen
    catch (const std::exception& e)
    {
        return RddiEx(RDDI_INTERNAL_ERROR, e.what());
    }
    catch (...)
    {
        return RddiEx(RDDI_INTERNAL_ERROR, "Caught Unknown Exception");
    }    
}

namespace
{
  struct Error
  {
    Error(unsigned code = 0u, std::string text = "") : code(code), text(text) {}

    unsigned code;
    std::string text;
  };

  Error g_err;
}

void Utils::RddiError::StoreError(unsigned code, const std::string& text)
{
    g_err = Error(code, text);
}

unsigned Utils::RddiError::LastErrorCode()
{
    return g_err.code;
}

std::string Utils::RddiError::LastErrorText()
{
    return g_err.text;
}

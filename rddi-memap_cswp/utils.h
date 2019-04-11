// utils.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/*! \file utils.h
 *  \brief Misc utilities.
 */

#ifndef INC_UTILS_H
#define INC_UTILS_H

#include "rddi_ex.h"
#include "rddi.h"

#define TRAP_EXCEPTIONS(block)                                                         \
    try                                                                                \
    {                                                                                  \
        block;                                                                         \
        Utils::RddiError::StoreErr(RddiEx(RDDI_SUCCESS, ""));                           \
    }                                                                                  \
    catch (...)                                                                        \
    {                                                                                  \
        Utils::RddiError::StoreErr(Utils::RddiError::MakeRddiExFromCurrentException()); \
    }

extern std::string GetErrorText(unsigned int errorCode);

namespace Utils
{
    bool FillStringParam(const std::string& src, char* dest, size_t len, 
                         size_t* used);

    void Version(int* major, int* minor, int* build, char *name, 
                 unsigned nameLen, unsigned* nameUsed);

    //! ensure pointer argument not null 
    template <class T> inline void CheckNotNull(T* p)
    {
        if (p == 0)
            throw RddiEx(RDDI_BADARG, "Bad argument to command. (Null pointer)");
    }

    template <class Src, class Dest>inline void FillOptionalParam(const Src& src, Dest* dest)
    {
        if (dest != 0)
            *dest = src;
    }

    namespace RddiError
    {
        void StoreErr(const RddiEx& rex);
        void StoreError(unsigned code, const std::string& text);
        RddiEx MakeRddiExFromCurrentException();
        unsigned LastErrorCode();
        std::string LastErrorText();
    }

}

#endif // INC_UTILS_H

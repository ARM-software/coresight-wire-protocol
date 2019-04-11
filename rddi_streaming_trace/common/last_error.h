// last_error.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef INC_LAST_ERROR_H
#define INC_LAST_ERROR_H

#include <string>

namespace Err
{
    void StoreError(unsigned code, const std::string& text);
    unsigned LastErrorCode();
    std::string LastErrorText();
}

#endif // INC_LAST_ERROR_H

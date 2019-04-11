// last_error.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#pragma warning (disable: 4275)
#include "last_error.h"

#include <map>
#include <boost/thread/mutex.hpp>
#include <boost/thread.hpp>

using namespace std;
using namespace boost;

struct Error
{
    Error(unsigned code = 0u, string text = "") : code(code), text(text) {}

    unsigned code;
    string text;
};

namespace
{
    map<thread::id, Error> g_err;
    mutex err_lock;
}

void Err::StoreError(unsigned code, const std::string& text)
{
    mutex::scoped_lock lock(err_lock);
    g_err[this_thread::get_id()] = Error(code, text);
}

unsigned Err::LastErrorCode()
{
    mutex::scoped_lock lock(err_lock);
    return g_err[this_thread::get_id()].code;
}

std::string Err::LastErrorText()
{
    mutex::scoped_lock lock(err_lock);
    return g_err[this_thread::get_id()].text;
}


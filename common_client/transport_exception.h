// transport_exception.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef TRANSPORT_EXCEPTION_H
#define TRANSPORT_EXCEPTION_H

#include <stdexcept>

/**
 * Exception thrown on transport (e.g. USB, TCP, etc) error
 */
class TransportException : public std::runtime_error
{
public:
    /**
     * Create exception
     *
     * @param msg Error message
     */
    explicit TransportException(const std::string& msg)
        : std::runtime_error(msg)
    {
    }
};

#endif /* TRANSPORT_EXCEPTION_H */


// st_error.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef ST_ERROR_H
#define ST_ERROR_H

#include <stdexcept>

/*!
 * \class StreamingTraceException
 * \brief Throw one of these to wrap an error.
 */
class StreamingTraceException : public std::exception
{
public:
    explicit StreamingTraceException(unsigned code, const char* detail)
        :  m_code(code), m_text(detail)
    {
    }

    explicit StreamingTraceException(unsigned code, const std::string& detail)
        :  m_code(code), m_text(detail)
    {
    }

    virtual const char* what() const throw() { return m_text.c_str(); }
    unsigned code() const throw() { return m_code; }
    ~StreamingTraceException() throw() {}

private:
    unsigned    m_code;
    std::string m_text;
};


#endif // ST_ERROR_H

// End of file st_error.h

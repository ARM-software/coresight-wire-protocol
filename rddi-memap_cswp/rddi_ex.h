// rddi_ex.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/*! \file rddi_ex.h
 *  \brief An exception carrying an RDDI error code.
 */

#ifndef INC_RDDI_EX_H
#define INC_RDDI_EX_H

#include <string>
#include <exception>

/*!
 * \class RddiEx
 * \brief Throw one of these to wrap an RDDI error.
 */
class RddiEx : public std::exception
{
public:
    explicit RddiEx(unsigned code, const std::string detail = "")
     :  m_code(code), m_text(detail)
    {
    }

    virtual const char* what() const throw() { return m_text.c_str(); }
    unsigned code() const throw() { return m_code; }
    ~RddiEx() throw() {}

    void AddContext(const std::string& context)
    {
        m_text.insert(0u, context + '\t');
    }

private:
    unsigned    m_code;
    std::string m_text;
};


#endif // INC_RDDI_EX_H

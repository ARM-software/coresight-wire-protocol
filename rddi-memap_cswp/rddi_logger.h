// rddi_logger.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef RDDI_LOGGER_H
#define RDDI_LOGGER_H

#include "rddi.h"

#include <sstream>
#include <string>
#include <memory>

#include <time.h>

#include "boost/thread/mutex.hpp"

#include "boost/noncopyable.hpp"


/**
 * A simple logger class that uses the log_stream to implement logging for
 * RDDI.  Using RDDI_LOGGER_MALOG__LEVEL you can prevent logging below a certain
 * level from being compiled.  Using RddiLogger::reportingLevel() one can limit
 * the logging performed at runtime.
 **/
class RddiLogger : boost::noncopyable
{
public:
    RddiLogger();

    virtual ~RddiLogger();

    int reportingLevel() const { return m_logLevel; }

    void setReportingLevel(int level);

    void setCallback(RDDILogCallback pfn, void* context);

    void stopLogging();

    void setLogFile(std::string logFile);

    void setLogFileReportingLevel(int level);

    int logFileReportingLevel() const { return m_logFile.empty() ? -1 : m_logFileLevel; }

    void log(const std::string& msg, int level);

private:
    boost::mutex m_Mutex;
    int m_logLevel;
    RDDILogCallback m_pLogger;
    void* m_pContext;
    std::string m_logFile;
    int m_logFileLevel;
};


/**
 * Class that provides an ostream to the log
 *
 * An ostringstream is wrapped and flushed to the logger on destruction.  The
 * C++ rules on temporary object lifetime ensures this happens at the end of
 * the source line.
 */
class RddiLogEntry
{
public:
    /**
     * Create a log entry with the given level
     */
    RddiLogEntry(RddiLogger& logger, int level)
        : m_Logger(logger),
          m_Level(level),
          m_Stream(new std::ostringstream)
    {
    }

    /**
     * Destroy a log entry, flushing to the logger
     */
    virtual ~RddiLogEntry()
    {
        if (m_Stream.get())
        {
            m_Logger.log(m_Stream->str(), m_Level);
        }
    }

    /**
     * Input to the log
     */
    template<class T>
    const RddiLogEntry& operator<<( const T& rhs ) const
    {
        if (m_Stream.get())
            *m_Stream << rhs;
        return *this;
    }

    /**
     * Pass std::endl / std::flush to the log
     */
    const RddiLogEntry& operator<<(std::ostream& ( *pf )(std::ostream&)) const
    {
        if (m_Stream.get())
            *m_Stream << pf;
        return *this;
    }

    /**
     * Pass manipulators (hex, dec etc) to the log
     */
    const RddiLogEntry& operator<<(std::ios_base& ( *pf )(std::ios_base&)) const
    {
        if (m_Stream.get())
            *m_Stream << pf;
        return *this;
    }

private:
    RddiLogger& m_Logger;
    int         m_Level;
    std::auto_ptr<std::ostringstream> m_Stream;
};


#ifndef RDDI_LOGGER_MALOG__LEVEL
#define RDDI_LOGGER_MALOG__LEVEL RDDI_LOGLEVEL_INFO
#endif

// Use a macro to ensure that the compiler can optimise out any logging below
// RDDI_LOGGER_MALOG__LEVEL and log entry isn't constructed when it is below
// reporting level

#define LOG(logger, level)                      \
    if (level > RDDI_LOGGER_MALOG__LEVEL) ;     \
    else if ((level > (logger).reportingLevel()) && (level > (logger).logFileReportingLevel())) ; \
    else RddiLogEntry((logger), level)

#define LOG_TRACE(logger)     LOG(logger, RDDI_LOGLEVEL_TRACE)
#define LOG_INFO(logger)      LOG(logger, RDDI_LOGLEVEL_INFO)
#define LOG_DEBUG(logger)     LOG(logger, RDDI_LOGLEVEL_DEBUG)
#define LOG_ERROR(logger)     LOG(logger, RDDI_LOGLEVEL_ERROR)
#define LOG_WARNING(logger)   LOG(logger, RDDI_LOGLEVEL_WARNING)
#define LOG_FATAL(logger)     LOG(logger, RDDI_LOGLEVEL_FATAL)

#endif //RDDI_LOGGER_H

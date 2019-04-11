// rddi_logger.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/*
 * \file rddi_logger.cpp
 * \brief Descriptive strings for RDDI errors.
 */
#include "rddi_logger.h"

#include "boost/noncopyable.hpp"

#include <cstdio>

#ifdef WIN32
#pragma warning(disable : 4996)
#endif

RddiLogger::RddiLogger()
    : m_logLevel(RDDI_LOGLEVEL_WARNING),
      m_pLogger(0),
      m_pContext(0),
      m_logFileLevel(RDDI_LOGLEVEL_INFO)
{
}


RddiLogger::~RddiLogger()
{
}


void RddiLogger::setCallback(RDDILogCallback pfn, void* context)
{
    if (pfn != 0)
    {
        boost::mutex::scoped_lock lock(m_Mutex);

        m_pLogger = pfn;
        m_pContext = context;
    }
    else
    {
        stopLogging();
    }
}


void RddiLogger::stopLogging()
{
    boost::mutex::scoped_lock lock(m_Mutex);

    /**
     * Stop logging coming out, needs to be called by RDDI_Close() to
     * make sure no logging sneaks out if another handle is opened
     **/
    m_pLogger = NULL;
    m_pContext = NULL;
}

void RddiLogger::setReportingLevel(int level)
{
    boost::mutex::scoped_lock lock(m_Mutex);

    m_logLevel = level;
}

void RddiLogger::setLogFile(std::string logFile)
{
    boost::mutex::scoped_lock lock(m_Mutex);

    m_logFile = logFile;
}

void RddiLogger::setLogFileReportingLevel(int level)
{
    boost::mutex::scoped_lock lock(m_Mutex);

    m_logFileLevel = level;
}

void RddiLogger::log(const std::string& msg, int level)
{
    boost::mutex::scoped_lock lock(m_Mutex);

    if (!msg.empty())
    {
        if (m_logFile.length() > 0 && level <= m_logFileLevel)
        {
            FILE *pFile = fopen(m_logFile.c_str(), "at");
            if (pFile != NULL)
            {
                time_t timestamp;
                time(&timestamp);
                tm* tm = localtime(&timestamp);
                fprintf(pFile, "%02d:%02d:%02d: [%d] %s\n", tm->tm_hour,tm->tm_min,tm->tm_sec, level, msg.c_str());
                fflush(pFile);
                fclose(pFile);
            }
        }

        if(m_pLogger != 0 && level <= m_logLevel)
        {
            m_pLogger(m_pContext, msg.c_str(), level);
        }
    }
}

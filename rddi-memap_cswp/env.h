// env.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/*! \file env.h
 *  \brief Implement RDDI functions for the environment.
 */
#ifndef INC_ENV_H
#define INC_ENV_H

#include <string>
#include <memory>
#include <map>
#include <boost/utility.hpp>
#include "rddi_streaming_trace.h"

#include <boost/shared_ptr.hpp>
#include "rddi_logger.h"

class Env : boost::noncopyable
{
public:
    Env();
    ~Env();

    void OpenFile(const char *pFilename);
    void GetSystemDescription(uint32* pDescriptionFormat, uint32* pDescriptionSize, uint8* pDescriptionDataBuffer, size_t bufferSize);

    int LastErrorCode() const;
    std::string LastErrorText() const;

    RddiLogger& logger() { return m_logger; }

    void MEM_AP_Connect(const char* clientInfo, char* targetInfo, size_t targetInfoLen);
    void MEM_AP_Disconnect();
    void MEM_AP_GetNumberOfAPs(int* apCount);
    void MEM_AP_Open(int apNumber);
    void MEM_AP_Close(int apNumber);
    void MEM_AP_ReadReg(int apNumber, int regID, uint32 *pValue);
    void MEM_AP_WriteReg(int apNumber, int regID, uint32 value);
    void MEM_AP_Read(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, void* buf);
    void MEM_AP_ReadRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, void* buf);
    void MEM_AP_Write(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, const void* buf);
    void MEM_AP_WriteRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, const void* buf);
    void MEM_AP_WriteValueRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, uint64 val);
    void MEM_AP_Fill(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, uint64 pattern);
    void MEM_AP_AccessBatch(int apNumber, uint64 baseAddress, MEM_AP_OP* ops, unsigned numOps, unsigned* opsCompleted);

    class Impl;

private:
    void Construct(const std::string& xmlFile);

    RddiLogger m_logger;

    typedef boost::shared_ptr<Impl> ImplPtr;
    ImplPtr m_impl;
};


#endif // INC_ENV_H

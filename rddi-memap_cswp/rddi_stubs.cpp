// rddi_stubs.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/*!
 *  \file rddi_stubs.cpp
 *  \brief Implementation of functions exported by RDDI dll
 */
#include <map>
#include <cstring>
#include <ostream>
#include <fstream>
#include <boost/shared_ptr.hpp>
#include <boost/current_function.hpp>
using namespace std;
using namespace boost;

#define _RDDI_EXPORT

// the imported versions of the files, from RDDI proper
#include "rddi.h"
#include "rddi_configinfo.h"
#include "rddi_debug.h"
#include "rddi_mem_ap.h"

#include "env_map.h"
#include "env.h"
#include "rddi_ex.h"
#include "utils.h"

using namespace Utils;
using namespace Utils::RddiError;
using EnvironmentMap::EnvPtr;

RDDI int RDDI_Open(RDDIHandle *pHandle, const void *pDetails)
{
    TRAP_EXCEPTIONS(
        CheckNotNull(pHandle);
        *pHandle = EnvironmentMap::Store(EnvPtr(new Env));
    );

    return LastErrorCode();
}

RDDI int RDDI_Close(RDDIHandle handle)
{
    // ensure that no logging tries to come back on a closed handle
    TRAP_EXCEPTIONS(
        RddiLogger& logger = EnvironmentMap::Lookup(handle)->logger();
        logger.stopLogging();
        );
    TRAP_EXCEPTIONS(EnvironmentMap::Remove(handle));
    return LastErrorCode();
}

RDDI int RDDI_GetVersion(RDDIHandle, int *pMajor, int *pMinor, int *pBuild, char *pDescription, size_t descriptionLen)
{
    TRAP_EXCEPTIONS(
        Version(pMajor, pMinor, pBuild, pDescription, descriptionLen, 0);
        );
    return LastErrorCode();
}

RDDI void RDDI_SetLogCallback(
    RDDIHandle handle,
    RDDILogCallback pfn,
    void* context,
    int maxLogLevel)
{
    TRAP_EXCEPTIONS(
        RddiLogger& logger = EnvironmentMap::Lookup(handle)->logger();
        logger.setCallback(pfn, context);
        logger.setReportingLevel(maxLogLevel);
        );
}

RDDI int RDDI_GetLastError(int *pError, char *pDetails, size_t detailsLen)
{
    try
    {
        // report code
        CheckNotNull(pError);
        *pError = LastErrorCode();

        if (detailsLen != 0u)
        {
            // report text
            CheckNotNull(pDetails);
            string errorText(LastErrorText());
            *pDetails = '\0';
            strncat(pDetails, errorText.c_str(), detailsLen - 1);

            if (detailsLen <= errorText.length())
            {
                throw RddiEx(RDDI_BUFFER_OVERFLOW);
            }
        }
        return RDDI_SUCCESS;
    }
    catch (const RddiEx& e)
    {
        return e.code();
    }
    catch (...)
    {
        return RDDI_INTERNAL_ERROR;
    }
}

RDDI int ConfigInfo_OpenFile(RDDIHandle handle, const char *pFilename)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->OpenFile(pFilename));
    return LastErrorCode();
}

RDDI int ConfigInfo_GetSystemDescription(RDDIHandle handle, uint32* pDescriptionFormat, uint32* pDescriptionSize, uint8* pDescriptionDataBuffer, size_t bufferSize)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->GetSystemDescription(pDescriptionFormat, pDescriptionSize, pDescriptionDataBuffer, bufferSize));
    return LastErrorCode();
}

RDDI int MEM_AP_Connect(RDDIHandle handle, const char* clientInfo, char* targetInfo, size_t targetInfoLen)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->MEM_AP_Connect(clientInfo, targetInfo, targetInfoLen));
    return LastErrorCode();
}

RDDI int MEM_AP_Disconnect(RDDIHandle handle)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->MEM_AP_Disconnect());
    return LastErrorCode();
}

RDDI int MEM_AP_GetNumberOfAPs(RDDIHandle handle, int* apCount)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->MEM_AP_GetNumberOfAPs(apCount));
    return LastErrorCode();
}

RDDI int MEM_AP_Open(RDDIHandle handle, int apNumber)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->MEM_AP_Open(apNumber));
    return LastErrorCode();
}

RDDI int MEM_AP_Close(RDDIHandle handle, int apNumber)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->MEM_AP_Close(apNumber));
    return LastErrorCode();
}

RDDI int MEM_AP_ReadReg(RDDIHandle handle, int apNumber, int regID, uint32 *pValue)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->MEM_AP_ReadReg(apNumber, regID, pValue));
    return LastErrorCode();
}

RDDI int MEM_AP_WriteReg(RDDIHandle handle, int apNumber, int regID, uint32 value)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->MEM_AP_WriteReg(apNumber, regID, value));
    return LastErrorCode();
}

RDDI int MEM_AP_Read(RDDIHandle handle, int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, void* buf)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->MEM_AP_Read(apNumber, addr, accSize, flags, size, buf));
    return LastErrorCode();
}

RDDI int MEM_AP_ReadRepeat(RDDIHandle handle, int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, void* buf)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->MEM_AP_ReadRepeat(apNumber, addr, accSize, flags, repeatCount, buf));
    return LastErrorCode();
}

RDDI int MEM_AP_Write(RDDIHandle handle, int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, const void* buf)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->MEM_AP_Write(apNumber, addr, accSize, flags, size, buf));
    return LastErrorCode();
}

RDDI int MEM_AP_WriteRepeat(RDDIHandle handle, int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, const void* buf)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->MEM_AP_WriteRepeat(apNumber, addr, accSize, flags, repeatCount, buf));
    return LastErrorCode();
}

RDDI int MEM_AP_WriteValueRepeat(RDDIHandle handle, int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, uint32 val)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->MEM_AP_WriteValueRepeat(apNumber, addr, accSize, flags, repeatCount, val));
    return LastErrorCode();
}

RDDI int MEM_AP_Fill(RDDIHandle handle, int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, uint64 pattern)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->MEM_AP_Fill(apNumber, addr, accSize, flags, repeatCount, pattern));
    return LastErrorCode();
}

RDDI int MEM_AP_AccessBatch(RDDIHandle handle, int apNumber, uint64 baseAddress, MEM_AP_OP* ops, unsigned numOps, unsigned* opsCompleted)
{
    TRAP_EXCEPTIONS(EnvironmentMap::Lookup(handle)->MEM_AP_AccessBatch(apNumber, baseAddress, ops, numOps, opsCompleted));
    return LastErrorCode();
}


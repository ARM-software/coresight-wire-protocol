// env.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/*!
 *  \file env.cpp
 *  \brief Env implementation
 */
#include "rddi_debug.h"
#include "rddi_mem_ap.h"
#include "env.h"

#include "cswp_client.h"
#include "cswp_usb_transport.h"
#include "cswp_tcp_transport.h"

#include <stdarg.h>

#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/xml_parser.hpp>
#include <boost/algorithm/string.hpp>

using namespace std;

#include "rddi_ex.h"
#include "utils.h"

using Utils::CheckNotNull;

const char CLIENT_ID[] = "RDDI-MEMAP_CSWP";

// Because we constuct the Env in 2 stages, we have 2 implementations...
class Env::Impl
{
public:
    virtual ~Impl() {}

    virtual void GetSystemDescription(uint32* pDescriptionFormat, uint32* pDescriptionSize, uint8* pDescriptionDataBuffer, size_t bufferSize) = 0;

    virtual void MEM_AP_Connect(const char* clientInfo, char* targetInfo, size_t targetInfoLen) = 0;
    virtual void MEM_AP_Disconnect() = 0;
    virtual void MEM_AP_GetNumberOfAPs(int* apCount) = 0;
    virtual void MEM_AP_Open(int apNumber) = 0;
    virtual void MEM_AP_Close(int apNumber) = 0;
    virtual void MEM_AP_ReadReg(int apNumber, int regID, uint32 *pValue) = 0;
    virtual void MEM_AP_WriteReg(int apNumber, int regID, uint32 value) = 0;
    virtual void MEM_AP_Read(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, void* buf) = 0;
    virtual void MEM_AP_ReadRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, void* buf) = 0;
    virtual void MEM_AP_Write(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, const void* buf) = 0;
    virtual void MEM_AP_WriteRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, const void* buf) = 0;
    virtual void MEM_AP_WriteValueRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, uint32 val) = 0;
    virtual void MEM_AP_Fill(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, uint64 pattern) = 0;
    virtual void MEM_AP_AccessBatch(int apNumber, uint64 baseAddress, MEM_AP_OP* ops, unsigned numOps, unsigned* opsCompleted) = 0;
};

// NullImplementation is a placeholder to return not configured errors if the environment has not yet
// been fully configured
class NullImplementation : public Env::Impl
{
public:
    virtual void GetSystemDescription(uint32* pDescriptionFormat, uint32* pDescriptionSize, uint8* pDescriptionDataBuffer, size_t bufferSize)
    {
        Fail();
    }

    virtual void MEM_AP_Connect(const char* clientInfo, char* targetInfo, size_t targetInfoLen)
    {
        Fail();
    }

    virtual void MEM_AP_Disconnect()
    {
        Fail();
    }

    virtual void MEM_AP_GetNumberOfAPs(int* apCount)
    {
        Fail();
    }

    virtual void MEM_AP_Open(int apNumber)
    {
        Fail();
    }

    virtual void MEM_AP_Close(int apNumber)
    {
        Fail();
    }

    virtual void MEM_AP_ReadReg(int apNumber, int regID, uint32 *pValue)
    {
        Fail();
    }

    virtual void MEM_AP_WriteReg(int apNumber, int regID, uint32 value)
    {
        Fail();
    }

    virtual void MEM_AP_Read(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, void* buf)
    {
        Fail();
    }

    virtual void MEM_AP_ReadRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, void* buf)
    {
        Fail();
    }

    virtual void MEM_AP_Write(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, const void* buf)
    {
        Fail();
    }

    virtual void MEM_AP_WriteRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, const void* buf)
    {
        Fail();
    }

    virtual void MEM_AP_WriteValueRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, uint32 val)
    {
        Fail();
    }

    virtual void MEM_AP_Fill(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, uint64 pattern)
    {
        Fail();
    }

    virtual void MEM_AP_AccessBatch(int apNumber, uint64 baseAddress, MEM_AP_OP* ops, unsigned numOps, unsigned* opsCompleted)
    {
        Fail();
    }

private:
    void Fail() const
    {
        throw RddiEx(RDDI_NO_CONFIG, "Not configured");
    }
};


// MemAPImpl does the real work with the mmap()-ed core files and fake registers.
class MemAPImpl : public Env::Impl
{
public:
    enum APType
    {
        AP_ERROR = -1,      //!< Not a valid AP
        AP_UNKNOWN = 0,     //!< Default value - no AP.
        AP_AHB,             //!< AHB-AP - AHB bus connection.
        AP_AHB_M3,          //!< M3 Type AHB-AP
        AP_AHB_SWIFT,       //!< Swift Type AHB-AP
        AP_AHB_FLYCATCHER,  //!< Flycatcher Type AHB-AP
        AP_AHB_PELICAN,     //!< Pelican Type AHB-AP
        AP_APB,             //!< APB-AP - APB bus connection - probably the debug bus.
        AP_AUTH,            //!< Authentication AP - for use in Authenticated debug systems
        AP_AXI,             //!< The AXI
        AP_JTAG,            //!< JTAG-AP - legacy  connection.
        AP_AHB_AMBA5       //!< AMBA 5 AHB-AP as found in V8-M
    };

    MemAPImpl(RddiLogger& logger, const std::string &xmlFile);

    virtual void GetSystemDescription(uint32* pDescriptionFormat, uint32* pDescriptionSize, uint8* pDescriptionDataBuffer, size_t bufferSize);

    virtual void MEM_AP_Connect(const char* clientInfo, char* targetInfo, size_t targetInfoLen);
    virtual void MEM_AP_Disconnect();
    virtual void MEM_AP_GetNumberOfAPs(int* apCount);
    virtual void MEM_AP_Open(int apNumber);
    virtual void MEM_AP_Close(int apNumber);
    virtual void MEM_AP_ReadReg(int apNumber, int regID, uint32 *pValue);
    virtual void MEM_AP_WriteReg(int apNumber, int regID, uint32 value);
    virtual void MEM_AP_Read(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, void* buf);
    virtual void MEM_AP_ReadRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, void* buf);
    virtual void MEM_AP_Write(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, const void* buf);
    virtual void MEM_AP_WriteRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, const void* buf);
    virtual void MEM_AP_WriteValueRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, uint32 val);
    virtual void MEM_AP_Fill(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, uint64 pattern);
    virtual void MEM_AP_AccessBatch(int apNumber, uint64 baseAddress, MEM_AP_OP* ops, unsigned numOps, unsigned* opsCompleted);
private:

    struct APInfo
    {
        std::string address;
        std::string type;
    };

    APInfo& getAP(int apNumber);

    void doRead(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, void* buf, bool incr);
    void doWrite(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, const void* buf, bool incr);

    void log(const char* fmt, ...);

    std::string m_configFile;
    FILE* m_logFile;

    std::string m_cswpAddr;
    std::string m_cswpTransportType;

    // Only for TCP transport
    std::string m_cswpIpAddr;
    int m_cswpNetPort;

    std::vector<APInfo> m_aps;

    bool m_connected;

    cswp_client_transport_t m_cswpTransport;
    cswp_client_t           m_cswpClient;
};

namespace
{
    enum
    {
        TRANSPORT_TYPES_USB,
        TRANSPORT_TYPES_TCP,
        Num_TRANSPORT_TYPES
    };

    std::string TRANSPORT_TYPES_STRINGS[Num_TRANSPORT_TYPES] = {
        "usb",
        "tcp"
    };

    size_t accessSizeBytes(MEM_AP_ACC_SIZE accSize)
    {
        switch (accSize)
        {
        case MEM_AP_ACC_8:  return 1;
        case MEM_AP_ACC_16: return 2;
        case MEM_AP_ACC_32: return 4;
        case MEM_AP_ACC_64: return 8;
        }
    }

    cswp_access_size_t mapAccessSize(MEM_AP_ACC_SIZE accSize)
    {
        switch (accSize)
        {
        case MEM_AP_ACC_8:  return CSWP_ACCESS_SIZE_8;
        case MEM_AP_ACC_16: return CSWP_ACCESS_SIZE_16;
        case MEM_AP_ACC_32: return CSWP_ACCESS_SIZE_32;
        case MEM_AP_ACC_64: return CSWP_ACCESS_SIZE_64;
        }
    }

    unsigned mapFlags(unsigned memapFlags, bool incr)
    {
        unsigned cswpFlags = 0;
        if (!incr)
            cswpFlags |= CSWP_MEM_NO_ADDR_INC;

        if (memapFlags & MEM_AP_OVERRIDE_INCR)
            cswpFlags |= CSWP_MEMAP_WITH_INCR((memapFlags & MEM_AP_INCR) >> MEM_AP_INCR_SHIFT);
        if (memapFlags & MEM_AP_OVERRIDE_PROT)
            cswpFlags |= CSWP_MEMAP_WITH_PROT((memapFlags & MEM_AP_PROT) >> MEM_AP_PROT_SHIFT);
        if (memapFlags & MEM_AP_OVERRIDE_MODE)
            cswpFlags |= CSWP_MEMAP_WITH_MODE((memapFlags & MEM_AP_MODE) >> MEM_AP_MODE_SHIFT);
        if (memapFlags & MEM_AP_OVERRIDE_TYPE)
            cswpFlags |= CSWP_MEMAP_WITH_TYPE((memapFlags & MEM_AP_TYPE) >> MEM_AP_TYPE_SHIFT);
        if (memapFlags & MEM_AP_OVERRIDE_ERR)
            cswpFlags |= CSWP_MEMAP_WITH_ERR((memapFlags & MEM_AP_ERR) >> MEM_AP_ERR_SHIFT);

        return cswpFlags;
    }

    // ADIv6 only
    unsigned decodeRegId(unsigned regId)
    {
        if (regId & MEMAP_REG_CTRL)
            return 0xD00 | (regId & 0xF);
        else if (regId & MEMAP_REG_BANKED_DATA)
            return 0xD10 | (regId & 0xF);
        else if (regId & MEMAP_REG_ID)
            return 0xDF0 | (regId & 0xF);
        return regId & 0xFFF;
    }
}

MemAPImpl::MemAPImpl(RddiLogger& logger,
                     const std::string &xmlFile)
    : m_connected(false),
      m_configFile(xmlFile),
      m_logFile(0)
{
    try
    {
        boost::property_tree::ptree config;

        boost::property_tree::read_xml(xmlFile, config);

        boost::optional<std::string> log = config.get_optional<std::string>("config.log.<xmlattr>.file");
        if (log)
            m_logFile = fopen(log->c_str(), "w");

        m_cswpAddr = config.get<std::string>("config.target.<xmlattr>.address");

        // get AP list from config
        const boost::property_tree::ptree& aps = config.get_child("config.memaps");
        for (boost::property_tree::ptree::const_iterator d = aps.begin(); d != aps.end(); ++d)
        {
            APInfo apInfo;
            apInfo.address = d->second.get<std::string>("<xmlattr>.address");
            apInfo.type = d->second.get<std::string>("<xmlattr>.type");
            m_aps.push_back(apInfo);
        }

        m_cswpTransportType = boost::algorithm::to_lower_copy(config.get<std::string>("config.target.<xmlattr>.transport"));
        if (TRANSPORT_TYPES_STRINGS[TRANSPORT_TYPES_TCP] == m_cswpTransportType)
        {
            m_cswpIpAddr = config.get<std::string>("config.target.<xmlattr>.ip");
            m_cswpNetPort = config.get<int>("config.target.<xmlattr>.port");
        }
    }
    catch (const std::exception& e)
    {
        throw RddiEx(RDDI_PARSE_FAILED, std::string("Failed to parse config file: ") + e.what());
    }
}

void MemAPImpl::GetSystemDescription(uint32* pDescriptionFormat, uint32* pDescriptionSize, uint8* pDescriptionDataBuffer, size_t bufferSize)
{
    int res = cswp_get_system_description(&m_cswpClient, pDescriptionFormat, pDescriptionSize, pDescriptionDataBuffer, bufferSize);
    if (res != CSWP_SUCCESS)
    {
        int rddiRes = (res == CSWP_UNSUPPORTED ? RDDI_CMDUNSUPP : RDDI_FAILED);
        throw RddiEx(rddiRes, "CSWP get system description failed");
    }
}

void MemAPImpl::MEM_AP_Connect(const char* clientInfo, char* targetInfo, size_t targetInfoLen)
{
    // open CSWP connection
    if (TRANSPORT_TYPES_STRINGS[TRANSPORT_TYPES_USB] ==  m_cswpTransportType)
        cswp_client_usb_transport_init(&m_cswpTransport, m_cswpAddr.c_str());
    else if (TRANSPORT_TYPES_STRINGS[TRANSPORT_TYPES_TCP] == m_cswpTransportType)
        cswp_client_tcp_transport_init(&m_cswpTransport, m_cswpIpAddr.c_str(), m_cswpNetPort);

    int res = cswp_client_init(&m_cswpClient, &m_cswpTransport);
    if (res != CSWP_SUCCESS)
        throw RddiEx(RDDI_FAILED, "Failed to initialise CSWP client");

    char serverID[256];
    unsigned serverProtocolVersion, serverVersion;
    res = cswp_init(&m_cswpClient, CLIENT_ID,
                    &serverProtocolVersion,
                    serverID, sizeof(serverID),
                    &serverVersion);
    if (res != CSWP_SUCCESS)
        throw RddiEx(RDDI_FAILED, "Failed to initialise CSWP connection (" + std::string(m_cswpClient.errorMsg) + ")");
    printf("Connected to %s: version 0x%X, protocol version %d \n",
           serverID, serverVersion, serverProtocolVersion);

    // configure CSWP with AP list
    std::vector<const char*> deviceList;
    std::vector<const char*> deviceType;
    for (int i = 0; i < m_aps.size(); ++i)
    {
        ostringstream s;
        s << "AP_" << i;
        deviceList.push_back(strdup(s.str().c_str()));
        deviceType.push_back(strdup(m_aps[i].type.c_str()));
    }
    cswp_set_devices(&m_cswpClient, deviceList.size(), &deviceList[0], &deviceType[0]);
    for (int i = 0; i < m_aps.size(); ++i)
        free((void*)deviceList[i]);

    // set AP addresses
    for (int i = 0; i < m_aps.size(); ++i)
    {
      if (strncmp(m_aps[i].type.c_str(), "mem-ap", 6) == 0)
      {
          res = cswp_set_config(&m_cswpClient, i, "BASE_ADDRESS", m_aps[i].address.c_str());
          if (res != CSWP_SUCCESS)
              throw RddiEx(RDDI_FAILED, "Failed to configure CSWP AP address");
      }
    }

    std::ostringstream s;
    s << "RDDI-MEMAP_CSWP 1.0: " << serverID << ": version " << serverVersion << ", protocol " << serverProtocolVersion;
    log(s.str().c_str());
    Utils::FillStringParam(s.str(), targetInfo, targetInfoLen, 0);

    m_connected = true;
}

void MemAPImpl::MEM_AP_Disconnect()
{
    if (!m_connected)
        throw RddiEx(RDDI_NOCONN, "MEM-AP interface not connected");

    int res = cswp_term(&m_cswpClient);
    m_connected = false;
    if (res != CSWP_SUCCESS)
        throw RddiEx(RDDI_FAILED, "Failed to terminate CSWP connection");
    cswp_client_term(&m_cswpClient);
}

void MemAPImpl::MEM_AP_GetNumberOfAPs(int* apCount)
{
    CheckNotNull(apCount);

    *apCount = m_aps.size();
}

void MemAPImpl::MEM_AP_Open(int apNumber)
{
    if (!m_connected)
        throw RddiEx(RDDI_NOCONN, "MEM-AP interface not connected");

    APInfo& apInfo = getAP(apNumber);

    int res = cswp_device_open(&m_cswpClient, apNumber, NULL, 0);
    if (res != CSWP_SUCCESS)
        throw RddiEx(RDDI_FAILED, "Failed to open CSWP device");
}

void MemAPImpl::MEM_AP_Close(int apNumber)
{
    if (!m_connected)
        throw RddiEx(RDDI_NOCONN, "MEM-AP interface not connected");

    APInfo& apInfo = getAP(apNumber);

    int res = cswp_device_close(&m_cswpClient, apNumber);
    if (res != CSWP_SUCCESS)
        throw RddiEx(RDDI_FAILED, "Failed to close CSWP device");
}

void MemAPImpl::MEM_AP_ReadReg(int apNumber, int regID, uint32 *pValue)
{
    if (!m_connected)
        throw RddiEx(RDDI_NOCONN, "MEM-AP interface not connected");

    CheckNotNull(pValue);

    APInfo& apInfo = getAP(apNumber);

    unsigned regIDs = decodeRegId(regID);
    int res = cswp_device_reg_read(&m_cswpClient, apNumber, 1, &regIDs, pValue, 1);
    if (res != CSWP_SUCCESS)
        throw RddiEx(RDDI_FAILED, "Failed to read CSWP register");
}


void MemAPImpl::MEM_AP_WriteReg(int apNumber, int regID, uint32 value)
{
    if (!m_connected)
        throw RddiEx(RDDI_NOCONN, "MEM-AP interface not connected");

    APInfo& apInfo = getAP(apNumber);

    unsigned regIDs = decodeRegId(regID);
    int res = cswp_device_reg_write(&m_cswpClient, apNumber, 1, &regIDs, &value, 1);
    if (res != CSWP_SUCCESS)
        throw RddiEx(RDDI_FAILED, "Failed to write CSWP register");
}

void MemAPImpl::MEM_AP_Read(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, void* buf)
{
    if (!m_connected)
        throw RddiEx(RDDI_NOCONN, "MEM-AP interface not connected");

    CheckNotNull(buf);

    doRead(apNumber, addr, accSize, flags, size, buf, true);
}

void MemAPImpl::MEM_AP_ReadRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, void* buf)
{
    if (!m_connected)
        throw RddiEx(RDDI_NOCONN, "MEM-AP interface not connected");

    CheckNotNull(buf);

    doRead(apNumber, addr, accSize, flags, accessSizeBytes(accSize)*repeatCount, buf, false);
}


void MemAPImpl::doRead(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, void* buf, bool incr)
{
    APInfo& apInfo = getAP(apNumber);
    size_t bytesRead;

    int res = cswp_device_mem_read(&m_cswpClient, apNumber,
                                   addr, size, mapAccessSize(accSize), mapFlags(flags, incr),
                                   (uint8_t*)buf, &bytesRead);
    if (res != CSWP_SUCCESS)
        throw RddiEx(RDDI_FAILED, "CSWP memory read failed");
}

void MemAPImpl::MEM_AP_Write(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, const void* buf)
{
    if (!m_connected)
        throw RddiEx(RDDI_NOCONN, "MEM-AP interface not connected");

    CheckNotNull(buf);

    doWrite(apNumber, addr, accSize, flags, size, buf, true);
}

void MemAPImpl::MEM_AP_WriteRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, const void* buf)
{
    if (!m_connected)
        throw RddiEx(RDDI_NOCONN, "MEM-AP interface not connected");

    CheckNotNull(buf);

    doWrite(apNumber, addr, accSize, flags, accessSizeBytes(accSize)*repeatCount, buf, false);
}

void MemAPImpl::doWrite(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, const void* buf, bool incr)
{
    APInfo& apInfo = getAP(apNumber);

    int res = cswp_device_mem_write(&m_cswpClient, apNumber,
                                    addr, size, mapAccessSize(accSize), mapFlags(flags, incr), (const uint8_t*)buf);
    if (res != CSWP_SUCCESS)
        throw RddiEx(RDDI_FAILED, "CSWP memory write failed");
}

void MemAPImpl::MEM_AP_WriteValueRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, uint32 val)
{
    if (!m_connected)
        throw RddiEx(RDDI_NOCONN, "MEM-AP interface not connected");

    // generate buffer and write
    size_t bufSz;
    uint32 repVal;
    size_t writeSize;

    switch (accSize)
    {
    case MEM_AP_ACC_8:
        repVal = ((val & 0xFF) << 24) | ((val & 0xFF) << 16) | ((val & 0xFF) << 8) | (val & 0xFF);
        bufSz = (repeatCount + 3) / 4;
        writeSize = repeatCount;
        break;

    case MEM_AP_ACC_16:
        repVal = ((val & 0xFFFF) << 16) | (val & 0xFFFF);
        bufSz = (repeatCount + 1) / 2;
        writeSize = repeatCount * 2;
        break;

    case MEM_AP_ACC_32:
        repVal = val;
        bufSz = repeatCount;
        writeSize = repeatCount * 4;
        break;

    default:
        throw RddiEx(RDDI_BADARG, "Unsupported access size");
    }

    std::vector<uint32> buf(bufSz, repVal);

    doWrite(apNumber, addr, accSize, flags, writeSize, &buf[0], false);

    // TODO: use transaction counter if available on AP
}

void MemAPImpl::MEM_AP_Fill(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, uint64 pattern)
{
    if (!m_connected)
        throw RddiEx(RDDI_NOCONN, "MEM-AP interface not connected");

    // generate buffer and write
    size_t bufSz;
    uint32 repVal;
    size_t writeSize;

    switch (accSize)
    {
    case MEM_AP_ACC_8:
        repVal = ((pattern & 0xFF) << 24) | ((pattern & 0xFF) << 16) | ((pattern & 0xFF) << 8) | (pattern & 0xFF);
        break;

    case MEM_AP_ACC_16:
        repVal = ((pattern & 0xFFFF) << 16) | (pattern & 0xFFFF);
        break;

    case MEM_AP_ACC_32:
        repVal = pattern;
        break;

    default:
        throw RddiEx(RDDI_BADARG, "Unsupported access size");
    }

    writeSize = accessSizeBytes(accSize) * repeatCount;
    bufSz = (writeSize + 3) / 4;
    std::vector<uint32> buf(bufSz, repVal);

    doWrite(apNumber, addr, accSize, flags, writeSize, &buf[0], true);

    // TODO: use transaction counter if available on AP
}

void MemAPImpl::MEM_AP_AccessBatch(int apNumber, uint64 baseAddress, MEM_AP_OP* ops, unsigned numOps, unsigned* opsCompleted)
{
    if (!m_connected)
        throw RddiEx(RDDI_NOCONN, "MEM-AP interface not connected");

    CheckNotNull(ops);
    CheckNotNull(opsCompleted);

    APInfo& apInfo = getAP(apNumber);

    // TODO: re-write to use CSWP batch / poll messages

    void* pData;
    unsigned localOps = 0;
    uint32 pollMask = 0xFFFFFFFF;
    uint32 pollValue = 0;

    log("MEM_AP_AccessBatch: %llx: %d\n", baseAddress, numOps);

    cswp_batch_begin(&m_cswpClient, true);

    int res = CSWP_SUCCESS;
    for (int i = 0; i < numOps && res == CSWP_SUCCESS; ++i)
    {
        MEM_AP_OP& op = ops[i];
        uint64 addr = baseAddress + op.addr;
        cswp_access_size_t accSize = mapAccessSize(op.accSize);
        unsigned cswpFlags = mapFlags(op.flags, true);
        size_t bytesRead;

        switch (op.op)
        {
        case MEM_AP_BATCH_OP_WRITE:
            if (op.indirect)
                pData = op.pData;
            else
                pData = &op.data;
            res = cswp_device_mem_write(&m_cswpClient, apNumber,
                                        addr, op.count,
                                        accSize, cswpFlags, (const uint8_t*)pData);
            break;

        case MEM_AP_BATCH_OP_WRITE_RPT:
            if (op.indirect)
                pData = op.pData;
            else
                pData = &op.data;
            res = cswp_device_mem_write(&m_cswpClient, apNumber,
                                        addr, accessSizeBytes(op.accSize)*op.count,
                                        accSize, cswpFlags | CSWP_MEM_NO_ADDR_INC, (const uint8_t*)pData);
            break;

        case MEM_AP_BATCH_OP_READ:
            if (op.indirect)
                pData = op.pData;
            else
                pData = &op.data;
            res = cswp_device_mem_read(&m_cswpClient, apNumber,
                                       addr, op.count,
                                       accSize, cswpFlags,
                                       (uint8_t*)pData, &bytesRead);
            break;

        case MEM_AP_BATCH_OP_READ_RPT:
            if (op.indirect)
                pData = op.pData;
            else
                pData = &op.data;
            res = cswp_device_mem_read(&m_cswpClient, apNumber,
                                       addr, accessSizeBytes(op.accSize)*op.count,
                                       accSize, cswpFlags | CSWP_MEM_NO_ADDR_INC,
                                       (uint8_t*)pData, &bytesRead);
            break;

        case MEM_AP_BATCH_OP_POLL_MASK:
            pollMask = op.data;
            localOps++;
            log("  poll mask: 0x%08X\n", pollMask);
            break;

        case MEM_AP_BATCH_OP_POLL_VALUE:
            pollValue = op.data;
            localOps++;
            log("  poll value: 0x%08X\n", pollMask);
            break;

        case MEM_AP_BATCH_OP_POLL_EQ:
            if (op.indirect)
                pData = op.pData;
            else
                pData = &op.data;
            res = cswp_device_mem_poll(&m_cswpClient, apNumber,
                                       addr, 4,
                                       accSize, cswpFlags, op.count, 0,
                                       (uint8_t*)&pollMask, (uint8_t*)&pollValue,
                                       (uint8_t*)pData, &bytesRead);
            break;

        case MEM_AP_BATCH_OP_POLL_NE:
            if (op.indirect)
                pData = op.pData;
            else
                pData = &op.data;
            res = cswp_device_mem_poll(&m_cswpClient, apNumber,
                                       addr, 4,
                                       accSize, cswpFlags | CSWP_MEM_POLL_MATCH_NE, op.count, 0,
                                       (uint8_t*)&pollMask, (uint8_t*)&pollValue,
                                       (uint8_t*)pData, &bytesRead);
            break;

        case MEM_AP_BATCH_OP_CHECK_EQ:
            res = cswp_device_mem_poll(&m_cswpClient, apNumber,
                                       addr, 4,
                                       accSize, cswpFlags | CSWP_MEM_POLL_CHECK_LAST, 0, 0,
                                       (uint8_t*)&pollMask, (uint8_t*)&pollValue,
                                       0, &bytesRead);
            break;

        case MEM_AP_BATCH_OP_CHECK_NE:
            res = cswp_device_mem_poll(&m_cswpClient, apNumber,
                                       addr, 4,
                                       accSize,
                                       cswpFlags | CSWP_MEM_POLL_CHECK_LAST | CSWP_MEM_POLL_MATCH_NE,
                                       0, 0,
                                       (uint8_t*)&pollMask, (uint8_t*)&pollValue,
                                       0, &bytesRead);
            break;
        }
    }

    res = cswp_batch_end(&m_cswpClient, opsCompleted);
    if (opsCompleted)
        (*opsCompleted) += localOps;

    if (res != CSWP_SUCCESS)
        throw RddiEx(RDDI_FAILED, "CSWP batch operation failed");
}

MemAPImpl::APInfo& MemAPImpl::getAP(int apNumber)
{
    if (apNumber >= m_aps.size())
        throw RddiEx(RDDI_BADARG, "Invalid AP");
    return m_aps[apNumber];
}


void MemAPImpl::log(const char* fmt, ...)
{
    if (m_logFile) {
        va_list args;
        va_start(args, fmt);
        vfprintf(m_logFile, fmt, args);
        va_end(args);
        fflush(m_logFile);
    }
}


Env::Env()
    : m_impl(new NullImplementation())
{
}

Env::~Env()
{
}

void Env::Construct(const string& xmlFile)
{
    m_impl.reset(new MemAPImpl(m_logger, xmlFile));
}

void Env::OpenFile(const char *pFilename)
{
    Construct(pFilename);
}

void Env::GetSystemDescription(uint32* pDescriptionFormat, uint32* pDescriptionSize, uint8* pDescriptionDataBuffer, size_t bufferSize)
{
    m_impl->GetSystemDescription(pDescriptionFormat, pDescriptionSize, pDescriptionDataBuffer, bufferSize);
}

void Env::MEM_AP_Connect(const char* clientInfo, char* targetInfo, size_t targetInfoLen)
{
    m_impl->MEM_AP_Connect(clientInfo, targetInfo, targetInfoLen);
}

void Env::MEM_AP_Disconnect()
{
    m_impl->MEM_AP_Disconnect();
}

void Env::MEM_AP_GetNumberOfAPs(int* apCount)
{
    m_impl->MEM_AP_GetNumberOfAPs(apCount);
}

void Env::MEM_AP_Open(int apNumber)
{
    m_impl->MEM_AP_Open(apNumber);
}

void Env::MEM_AP_Close(int apNumber)
{
    m_impl->MEM_AP_Close(apNumber);
}

void Env::MEM_AP_ReadReg(int apNumber, int regID, uint32 *pValue)
{
    m_impl->MEM_AP_ReadReg(apNumber, regID, pValue);
}

void Env::MEM_AP_WriteReg(int apNumber, int regID, uint32 value)
{
    m_impl->MEM_AP_WriteReg(apNumber, regID, value);
}

void Env::MEM_AP_Read(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, void* buf)
{
    m_impl->MEM_AP_Read(apNumber, addr, accSize, flags, size, buf);
}

void Env::MEM_AP_ReadRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, void* buf)
{
    m_impl->MEM_AP_ReadRepeat(apNumber, addr, accSize, flags, repeatCount, buf);
}

void Env::MEM_AP_Write(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, const void* buf)
{
    m_impl->MEM_AP_Write(apNumber, addr, accSize, flags, size, buf);
}

void Env::MEM_AP_WriteRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, const void* buf)
{
    m_impl->MEM_AP_WriteRepeat(apNumber, addr, accSize, flags, repeatCount, buf);
}

void Env::MEM_AP_WriteValueRepeat(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, uint32 val)
{
    m_impl->MEM_AP_WriteValueRepeat(apNumber, addr, accSize, flags, repeatCount, val);
}

void Env::MEM_AP_Fill(int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, uint64 pattern)
{
    m_impl->MEM_AP_Fill(apNumber, addr, accSize, flags, repeatCount, pattern);
}

void Env::MEM_AP_AccessBatch(int apNumber, uint64 baseAddress, MEM_AP_OP* ops, unsigned numOps, unsigned* opsCompleted)
{
    m_impl->MEM_AP_AccessBatch(apNumber, baseAddress, ops, numOps, opsCompleted);
}




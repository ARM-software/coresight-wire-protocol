/*
 * rddi_mem_ap.h - RDDI the MEM-AP interface
 * Copyright (C) 2018 Arm Limited. All rights reserved.
 */
/*!
 * @file
 *
 * @brief This file describes the MEM-AP interface of the RDDI module.
 *
 * RDDI MEM-AP provides access to CoreSight MEM-APs and is intended as an
 * abstraction layer for debug tools to access CoreSight components.  The
 * interface provides memory access functions for operations on the bus
 * attached to a MEM-AP and register functions to access the registers of the
 * MEM-AP.
 *
 * To connect to a MEM-AP, the client should use the following sequence:
 *  - Open the RDDI interface with RDDI_Open()
 *  - Configure the RDDI interface with ConfigInfo_OpenFile().  The
 *    configuration file is an implementation defined format.
 *  - Connect to the MEM-AP interface with MEM_AP_Connect()
 *  - Open a specific MEM-AP with MEM_AP_Open()
 *  - Perform operations using the MEM_AP_xxx() functions
 *
 * To close the connection:
 *  - Close the open MEM-APs with MEM_AP_Close()
 *  - Close the MEM-AP interface with MEM_AP_Disconnect()
 *  - Close the RDDI interface with RDDI_Close()
 *
 * An implementation of RDDI MEM-AP can provide access to several MEM-APs, for
 * example a platform may have APB-AP for debug components and AXI-AP for
 * access to system memory.  MEM-APs are numbered consecutively from 0.  The
 * number of MEM-APs provided by a connection can be discovered using the
 * MEM_AP_GetNumberOfAPs() function after the MEM-AP interface has been
 * connected.  Each AP can then be identified using its ID registers.
 *
 * Register IDs are the offset of the register from the AP's base address.
 * For example DRW has ID 0x0C and IDR has ID 0xFC.
 *
 * Higher level debugger operations can involve many operations on a MEM-AP.
 * In order to improve efficiency by minimising the number of round trips to a
 * target, RDDI MEM-AP can combine multiple operations into a single call
 * using the MEM_AP_AccessBatch() function.  This function takes an array of
 * memory access operations that the implementation will perform as
 * efficiently as it can.  The following operations are defined:
 * - MEM_AP_BATCH_OP_WRITE:
 *   Write a block of memory
 * - MEM_AP_BATCH_OP_WRITE_RPT:
 *   Repeatedly write to a memory location.
 * - MEM_AP_BATCH_OP_READ:
 *   Read a block of memory
 * - MEM_AP_BATCH_OP_READ_RPT:
 *   Repeatedly read a memory location.
 * - MEM_AP_BATCH_OP_POLL_EQ:
 *   Repeatedly read a memory location until it matches a target value.  The
 *   last read value is returned to the caller.  The target value and mask
 *   (applied to the read value and target value) are set with
 *   MEM_AP_BATCH_OP_POLL_VALUE and MEM_AP_BATCH_OP_POLL_MASK.
 * - MEM_AP_BATCH_OP_POLL_NE:
 *   Repeatedly read a memory location until it does not match a target
 *   value. The last read value is returned to the caller.  The target value
 *   and mask (applied to the read value and target value) are set with
 *   MEM_AP_BATCH_OP_POLL_VALUE and MEM_AP_BATCH_OP_POLL_MASK.
 * - MEM_AP_BATCH_OP_CHECK_EQ / MEM_AP_BATCH_OP_CHECK_NE:
 *   Check the last value read from a poll operation using the current mask
 *   and value.  This allows for polling until an operation is complete, then
 *   setting a new mask / value to check error bits.
 * - MEM_AP_BATCH_OP_POLL_VALUE / MEM_AP_BATCH_OP_POLL_MASK
 *   Set the target value and mask for poll / check operations
 */


#ifndef RDDI_MEM_AP_H
#define RDDI_MEM_AP_H

// RDDI.h defines the dll exports etc.
#include "rddi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Access size for memory access commands
 */
typedef enum {
    MEM_AP_ACC_8,  //!< 8-bit access
    MEM_AP_ACC_16, //!< 16-bit access
    MEM_AP_ACC_32, //!< 32-bit access
    MEM_AP_ACC_64  //!< 64-bit access
} MEM_AP_ACC_SIZE;

/**
 * Register access RegID flags
 *
 * Encoding for register access RegID as register banks on MEM-APs can have
 * difference base offsets according to the Debug Interface version.
 */
/** Set to use the Control register bank base (ADIv6 - 0xD00) */
#define MEMAP_REG_CTRL                0x1000
/** Set to use the Banked Data register bank base (ADIv6 - 0xD10) */
#define MEMAP_REG_BANKED_DATA         0x2000
/** Set to use the ID register bank base (ADIv6 - 0xDF0) */
#define MEMAP_REG_ID                  0x4000
/** Use the Control register bank base with offset x */
#define MEMAP_REG_CTRL_BASE(x)        (MEMAP_REG_CTRL | (x & 0xF))
/** Use the Banked Data register bank base with offset x */
#define MEMAP_REG_BANKED_DATA_BASE(x) (MEMAP_REG_BANKED_DATA | (x & 0xF))
/** Use the ID register bank base with offset x */
#define MEMAP_REG_ID_BASE(x)          (MEMAP_REG_ID | (x & 0xF))

/*
 * Flags for memory access commands
 *
 * bits 6:0 implementation defined
 */
/** Set to override MEM-AP.CSW INCR bits */
#define MEM_AP_OVERRIDE_INCR (1 << 7)
/** Set to override MEM-AP.CSW PROT bits */
#define MEM_AP_OVERRIDE_PROT (1 << 8)
/** Set to override MEM-AP.CSW MODE bits */
#define MEM_AP_OVERRIDE_MODE (1 << 9)
/** Set to override MEM-AP.CSW TYPE bits */
#define MEM_AP_OVERRIDE_TYPE (1 << 10)
/** Set to override MEM-AP.CSW ERR bits */
#define MEM_AP_OVERRIDE_ERR  (1 << 11)
/** Offset of MEM-AP.CSW INCR value */
#define MEM_AP_INCR_SHIFT    12
/** Mask for MEM-AP.CSW INCR value */
#define MEM_AP_INCR          ( 0x3 << MEM_AP_INCR_SHIFT)
/** Offset of MEM-AP.CSW PROT value */
#define MEM_AP_PROT_SHIFT    14
/** Mask for MEM-AP.CSW PROT value */
#define MEM_AP_PROT          (0xFF << MEM_AP_PROT_SHIFT)
/** Offset of MEM-AP.CSW MODE value */
#define MEM_AP_MODE_SHIFT    22
/** Mask for MEM-AP.CSW MODE value */
#define MEM_AP_MODE          ( 0xF << MEM_AP_MODE_SHIFT)
/** Offset of MEM-AP.CSW TYPE value */
#define MEM_AP_TYPE_SHIFT    26
  /** Mask for MEM-AP.CSW TYPE value */
#define MEM_AP_TYPE          ( 0xF << MEM_AP_TYPE_SHIFT)
/** Offset of MEM-AP.CSW ERR value */
#define MEM_AP_ERR_SHIFT     30
/** Mask for MEM-AP.CSW ERR value */
#define MEM_AP_ERR           ( 0x3 << MEM_AP_ERR_SHIFT)

/** Override MEM-AP.CSW INCR */
#define MEM_AP_WITH_INCR(x)  (MEM_AP_OVERRIDE_INCR | (((x) << MEM_AP_INCR_SHIFT) & MEM_AP_INCR))
/** Override MEM-AP.CSW PROT */
#define MEM_AP_WITH_PROT(x)  (MEM_AP_OVERRIDE_PROT | (((x) << MEM_AP_PROT_SHIFT) & MEM_AP_PROT))
/** Override MEM-AP.CSW MODE */
#define MEM_AP_WITH_MODE(x)  (MEM_AP_OVERRIDE_MODE | (((x) << MEM_AP_MODE_SHIFT) & MEM_AP_MODE))
/** Override MEM-AP.CSW TYPE */
#define MEM_AP_WITH_TYPE(x)  (MEM_AP_OVERRIDE_TYPE | (((x) << MEM_AP_TYPE_SHIFT) & MEM_AP_TYPE))
/** Override MEM-AP.CSW ERR */
#define MEM_AP_WITH_ERR(x)   (MEM_AP_OVERRIDE_ERR | (((x) << MEM_AP_ERR_SHIFT) & MEM_AP_ERR))


/**
 * Operations in a batch
 */
typedef enum {
    MEM_AP_BATCH_OP_WRITE = 0,      //!< write operation
    MEM_AP_BATCH_OP_WRITE_RPT,      //!< repeat write operation
    MEM_AP_BATCH_OP_READ,           //!< read operation
    MEM_AP_BATCH_OP_READ_RPT,       //!< repeat read operation
    MEM_AP_BATCH_OP_POLL_MASK,      //!< set poll mask
    MEM_AP_BATCH_OP_POLL_VALUE,     //!< set poll value
    MEM_AP_BATCH_OP_POLL_EQ,        //!< poll until read data equals value (with mask)
    MEM_AP_BATCH_OP_POLL_NE,        //!< poll until read data does not equal value (with mask)
    MEM_AP_BATCH_OP_CHECK_EQ,       //!< check last read data equals value (with mask)
    MEM_AP_BATCH_OP_CHECK_NE        //!< check last read data does not equal value (with mask)
} MEM_AP_BATCH_OP_ACTION;

/**
 * Element of a batch operation
 */
typedef struct {
    uint8 op;                //!< Operation type: one of MEM_AP_BATCH_OP_ACTION
    uint8 indirect;          //!< Whether data is single value or pointer to values
    uint32 addr;             //!< Address to read/write
    MEM_AP_ACC_SIZE accSize; //!< Access size for operation
    unsigned flags;          //!< Flags controlling the behaviour of the access
    /**
     * @brief Size of operation
     *
     * Meaning depends on operation
     * - MEM_AP_BATCH_OP_WRITE: number of bytes to write
     * - MEM_AP_BATCH_OP_READ: number of bytes to read
     * - MEM_AP_BATCH_OP_WRITE_RPT: number of elements to write
     * - MEM_AP_BATCH_OP_READ_RPT: number of elements to read
     * - MEM_AP_BATCH_OP_POLL_EQ,MEM_AP_BATCH_OP_POLL_NE: retry count
     */
    unsigned count;
    union
    {
        uint32  data;        //!< data to write (indirect == 0)
        void*   pData;       //!< data to read/write (indirect == 1)
    };
} MEM_AP_OP;

/**
 * Connect to MEM-AP interface
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] clientInfo Client information string
 * @param[out] targetInfo Buffer to receive target information string
 * @param[in] targetInfoLen Size of targetInfo buffer
 * @return RDDI_SUCCESS on success, otherwise RDDI_xxxx on error
 */
RDDI int MEM_AP_Connect(RDDIHandle handle, const char* clientInfo, char* targetInfo, size_t targetInfoLen);

/**
 * Disconnect from MEM-AP interface
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @return RDDI_SUCCESS on success, otherwise RDDI_xxxx on error
 */
RDDI int MEM_AP_Disconnect(RDDIHandle handle);

/**
 * Get number of MEM-APs available on MEM-AP interface
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[out] apCount Receives number of MEM-APs available
 * @return RDDI_SUCCESS on success, otherwise RDDI_xxxx on error
 */
RDDI int MEM_AP_GetNumberOfAPs(RDDIHandle handle, int* apCount);

/**
 * Open a MEM-AP
 *
 * A MEM-AP must be opened before any register of memory access is performed
 * on that AP.  This allows the implementation to allocate any resources
 * required and enable access to the MEM-AP registers
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] apNumber The index of the MEM-AP to open
 * @return RDDI_SUCCESS on success, otherwise RDDI_xxxx on error
 */
RDDI int MEM_AP_Open(RDDIHandle handle, int apNumber);

/**
 * Close a MEM-AP
 *
 * The MEM-AP should be closed at the end of a session to allow the
 * implementation to disable access to the MEM-AP and release any resources
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] apNumber The index of the MEM-AP to close
 * @return RDDI_SUCCESS on success, otherwise RDDI_xxxx on error
 */
RDDI int MEM_AP_Close(RDDIHandle handle, int apNumber);

/**
 * Read a MEM-AP register
 *
 * The register ID is the offset of the register from the AP's base address.
 * For example DRW has ID 0x0C and IDR has ID 0xFC.
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] apNumber The index of the MEM-AP to access
 * @param[in] regID The ID of the register
 * @param[out] pValue Receives the register value
 * @return RDDI_SUCCESS on success, otherwise RDDI_xxxx on error
 */
RDDI int MEM_AP_ReadReg(RDDIHandle handle, int apNumber, int regID, uint32 *pValue);

/**
 * Write a MEM-AP register
 *
 * The register ID is the offset of the register from the AP's base address.
 * For example DRW has ID 0x0C and IDR has ID 0xFC.
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] apNumber The index of the MEM-AP to access
 * @param[in] regID The ID of the register
 * @param[in] value The register value
 * @return RDDI_SUCCESS on success, otherwise RDDI_xxxx on error
 */
RDDI int MEM_AP_WriteReg(RDDIHandle handle, int apNumber, int regID, uint32 value);

/**
 * Read a block of memory from a MEM-AP
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] apNumber The index of the MEM-AP to access
 * @param[in] addr The address to read from
 * @param[in] accSize The access size to use
 * @param[in] flags Flags controlling the behaviour of the access
 * @param[in] size The number of bytes to read
 * @param[out] buf Receives the read data
 * @return RDDI_SUCCESS on success, otherwise RDDI_xxxx on error
 */
RDDI int MEM_AP_Read(RDDIHandle handle, int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, void* buf);

/**
 * Repeatedly read an address on a MEM-AP
 *
 * This function repeatedly reads from the address without incrementing the AP
 * TAR.  The total number of bytes read is numberOfBytes(accSize) * repeatCount
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] apNumber The index of the MEM-AP to access
 * @param[in] addr The address to read from
 * @param[in] accSize The access size to use
 * @param[in] flags Flags controlling the behaviour of the access
 * @param[in] repeatCount The number of elements to read
 * @param[out] buf Receives the read data
 * @return RDDI_SUCCESS on success, otherwise RDDI_xxxx on error
 */
RDDI int MEM_AP_ReadRepeat(RDDIHandle handle, int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, void* buf);

/**
 * Write a block of memory to a MEM-AP
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] apNumber The index of the MEM-AP to access
 * @param[in] addr The address to write from
 * @param[in] accSize The access size to use
 * @param[in] flags Flags controlling the behaviour of the access
 * @param[in] size The number of bytes to write
 * @param[in] buf The data to write
 * @return RDDI_SUCCESS on success, otherwise RDDI_xxxx on error
 */
RDDI int MEM_AP_Write(RDDIHandle handle, int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned size, const void* buf);

/**
 * Repeatedly write an address on a MEM-AP
 *
 * This function repeatedly writes to the address without incrementing the AP
 * TAR.  The total number of bytes written is numberOfBytes(accSize) * repeatCount
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] apNumber The index of the MEM-AP to access
 * @param[in] addr The address to write to
 * @param[in] accSize The access size to use
 * @param[in] flags Flags controlling the behaviour of the access
 * @param[in] repeatCount The number of elements to write
 * @param[out] buf The data to write
 * @return RDDI_SUCCESS on success, otherwise RDDI_xxxx on error
 */
RDDI int MEM_AP_WriteRepeat(RDDIHandle handle, int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, const void* buf);

/**
 * Repeatedly write a value to an address on a MEM-AP
 *
 * This function repeatedly writes a value to the address without incrementing the AP
 * TAR.  The total number of bytes written is numberOfBytes(accSize) * repeatCount
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] apNumber The index of the MEM-AP to access
 * @param[in] addr The address to write to
 * @param[in] accSize The access size to use
 * @param[in] flags Flags controlling the behaviour of the access
 * @param[in] repeatCount The number of elements to write
 * @param[out] val The value to write
 * @return RDDI_SUCCESS on success, otherwise RDDI_xxxx on error
 */
RDDI int MEM_AP_WriteValueRepeat(RDDIHandle handle, int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, uint32 val);

/**
 * Fill a block of memory on a MEM-AP
 *
 * This function fills memory with the given pattern.  The
 * numberOfBytes(accSize) least significant bytes of pattern are used to fill
 * the given address range.  The total number of bytes written is
 * numberOfBytes(accSize) * repeatCount
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] apNumber The index of the MEM-AP to access
 * @param[in] addr The address to read from
 * @param[in] accSize The access size to use
 * @param[in] flags Flags controlling the behaviour of the access
 * @param[in] repeatCount The number of times to write the pattern
 * @param[in] pattern The value to write
 * @return RDDI_SUCCESS on success, otherwise RDDI_xxxx on error
 */
RDDI int MEM_AP_Fill(RDDIHandle handle, int apNumber, uint64 addr, MEM_AP_ACC_SIZE accSize, unsigned flags, unsigned repeatCount, uint64 pattern);

/**
 * Run a batch of operations on a MEM-AP
 *
 * opsCompleted returns how many operations were completed.  A sequence may
 * terminate early if a poll fails to read the expected value or a check
 * operation fails.
 *
 * @param[in] handle Session handle previously created by RDDI_Open()
 * @param[in] apNumber The index of the MEM-AP to access
 * @param[in] baseAddress The base address for accesses
 * @param[in] ops The operations to perform
 * @param[in] numOps The number of operations to perform
 * @param[out] opsCompleted The number of operations completed
 * @return RDDI_SUCCESS on success, otherwise RDDI_xxxx on error
 *
 * <b>Example</b> - To halt a V7M core, polling for the halt to complete:
 *
 * \dontinclude mem_ap.c
 * \skip v7m_halt_core
 * \until }
 * \until }
 */
RDDI int MEM_AP_AccessBatch(RDDIHandle handle, int apNumber, uint64 baseAddress, MEM_AP_OP* ops, unsigned numOps, unsigned* opsCompleted);

#ifdef __cplusplus
}
#endif

#endif // RDDI_MEM_AP_H

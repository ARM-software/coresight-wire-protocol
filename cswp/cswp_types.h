// cswp_types.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/**
 * @file cswp_types.h
 * @brief Type definitions for CSWP
 */

#ifndef CSWP_TYPES_H
#define CSWP_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdlib.h>

/**
 * Result codes for CSWP calls
 */
typedef enum
{
    CSWP_SUCCESS                = 0x0000, /**< Successful operation */
    CSWP_FAILED                 = 0x0001, /**< Other error */
    CSWP_CANCELLED              = 0x0002, /**< Not executed due to previous failure */
    CSWP_NOT_INITIALIZED        = 0x0003, /**< Not initialized */
    CSWP_BUFFER_FULL            = 0x0010, /**< Insufficient space in CSWP_BUFFER when encoding */
    CSWP_BUFFER_EMPTY           = 0x0011, /**< Insufficient data left in CSWP_BUFFER when decoding */
    CSWP_OUTPUT_BUFFER_OVERFLOW = 0x0012, /**< Insufficient space in output buffer when decoding */
    CSWP_COMMS                  = 0x0020, /**< Communication error */
    CSWP_INCOMPATIBLE           = 0x0021, /**< The server is not compatible with the client */
    CSWP_TIMEOUT                = 0x0022, /**< A timeout occurred executing a command */
    CSWP_UNSUPPORTED            = 0x0023, /**< Command unsupported */
    CSWP_DEVICE_UNSUPPORTED     = 0x0024, /**< Unsupported device */
    CSWP_INVALID_DEVICE         = 0x0025, /**< Invalid device ID */
    CSWP_BAD_ARGS               = 0x0026, /**< Bad arguments to command */
    CSWP_NOT_PERMITTED          = 0x0027, /**< Operation not permitted */
    CSWP_REG_FAILED             = 0x0200, /**< Register access failed */
    CSWP_REG_PARTIAL            = 0x0201, /**< Attempt to access part of a multiple element register */
    CSWP_MEM_FAILED             = 0x0300, /**< Memory access failed */
    CSWP_MEM_INVALID_ADDRESS    = 0x0301, /**< Invalid address for memory access */
    CSWP_MEM_BAD_ACCESS_SIZE    = 0x0302, /**< Invalid access size for memory access */
    CSWP_MEM_POLL_NO_MATCH      = 0x0303, /**< Poll did not match */
} cswp_result_t;

/**
 * Buffer for CSWP messages
 */
typedef struct
{
    size_t size;   /**< Max size of buffer */
    size_t used;   /**< Number of bytes used in buffer */
    size_t pos;    /**< Read position */
    uint8_t buf[]; /**< Data */
} CSWP_BUFFER;

/**
 * Use a 64-bit integer to hold varint
 */
typedef uint64_t varint_t;

/**
 * CSWP protocol versions
 */
typedef enum
{
    CSWP_PROTOCOL_v1 = 1,
} cswp_protocol_ver_t;

/**
 * Command identifiers for CSWP
 */
typedef enum
{
    CSWP_NONE                    = 0,

    /* connection commands */
    CSWP_INIT                    = 0x00000001, /**< Initialize CSWP session */
    CSWP_TERM                    = 0x00000002, /**< Terminate CSWP session */
    CSWP_CLIENT_INFO             = 0x00000005, /**< Information from a client */
    CSWP_SET_DEVICES             = 0x00000010, /**< Set device list */
    CSWP_GET_DEVICES             = 0x00000011, /**< Get device list */
    CSWP_GET_SYSTEM_DESCRIPTION  = 0x00000012, /**< Get system description file (SDF format) */
    /* device commands */
    CSWP_DEVICE_OPEN             = 0x00000100, /**< Open device */
    CSWP_DEVICE_CLOSE            = 0x00000101, /**< Close device */
    CSWP_SET_CONFIG              = 0x00000102, /**< Set configuration item */
    CSWP_GET_CONFIG              = 0x00000103, /**< Set configuration item */
    CSWP_GET_DEVICE_CAPABILITIES = 0x00000104, /**< Get device capabilities */
    /* register commands */
    CSWP_REG_LIST                = 0x00000200, /**< Get available registers */
    CSWP_REG_READ                = 0x00000201, /**< Read registers */
    CSWP_REG_WRITE               = 0x00000202, /**< Write registers */
    /* memory commands */
    CSWP_MEM_READ                = 0x00000300, /**< Read memory */
    CSWP_MEM_WRITE               = 0x00000301, /**< Write memory */
    CSWP_MEM_POLL                = 0x00000302, /**< Poll memory location */
    /* async commands */
    CSWP_ASYNC_MESSAGE           = 0x00001000, /**< Error/information message */
    /* implementation specific commands */
    CSWP_IMPLEMENTATION_DEFINED_BEGIN = 0x8000, /**< First implementation defined command */
    CSWP_IMPLEMENTATION_DEFINED_END   = 0xFFFF, /**< Last implementation defined command */
} cswp_commands_t;

#define CSWP_PROTOCOL_VERSION 1 /**< Protocol version 1 */

/**
 * Access sizes for memory access commands
 */
typedef enum
{
    CSWP_ACCESS_SIZE_DEF = 0,
    CSWP_ACCESS_SIZE_8   = 1,
    CSWP_ACCESS_SIZE_16  = 2,
    CSWP_ACCESS_SIZE_32  = 3,
    CSWP_ACCESS_SIZE_64  = 4,
} cswp_access_size_t;

/**
 * Log levels for debug within command processor
 */
typedef enum
{
    CSWP_LOG_ERROR = 0,
    CSWP_LOG_WARN  = 1,
    CSWP_LOG_INFO  = 2,
    CSWP_LOG_DEBUG = 3,
} cswp_log_level_t;

/**
 * Server capabilities
 */
typedef enum
{
    CSWP_CAP_REG = 0x1, /**< Register commands supported */
    CSWP_CAP_MEM = 0x2, /**< Memory commands supported */
    CSWP_CAP_MEM_POLL = 0x200 /**< Memory poll command supported */
} cswp_cap_t;

/**
 * Register information
 */
typedef struct
{
    /**
     * Register ID
     *
     * Device assigned register ID.  IDs do not have to be consecutive,
     * allowing an implementation to encode access information into the ID
     */
    unsigned id;

    /**
     * Register name
     *
     * Unique name used to identify the register.  Should be a valid C
     * identifier.
     */
    const char* name;

    /**
     * Size of the register in 32-bit units
     *
     * A 32-bit register will have size 1, a 64-bit register size 2, etc
     */
    unsigned size;

    /**
     * Display name of the register
     */
    const char* displayName;

    /**
     * Description of the register
     */
    const char* description;
} cswp_register_info_t;

/**
 * Common memory access flags
 */
#define CSWP_MEM_NO_ADDR_INC     (1 << 0) /**< Flag to disable auto address increment for a MEM-AP */
#define CSWP_MEM_POLL_MATCH_NE   (1 << 1) /**< Flag Match Not Equal for poll operation */
#define CSWP_MEM_POLL_CHECK_LAST (1 << 2) /**< Flag Check last for poll operation */

/**
 * MEM-AP memory access flags
 */
#define CSWP_MEMAP_OVERRIDE_INCR (1 << 8)                        /**< Flag to override auto address increment default behaviour */
#define CSWP_MEMAP_OVERRIDE_PROT (1 << 9)                        /**< Flag to override default HPROT */
#define CSWP_MEMAP_OVERRIDE_MODE (1 << 10)                       /**< Flag to override access mode */
#define CSWP_MEMAP_OVERRIDE_TYPE (1 << 11)                       /**< Flag to override type */
#define CSWP_MEMAP_OVERRIDE_ERR  (1 << 12)                       /**< Flag to override error */
#define CSWP_MEMAP_INCR_SHIFT    13                              /**< Shift for MEM-AP auto increment */
#define CSWP_MEMAP_INCR          ( 0x3 << CSWP_MEMAP_INCR_SHIFT) /**< Mask for MEM-AP auto increment field */
#define CSWP_MEMAP_PROT_SHIFT    15                              /**< Shift for HPROT */
#define CSWP_MEMAP_PROT          (0x7F << CSWP_MEMAP_PROT_SHIFT) /**< Mask for HPROT field */
#define CSWP_MEMAP_MODE_SHIFT    22                              /**< Shift for mode */
#define CSWP_MEMAP_MODE          ( 0xF << CSWP_MEMAP_MODE_SHIFT) /**< Mask for mode field */
#define CSWP_MEMAP_TYPE_SHIFT    26                              /**< Shift for type */
#define CSWP_MEMAP_TYPE          ( 0xF << CSWP_MEMAP_TYPE_SHIFT) /**< Mask for type field */
#define CSWP_MEMAP_ERR_SHIFT     30                              /**< Shift for error */
#define CSWP_MEMAP_ERR           ( 0x3 << CSWP_MEMAP_ERR_SHIFT)  /**< Mask for error field */

/** Construct data to update auto increment value */
#define CSWP_MEMAP_WITH_INCR(x)  (CSWP_MEMAP_OVERRIDE_INCR | (((x) << CSWP_MEMAP_INCR_SHIFT) & CSWP_MEMAP_INCR))
/** Construct data to update HPROT value */
#define CSWP_MEMAP_WITH_PROT(x)  (CSWP_MEMAP_OVERRIDE_PROT | (((x) << CSWP_MEMAP_PROT_SHIFT) & CSWP_MEMAP_PROT))
/** Construct data to update mode value */
#define CSWP_MEMAP_WITH_MODE(x)  (CSWP_MEMAP_OVERRIDE_MODE | (((x) << CSWP_MEMAP_MODE_SHIFT) & CSWP_MEMAP_MODE))
/** Construct data to update type value */
#define CSWP_MEMAP_WITH_TYPE(x)  (CSWP_MEMAP_OVERRIDE_TYPE | (((x) << CSWP_MEMAP_TYPE_SHIFT) & CSWP_MEMAP_TYPE))
/** Construct data to update error value */
#define CSWP_MEMAP_WITH_ERR(x)   (CSWP_MEMAP_OVERRIDE_ERR | (((x) << CSWP_MEMAP_ERR_SHIFT) & CSWP_MEMAP_ERR))

#ifdef __cplusplus
}
#endif

#endif /* CSWP_TYPES_H */

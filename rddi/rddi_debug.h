/*
 * rddi_debug.h - Debug header for RDDI
 * Copyright (C) 2007,2017 Arm Limited. All rights reserved.
 */

/**  \file

    \brief  This file describes the Debug functionality of the RDDI module
    along with the necessary definitions.

*/

#ifndef RDDI_DEBUG_H
#define RDDI_DEBUG_H

#include "rddi.h"
#include "rddi_event.h"

#ifndef DOXYGEN_SHOULD_SKIP_THIS
#define RDDI_NO_ASYNC_EVENTS                    0x0007
#define RDDI_ITEMNOTSUP                         0x000F
#define RDDI_REGACCESS                          0x0010
/*      REMOVED                                 0x0011     */
#define RDDI_BUSERR                             0x0012
#define RDDI_TIMEOUT                            0x0013
#define RDDI_VERIFY                             0x0014
#define RDDI_BADACC                             0x0015
#define RDDI_RWFAIL                             0x0016
#define RDDI_MEMACCESS                          0x0017
#define RDDI_BADMODE                            0x0018
#define RDDI_BADPC                              0x0019
#define RDDI_BADINSTR                           0x001A
#define RDDI_BADSTACK                           0x001B
#define RDDI_NOBREAKS                           0x001C
#define RDDI_BREAKFAIL                          0x001D
#define RDDI_CANNOTSTOP                         0x001E
#define RDDI_EXETIMEOUT                         0x001F
#define RDDI_INRESET                            0x0020
#define RDDI_RUNNING                            0x0021
#define RDDI_NOTRUNNING                         0x0022
#define RDDI_CORESTATE_DEBUGPRIV                0x0023
/*      REMOVED                                 0x0024     */
#define RDDI_SWBRK_MEMERR                       0x0025
#define RDDI_BRKCLR_BADID                       0x0026
#define RDDI_BRKCLR_BADADDR                     0x0027
#define RDDI_SWBRK_NOHWRSRC                     0x0028
#define RDDI_PROCBRK_NOHWRSRC                   0x0029
#define RDDI_SWBRK_DEBUGPRIV                    0x002A
#define RDDI_HWBRK_NORESRC                      0x002B
#define RDDI_HWBRK_HWFAIL                       0x002C
#define RDDI_PROCBRK_HWFAIL                     0x002D
#define RDDI_DEVUNKNOWN                         0x002E
#define RDDI_DEVINUSE                           0x002F
#define RDDI_NOCONN                             0x0030
#define RDDI_NODRIVER                           0x0031
#define RDDI_COMMS                              0x0032
#define RDDI_BADDLL                             0x0033
#define RDDI_STATEFAIL                          0x0034
#define RDDI_NOREMOTE                           0x0035
#define RDDI_ENDIAN                             0x0036
#define RDDI_HARDWAREINITFAIL                   0x0037
#define RDDI_DEVBUSY                            0x0038
#define RDDI_NOINIT                             0x0039
#define RDDI_LOSTCONN                           0x003A
#define RDDI_NOVCC                              0x003B
#define RDDI_CMDUNKNOWN                         0x003C
/*      REMOVED                                 0x003D     */
#define RDDI_CMDUNSUPP                          0x003E
#define RDDI_TARGFAULT                          0x003F
#define RDDI_TARGSTATE                          0x0040
#define RDDI_NORESPONSE                         0x0041
/*      REMOVED                                 0x0042     */
/*      MOVED TO rddi.h                         0x0043     */
#define RDDI_INCOMP_RVMSG                       0x0044
#define RDDI_INCOMP_CLIENT                      0x0045
#define RDDI_DEBUGPRIV                          0x0046
#define RDDI_WRONGIR                            0x0047
#define RDDI_WRONGDEV                           0x0048
#define RDDI_NOJTAG                             0x0049
#define RDDI_OVERWRITE                          0x004A
#define RDDI_EMUTIMEOUT                         0x004B
#define RDDI_CHANOVERFLOW                       0x004C
#define RDDI_BADCHANNELID                       0x004D
#define RDDI_NO_VEH_INIT                        0x004E
#define RDDI_CMD_FAILED                         0x004F
#define RDDI_QUEUE_FULL                         0x0050
#define RDDI_QUEUE_EMPTY                        0x0051
#define RDDI_UNKNOWN_MEMORY                     0x0052
#define RDDI_STEPRUN                            0x0053
#define RDDI_NOCOREPOWER                        0x0054
#define RDDI_COREOSLOCK                         0x0055
#define RDDI_MONITOR_DEBUG                      0x0056
#define RDDI_NODEBUGPOWER                       0x0057
#define RDDI_UNKNOWN_EXEC                       0x0058
/*      REMOVED                                 0x0059     */
#define RDDI_UNKNOWN                            0x005A
#define RDDI_UNKNOWN_GENERIC                    0x005B
#define RDDI_UNKNOWN_EMU                        0x005C
#define RDDI_UNKNOWN_SIM                        0x005D
#define RDDI_UNKNOWN_RTOS                       0x005E
#define RDDI_FUTURE                             0x005F
#define RDDI_UNKNOWN_INIT                       0x0060
#define RDDI_ME_LINUX                           0x0061
#define RDDI_ME_NOT_FOUND                       0x0062
#define RDDI_ME_NOT_PRESENT                     0x0063
#define RDDI_ME_NOT_STARTED                     0x0064
#define RDDI_NO_CONFIG_FILE                     0x0065
/*      RDDI_NO_CONFIG                          0x0066 defined in rddi.h  */
#define RDDI_NO_CS_ASSOCIATIONS                 0x0067
#define RDDI_UNKNOWN_CONFIG                     0x0068
#define RDDI_UNKNOWN_BREAK                      0x0069
/*      REMOVED                                 0x006A   */
/*      REMOVED                                 0x006B   */
#define RDDI_UNKNOWN_CAP                        0x006C
#define RDDI_STEP_ABORT                         0x006D
/*      RDDI_EXTERNAL_CMD_FAILED                0x006E defined in rddi.h  */
#define RDDI_CALLBACK_EXISTS                    0x006F
/*      RDDI_LICENSE_FAILED                     0x0070 defined in rddi.h  */

#define RDDI_SESSION_PAUSED                     0x0071
#define RDDI_COREOSDLK                          0x0072
#define RDDI_AUTOTUNE_FAILURE                   0x0073

/* CADI-specific errors */
#define RDDI_CADI_INFO                          0x0200
#define RDDI_CADI_INIT                          0x0202
#define RDDI_CADI_TARGET_INFO                   0x0203
#define RDDI_CADI_TARGET_CONNECT                0x0204
#define RDDI_CADI_TARGET_EXEC                   0x0205
#define RDDI_CADI_TARGET_FEATURES               0x0206
#define RDDI_CADI_TARGET_REG_GROUPS             0x0207
#define RDDI_CADI_TARGET_REG_UNSUP              0x0208
#define RDDI_CADI_TARGET_MEM_REGIONS            0x0209
#define RDDI_CADI_CALLBACK_FAIL                 0x020A
#define RDDI_CADI_BAD_BREAK_ID                  0x020B
#define RDDI_CADI_MODEL_MISMATCH                0x020C

#endif

/*! The number of chars which identify a Processor Break */
#define PROC_BREAK_ID_LEN   4
/*! The type used for a Processor break ID. Note that the ID
    is just a number of characters without a final '\\0' */
typedef char ProcBreakID[PROC_BREAK_ID_LEN];

/**
 * \enum RDDI_MEM_RULE
 * Provides the allowed values for the rule parameter in the
 * memory access calls e.g. Debug_MemWrite()
 */
typedef enum
{
    RDDI_MRUL_TYPE_MSK = 0x000F,                /*!< Mask for type of access  */
     RDDI_MRUL_NORMAL = 0x0000,                 /*!<  normal memory           */
     RDDI_MRUL_INTERNAL=0x0006,                 /*!<  flag to specify we are reading RAMs */
    RDDI_MRUL_MODE_MSK = 0x00F0,                /*!< Mask for mode of access  */
     RDDI_MEMMODE_CURRENT = 0x0000,             /*!<  use current mode        */
     RDDI_MEMMODE_USER = 0x0010,                /*!<  use non-privileged mode */
     RDDI_MEMMODE_PRIVILEGED = 0x0020,          /*!<  use privileged mode     */
    RDDI_MRUL_CSDAP_MSK = 0x0300,               /*!< Mask for Coresight flags */
     RDDI_CSDAP_PAGESPEC = 0x0100,              /*!<  memory page value used to
                                                specify coresight parameters  */
     RDDI_CSDAP_USEHPROT = 0x0200,              /*!< Indicates that the page
                                                    parameter contains the
                                                    HProt / AP.CSW MSByte value
                                                    to use the page value for
                                                    AHB access                */
    RDDI_MEMSPACE_MSK                 =0x7000,  /*!< TrustZone world for access */
     RDDI_MEMSPACE_CURRENT            =0x0000,  /*!< use current world        */
     RDDI_MEMSPACE_SECURE             =0x1000,  /*!< use secure world         */
     RDDI_MEMSPACE_NON_SECURE         =0x2000,  /*!< use non-secure world     */
     RDDI_MEMSPACE_HYPERVISOR         =0x3000,  /*!< use hypervisor mode (non-secure world) */
     RDDI_MEMSPACE_MONITOR            =0x4000,  /*!< use monitor/EL3 mode secure world */
     RDDI_MEMSPACE_HYPERVISOR_SECURE  =0x5000,  /*!< use hypervisor mode (secure world) */
    RDDI_MEM_MMU_MSK            =0x8000,        /*!< memory management access */
     RDDI_MEM_MMU_VIRTUAL       =0x0000,        /*!< MMU not disabled, left as is */
     RDDI_MEM_MMU_PHYSICAL      =0x8000,        /*!< MMU disabled: used in conjunction with RDDI_MEM_PHYS_DISABLE_STAGES_MSK */
    RDDI_MEM_PHYS_DISABLE_STAGES_MSK  = 0x0C00, /*!< Which MMU stages to disable for physical access */
     RDDI_MEM_PHYS_DISABLE_STAGES_ALL = 0x0000, /*!< Disable all stages */
     RDDI_MEM_PHYS_DISABLE_STAGES_1   = 0x0400, /*!< Disable 1st stage */
     RDDI_MEM_PHYS_DISABLE_STAGES_2   = 0x0800, /*!< Disable stages 1-2 */
     RDDI_MEM_PHYS_DISABLE_STAGES_3   = 0x0C00, /*!< Disable stages 1-3 */
} RDDI_MEM_RULE;

/* Deprecated flags */
enum
{
    RDDI_MEMMODE_TRUST_ZONE_MSK = RDDI_MEMSPACE_MSK,               /*!< Mask for TrustZone world */
    RDDI_MEMMODE_TRUST_ZONE_CURRENT = RDDI_MEMSPACE_CURRENT,       /*!<  use current world       */
    RDDI_MEMMODE_TRUST_ZONE_SECURE = RDDI_MEMSPACE_SECURE,         /*!<  use secure world        */
    RDDI_MEMMODE_TRUST_ZONE_NON_SECURE = RDDI_MEMSPACE_NON_SECURE  /*!<  use non-secure world  */
};


/**
 * \enum RDDI_ACC_SIZE
 * Provides the allowed values for the size parameter in the
 * memory access calls e.g. Debug_MemWrite()
 */
typedef enum
{
    RDDI_ACC_DEF = 0,       /*!< use default access size */
    RDDI_ACC_BYTE = 1,      /*!< use 8-bit access size   */
    RDDI_ACC_HALF = 2,      /*!< use 16-bit access size  */
    RDDI_ACC_WORD = 4,      /*!< use 32-bit access size  */
    RDDI_ACC_WORD40 = 5,    /*!< use 40-bit access size  */
    RDDI_ACC_WORD64 = 8     /*!< use 64-bit access size  */
} RDDI_ACC_SIZE;

/**
 * \enum RDDI_BRK_MEMSPACE
 * Flag used to specify the memory space for a call that sets a software breakpoint
 * e.g. Debug_SetSWBreak()
 */
typedef enum
{
    RDDI_BRK_MEMSPACE_MONITOR    =0x20,   /*!< address is in EL3/Monitor secure world (TrustZone) */
    RDDI_BRK_MEMSPACE_SECURE     =0x40,   /*!< address is in secure world (TrustZone) */
    RDDI_BRK_MEMSPACE_NON_SECURE =0x80,   /*!< address is in non-secure world (TrustZone) */
    RDDI_BRK_MEMSPACE_HYPERVISOR =0xC0,   /*!< address is in hypervisor world */
    RDDI_BRK_MEMSPACE_MASK       =0xE0    /*!< Address world mask       */
} RDDI_BRK_MEMSPACE;


/**
 * \enum RDDI_BRK_RULE
 * Flag used to specify options for call that sets a software breakpoint
 * e.g. Debug_SetSWBreak()
 */
typedef enum
{
    RDDI_BRUL_TYPE_MSK = 0x07,                                        /*!< Mask of type of SW break  */
    RDDI_BRUL_STD = 0x00,                                             /*!< standard break for processor/vehicle */
    RDDI_BRUL_ALT = 0x01,                                             /*!< alternate break (e.g. Thumb) */
    RDDI_BRUL_JAVA = 0x02,                                            /*!< Java break */
    RDDI_BRUL_A64 = 0x03,                                             /*!< AArch64 break */
    RDDI_BRUL_SHARED = 0x08,                                          /*!< Breakpoint is shared with other cores in cluster */
    /* world flags are deprecated */
    RDDI_BRUL_TRUST_ZONE_MSK = RDDI_BRK_MEMSPACE_MASK,                /*!< Address world mask         */
    RDDI_BRUL_TRUST_ZONE_SECURE = RDDI_BRK_MEMSPACE_SECURE,           /*!< address is in secure world (TrustZone) */
    RDDI_BRUL_TRUST_ZONE_NON_SECURE = RDDI_BRK_MEMSPACE_NON_SECURE    /*!< address is in non-secure world (TrustZone) */
} RDDI_BRK_RULE;

/**
 * \enum RDDI_HWBRK_TYPE
 * Flag used to describe the type of breakpoint for call that sets a
 * hardware breakpoint e.g. Debug_SetHWBreak()
 */
typedef enum
{
    RDDI_HWBRK_EXEC = 0,     /*!< break on instruction  */
    RDDI_HWBRK_READ = 1,     /*!< break on read  */
    RDDI_HWBRK_WRITE = 2,    /*!< break on write  */
    RDDI_HWBRK_ACCESS = 3    /*!< break on access (read or write)  */
} RDDI_HWBRK_TYPE;


/**
 * \enum RDDI_HWBRK_ATYPE
 * Flag used to indicate the sort of address match to use in a call that sets a
 * hardware breakpoint e.g. Debug_SetHWBreak()
 */
typedef enum
{
    RDDI_HWBRK_ANONE = 0,      /*!< no address specified */
    RDDI_HWBRK_ASINGLE = 1,    /*!< single address  */
    RDDI_HWBRK_ARANGE = 2,     /*!< address range  */
    RDDI_HWBRK_AMASK = 3       /*!< address + mask (~0 is all)  */
} RDDI_HWBRK_ATYPE;


/**
 * \enum RDDI_HWBRK_DTYPE
 * Flag used to indicate the sort of data match to use in a call that sets a
 * hardware breakpoint e.g. Debug_SetHWBreak()
 */
typedef enum
{
    RDDI_HWBRK_DNONE = 0,      /*!< no data specified */
    RDDI_HWBRK_DSINGLE = 1,    /*!< single data value  */
    RDDI_HWBRK_DRANGE = 2,     /*!< data range  */
    RDDI_HWBRK_DMASK = 3       /*!< data + mask (~0 is all)  */
} RDDI_HWBRK_DTYPE;


/**
 * \enum RDDI_HWBRK_FLAGS
 * Flag used to specify the breakpoint behaviour to use in a call that sets a
 * hardware breakpoint e.g. Debug_SetHWBreak()
 */
typedef enum
{
    RDDI_HWBFLG_COUNT = 0x0001,       /*!< pass count valid  */
    RDDI_HWBFLG_AFTER = 0x0002,       /*!< then_handle is from-break  */
    RDDI_HWBFLG_INTER = 0x0004,       /*!< interbreak flags affect us  */
    RDDI_HWBFLG_NOT_ADDR = 0x0010,    /*!< *not range* for address  */
    RDDI_HWBFLG_NOT_DATA = 0x0020,    /*!< *not range* for data match  */
    RDDI_HWBFLG_NOT_THEN = 0x0040,    /*!< then is then-disable  */
    RDDI_HWBFLG_AND = 0x0080,         /*!< then is *and* with another  */
    RDDI_HWBFLG_COUNTCLR = 0x0100,    /*!< auto-clear counter before start  */
    RDDI_HWBFLG_THENDIS = 0x0200,     /*!< auto-disable for then-enable  */
    RDDI_HWBFLG_ISDIS = 0x8000        /*!< is disabled now for then-ena  */
} RDDI_HWBRK_FLAGS;

/**
 * \enum RDDI_BRK_INPUT_FLAGS
 * Breakpoint input flags.
 */
typedef enum
{
    RDDI_BRK_INPUT_EXT_MASK = 0x00000003,     /*!< Mask for external input    */
     RDDI_BRK_INPUT_EXT_IGNORE = 0x00000000,  /*!< ignore external input trigger */
     RDDI_BRK_INPUT_EXT_LOW = 0x00000001,     /*!< match against low external
                                                               input trigger  */
     RDDI_BRK_INPUT_EXT_HIGH = 0x00000002,    /*!< match against high external
                                                               input trigger  */
    RDDI_BRK_INPUT_PRIV_MASK = 0x0000080C,    /*!< Mask for privilege level   */
     RDDI_BRK_INPUT_PRIV_ALL = 0x00000000,    /*!< match against all privilege
                                                                      levels  */
     RDDI_BRK_INPUT_PRIV_SUP = 0x00000004,    /*!< match against
                                                    privileged accesses only  */
     RDDI_BRK_INPUT_PRIV_USER = 0x00000008,   /*!< match against non-
                                                    privileged accesses only  */
     RDDI_BRK_INPUT_PRIV_HYP = 0x0000000C,    /*!< match against hypervisor
                                                    privilege accesses only  */
     RDDI_BRK_INPUT_PRIV_MON = 0x00000800,    /*!< match against monitor
                                                    privilege accesses only  */
    RDDI_BRK_INPUT_TRUST_ZONE_MASK = 0x00000030, /*!< Mask for TrustZone world*/
     RDDI_BRK_INPUT_TRUST_ZONE_BOTH = 0x00000000,/*!< Either secure or non secure */
     RDDI_BRK_INPUT_TRUST_ZONE_NON_SEC = 0x00000010,  /*!<  Non secure only   */
     RDDI_BRK_INPUT_TRUST_ZONE_SEC = 0x00000020,      /*!<  Secure only       */
    RDDI_BRK_INPUT_SIZE_MASK = 0x000F0000,    /*!< Mask for access size       */
     RDDI_BRK_INPUT_SIZE_DEFAULT = 0x00000000,/*!< match against a default set
                                                             of access sizes  */
     RDDI_BRK_INPUT_SIZE_8 = 0x00010000,   /*!< match against 8-bit accesses  */
     RDDI_BRK_INPUT_SIZE_16 = 0x00020000,  /*!< match against 16-bit accesses */
     RDDI_BRK_INPUT_SIZE_32 = 0x00040000,  /*!< match against 32-bit accesses */
     RDDI_BRK_INPUT_SIZE_64 = 0x00080000,   /*!< match against 64-bit accesses */
    RDDI_BRK_INPUT_LINK_MASK = 0x00000100,    /*!< Mask for linked breakpoint */
     RDDI_BRK_INPUT_LINK_TARGET = 0x00000100, /*!< Breakpoint is linked to */
    RDDI_BRK_INPUT_HYP_MASK = 0x00000400,     /*!< Mask for hypervisor enable */
     RDDI_BRK_INPUT_HYP_ENABLE = 0x00000400,  /*!< match in hypervisor mode */
    RDDI_BRK_INPUT_MATCH_INV         =   0x00000040, /*!< Match any address other than the specified one */
    RDDI_BRK_INPUT_MATCH_CONTEXT_ID  =   0x00000080, /*!< Match context ID */
    RDDI_BRK_INPUT_MATCH_VMID        =   0x00000200, /*!< Match against a given virtual machine ID */
    RDDI_BRK_INPUT_MATCH_MASK        =   0x000002C0

} RDDI_BRK_INPUT_FLAGS;

/**
 * \enum RDDI_CODESEQ_FLAGS
 *  Flag describing the valid values for the type parameter when setting up a
 *  code sequence to run with Debug_RegCodeSequence()
 */
typedef enum
{
    RDDI_CODESEQ_OPMSK = 0x03,     /*!< Mask used to set read/write status    */
     RDDI_CODESEQ_READ = 0x01,     /*!< Set if the code sequence causes data
                                                  to be read from the target  */
     RDDI_CODESEQ_WRITE = 0x02,    /*!< Set if the code sequence causes data
                                                  to be written to the target */
    RDDI_CODESEQ_TYPEMSK = 0x1C,   /*!< Mask used to set type classification  */
     RDDI_CODESEQ_UNKNOWN = 0x04,  /*!< Unclassified type                     */
     RDDI_CODESEQ_COPRO = 0x08,    /*!< Indicates coprocessor register access */
     RDDI_CODESEQ_COPROMEM = 0x0C, /*!< Indicates coprocessor memory access   */
     RDDI_CODESEQ_MEMORY = 0x10,   /*!< Indicates special memory access       */
     RDDI_CODESEQ_MONITOR = 0x14   /*!< Indicates monitor expansion           */
} RDDI_CODESEQ_FLAGS;


/**
 * \enum RDDI_ARM_ISET
 * Specifies valid instruction sets that can be used when setting up a
 * code sequence, to run with Debug_RegCodeSequence()
 */
typedef enum
{
    RDDI_ARM_ISET_ARM = 0,       /*!< Arm instruction set   */
    RDDI_ARM_ISET_THUMB = 1,     /*!< THUMB instruction set */
    RDDI_ARM_ISET_JAVA = 2       /*!< JAVA bytecode         */
} RDDI_ARM_ISET;


/**
 * \enum RDDI_ARM_IFLAGS
    Specifies particular processor conditions that a code sequence
    requires. Formed from three independent flag sets:
            \li Core mode flags.
            \li IRQ flags.
            \li FIQ flags.
    Used with Debug_RegCodeSequence()
 */
typedef enum
{
    RDDI_ARM_IMODE_MSK = 0x0008,   /*!< Core Mode Flag Mask                   */
     RDDI_ARM_IMODE_ANY = 0x0000,     /*!< Any mode                           */
     RDDI_ARM_IMODE_ANYPRIV = 0x0001, /*!< Any privileged mode                */
     RDDI_ARM_IMODE_USR = 0x0002,     /*!< USR mode                           */
     RDDI_ARM_IMODE_FIQ = 0x0003,     /*!< FIQ mode                           */
     RDDI_ARM_IMODE_IRQ = 0x0004,     /*!< IRQ mode                           */
     RDDI_ARM_IMODE_SVC = 0x0005,     /*!< SVC mode                           */
     RDDI_ARM_IMODE_ABT = 0x0006,     /*!< ABT mode                           */
     RDDI_ARM_IMODE_UND = 0x0007,     /*!< UND mode                           */
     RDDI_ARM_IMODE_SYS = 0x0008,     /*!< SYS mode                           */
     RDDI_ARM_IIRQ_MSK = 0x0030,   /*!< IIRQ Flag Mask                        */
     RDDI_ARM_IIRQ_CARE = 0x0010,     /*!< IIRQ care mode - indicates whether
                                           RDDI_ARM_IIRQ will be checked      */
     RDDI_ARM_IIRQ = 0x0020,          /*!< IIRQ mode (ignored if
                                           RDDI_ARM_IIRQ_CARE unset)          */
     RDDI_ARM_IFIQ_MSK = 0x00C0,   /*!< IFIQ Flag Mask                        */
     RDDI_ARM_IFIQ_CARE = 0x0040,     /*!< IFIQ care flag - indicates whether
                                           RDDI_ARM_IFIQ will be checked      */
     RDDI_ARM_IFIQ = 0x0080           /*!< IFIQ mode (ignored if
                                           RDDI_ARM_IFIQ_CARE unset)          */
} RDDI_ARM_IFLAGS;


/**
 * \enum RDDI_STEP_FLAGS
 * Specifies how to handle interrupts when stepping with Debug_Step()
 */
typedef enum
{
    RDDI_STEP_INT_MSK = 0x03,   /*!< mask for how interrupts handled on step */
     RDDI_STEP_DEF = 0x00,      /*!<  use default model for step             */
     RDDI_STEP_DISINT = 0x01,   /*!<  do not enter ISR (disable)             */
     RDDI_STEP_SKIPINT = 0x02,  /*!<  do not enter ISR (skip)                */
     RDDI_STEP_ENAINT = 0x03,   /*!<  enter ISR if pending interrupt         */
    RDDI_STEP_CYCLE = 0x10      /*!< step by cycle (only supported on some
                                                                simulators)  */
} RDDI_STEP_FLAGS;


/**
 * \enum RDDI_CHAN_OP
 * Specifies the operation to be performed by a call that controls a
 * communications channel e.g. Debug_ChanCtrl()
 */
typedef enum
{
    RDDI_CHAN_START = 0,   /*!< start using channel */
    RDDI_CHAN_STOP = 1,    /*!< stop using channel */
    RDDI_CHAN_PURGEOUT = 2,/*!< purge any data waiting to be sent to target */
    RDDI_CHAN_PURGEIN = 3, /*!< purge any data waiting to return to the client */
    RDDI_CHAN_RESET = 4    /*!< Reset the channel */
} RDDI_CHAN_OP;


/**
 * \enum RDDI_SYNCH_OP
 * Specifies the operation to be performed by a call that controls
 * synchronization of multiple devices e.g. Debug_Synchronize()
 */
typedef enum
{
    RDDI_SYNCH_TEST = 0,    /*!< test for support     */
    RDDI_SYNCH_STOP = 1,    /*!< stop target          */
    RDDI_SYNCH_GO = 2,      /*!< start target running */
    RDDI_SYNCH_STEP = 3     /*!< step an instruction  */
} RDDI_SYNCH_OP;

/*! The number of words in the system capabilities block.
    See Debug_GetCapabilities(). */
#define RDDI_CAPDATA_SYS_SIZE 1
/*! The offset within the system capabilities block of the Global flags word */
#define RDDI_CAP_SYS_GLFLAGS 0
/*!   If set then multiple clients are supported */
#define   RDDI_CAP_SYS_GLF_MCLNTS 0x00000001
/*!   If set then passive connections are supported */
#define   RDDI_CAP_SYS_GLF_PASSIVE 0x00000002
/*!   If set then SMP is supported */
#define   RDDI_CAP_SYS_GLF_SMP 0x00000010

/*! The number of words in the device capabilities block */
#define RDDI_CAPDATA_DEV_SIZE 3
/*! The offset within the device capabilities block of the supported message
    groups bitset */
#define RDDI_CAP_DEV_MSGGRP 0
/*!   Set if the device supports register messages */
#define   RDDI_CAP_DEV_MGRP_REG 0x00000001
/*!   Set if the device supports memory messages */
#define   RDDI_CAP_DEV_MGRP_MEM 0x00000002
/*!   Set if the device supports execution messages */
#define   RDDI_CAP_DEV_MGRP_EXE 0x00000004
/*!   Set if the device supports asynchronous messages */
#define   RDDI_CAP_DEV_MGRP_ASYN 0x00000008
/*!   Set if the device supports breakpoint messages */
#define   RDDI_CAP_DEV_MGRP_BRK 0x00000010
/*!   Set if the device supports debug channel messages */
#define   RDDI_CAP_DEV_MGRP_CHAN 0x00000020
/*!   Set if the device supports register info messages */
#define   RDDI_CAP_DEV_MGRP_REGINFO 0x00000040
/*! The offset within the device capabilities block of the supported message
    groups while executing bitset The bitset values are the same as those in
    the device capabilities block.
 */
#define RDDI_CAP_DEV_MSGGRPEXEC 1
/*! The offset within the device capabilities block of the connection options */
#define RDDI_CAP_DEV_CONFLAGS 2
/*!   Set if the client may specify endianness */
#define   RDDI_CAP_DEV_CON_ENDSPEC 0x00000001
/*!   Set if the debug vehicle can detect endianness */
#define   RDDI_CAP_DEV_CON_ENDDET 0x00000002

/*! The number of words in a device register capabilities block
    excluding the register descriptors themselves.
*/
#define RDDI_CAPDATA_REG_SIZE 6
/*! The offset within a device register capabilities block of the general
    options bitset */
#define RDDI_CAP_REG_FLAGS 0
/*!   Set if register lists supported */
#define   RDDI_CAP_REG_FLG_REGLIST 0x00000001
/*!   Set if code sequences supported */
#define   RDDI_CAP_REG_FLG_CODESEQ 0x00000002
/*!   Set if 32 bit register IDs are supported */
#define   RDDI_CAP_REG_FLG_32BITIDS 0x00000004
/*!   Set if register access operation lists are supported */
#define   RDDI_CAP_REG_FLG_REGOP_LISTS 0x00000008
/*! The offset within a device register capabilities block of the Maximum
    number of register lists (includes list 0) */
#define RDDI_CAP_REG_MAXLISTS 1
/*! The offset within a device register capabilities block of the Maximum
    number of registers per register list */
#define RDDI_CAP_REG_MAXLISTREG 2
/*! The offset within a device register capabilities block of the Maximum
    number of instructions in code sequence */
#define RDDI_CAP_REG_SEQILEN 3
/*! The offset within a device register capabilities block of the Maximum
    words of input data to code sequence */
#define RDDI_CAP_REG_SEQDIN 4
/*! The offset within a device register capabilities block of the Maximum
    words of output data from code sequence */
#define RDDI_CAP_REG_SEQDOUT 5

/*! The size, in words, of a register descriptor. The ICE will return a
    register descriptor for each block of register IDs it wishes to publicise.
*/
#define RDDI_CAP_REGMAP_DSCR_SIZE 2
/*! The offset within a register descriptor of the type/flags field */
#define RDDI_CAP_REGMAP_DSCR_FLAGS 0
/*!   The mask for the type sub-field */
#define   RDDI_CAP_REGMAP_TYP_MSK 0x0000000F
/*!   The value for a register ID mask type */
#define   RDDI_CAP_REGMAP_TYP_MASKED 0x0
/*!   The value for a register ID range type */
#define   RDDI_CAP_REGMAP_TYP_RANGE 0x1
/*! The offset within a register descriptor of the register ID field.
    If the type is an RDDI_CAP_REGMAP_TYP_MASKED the high 16 bits give
    a mask (m) and the low 16 bits a value (v). A register ID (r) is
    supported if (r & m) == v. If the type is an RDDI_CAP_REGMAP_TYP_RANGE
    the high 16 bits give a count (c) and the low 16 bits a start value (v).
    A register ID (r) is supported if (r < v + c).
*/
#define RDDI_CAP_REGMAP_DSCR_REGS 1

/*! The size, in words, of the memory capabilities block */
#define RDDI_CAPDATA_MEM_SIZE 8
/*! The offset into the memory capabilities block of the general options field */
#define RDDI_CAP_MEM_FLAGS 0
/*!   Set if memory pages supported */
#define   RDDI_CAP_MEM_FLG_PAGES 0x00000001
/*!   Set if verify on write supported */
#define   RDDI_CAP_MEM_FLG_VERIFY 0x00000002
/*!   Set if download optimizations supported */
#define   RDDI_CAP_MEM_FLG_DLOAD 0x00000004
/*!   Set if memory fill supported */
#define   RDDI_CAP_MEM_FLG_FILL 0x00000008
/*!   Set if memory copy supported */
#define   RDDI_CAP_MEM_FLG_COPY 0x00000010
/*!   Set if 64 bit addressing supported */
#define   RDDI_CAP_MEM_FLG_UPPER 0x00000020
/*!   Set if physical addresses supported */
#define   RDDI_CAP_MEM_FLG_PHYSICAL 0x00000080
/*!   Set if cache debug mode is supported */
#define   RDDI_CAP_MEM_FLG_CACHE_DEBUG    0x00000100
/*!   Set if cache preservation is supported */
#define   RDDI_CAP_MEM_FLG_CACHE_PRESERVE 0x00000200
/*!   Set if extended memory read list command is supported */
#define   RDDI_CAP_MEM_FLG_READ_LIST_EX 0x00000400

/*! The offset within memory capabilities block of the supported access size /follow
    field. The access sizes supported are described using bit positions, so
    that bit 0 represents 1 byte, bit 1 represents 2 bytes, bit 3 represents
    3 bytes and so on. So in general if a device supports and access size of
    n bytes it should set the 2^(n-1) bit in this field.
*/
#define RDDI_CAP_MEM_ACCSIZE 1
/*! The offset within memory capabilities block of the supported memory
    access types field. The exact interpretation of these fields is
    device dependant.
*/
#define RDDI_CAP_MEM_ACCTYPE 2
/*!   Set if slow memory access is supported */
#define   RDDI_CAP_MEM_ATY_SLOW 0x00000001
/*!   Set if external memory access is supported */
#define   RDDI_CAP_MEM_ATY_EXT 0x00000002
/*!   Set if DMA memory access is supported */
#define   RDDI_CAP_MEM_ATY_DMA 0x00000004
/*!   Set if coprocessor memory access is supported */
#define   RDDI_CAP_MEM_ATY_COPRO 0x00000008
/*!   Set if background memory access is supported */
#define   RDDI_CAP_MEM_ATY_BCKG 0x00000010
/*! The offset within memory capabilities block of the max read block
    size field. This gives the largest block size (in bytes) that can
    be requested in a single memory read call.
*/
#define RDDI_CAP_MEM_MAXREAD 3
/*! The offset within memory capabilities block of the max write block
    size field. This gives the largest block size (in bytes) that can
    be requested in a single memory write call.
*/
#define RDDI_CAP_MEM_MAXWRITE 4
/*! The offset within memory capabilities block of the ideal read block
    size field. This is an indication that if this size is used the
    device read request will have best performance.
*/
#define RDDI_CAP_MEM_BESTREAD 5
/*! The offset within memory capabilities block of the ideal write block
    size field. This is an indication that if this size is used the
    device write request will have best performance.
*/
#define RDDI_CAP_MEM_BESTWRITE 6
/*! The offset within memory capabilities block of the memory fill flags */
#define RDDI_CAP_MEM_FILLFLG 7
/*!   Set if filling with zeroes is supported */
#define   RDDI_CAP_MEM_FILL_ZERO 0x00000001
/*!   Set if filling with any pattern is supported */
#define   RDDI_CAP_MEM_FILL_ANY 0x00000002

/*! The size, in words, of the execution capabilities block */
#define RDDI_CAPDATA_EXE_SIZE 9
/*! The offset within execution capabilities block of the general
    execution flags field */
#define RDDI_CAP_EXE_FLAGS 0
/*!   Set if supports polled status */
#define   RDDI_CAP_EXE_FLG_POLLED 0x00000001
/*!   Set if supports async notification of status */
#define   RDDI_CAP_EXE_FLG_ASYNC 0x00000002
/*!   Set if supports stop escalation */
#define   RDDI_CAP_EXE_FLG_ESC 0x00000004
/*!   Set if supports go until */
#define   RDDI_CAP_EXE_FLG_GUNTIL 0x00000008
/*!   Set if supports step range */
#define   RDDI_CAP_EXE_FLG_SRANGE 0x00000010
/*!   Set if  supports synchronization (between devices) operations */
#define   RDDI_CAP_EXE_FLG_SYNCH 0x00000020
/*!   Set if supports register pre-collection */
#define   RDDI_CAP_EXE_FLG_PRECOL 0x00000040
/*!   Set if supports memory pre-collection */
#define   RDDI_CAP_EXE_FLG_MPRECOL 0x00000080
/*!   Set if supports reverse execution */
#define   RDDI_CAP_EXE_FLG_REVERSE 0x00000100

/*! Offset of connection execution options */
#define RDDI_CAP_EXE_CONFLAGS 1
/*! Can connect and stop device */
#define   RDDI_CAP_EXE_CON_STOP 1
/*! Can connect without stopping device */
#define   RDDI_CAP_EXE_CON_RUN 2

/*! Offset of disconnection execution options */
#define RDDI_CAP_EXE_DCONFLAGS 2
/*! Can disconnect leaving device stopped */
#define   RDDI_CAP_EXE_DCON_STOP 1
/*! Can disconnect leaving device running */
#define   RDDI_CAP_EXE_DCON_RUN 2
/*! Can disconnect leaving device running with breakpoints */
#define   RDDI_CAP_EXE_DCON_RUNDBG 4
/*! Can disconnect leaving device running without breakpoints */
#define   RDDI_CAP_EXE_DCON_RUNFREE 8

/*! The offset within execution capabilities block of the reset
    flags field */
#define RDDI_CAP_EXE_RST 3
/*!   Set if capable of no reset */
#define   RDDI_CAP_EXE_RST_NONE 0x00000001
/*!   Set if capable of general HW reset (not specific to bus/core) */
#define   RDDI_CAP_EXE_RST_HW 0x00000002
/*!   Set capable of a device specific bus reset */
#define   RDDI_CAP_EXE_RST_BUS 0x00000004
/*!   Set if capable of a device specific core reset */
#define   RDDI_CAP_EXE_RST_CORE 0x00000008
/*!   Set if capable of a device specific fake reset. A fake reset usually
      means just setting the register values to a post reset value.
*/
#define   RDDI_CAP_EXE_RST_FAKE 0x00000010
/*!   Set if capable of a JTAG (nTRST) reset */
#define   RDDI_CAP_EXE_RST_JTAG 0x00000020
/*!   Set if capable of holding a reset asserted */
#define   RDDI_CAP_EXE_RST_LEVEL 0x00000040
/*!   Set if the device has a reset control register
      which controls vector/processor reset.
      (this will be configured separately)
*/
#define   RDDI_CAP_EXE_RST_CTRLREG 0x00000100
/*!   Set if the device has a reset control register
      which controls system reset.
      (this will be configured separately)
*/
#define   RDDI_CAP_EXE_RST_AUXCTRLREG 0x00000200
/*!   Set if the target supports application restart */
#define   RDDI_CAP_EXE_RST_APP 0x00010000


/*! The offset within execution capabilities block of the step options field */
#define RDDI_CAP_EXE_STEP 6
/*!   Set if device supports multiple steps */
#define   RDDI_CAP_EXE_STEP_MULTI 0x00000001
/*!   Set if device supports cycle stepping */
#define   RDDI_CAP_EXE_STEP_CYCLE 0x00000002
/*!   Set if device supports interrupt disable during step */
#define   RDDI_CAP_EXE_STEP_INTDIS 0x00000004
/*!   Set if device supports interrupt skip during step */
#define   RDDI_CAP_EXE_STEP_INTSKIP 0x00000008
/*!   Set if device supports interrupt enable during step */
#define   RDDI_CAP_EXE_STEP_INTENA 0x00000010
/*!   Set if step range supports step into */
#define   RDDI_CAP_EXE_STEP_IN 0x00000020
/*!   Set if step range supports step over */
#define   RDDI_CAP_EXE_STEP_OVER 0x00000040
/*!   Set if stepping is done with breakpoints (rather than HW) */
#define   RDDI_CAP_EXE_STEP_BPTS 0x00000080

/*! The offset within execution capabilities block of the connection reset options */
#define RDDI_CAP_EXE_CONRST 4

/*! The offset within execution capabilities block of the disconnection reset options */
#define RDDI_CAP_EXE_DCONRST 5

/*! The offset within execution capabilities block of the max stop
    locations supported by go until.
*/
#define RDDI_CAP_EXE_GUNTILMAX 7
/*! The offset within execution capabilities block of the device
    synchronization options. The synchronization options are specified
    for Go, Step and Stop as a nibble value (4 bits) for each. Each nibble
    value indicates the level of synchronization supported (None, Tight,
    Fairly Tight, Loose). The definitions of these terms is:
<ul>
<li> None:
     This device cannot take part in a synchronized execution request
<li> Tight:
     This device will achieve the synchronized execution request within
     a few us of other devices which also indicate they support tight
     synchronized execution requests.
<li> Fairly Tight:
     This device will achieve the synchronized execution request within
     a few ms of other devices which also indicate they support tight
     or fairly tight synchronized execution requests.
<li> Loose:
     This device will achieve the synchronized execution request within
     a few 100ms of other devices.
</ul>
*/
#define RDDI_CAP_EXE_SYNCH 8
/*!   The mask for the Go sync options */
#define   RDDI_CAP_EXE_SYNC_GO_MSK 0x0000000F
/*!   The shift for the Go sync options */
#define   RDDI_CAP_EXE_SYNC_GO_SHFT 0
/*!   The mask for the Step sync options */
#define   RDDI_CAP_EXE_SYNC_STEP_MSK 0x000000F0
/*!   The shift for the Step sync options */
#define   RDDI_CAP_EXE_SYNC_STEP_SHFT 4
/*!   The mask for the Stop sync options */
#define   RDDI_CAP_EXE_SYNC_STOP_MSK 0x00000F00
/*!   The shift for the Stop sync options */
#define   RDDI_CAP_EXE_SYNC_STOP_SHFT 8
/*!   The nibble value for not supported */
#define   RDDI_CAP_EXE_SYNC_NSUP 0
/*!   The nibble value for tight synchronization */
#define   RDDI_CAP_EXE_SYNC_TIGHT 1
/*!   The nibble value for fairly tight synchronization */
#define   RDDI_CAP_EXE_SYNC_INTER 2
/*!   The nibble value for loose synchronization */
#define   RDDI_CAP_EXE_SYNC_LOOSE 3

/*! The size, in words, of the breakpoint capabilities block */
#define RDDI_CAPDATA_BRK_BASESIZE 5
/*! The offset within breakpoints capabilities block of the
    general options field */
#define RDDI_CAP_BRK_FLAGS 0
/*!   Set if supports software breakpoints */
#define   RDDI_CAP_BRK_FLG_SW 0x00000001
/*!   Set if supports hardware breakpoints */
#define   RDDI_CAP_BRK_FLG_HW 0x00000002
/*!   Set if supports processor event  breakpoints */
#define   RDDI_CAP_BRK_FLG_PROC 0x00000004
/*!   Set if using a unit removes it from all groups */
#define   RDDI_CAP_BRK_FLG_GRPOVER 0x00000008
/*!   Set if software breakpoints can be shared between cores in a cluster */
#define   RDDI_CAP_BRK_FLG_SW_SHARED 0x00000010
/*! The offset within breakpoints capabilities block of the
    HW capability flags */
#define RDDI_CAP_BRK_HWCAP 1
/*!   Set if data-value-only breaks allowed */
#define   RDDI_CAP_BRK_HW_DATAONLY 0x00000001
/*!   Set if supports data address */
#define   RDDI_CAP_BRK_HW_DADDR 0x00000002
/*!   Set if supports execution (for ROM) */
#define   RDDI_CAP_BRK_HW_EXEC 0x00000004
/*!   Set if supports test on data value */
#define   RDDI_CAP_BRK_HW_DATAVAL 0x00000008
/*!   Set if supports exec range */
#define   RDDI_CAP_BRK_HW_ERANGE 0x00000010
/*!   Set if supports exec mask */
#define   RDDI_CAP_BRK_HW_EMASK 0x00000020
/*!   Set if supports address range */
#define   RDDI_CAP_BRK_HW_ARANGE 0x00000040
/*!   Set if supports address mask */
#define   RDDI_CAP_BRK_HW_AMASK 0x00000080
/*!   Set if data ranges allowed */
#define   RDDI_CAP_BRK_HW_DRANGE 0x00000100
/*!   Set if data masked values allowed */
#define   RDDI_CAP_BRK_HW_DMASK 0x00000200
/*!   Set if supports pass count */
#define   RDDI_CAP_BRK_HW_COUNT 0x00000400
/*!   Set if supports measuring time */
#define   RDDI_CAP_BRK_HW_TIMED 0x00000800
/*!   Set if supports and-then tests */
#define   RDDI_CAP_BRK_HW_THEN 0x00001000
/*!   Set if supports AND operation between breaks */
#define   RDDI_CAP_BRK_HW_AND 0x00002000
/*!   Set if supports register breaks */
#define   RDDI_CAP_BRK_HW_REGISTER 0x00004000
/*!   Set if supports program bus breaks (for dual bus systems) */
#define   RDDI_CAP_BRK_HW_PDATA 0x00008000
/*!   Set if address cannot test for read vs write */
#define   RDDI_CAP_BRK_HW_A_NORW 0x00010000
/*!   Set if data cannot test for read vs write */
#define   RDDI_CAP_BRK_HW_D_NORW 0x00020000
/*!   Set if supports digital input match */
#define   RDDI_CAP_BRK_HW_IN 0x00040000
/*!   Set if supports digital output actions */
#define   RDDI_CAP_BRK_HW_OUT 0x00080000
/*!   Set if not range for address is supported */
#define   RDDI_CAP_BRK_HW_NOT_ADDR 0x00100000
/*!   Set if not range for data match is supported */
#define   RDDI_CAP_BRK_HW_NOT_DATA 0x00200000
/*!   Set if then disable (then not) is supported */
#define   RDDI_CAP_BRK_HW_NOT_THEN 0x00400000
/*!   Set if interbreak matches affect all */
#define   RDDI_CAP_BRK_HW_INTERALL 0x00800000
/*!   Set if 'stop' action is supported */
#define   RDDI_CAP_BRK_HW_ACT_STOP 0x10000000
/*!   Set if 'trace on' action is supported */
#define   RDDI_CAP_BRK_HW_ACT_TRCON 0x20000000
/*!   Set if 'trace off' action is supported */
#define   RDDI_CAP_BRK_HW_ACT_TRCOFF 0x40000000
/*!   Set if 'trigger out' action is supported */
#define   RDDI_CAP_BRK_HW_ACT_TRGOUT 0x80000000
/*! The offset within breakpoints capabilities block of the
    number of HW break units */
#define RDDI_CAP_BRK_HWUNITS 2
/*! The offset within breakpoints capabilities block of the
    number of units that can be used for exec breaks */
#define RDDI_CAP_BRK_EXECUNITS 3
/*! The offset within breakpoints capabilities block of the
    input match capabilities */
#define RDDI_CAP_BRK_HWINPUT 4
/*!   Set if supports external input trigger */
#define   RDDI_CAP_BRK_HWINPUT_EXT 0x00000001
/*!   Set if supports matching against privilege level */
#define   RDDI_CAP_BRK_HWINPUT_PRIV 0x00000002
/*!   Set if supports 8-bit matches */
#define   RDDI_CAP_BRK_HWINPUT_SIZE_8 0x00010000
/*!   Set if supports 16-bit matches */
#define   RDDI_CAP_BRK_HWINPUT_SIZE_16 0x00020000
/*!   Set if supports 32-bit matches */
#define   RDDI_CAP_BRK_HWINPUT_SIZE_32 0x00040000
/*!   Set if supports 8 and 16-bit matches */
#define   RDDI_CAP_BRK_HWINPUT_SIZE_8_16 0x00080000
/*!   Set if supports 8 and 32-bit matches */
#define   RDDI_CAP_BRK_HWINPUT_SIZE_8_32 0x00100000
/*!   Set if supports 16 and 32-bit accesses */
#define   RDDI_CAP_BRK_HWINPUT_SIZE_16_32 0x00200000
/*!   Set if supports 8, 16 and 32-bit matches */
#define   RDDI_CAP_BRK_HWINPUT_SIZE_8_16_32 0x00400000
/*!   Set if supports 8, 16, 32 and 64-bit matches */
#define   RDDI_CAP_BRK_HWINPUT_SIZE_8_16_32_64 0x00800000
/*!   Set if supports inverted matches */
#define   RDDI_CAP_BRK_HWINPUT_MATCH_INV 0x00000040
/*!   Set if supports context ID matches */
#define   RDDI_CAP_BRK_HWINPUT_MATCH_CONTEXT_ID 0x00000080
/*!   Set if supports virtual machine ID matches */
#define   RDDI_CAP_BRK_HWINPUT_MATCH_VMID 0x00000200
/*!   Set if supports linked breakpoints */
#define   RDDI_CAP_BRK_HWINPUT_LINK 0x00000100
/*!   Set if supports hypervisor mode matches */
#define   RDDI_CAP_BRK_HWINPUT_HYP 0x00000400


/*! Size of each H/W breakpoint descriptor (in words) */
#define RDDI_CAP_BRK_DSCR_SIZE 3
/*! The offset within a H/W breakpoints descriptor of the
    capabilities field */
#define RDDI_CAP_BRK_DSCR_HWCAP 0
/*! The offset within a H/W breakpoints descriptor of the
    number of bits in pass counter, if supported */
#define RDDI_CAP_BRK_DSCR_PASSCBITS 1
/*! The offset within a H/W breakpoints descriptor of the
    mask of the units included in this group */
#define RDDI_CAP_BRK_DSCR_UNITS 2

/*! The size of the basic details section of the debug channel capabilities block, in 32-bit words.
    The basic details are followed by 0 or more channel descriptors. */
#define RDDI_CAPDATA_CHAN_BASESIZE 1
/*! The offset within a debug channel capabilities block of the
    general options */
#define RDDI_CAP_CHAN_FLAGS 0
/*! Debug channel supports polled read */
#define RDDI_CHAN_FLG_POLLED  0x00000001
/*! Debug channel supports async read */
#define RDDI_CHAN_FLG_ASYNC   0x00000002
/*! The size of each debug channel descriptor (in 32-bit words).
    Each channel descriptor is a target-specific 32 bit ID which is
    often created from ASCII characters. For example, for an
    Arm &reg; Architecture DCC channel the channel ID is created
    from 'D','D','C' as follows:
\code
    ((int32)'C' << 16) || ((int32)'C' << 8) || (int32)'D' = 0x00434344
\endcode    */
#define RDDI_CAP_CHAN_DSCR_SIZE 1

/*! The register stores integer values */
#define   RDDI_CAP_REG_TYPE_INT       0
/*! The register stores floating point values */
#define   RDDI_CAP_REG_TYPE_FLOAT     1

/*! The register is part of the general group */
#define   RDDI_CAP_REG_GROUP_GENERAL  0
/*! The register is part of the floating point group */
#define   RDDI_CAP_REG_GROUP_FLOAT    1
/*! The register is part of the CP15 group */
#define   RDDI_CAP_REG_GROUP_CP15     2
/*! The register is part of another group */
#define   RDDI_CAP_REG_GROUP_OTHER    3

#if DOXYGEN_ACTIVE
/*! Capability block IDs. See Debug_GetCapabilities(). When calling this
    function, you get back a block of 32 bit words. The number and format
    of the words depends on which capability block is requested.

    \link RDDI_CAP_ID::RDDI_CAP_SYS RDDI_CAP_SYS \endlink ICE capabilities
\code
    +------------------------------+
 +0 | RDDI_CAP_SYS_GLFLAGS         |
    | ------[Bit Definitions]----- |
    |    RDDI_CAP_SYS_GLF_MCLNTS   |
    |    RDDI_CAP_SYS_GLF_PASSIVE  |
    +------------------------------+
 +1 | <reserved>                   |
    +------------------------------+
 +2 | <reserved>                   |
    +------------------------------+
 +3 | RDDI_CAP_SYS_MAXDEV          |
    +------------------------------+
\endcode

    \link RDDI_CAP_ID::RDDI_CAP_DEV RDDI_CAP_DEV \endlink Device Capabilities
\code
    +------------------------------+
 +0 | RDDI_CAP_DEV_MSGGRP          |
    | ------[Bit Definitions]----- |
    |    RDDI_CAP_DEV_MGRP_REG     |
    |    RDDI_CAP_DEV_MGRP_MEM     |
    |    RDDI_CAP_DEV_MGRP_EXE     |
    |    RDDI_CAP_DEV_MGRP_ASYN    |
    |    RDDI_CAP_DEV_MGRP_BRK     |
    |    RDDI_CAP_DEV_MGRP_CHAN    |
    +------------------------------+
 +1 | RDDI_CAP_DEV_MSGGRPEXEC      |
    | ------[Bit Definitions]----- |
    |    RDDI_CAP_DEV_MGRP_REG     |
    |    RDDI_CAP_DEV_MGRP_MEM     |
    |    RDDI_CAP_DEV_MGRP_EXE     |
    |    RDDI_CAP_DEV_MGRP_ASYN    |
    |    RDDI_CAP_DEV_MGRP_BRK     |
    |    RDDI_CAP_DEV_MGRP_CHAN    |
    +------------------------------+
 +2 | RDDI_CAP_DEV_CONFLAGS        |
    | ------[Bit Definitions]----- |
    |    RDDI_CAP_DEV_CON_ENDSPEC  |
    |    RDDI_CAP_DEV_CON_ENDDET   |
    +------------------------------+
\endcode

    \link RDDI_CAP_ID::RDDI_CAP_REG RDDI_CAP_REG \endlink Register capabilities
\code
    +------------------------------+
 +0 | RDDI_CAP_REG_FLAGS           |
    | ------[Bit Definitions]----- |
    |    RDDI_CAP_REG_FLG_REGLIST  |
    |    RDDI_CAP_REG_FLG_CODESEQ  |
    +------------------------------+
 +1 | RDDI_CAP_REG_MAXLISTS        |
    +------------------------------+
 +2 | RDDI_CAP_REG_MAXLISTREG      |
    +------------------------------+
 +3 | RDDI_CAP_REG_SEQILEN         |
    +------------------------------+
 +4 | RDDI_CAP_REG_SEQDIN          |
    +------------------------------+
 +5 | RDDI_CAP_REG_SEQDOUT         |
    +------------------------------+
\endcode

    \link RDDI_CAP_ID::RDDI_CAP_REGMAP RDDI_CAP_REGMAP \endlink Register maps and lists
\code
    +---------------------------------+
 +0 || RDDI_CAP_REGMAP_DSCR_FLAGS    ||
    || ------[Bit Definitions]------ ||
    ||    RDDI_CAP_REGMAP_TYP_MSK    ||
    ||    RDDI_CAP_REGMAP_TYP_MASKED || X n
    ||    RDDI_CAP_REGMAP_TYP_RANGE  ||
    ||-------------------------------||
 +1 || RDDI_CAP_REGMAP_DSCR_REGS     ||
    +---------------------------------+
\endcode

    \link RDDI_CAP_ID::RDDI_CAP_MEM RDDI_CAP_MEM \endlink Memory capabilities
\code
    +-----------------------------------+
 +0 | RDDI_CAP_MEM_FLAGS                |
    |  ------[Bit Definitions]-----     |
    |    RDDI_CAP_MEM_FLG_PAGES         |
    |    RDDI_CAP_MEM_FLG_VERIFY        |
    |    RDDI_CAP_MEM_FLG_DLOAD         |
    |    RDDI_CAP_MEM_FLG_FILL          |
    |    RDDI_CAP_MEM_FLG_COPY          |
    |    RDDI_CAP_MEM_FLG_UPPER         |
    |    RDDI_CAP_MEM_FLG_PHYSICAL      |
    |    RDDI_CAP_MEM_FLG_CACHE_DEBUG   |
    |    RDDI_CAP_MEM_FLG_CACHE_PRESERVE|
    |    RDDI_CAP_MEM_FLG_READ_LIST_EX  |
    +-----------------------------------+
 +1 | RDDI_CAP_MEM_ACCSIZE              |
    +-----------------------------------+
 +2 | RDDI_CAP_MEM_ACCTYPE              |
    |  ------[Bit Definitions]-----     |
    |    RDDI_CAP_MEM_ATY_SLOW          |
    |    RDDI_CAP_MEM_ATY_EXT           |
    |    RDDI_CAP_MEM_ATY_DMA           |
    |    RDDI_CAP_MEM_ATY_COPRO         |
    |    RDDI_CAP_MEM_ATY_BCKG          |
    +-----------------------------------+
 +3 | RDDI_CAP_MEM_MAXREAD              |
    +-----------------------------------+
 +4 | RDDI_CAP_MEM_MAXWRITE             |
    +-----------------------------------+
 +5 | RDDI_CAP_MEM_BESTREAD             |
    +-----------------------------------+
 +6 | RDDI_CAP_MEM_BESTWRITE            |
    +-----------------------------------+
 +7 | RDDI_CAP_MEM_FILLFLG              |
    |   ------[Bit Definitions]-----    |
    |    RDDI_CAP_MEM_FILL_ZERO         |
    |    RDDI_CAP_MEM_FILL_ANY          |
    +-----------------------------------+
 \endcode


    \link RDDI_CAP_ID::RDDI_CAP_EXE RDDI_CAP_EXE \endlink Execution Capabilities
\code
    +--------------------------------+
 +0 | RDDI_CAP_EXE_FLAGS             |
    |  ------[Bit Definitions]-----  |
    |    RDDI_CAP_EXE_FLG_GUNTIL     |
    |    RDDI_CAP_EXE_FLG_SRANGE     |
    |    RDDI_CAP_EXE_FLG_SYNCH      |
    +--------------------------------+
 +1 | <reserved>                     |
    +--------------------------------+
 +2 | <reserved>                     |
    +--------------------------------+
 +3 | RDDI_CAP_EXE_RST               |
    |  ------[Bit Definitions]-----  |
    |    RDDI_CAP_EXE_RST_HW         |
    |    RDDI_CAP_EXE_RST_BUS        |
    |    RDDI_CAP_EXE_RST_CORE       |
    |    RDDI_CAP_EXE_RST_FAKE       |
    |    RDDI_CAP_EXE_RST_JTAG       |
    |    RDDI_CAP_EXE_RST_LEVEL      |
    |    RDDI_CAP_EXE_RST_CTRLREG    |
    +--------------------------------+
 +4 | <reserved>                     |
    +--------------------------------+
 +5 | <reserved>                     |
    +--------------------------------+
 +6 | RDDI_CAP_EXE_STEP              |
    |  ------[Bit Definitions]-----  |
    |    RDDI_CAP_EXE_STEP_MULTI     |
    |    RDDI_CAP_EXE_STEP_CYCLE     |
    |    RDDI_CAP_EXE_STEP_INTDIS    |
    |    RDDI_CAP_EXE_STEP_INTSKIP   |
    |    RDDI_CAP_EXE_STEP_INTENA    |
    |    RDDI_CAP_EXE_STEP_IN        |
    |    RDDI_CAP_EXE_STEP_OVER      |
    |    RDDI_CAP_EXE_STEP_BPTS      |
    +--------------------------------+
 +7 | RDDI_CAP_EXE_GUNTILMAX         |
    +--------------------------------+
 +8 | RDDI_CAP_EXE_SYNCH             |
    |  ------[Bit Definitions]-----  |
    |    RDDI_CAP_EXE_SYNC_GO_MSK    |
    |    RDDI_CAP_EXE_SYNC_GO_SHFT   |
    |                                |
    |    RDDI_CAP_EXE_SYNC_STEP_MSK  |
    |    RDDI_CAP_EXE_SYNC_STEP_SHFT |
    |                                |
    |    RDDI_CAP_EXE_SYNC_STOP_MSK  |
    |    RDDI_CAP_EXE_SYNC_STOP_SHFT |
    |                                |
    |    RDDI_CAP_EXE_SYNC_NSUP      |
    |    RDDI_CAP_EXE_SYNC_TIGHT     |
    |    RDDI_CAP_EXE_SYNC_INTER     |
    |    RDDI_CAP_EXE_SYNC_LOOSE     |
    +--------------------------------+
\endcode

    \link RDDI_CAP_ID::RDDI_CAP_BRK RDDI_CAP_BRK \endlink Breakpoint Capabilities
\code
    +------------------------------------------+
 +0 | RDDI_CAP_BRK_FLAGS                       |
    | ----------[Bit Definitions]------------- |
    |    RDDI_CAP_BRK_FLG_SW                   |
    |    RDDI_CAP_BRK_FLG_HW                   |
    |    RDDI_CAP_BRK_FLG_PROC                 |
    |    RDDI_CAP_BRK_FLG_GRPOVER              |
    +------------------------------------------+
 +1 | RDDI_CAP_BRK_HWCAP                       |
    | ----------[Bit Definitions]------------- |
    |    RDDI_CAP_BRK_HW_DATAONLY              |
    |    RDDI_CAP_BRK_HW_DADDR                 |
    |    RDDI_CAP_BRK_HW_EXEC                  |
    |    RDDI_CAP_BRK_HW_DATAVAL               |
    |    RDDI_CAP_BRK_HW_ERANGE                |
    |    RDDI_CAP_BRK_HW_EMASK                 |
    |    RDDI_CAP_BRK_HW_ARANGE                |
    |    RDDI_CAP_BRK_HW_AMASK                 |
    |    RDDI_CAP_BRK_HW_DRANGE                |
    |    RDDI_CAP_BRK_HW_DMASK                 |
    |    RDDI_CAP_BRK_HW_COUNT                 |
    |    RDDI_CAP_BRK_HW_TIMED                 |
    |    RDDI_CAP_BRK_HW_THEN                  |
    |    RDDI_CAP_BRK_HW_AND                   |
    |    RDDI_CAP_BRK_HW_REGISTER              |
    |    RDDI_CAP_BRK_HW_PDATA                 |
    |    RDDI_CAP_BRK_HW_A_NORW                |
    |    RDDI_CAP_BRK_HW_D_NORW                |
    |    RDDI_CAP_BRK_HW_IN                    |
    |    RDDI_CAP_BRK_HW_OUT                   |
    |    RDDI_CAP_BRK_HW_NOT_ADDR              |
    |    RDDI_CAP_BRK_HW_NOT_DATA              |
    |    RDDI_CAP_BRK_HW_NOT_THEN              |
    |    RDDI_CAP_BRK_HW_INTERALL              |
    |    RDDI_CAP_BRK_HW_ACT_STOP              |
    |    RDDI_CAP_BRK_HW_ACT_TRCON             |
    |    RDDI_CAP_BRK_HW_ACT_TRCOFF            |
    |    RDDI_CAP_BRK_HW_ACT_TRGOUT            |
    +------------------------------------------+
 +2 | RDDI_CAP_BRK_HWUNITS                     |
    +------------------------------------------+
 +3 | RDDI_CAP_BRK_EXECUNITS                   |
    +------------------------------------------+
 +4 | RDDI_CAP_BRK_HWINPUT                     |
    | ----------[Bit Definitions]------------- |
    |    RDDI_CAP_BRK_HWINPUT_EXT              |
    |    RDDI_CAP_BRK_HWINPUT_PRIV             |
    |    RDDI_CAP_BRK_HWINPUT_MATCH_INV        |
    |    RDDI_CAP_BRK_HWINPUT_MATCH_CONTEXT_ID |
    |    RDDI_CAP_BRK_HWINPUT_LINK             |
    |    RDDI_CAP_BRK_HWINPUT_MATCH_VMID       |
    |    RDDI_CAP_BRK_HWINPUT_HYP              |
    |    RDDI_CAP_BRK_HWINPUT_SIZE_8           |
    |    RDDI_CAP_BRK_HWINPUT_SIZE_16          |
    |    RDDI_CAP_BRK_HWINPUT_SIZE_32          |
    |    RDDI_CAP_BRK_HWINPUT_SIZE_8_16        |
    |    RDDI_CAP_BRK_HWINPUT_SIZE_8_32        |
    |    RDDI_CAP_BRK_HWINPUT_SIZE_16_32       |
    |    RDDI_CAP_BRK_HWINPUT_SIZE_8_16_32     |
    +------------------------------------------+
    || RDDI_CAP_BRK_DSCR_HWCAP                ||
    ||----------[Bit Definitions]-------------||
    ||   RDDI_CAP_BRK_HW_DATAONLY             ||
    ||   RDDI_CAP_BRK_HW_DADDR                ||
    ||   RDDI_CAP_BRK_HW_EXEC                 ||
    ||   RDDI_CAP_BRK_HW_DATAVAL              ||
    ||   RDDI_CAP_BRK_HW_ERANGE               ||
    ||   RDDI_CAP_BRK_HW_EMASK                ||
    ||   RDDI_CAP_BRK_HW_ARANGE               ||
    ||   RDDI_CAP_BRK_HW_AMASK                ||
    ||   RDDI_CAP_BRK_HW_DRANGE               ||
    ||   RDDI_CAP_BRK_HW_DMASK                ||
    ||   RDDI_CAP_BRK_HW_COUNT                ||
    ||   RDDI_CAP_BRK_HW_TIMED                ||
    ||   RDDI_CAP_BRK_HW_THEN                 ||
    ||   RDDI_CAP_BRK_HW_AND                  || X n
    ||   RDDI_CAP_BRK_HW_REGISTER             ||
    ||   RDDI_CAP_BRK_HW_PDATA                ||
    ||   RDDI_CAP_BRK_HW_A_NORW               ||
    ||   RDDI_CAP_BRK_HW_D_NORW               ||
    ||   RDDI_CAP_BRK_HW_IN                   ||
    ||   RDDI_CAP_BRK_HW_OUT                  ||
    ||   RDDI_CAP_BRK_HW_NOT_ADDR             ||
    ||   RDDI_CAP_BRK_HW_NOT_DATA             ||
    ||   RDDI_CAP_BRK_HW_NOT_THEN             ||
    ||   RDDI_CAP_BRK_HW_INTERALL             ||
    ||   RDDI_CAP_BRK_HW_ACT_STOP             ||
    ||   RDDI_CAP_BRK_HW_ACT_TRCON            ||
    ||   RDDI_CAP_BRK_HW_ACT_TRCOFF           ||
    ||   RDDI_CAP_BRK_HW_ACT_TRGOUT           ||
    ||----------------------------------------||
    || RDDI_CAP_BRK_DSCR_PASSCBITS            ||
    ||----------------------------------------||
    || RDDI_CAP_BRK_DSCR_UNITS                ||
    +------------------------------------------+
\endcode

    \link RDDI_CAP_ID::RDDI_CAP_CHAN RDDI_CAP_CHAN \endlink Debug comms channel capabilities
\code
    +------------------------------+
 +0 | <reserved>                   |
    +------------------------------+
 +1 || RDDI_CAP_CHAN_DSCR_ID      || X n
    +------------------------------+
\endcode

    \link RDDI_CAP_ID::RDDI_CAP_REGINFO RDDI_CAP_REGINFO \endlink Register Information.
<br> This block begins with 3 words of information that contain a reference to a string containing the name of the register architecture (e.g. "Arm"), a reference to a string for the GDB feature name (e.g. "org.gnu.gdb.arm.core") and the register number of the PC. These are followed by sets of 5 words for each register being described. These specify each register's name (as a reference to a string), its size (32 or 64 bit), its type (integer or floating point), the register group and ID number.
\code
    +---------------------------------------------------------+
 +0 | Architecture string offset                              |
    +---------------------------------------------------------+
 +1 | Feature name string offset                              |
    +---------------------------------------------------------+
 +2 | PC register ID                                          |
    +---------------------------------------------------------+
 +3 || < register name string reference>                     ||
    || < size in bits >                                      ||
    || RDDI_CAP_REG_TYPE_INT or RDDI_CAP_REG_TYPE_FLOAT      || X n
    || RDDI_CAP_REG_GROUP_GENERAL or RDDI_CAP_REG_GROUP_FLOAT||
    || < register ID >                                       ||
    +---------------------------------------------------------+
\endcode

    \link RDDI_CAP_ID::RDDI_CAP_RAMINFO RDDI_CAP_RAMINFO \endlink RAM Information.
<br> The RAM information block describes a number of RAMs obtained from a core and encodes attributes specifying their content.

This block begins with a flag encoding whether all the information is known
in the remainder of the block that follows. Some of the information cannot be obtained about the RAMs while
the core is running. The block must be requested when the core is stopped to obtain some information e.g. data from registers.

After this there are n subblocks of data, each encoding information about a single RAM. The first word of the subblock specifies how many words are in this subblock. Specific RAMs may have more attributes than others, and the subblocks are not necessarily all the same size. The second word denotes the RAM type (e.g. L1 Cache). The final m words are RAM specific and variable in length depending on the RAM information they encode.

\code
    +---------------------------------------------------------+
 +0 | Flag specifying known/unknown data in the block         |
    +---------------------------------------------------------+
 +1 | Number of RAMs in the following n subblocks             |
    +---------------------------------------------------------+
 +2 || < Size of this subblock >                             ||
    || < RAM type >                                          ||
    ||---------------[ Variable RAM info ]-------------------||
    || < Attribute 1 >                                       || x n
    || < Attribute 2 >                                       ||
    || ...                                                   ||
    || < Attribute m >                                       ||
    +---------------------------------------------------------+
\endcode

The block for an L1 Cache RAM is pictured below. After the required subblock size and RAM type in the subblock, the third word contains a reference to a string containing the name of the RAM (e.g. "L1-I_DATA"), the fourth word is the unique ID of this RAM - when the \link RDDI_MEM_RULE::RDDI_MRUL_INTERNAL RDDI_MRUL_INTERNAL\endlink flag is set, this specifies that a RAM should be read and the ID specifies which RAM to read.

The next 4 attributes denote the size in bytes, the number of sets, the number of ways and the line size in bytes. The number of sets, ways and the line size are not guaranteed to be known if the block is requested when the core is still running.

\code
    +---------------------------------------------------------+
 +0 | Flag specifying known/unknown data in the block         |
    +---------------------------------------------------------+
 +1 | Number of RAMs in the following n subblocks             |
    +---------------------------------------------------------+
 +2 || < Size of this subblock >                             ||
    || < RAM type >                                          ||
    || < RAM name string reference >                         ||
    || < ID >                                                ||
    || < Size  >                                             || X n
    || < Number of sets >                                    ||
    || < Number of ways >                                    ||
    || < Line size >                                         ||
    +---------------------------------------------------------+
\endcode


    \link RDDI_CAP_ID::RDDI_CAP_STRINGS RDDI_CAP_STRINGS \endlink Capabilities strings.
\n A series of null terminated character strings referenced by the capabilities blocks. The offsets specified in the capability (e.g. by RDDI_CAP_REGINFO) refers to the position within this block of the first character of the string. These offsets begin from "0". For example, if the second word of the REGINFO capabilities block is "4", this means that the "feature name" string begins at character 4 of the RDDI_CAP_STRINGS block.
 */
#endif
typedef enum
{
    RDDI_CAP_SYS = 0,    /*!< ICE capabilities */
    RDDI_CAP_DEV = 1,    /*!< Device capabilities */
    RDDI_CAP_REG = 2,    /*!< Register capabilities */
    RDDI_CAP_REGMAP = 3, /*!< Register maps and lists */
    RDDI_CAP_MEM = 4,    /*!< Memory capabilities */
    RDDI_CAP_EXE = 5,    /*!< Execution capabilities */
    RDDI_CAP_BRK = 6,    /*!< Breakpoint capabilities */
    RDDI_CAP_CHAN = 7,   /*!< Debug comms channel capabilities */
    RDDI_CAP_REGINFO = 8,/*!< Register Information capabilities */
    RDDI_CAP_RAMINFO = 9,/*!< RAM Information capabilities */
    RDDI_CAP_STRINGS = 255 /*!< Strings referenced by capabilities */
} RDDI_CAP_ID;

/**
 * \enum RDDI_REG_LISTS
 * Register list identifier for use with Debug_SetRegList()
 */
typedef enum
{
    RDDI_REG_LIST_DEFAULT = 0   /*!< Default list, returned when the target stops */
} RDDI_REG_LISTS;

/**
 * \enum RDDI_RST_FLAGS
 * Provides the values passed to the resetType parameter
 * of the Debug_SystemReset() call
 */
typedef enum
{
    RDDI_RST_PERFORM = 0x00,    /*!< Assert and then de-assert the nSRST line,
                                     using delays and timeouts set using debug
                                     vehilce (e.g. DSTREAM) configuration items */
    RDDI_RST_ASSERT = 0x01,     /*!< Assert the nSRST line  */
    RDDI_RST_DEASSERT = 0x02    /*!< De-assert the nSRST line  */
} RDDI_RST_FLAGS;


#if DOXYGEN_ACTIVE
/** \page rddireturns RDDI Error Returns

\li <B>RDDI_NO_ASYNC_EVENTS</B> Returned only by Debug_Poll() to signify that
there are no asynchronous events available in the poll queue. As such this code
should not be considered to be a critical error - simply an indication that no
asynchronous events are ready for collection.
\li <B>RDDI_ITEMNOTSUP</B> Returned by Debug_GetConfig() or Debug_SetConfig()
if the requested configuration item is not supported by the specified device or
vehicle.
\li <B>RDDI_REGACCESS</B> An attempt to read from or write to a register failed.
\li <B>RDDI_BUSERR</B> A particular address in a memory operation caused a bus
error.
\li <B>RDDI_TIMEOUT</B> The operation timed out while reading/writing a
particular address.
\li <B>RDDI_VERIFY</B> The requested verify step for a memory write failed.
\li <B>RDDI_BADACC</B> The specified address and access size are not compatible.
Ensure the address is correctly aligned for the specified access size.
\li <B>RDDI_RWFAIL</B> An operation failed to access memory or a register for
reasons not covered by any other reason. This is likely to be RDDI
implementation specific. It could be due to a failure to access a co-processor
register whilst processing Debug_MemDownload() or failure to set the CPSR
register whilst processing Debug_RegCodeSequence().
\li <B>RDDI_MEMACCESS</B> An attempt to read or write memory failed.
\li <B>RDDI_BADMODE</B> The processor is in a mode that prevents the step/go.
E.g. stepping may not be allowed when the core is in Java mode.
\li <B>RDDI_BADPC</B> The address specified in the step/go command is invalid.
\li <B>RDDI_BADINSTR</B> The requested step/go operation could not be performed
due to the current instruction.
\li <B>RDDI_BADSTACK</B> The requested step/go operation could not be performed
due to the current state of the processor stack.
\li <B>RDDI_NOBREAKS</B> No breakpoints are available to perform the requested
operation step. Some implementations of RDDI use breakpoints for step
operations.
\li <B>RDDI_BREAKFAIL</B> A failure was encountered when trying to set or clear
a breakpoint that is not covered by any other reason. Some implementations of
RDDI use breakpoints for step operations. The breakpoint system may be
initialised on connection to a device. An attempt may be made to clear
breakpoints when disconnecting.
\li <B>RDDI_CANNOTSTOP</B> The target processor could not be stopped.
\li <B>RDDI_EXETIMEOUT</B> A timeout was encountered waiting for a finite
execution operation (usually a step) to complete. The warning indicates that
the device had to be forcibly stopped (the stop reason also indicates this).
\li <B>RDDI_INRESET</B> The device is being held in reset outside the control
of the vehicle.
\li <B>RDDI_SESSION_PAUSED</B> The debug session has been paused.  No access
to the target is permitted.
\li <B>RDDI_RUNNING</B> The processor is running but the client's
request can only be completed when the processor is stopped.
\li <B>RDDI_NOTRUNNING</B> The processor is not running but the client's
request can only be completed when the processor is running.
\li <B>RDDI_CORESTATE_DEBUGPRIV</B> Insufficient debug privileges are available
to restore the core state.
\li <B>RDDI_SWBRK_MEMERR</B> Unable to set a software breakpoint in memory.
\li <B>RDDI_BRKCLR_BADID</B> Unable to clear the breakpoint with the supplied
breakpoint ID. This may indicate that the client has become out-of-sync
with the vehicle.
\li <B>RDDI_SWBRK_NOHWRSRC</B> Unable to set all the requested software
breakpoints because there are insufficient hardware resources available.
\li <B>RDDI_PROCBRK_NOHWRSRC</B> Processor events are being simulated using
software breakpoints, but there are insufficient hardware resources available
to set these software breakpoints.
\li <B>RDDI_SWBRK_DEBUGPRIV</B> Unable to set or clear a software breakpoint due
to insufficient debug privileges being available.
\li <B>RDDI_HWBRK_NORESRC</B> Unable to set a hardware breakpoint due to
insufficient breakpoint resources.
\li <B>RDDI_HWBRK_HWFAIL</B> An access to the hardware breakpoint resources failed.
\li <B>RDDI_PROCBRK_HWFAIL</B> An access to the processor events hardware failed.
\li <B>RDDI_DEVUNKNOWN</B> A command has been sent to a device that is not in
the current device list.
\li <B>RDDI_DEVINUSE</B> Returned when the client tried to make an active
connection to a device that already has an active connection. Also returned
when a client with a passive makes a request that requires an active connection.
\li <B>RDDI_NOCONN</B> No connection has been made to the specified vehicle.
Ensure Debug_Connect() or JTag_Connect() has been called successfully and the
client has not disconnected.
\li <B>RDDI_NODRIVER</B> The driver for the selected connection could not be
found.
\li <B>RDDI_COMMS</B> General error returned when there is a problem with the
communications channel.
\li <B>RDDI_BADDLL</B> Reported when is has not been possible to load a local
RDDI DLL. Each implementation of RDDI will use different DLLs/".so" files.
\li <B>RDDI_STATEFAIL</B> The device state requested on connection could not be
achieved.
\li <B>RDDI_NOREMOTE</B> The status of the remote device cannot be determined.
\li <B>RDDI_ENDIAN</B> Unable to determine the endianness of the device.
\li <B>RDDI_HARDWAREINITFAIL</B> Unable to initialise due to a hardware
failure (for example a firmware load problem).
\li <B>RDDI_DEVBUSY</B> The vehicle has been left in a busy state by a
previous call or by another client.
\li <B>RDDI_NOINIT</B> No connection has been made to the specified device.
Ensure Debug_OpenConn() or Debug_OpenPassiveConn() has been called successfully,
and that the connection has not been subsequently closed.
\li <B>RDDI_LOSTCONN</B> The connection to the remote vehicle or device has been
lost.
\li <B>RDDI_NOVCC</B> The device is not powered or has been disconnected.
\li <B>RDDI_CMDUNKNOWN</B> The vehicle has received a command that it did not
understand.
\li <B>RDDI_CMDUNSUPP</B> The specified command is currently unsupported, or
has arguments that are unsupported.
\li <B>RDDI_TARGFAULT</B> A general error to cover problems encountered on a
device not covered by other errors.
\li <B>RDDI_TARGSTATE</B> The requested operation failed due to the device being
in the wrong state or mode.
\li <B>RDDI_NORESPONSE</B> The requested operation failed because it timed
out waiting for a response from the device.
\li <B>RDDI_OUTOFMEM</B> Unable to allocate sufficient memory to complete
the requested operation.
\li <B>RDDI_INCOMP_RVMSG</B> The versions of the protocol that RDDI and the
vehicle are using to communicate with each other are incompatible.
\li <B>RDDI_INCOMP_CLIENT</B> The vehicle is not compatible with the version of
RDDI being used.
\li <B>RDDI_DEBUGPRIV</B> Insufficient debug privileges are available to
perform the requested operation.
\li <B>RDDI_WRONGIR</B> Some implementations of RDDI allow users to specify a
list of devices for their scanchain. A device that was specified could not be
found in the available devices.
\li <B>RDDI_WRONGDEV</B> Some implementations of RDDI allow users to specify a
list of devices for their scanchain. The list does not match the available
devices.
\li <B>RDDI_NOJTAG</B> Operation failed due to a JTAG problem.
\li <B>RDDI_OVERWRITE</B> The requested operation would overwrite the monitor.
\li <B>RDDI_EMUTIMEOUT</B> Operation failed because of a time out when
waiting for a response from hardware. The cause will be RDDI implementation
specific. It could be caused by the lack of response from emulation hardware or
from a remote server.
\li <B>RDDI_CHANOVERFLOW</B> Returned from Debug_ChanSend() when more data than
can be coped with is sent by the client. The error indicates that all the data
that was sent has been rejected.
\li <B>RDDI_BADCHANNELID</B> The specified channel ID was invalid.
\li <B>RDDI_NO_VEH_INIT</B> RDDI is unable to find an instance of the vehicle.
\li <B>RDDI_CMD_FAILED</B> The operation failed because of a failure when RDDI
is communicating with another entity. The cause will be RDDI implementation
specific.  It could be because a GDB server returned an unexpected error or an
empty device list was encountered.
\li <B>RDDI_QUEUE_FULL</B> An internal RDDI queue is full and event data may
have been discarded.
\li <B>RDDI_QUEUE_EMPTY</B> An internal RDDI queue is empty.
\li <B>RDDI_UNKNOWN_MEMORY</B> An unknown register or memory error was
encountered.
\li <B>RDDI_STEPRUN</B> The core could not be stopped after a step operation.
The core will now be running.
\li <B>RDDI_NOCOREPOWER</B> The operation failed because the core has powered
down.
\li <B>RDDI_COREOSLOCK</B> The operation failed because the OS has locked out
debug during OS save restore.
\li <B>RDDI_COREOSLOCK</B> The operation failed because the OS has locked out
debug during powerdown (OS Double Lock).
\li <B>RDDI_MONITOR_DEBUG</B> The operation failed due to monitor debug mode
being selected on the device.
\li <B>RDDI_NODEBUGPOWER</B> The debug system on the target is not powered.
RDDI can do very little with this system.
\li <B>RDDI_UNKNOWN_EXEC</B> An unknown execution error was encountered.
\li <B>RDDI_UNKNOWN</B> An unknown error was encountered. Not even its type
could be determined.
\li <B>RDDI_UNKNOWN_GENERIC</B> An unknown general/common error was encountered.
\li <B>RDDI_UNKNOWN_EMU</B> An unknown emulator error was encountered.
\li <B>RDDI_UNKNOWN_SIM</B> An unknown simulator error was encountered.
\li <B>RDDI_UNKNOWN_RTOS</B> An unknown RTOS specific error was encountered.
\li <B>RDDI_FUTURE</B> An error has been received with a code reserved for
\li <B>RDDI_UNKNOWN_INIT</B> An unknown initialisation/connection error was
encountered.
\li <B>RDDI_ME_LINUX</B> RVI-ME is not available under Linux.
\li <B>RDDI_ME_NOT_FOUND</B> The RVI-ME server could not be found.
\li <B>RDDI_ME_NOT_PRESENT</B> The RVI-ME server does not exist or could not be
\li <B>RDDI_ME_NOT_STARTED</B> The RVI-ME server could not be started.
\li <B>RDDI_NO_CONFIG_FILE</B> The configuration file supplied to
ConfigInfo_OpenFile() cannot be found. Ensure the file and path are correct,
that the configuration is readable and not in use by another application.
\li <B>RDDI_NO_CONFIG</B> The connection has not been configured. Ensure that
ConfigInfo_OpenFile() has been called successfully.
\li <B>RDDI_NO_CS_ASSOCIATIONS</B> No CoreSight association data could be found
in the supplied configuration data.
\li <B>RDDI_UNKNOWN_CONFIG</B> An unknown configuration file error was
encountered. This could be because of an operating system specific problem,
check the file exists and has correct access permissions.
\li <B>RDDI_UNKNOWN_BREAK</B> An unknown breakpoint error was encountered.
\li <B>RDDI_UNKNOWN_CAP</B> The requested capabilities block is not supported
by this implementation of RDDI.
\li <B>RDDI_STEP_ABORT</B> The operation failed, possibly because a breakpoint
was hit.
\li <B>RDDI_EXTERNAL_CMD_FAILED</B> An external command or application failed
to execute. This could be because the user's environment is not configured to
launch an application required by ConfigInfo_EditFile(). For some RDDI
implementations commands may be specified in the configuration data, and these
may have failed.
\li <B>RDDI_LICENSE_FAILED</B> An appropriate license could not be found.
\li <B>RDDI_CALLBACK_EXISTS</B> Debug_RegisterCallback() may only be called
once for a handle. Attempting to register a second callback will result in this
error.
\li <B>RDDI_CADI_INFO</B> Unable to obtain information about the available
models from the CADI interface.
\li <B>RDDI_CADI_INIT</B> Unable to initialise the internal CADI interface used
to communicate with external models.
\li <B>RDDI_CADI_TARGET_INFO</B> Unable to obtain information about the
available devices in the model currently connected to.
\li <B>RDDI_CADI_TARGET_CONNECT</B> Unable to connect to the requested device
from CADI.
\li <B>RDDI_CADI_TARGET_EXEC</B> Failed to start the device executing using
CADI.
\li <B>RDDI_CADI_TARGET_FEATURES</B> RDDI was unable to obtain necessary
information about the current model from CADI. RDDI sometimes need to
interrogate the model to find out about supported registers, breakpoints,
memory layout etc.
\li <B>RDDI_CADI_TARGET_REG_GROUPS</B> RDDI was unable to obtain information
from CADI about the register groups used by the current model.
\li <B>RDDI_CADI_TARGET_REG_UNSUP</B> The requested register is not currently
supported. Check that the name corresponding to the requested register ID has
been included in the configuration data. All the registered that are supported
will be listed in the register capabilities.
\li <B>RDDI_CADI_TARGET_MEM_REGIONS</B> RDDI was unable to obtain information
from CADI about the memory regions supported by the current model.
\li <B>RDDI_CADI_CALLBACK_FAIL</B> RDDI was unable to either add or remove an
callback using the internal CADI interface. This may be seen when connecting or
disconnecting from a target.
\li <B>RDDI_CADI_BAD_BREAK_ID</B> CADI has reported that a device has stopped
on a breakpoint, but that breakpoint ID is unknown to RDDI.
\li <B>RDDI_CADI_MODEL_MISMATCH</B> RDDI has been configured to run with a given
model but has detected that a model of a different kind is already running.

*/
#endif

/*
 * Breakpoint class types
 */
/*! The breakpoint class for SW (instruction) breakpoints */
#define RDDI_BCLASS_SW      0x01
/*! The breakpoint class for HW breakpoints */
#define RDDI_BCLASS_HW      0x02
/*! The breakpoint class for processor (e.g. vector catch) breakpoints */
#define RDDI_BCLASS_PROC    0x04
/*! The breakpoint class for all known breakpoint types */
#define RDDI_BCLASS_ALL     RDDI_BCLASS_SW|RDDI_BCLASS_HW|RDDI_BCLASS_PROC
/*!
 * The struct definition of a memory read operation
 */
typedef struct {
    uint32 page;        /*!< see Debug_MemRead() for a definition of the page parameter */
    uint32 address;     /*!< see Debug_MemRead() for a definition of the address parameter */
    int    accessSize;  /*!< see Debug_MemRead() for a definition of the accessSize parameter */
    int    byteCount;   /*!< see Debug_MemRead() for a definition of the byteCount parameter */
    uint8 *pValues;     /*!< see Debug_MemRead() for a definition of the pValues parameter */
    size_t valuesLen;   /*!< see Debug_MemRead() for a definition of the valuesLen parameter */
} RDDI_MEM_READ_OP;


/*!
 * The struct definition of an extended memory read operation
 */
typedef struct {
    uint32 page;        /*!< see Debug_MemRead() for a definition of the page parameter */
    uint32 address;     /*!< see Debug_MemRead() for a definition of the address parameter */
    int    accessSize;  /*!< see Debug_MemRead() for a definition of the accessSize parameter */
    int    byteCount;   /*!< see Debug_MemRead() for a definition of the byteCount parameter */
    uint8 *pValues;     /*!< see Debug_MemRead() for a definition of the pValues parameter */
    size_t valuesLen;   /*!< see Debug_MemRead() for a definition of the valuesLen parameter */
    uint32 errorCode;   /*!< the error code returned by an individual memory operation. */
    size_t errorLength; /*!< the length of the client-supplied error message
                         * buffer in characters. Note that this includes the
                         * terminating NULL, and so is one more than the
                         * number of characters that can be returned from
                         * RDDI. If necessary, the descriptive string is
                         * truncated to fit in the buffer but note that the
                         * error RDDI_BUFFER_OVERFLOW is returned in the
                         * errorCode field in this case, the orginal error
                         * code will be preserved. The suggested minimum
                         * size for this buffer is 250 characters. If the
                         * description is not needed then pErrorMsg may be
                         * passed as NULL or errorLength as zero, and
                         * pErrorMsg will be ignored without error.       */
    char *pErrorMsg;    /*!< a client-supplied buffer to receive an error
                         * description string    */
} RDDI_MEM_READ_OP_EX;

/*!
 * Register access operation
 */
typedef struct {
    uint32 registerID;   /*!< the ID of the register to read/write */
    uint8  registerSize; /*!< the size of the register. A value of one indicates 32-bit
                          *   registers, two indicates 64-bit registers (paired register
                          *   IDs) and so on. A value of zero is not permitted. */
    uint8  rwFlag;       /*!< a value indicating if the register is to be read from (0)
                          *   or written to (1). */
    uint32 *pRegisterValue; /*!< A buffer containing the value read from the register (for
                             * a read operation) or written to the register (for a write
                             * operation). The registerSize field will contain the size of
                             * this buffer (in words). */
    uint32 errorCode;     /*!< the error code returned by an individual register operation. */
    size_t errorLength;   /*!< the length of the client-supplied error message
                           * buffer in characters. Note that this includes the
                           * terminating NULL, and so is one more than the
                           * number of characters that can be returned from
                           * RDDI. If necessary, the descriptive string is
                           * truncated to fit in the buffer but note that the
                           * error RDDI_BUFFER_OVERFLOW would not be returned
                           * via the errorCode field in this case, the orginal
                           * error code will be preserved. The suggested minimum
                           * size for this buffer is 128 characters. If the
                           * description is not needed then pErrorMsg may be
                           * passed as NULL or errorLength as zero, and
                           * pErrorMsg will be ignored without error.       */
    char *pErrorMsg;      /*!< a client-supplied buffer to receive a NULL
                           * terminated error description string    */
} RDDI_REG_ACC_OP;



#if DOXYGEN_ACTIVE
/**
    Obtain a count of the number of devices on the scan chain.
    This count does not include device 0 as this is reserved for special use
    by the various flavours of RDDI. For example, when using the
    RDDI_Debug_RVI flavour of RDDI, device 0 refers to the DSTREAM unit itself.
    For example usage see Debug_GetDeviceDetails();

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[out] pDeviceCount The number of devices on the scan chain

    The device count is taken from the current configuration file. Therefore
    ConfigInfo_OpenFile() must have been called with a valid configuration
    file before Debug_GetDeviceCount() is used.

 */
#endif
RDDI int Debug_GetDeviceCount(
        RDDIHandle handle,
        int *pDeviceCount);

#if DOXYGEN_ACTIVE
/**
    Obtain the name and options of a device on the scan chain.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device index on the scan chain, starting from 1
    \param[out] pName Client-supplied buffer to take the device name
    \param[in] nameLen Length of pName in characters. Note that this includes
        the terminating NUL, and so is one more than the number of characters
        that may be received by the buffer
    \param[out] pOptions Client-supplied buffer to take the device options. The
        options are tab-delimited
    \param[in] optionsLen Length of pOptions in characters. This must also include
        space for the terminating NUL

    The device details are taken from the current configuration file. Therefore
    ConfigInfo_OpenFile() must have been called with a valid configuration file
    before Debug_GetDeviceDetails() is used.

    It is not mandatory for a client to request both the name and options for a device. Either pName
    or pOptions (but not both) may be passed as NULL or zero-length buffers, in which case the buffer
    is disregarded. If either pName or pOptions is not big enough to take the device details then
    truncation occurs and RDDI_BUFFER_OVERFLOW is returned.

    <B>Example</B> - To obtain and print the name and options of every device on
    the scan chain:

    \dontinclude devicedetails.c
    \skip int deviceCount;
    \until }
    \until }
    \until }
    \until }
    \until }
    \until }
*/
#endif
RDDI int Debug_GetDeviceDetails(
        RDDIHandle handle,
        int deviceNo,
        char *pName, size_t nameLen,
        char *pOptions, size_t optionsLen);

#if DOXYGEN_ACTIVE
/**
    Initialize a connection to the requested vehicle. This function uses the
    address and configuration details from the configuration file already opened
    by the ConfigInfo_OpenFile() function to provide a fully configured
    connection.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] pUserName Username to be used for this session. If this
        parameter is NULL or a zero length string then a username is retrieved
        from the host if possible.
    \param[out] pClientInfo Client supplied buffer to receive version
        information about software module versions.  This information may be
        needed by Support if problems arise.
    \param[in] clientInfoLen The length of the client-supplied client
        information buffer in characters. Note that this includes the
        terminating NULL, and so is one more than the number of characters
        that can be returned from RDDI. If necessary, the information string is
        truncated to fit in the buffer and RDDI_BUFFER_OVERFLOW is returned.
    \param[out] pICEInfo Client supplied buffer to receive information about the
        vehicle (for a DSTREAM this includes firmware and hardware revisions and
        dates).
    \param[in] iceInfoLen The length of the client-supplied information
        buffer in characters. Note that this includes the terminating NULL, and
        so is one more than the number of characters that can be returned from
        RDDI. If necessary, the information string is truncated to fit in
        the buffer and RDDI_BUFFER_OVERFLOW is returned.
    \param[out] pCopyrightInfo Client supplied buffer to receive copyright
        information.
    \param[in] copyrightInfoLen The length of the client-supplied copyright
        buffer in characters. Note that this includes the terminating NULL, and
        so is one more than the number of characters that can be returned from
        RDDI. If necessary, the copyright string is truncated to fit in the
        buffer and RDDI_BUFFER_OVERFLOW is returned.

    The recommended minimum size for the pClientInfo, pICEInfo and
    pCopyrightInfo buffers is 250 characters. All three of these parameters may be passed
    as NULL or as zero-length if the information is not required.


    <B>Example</B> - To connect to the vehicle specified in the configuration
    file opened by ConfigInfo_OpenFile():

    \dontinclude vehicle.c
    \skip char clientInfo[BUFLEN];
    \until }
    \until }
 */
#endif
RDDI int Debug_Connect(
        RDDIHandle handle,
        const char *pUserName,
        char *pClientInfo, size_t clientInfoLen,
        char *pICEInfo, size_t iceInfoLen,
        char *pCopyrightInfo, size_t copyrightInfoLen);

#if DOXYGEN_ACTIVE
/**
    Close an existing connection. The connection must previously have
    been opened with the Debug_Connect() function. With the RDDI_Debug_RVI flavour
    of RDDI, when the connection is closed the debug vehicle will set itself to a
    default state ready for the next connection.
    For example usage see Debug_Connect();

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] termAll If this parameter is non-zero then all connections are
        are closed, including connections owned by other clients. This
        facility should be used with extreme caution.
 */
#endif
RDDI int Debug_Disconnect(
        RDDIHandle handle,
        int termAll);

#if DOXYGEN_ACTIVE
/**
    Open a connection to a device. A connection must first have been
    initialized using Debug_Connect(), but not closed. This function uses the
    configuration information in the file opened and parsed by the
    ConfigInfo_OpenFile() function to provide a fully configured active
    connection to the chosen device.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
    \param[out] pId Device identification number (for example the contents of
        the JTAG Device ID register). This may be passed as NULL.
    \param[out] pVersion Device core/chip version number. This may be passed as NULL.
    \param[out] pMessage Client supplied buffer to receive a connection message.
    \param[in] messageLen The length of the client-supplied connection message
        buffer in characters. Note that this includes the terminating NULL, and
        so is one more than the number of characters that can be returned from
        RDDI. If necessary, the connection message string is truncated to fit
        in the buffer and RDDI_BUFFER_OVERFLOW is returned.

    If the connection message is not required then the pMessage buffer may be
    NULL or zero length - in either case the buffer is ignored and is not
    populated, and does not cause RDDI_BUFFER_OVERFLOW to be returned.
    If the connection message is required then the minimum recommended size of
    the buffer is 250 characters.

    <B>Example</B> - To connect to a core and stop the core:

    \dontinclude device.c
    \skip int id = 0;
    \line garbage
    \skip int id = 0;
    \until }
    \until }
    \until }
 */
#endif
RDDI int Debug_OpenConn(
        RDDIHandle handle,
        int deviceNo,
        int *pId,
        int *pVersion,
        char *pMessage, size_t messageLen);

#if DOXYGEN_ACTIVE
/**
    Open a passive connection to a device. A connection must first
    have been initialized using Debug_Connect(), but not closed. This function
    uses the configuration information in the file opened and parsed by the
    ConfigInfo_OpenFile() function to provide a fully configured passive
    connection to the chosen device.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
    \param[out] pId Device identification number (for example the contents of
        the JTAG Device ID register)
    \param[out] pVersion Device core/chip version number.
    \param[out] pMessage Client supplied buffer to receive a connection message.
    \param[in] messageLen The length of the client-supplied connection message
        buffer in characters. Note that this includes the terminating NULL, and
        so is one more than the number of characters that can be returned from
        RDDI. If necessary, the connection message string is truncated to fit
        in the buffer and RDDI_BUFFER_OVERFLOW is returned.

    pId, pVersion and pMessage are optional and may be passed as NULL. pMessage is
    also ignored if messageLen is zero.

    <B>Example</B> - To connect to a device:

    \dontinclude device.c
    \skip int id = 0;
    \until printf("    Connection message: %s\n", message);

    If the connection message is not required then the message buffer may be
    NULL or zero length - in either case the buffer is ignored and is not
    populated, and does not cause RDDI_BUFFER_OVERFLOW to be returned.
    If the connection message is required then the minimum recommended size of
    the buffer is 250 characters.

 */
#endif
RDDI int Debug_OpenPassiveConn(
        RDDIHandle handle,
        int deviceNo,
        int *pId,
        int *pVersion,
        char *pMessage, size_t messageLen);

#if DOXYGEN_ACTIVE
/**
    Close a connection to a device. The connection must first have been opened
    using the Debug_OpenConn() or Debug_OpenPassiveConn() function.
    For example usage see Debug_OpenConn() or Debug_OpenPassiveConn().

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID.
 */
#endif
RDDI int Debug_CloseConn(
        RDDIHandle handle,
        int deviceNo);

#if DOXYGEN_ACTIVE
/**
    Retrieve a capabilities block for the current connection. For example usage
    see Debug_ChanCtrl().

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID.  The device ID of any device visible from
     the vehicle.  For some implementation of RDDI being used, this can be zero
     for the vehicle itself.  When requesting capability
        blocks from a device it is necessary to have first connected to the
        device using Debug_OpenConn().
    \param[in] blockID The capabilities block ID. Currently this can be one of
        the ::RDDI_CAP_ID enum values:
        \li \link ::RDDI_CAP_ID::RDDI_CAP_SYS RDDI_CAP_SYS \endlink Global
            capabilities. Defines which other capability blocks are
            available from the specified device.
        \li \link ::RDDI_CAP_ID::RDDI_CAP_DEV RDDI_CAP_DEV \endlink
            Gives details of which processor architectures, device variants,
            manufacturer parts and processor options are supported.
        \li \link ::RDDI_CAP_ID::RDDI_CAP_REG RDDI_CAP_REG \endlink
            Lists register capabilities, including register list capabilities
            and code sequence support.
        \li \link ::RDDI_CAP_ID::RDDI_CAP_REGMAP RDDI_CAP_REGMAP \endlink
            Register map information. Lists the register IDs supported by a
            particular device.
        \li \link ::RDDI_CAP_ID::RDDI_CAP_MEM RDDI_CAP_MEM \endlink
            Lists memory capabilities including whether FIL/COPY are
            supported, what memory types are available and so on.
        \li \link ::RDDI_CAP_ID::RDDI_CAP_EXE RDDI_CAP_EXE \endlink
            Lists execution capabilities, including what stepping
            functionality is supported, how stepping is done (breakpoints
            or hardware) and so on.
        \li \link ::RDDI_CAP_ID::RDDI_CAP_BRK RDDI_CAP_BRK \endlink
            For devices that support the breakpoint messages, this
            gives the capabilities, including details of software and
            hardware break support.
        \li \link ::RDDI_CAP_ID::RDDI_CAP_CHAN RDDI_CAP_CHAN \endlink
            For devices that support channel communications, this gives
            the capabilities, including a list of the supported channel IDs.
        \li \link ::RDDI_CAP_ID::RDDI_CAP_REGINFO RDDI_CAP_REGINFO \endlink
            Register Information capabilities
        \li \link ::RDDI_CAP_ID::RDDI_CAP_RAMINFO RDDI_CAP_RAMINFO \endlink
            RAM location capabilities
        \li \link ::RDDI_CAP_ID::RDDI_CAP_STRINGS RDDI_CAP_STRINGS \endlink
            Strings referenced by capabilities


    \param[out] pBlockData Client-supplied buffer to receive the capabilities information.
    \param[in] blockDataLen The size of the client-supplied buffer in bytes.
    \param[out] pBlockDataUsed The number of bytes of capability data returned.

      If the client-supplied buffer is not big enough to receive the capabilities information
      then RDDI_BUFFER_OVERFLOW is returned but pBlockDataUsed receives the correct size of the buffer.
      To facilitate this the buffer may be passed as NULL or as zero length without error - it
      is simply ignored by the call.
 */
#endif
RDDI int Debug_GetCapabilities(
        RDDIHandle handle,
        int deviceNo,
        int blockID,
        uint32 *pBlockData, size_t blockDataLen,
        size_t *pBlockDataUsed);

#if DOXYGEN_ACTIVE
/**
    Control target communication channels

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID. It is necessary to have first connected to
        the device using Debug_OpenConn() or Debug_OpenPassiveConn().
    \param[in] channelID Specifies a particular communication channel to perform
        the operation on. Individual devices typically only support a number of
        communication channels - appropriate channel IDs can be obtained from
        the RDDI_CAP_CHAN capability block.
    \param[in] operation The operation to perform on the specified channel:
        \li RDDI_CHAN_START Indicates that the client wants to use the channel.
            The ICE then begins monitoring the channel for received data. The
            client must start the channel before it sends or receives any data.
        \li RDDI_CHAN_STOP Indicates that the client wants to stop using the
            channel. The ICE should stop monitoring the channel for received data.
        \li RDDI_CHAN_PURGEOUT This throws away any pending data waiting to be
            sent down to the target.
        \li RDDI_CHAN_PURGEIN This throws away any pending received data waiting
            to be sent back to the client.
        \li RDDI_CHAN_RESET This resets the specified communications channel.
            The exact action taken depends on the channel.

    A target declares which channel IDs it can support in the RDDI_CAP_CHAN capabilities block.

    <B>Example</B> - To open a channel for communication, send some data down and print out the response.
    Error handling has been omitted for clarity:

    This method is used in the example code below to poll for an sync message using Debug_Poll()

    \dontinclude dcc.c

    \skip static int AsyncPoll(RDDIHandle handle)
    \line garbage
    \skip static int AsyncPoll(RDDIHandle handle)
    \until }
    \until }

    \dontinclude dcc.c
    \skip void DCC_poll(RDDIHandle sesHandle, int deviceNo)
    \until }
    \until }
    \until }
    \until }

    The channel ID given in the above example is a commonly used code formed from the
    letters 'D','C','C' as follows:
        ((int32)'C' << 16) || ((int32)'C' << 8) || (int32)'D' = 0x00434344

 */
#endif
RDDI int Debug_ChanCtrl(
        RDDIHandle handle,
        int deviceNo,
        int channelID,
        int operation);

#if DOXYGEN_ACTIVE
/**
    Send data across a communications channel

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID. It is necessary to have first connected to
        the device using Debug_OpenConn() or Debug_OpenPassiveConn().
    \param[in] channelID Specifies a particular communication channel through
        which to send data. Individual devices typically only support a number
        of communication channels - appropriate channel IDs can be obtained
        from the RDDI_CAP_CHAN capability block. Before sending data it is
        necessary to have opened the channel using Debug_ChanCtrl() with
        RDDI_CHAN_START.
    \param[in] pData The data to send to the target over the specified channel.
        The format of this data depends on the channel. For a packet-based
        protocol channel, the data should correspond to one complete packet.
    \param[in] dataLen The amount of data to send, in bytes.
    \param[out] pOutBufSpace The amount of free space (in bytes) in the ICE's
        output buffer for this channel, sampled as the reply is sent. The
        largest possible value 0xFFFF (64KB) indicates 64KB or more data is
        available (if the ICE's buffer is larger than 64KB).
    \param[out] pInBufData The amount of data (in bytes) currently available
        in the ICE's input buffer for this channel. The largest possible value
        0xFFFF (64KB) indicates 64KB or more data is available (if the ICE's
        buffer is larger than 64KB).

    If there is no data to send (the client simply wishes to query for buffer stats) then
    pData may be passed as NULL or dataLen as zero. If the client is not interested in buffer
    stats then pOutBufSpace and pInBufData may be passed as NULL. If no useful work can be carried out
    by this function (ie pData NULL or zero length, and pOutBufSpace and pInBufData NULL)
    then an error occurs and RDDI_BADARG is returned.

    <B>Example</B> - For example usage see Debug_ChanCtrl().
 */
#endif
RDDI int Debug_ChanSend(
        RDDIHandle handle,
        int deviceNo,
        int channelID,
        const uint8 *pData, size_t dataLen,
        int *pOutBufSpace,
        int *pInBufData);

#if DOXYGEN_ACTIVE
/**
    Get the value of a configuration item

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID.  the device ID of any device visible to the
        vehicle. For some implementations of RDDI his can be zero for the
        vehicle itself.
        To get the value of a configuration item it is necessary to have first
        connected to the device using either Debug_Connect() or
        Debug_OpenConn() as appropriate.
    \param[in] pName The name of the configuration item.
    \param[out] pValue A client-supplied buffer to receive the configuration
        item value. The value is returned as a NULL-terminated textual
        representation.
    \param[in] valueLen The length of the client-supplied buffer in characters.
        Note that this includes the terminating NULL, and so is one more than the
        number of characters that can be returned from RDDI. If the configuration
        item value string is too large to fit in the buffer then an error is
        returned.

     There is one pre-defined configuration item: "CONFIG_ITEMS". This is a
     read-only config item that contains a newline separated list of the names
     of all the configuration items for the selected device (or for the vehicle
     itself for implementations of RDDI that support such connections to device
     ID 0). This can be used to retrieve all the
     configuration items for the device without prior knowledge of their
     existence.

    <B>Example</B> - To retrieve the current values of all the supported
    configuration items, and then set each config item to its current
    value:

    \dontinclude config.c
    \skip char *pValue = (char *)malloc(CONFBUFLEN);
    \until }
    \until }
    \until }
    \until }
    \until }
 */
#endif
RDDI int Debug_GetConfig(
        RDDIHandle handle,
        int deviceNo,
        const char *pName,
        char *pValue, size_t valueLen);

#if DOXYGEN_ACTIVE
/**
    Set the value of a configuration item. For example usage see Debug_GetConfig().

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID.  The device ID of any device visible from
        the vehicle. For some implementation of RDDI being used, this can be
        zero for the vehicle itself.
        To set the value of a configuration item it is necessary to have first
        connected to the device using either Debug_Connect() or
        Debug_OpenConn() as appropriate, although it should be noted that ICEs
        and devices may not support all configuration items at all times -
        certain items for example may only be available when a device is stopped.
        Therefore a failure to set a configuration item may not be a critical error.
    \param[in] pName The name of the configuration item. A list of configuration
        items supported by the device can be obtained by querying for the
        value of the CONFIG_ITEMS configuration item.
    \param[in] pValue The new configuration item value, supplied as a
        NULL-terminated textual representation.

    The following error codes may be returned:
        \li RDDI_INVALID_CONFIG_ITEM The requested configuration item is not
            supported by the specified device.
        \li RDDI_WRONG_STATE The requested configuration item is supported by
            the specified device, but it is not possible to set the value
            in the current state. For example, the configuration item is only
            available when the device is in pre-connect mode, but
            Debug_SetConfig() has been used with the device in active connection.
        \li RDDI_BAD_PARAMETER The requested configuration item is supported by
            the specified device and it is possible to set the value in
            the current state, but the value given could not be converted to the
            correct format - for example the configuration item expects an
            integer value but the value given was the ASCII string "50MHz".
 */
#endif
RDDI int Debug_SetConfig(
        RDDIHandle handle,
        int deviceNo,
        const char *pName,
        const char *pValue);

#if DOXYGEN_ACTIVE
/**
    Get the meta-information about a configuration item

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID.  the device ID of any device visible to the
        vehicle. For some implementations of RDDI his can be zero for the
        vehicle itself.
        To get the value of a configuration item it is necessary to have first
        connected to the device using either Debug_Connect() or
        Debug_OpenConn() as appropriate.
    \param[in] pName The name of the configuration item.
    \param[in] pKeyName The name of the information requested (MAX/MIN/DEFAULT)
    \param[out] pValue A client-supplied buffer to receive the configuration
        item value. The value is returned as a NULL-terminated textual
        representation.
    \param[in] valueLen The length of the client-supplied buffer in characters.
        Note that this includes the terminating NULL, and so is one more than the
        number of characters that can be returned from RDDI. If the configuration
        item value string is too large to fit in the buffer then an error is
        returned.

     A configuration items information is non-mutable even if the configuration
     item is mutable.  If NULL is passed into the pKeyname parameter then a tab
     delimited list of valid keys will be returned for the selected configuration
     item.

    <B>Example</B> - To retrieve a list of information keys for a configuration
    item.

    \dontinclude config.c
    \skip char *pValue = (char *)malloc(CONFBUFLEN);
    \until }
    \until }
    \until }
    \until }
    \until }

    <B>Example</B> - To retrieve the type of a configuration item.

    \dontinclude config.c
    \skip char *pValue = (char *)malloc(CONFBUFLEN);
    \until }
    \until }
    \until }
    \until }
    \until }
 */
#endif
RDDI int Debug_GetConfigInfo(
        RDDIHandle handle,
        int deviceNo,
        const char *pName,
        const char* pKeyName,
        char *pValue, size_t valueLen);

#if DOXYGEN_ACTIVE
/**
    Set the JTAG clock speed.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] pFreq A string representation of the requested clock speed in Hertz.
        Adaptive clocking can be requested by postfixing the speed with the
        character 'A'. For an RVI-based system the clock speed is a minimum of
        1kHz and a maximum of 50MHz.

    <B>Example</B> - To set adaptive clocking sampled at 50MHz:

    \dontinclude vehicle.c
    \skip void SetAdaptiveClockingWith50MHzSampleRate(RDDIHandle sesHandle)
    \skipline int error = Debug_SetClockSpeed
    \until );

    <B>Example</B> - To set non-adaptive clocking at 10MHz:

    \dontinclude vehicle.c
    \skip void SetNonAdaptiveClockingAt10MHz(RDDIHandle sesHandle)
    \skipline int error = Debug_SetClockSpeed
    \until );

    <B>Example</B> - To set adaptive clocking sampled at 10MHz:

    \dontinclude vehicle.c
    \skip void SetAdaptiveClockingWith10MHzSampleRate(RDDIHandle sesHandle)
    \skipline int error = Debug_SetClockSpeed
    \until );
 */
#endif
RDDI int Debug_SetClockSpeed(
        RDDIHandle handle,
        const char *pFreq);

#if DOXYGEN_ACTIVE
/**
    Write target memory

    \param[in]  handle Session handle previously created by RDDI_Open()
    \param[in]  deviceNo Device ID. To access memory on a device it is necessary
                to have first connected using Debug_OpenConn()
    \param[in]  page The meaning of the page is device specific, it is 0 for
                devices that do not support page.
                When used with the Coresight DAP template:
                    <B>Bits 0-7</B> of this parameter specify the AP selector for the
                    memory operation. An error message will be returned if the
                    operation is not appropriate for the selected AP, or no AP
                    corresponding to the selector.
                    <B>Bits 8-15</B> of this parameter are used to specify the top 8
                    bits of the AP control status word (principally HProt AHB signal
                    values) to use with the AHB-AP (or other bus AP that can use
                    HProt type signals - e.g. APB-AP).  It will be ignored if the
                    AP selected is not an appropriate AP.
                    <B>Bits 16-23</B> of this parameter are used to specify the
                    coresight access hardware to be used. A value of 0 indicates
                    that the specified AP is to be used to access memory. A value of
                    1 is used to force the use of coresight v7 debug hardware (for
                    example a Cortex-R4 / Cortex-A8 debug module). The debug hardware
                    must have been previously configured if this is used to access
                    memory. This value will be ignored by targets that do not support
                    it.
    \param[in]  address The address of the first location to write. Note that on
                some devices there may be limitations on the allowed address
                value in conjunction with the requested access size. For
                example, on an Arm7TDMI &tm; core a request to access a 32 bit
                sized location will be rejected if the address is not
                aligned on a 4 byte boundary.
    \param[in]  accessSize Indicates the size of accesses to be used for writing.
                See ::RDDI_ACC_SIZE for the valid values.
    \param[in]  rule Bitfield specifies special access rules for specific
                ICEs and processors. See ::RDDI_MEM_RULE for the valid values.
    \param[in]  check If non-zero this indicates that the ICE should verify the
                write is successful, by reading back the affected addresses and
                checking against the supplied data.
    \param[in]  byteCount The number of bytes to write. The size is always specified
                in bytes, regardless of the requested access size. However, the
                byte size must be a multiple of the access size.
    \param[in]  pValues Buffer of values to write to target memory.

    <B>Example</B> - To fill an area of memory with a pattern, overwrite it with
    data and read the area back:

    \dontinclude memory.c
    \skip = Debug_MemFill
    \until TESTCHECKNZ(0 == memcmp(pData, pReadData, MEMBUFLEN));
 */
#endif
RDDI int Debug_MemWrite(
        RDDIHandle handle,
        int deviceNo,
        uint32 page,
        uint32 address,
        int accessSize,
        int rule,
        int check,
        int byteCount,
        const uint8 *pValues);

#if DOXYGEN_ACTIVE
/**
    Read target memory. For an example of usage see Debug_MemWrite().

    \param[in]  handle Session handle previously created by RDDI_Open()
    \param[in]  deviceNo Device ID. To access memory on a device it is necessary
                to have first connected using Debug_OpenConn()
    \param[in]  page The meaning of the page is device specific and is 0 for
                devices that do not support page.
    \param[in]  address The address of the first location to read. Note that on
                some devices there may be limitations on the allowed address
                value in conjunction with the requested access size. For
                example, on an Arm7TDMI core a request to access a 32 bit
                sized location will be rejected if the address is not
                aligned on a 4 byte boundary.
    \param[in]  accessSize Indicates the size of accesses to be used for writing.
                See ::RDDI_ACC_SIZE for the valid values.
    \param[in]  rule Bitfield specifies special access rules for specific
                ICEs and processors. See ::RDDI_MEM_RULE for the valid values.
    \param[in]  byteCount The number of bytes to read. The size is always specified
                in bytes, regardless of the requested access size. However, the
                byte size must be a multiple of the access size.
    \param[out] pValues A client-supplied buffer to receive the requested
                memory contents.
    \param[in]  valuesLen The size of the client buffer in bytes. If this is
                not large enough to receive the requested amount of data then
                an error is returned.
 */
#endif
RDDI int Debug_MemRead(
        RDDIHandle handle,
        int deviceNo,
        uint32 page,
        uint32 address,
        int accessSize,
        int rule,
        int byteCount,
        uint8 *pValues, size_t valuesLen);

#if DOXYGEN_ACTIVE
/**
    Read a list of target memory values

    \param[in]  handle Session handle previously created by RDDI_Open()
    \param[in]  deviceNo Device ID. To access memory on a device it is necessary
                to have first connected using Debug_OpenConn()
    \param[in]  rule
                See Debug_MemRead() for the rule specification
    \param[in]  listLength the number of entries in the list and so the number of entries
                in the memList array
    \param[in]  memList The array of RDDI_MEM_READ_OP structs which forms the list of
                memory accesses to perform
*/
#endif
RDDI int Debug_MemReadList(
        RDDIHandle handle,
        int deviceNo,
        int rule,
        size_t listLength,
        RDDI_MEM_READ_OP* memList);

#if DOXYGEN_ACTIVE
/**
    Read a list of target memory values. Success or failure of individual reads
    are reported independently, but if a code other than RDDI_SUCCESS is returned
    by the function it implies that all operations have failed.

    \param[in]  handle Session handle previously created by RDDI_Open()
    \param[in]  deviceNo Device ID. To access memory on a device it is necessary
                to have first connected using Debug_OpenConn()
    \param[in]  rule
                See Debug_MemRead() for the rule specification
    \param[in]  listLength the number of entries in the list and so the number of
                entries in the memList array
    \param[in]  memList The array of RDDI_MEM_READ_OP_EX structs which forms the
                list of memory accesses to perform
*/
#endif
RDDI int Debug_MemReadListEx(
        RDDIHandle handle,
        int deviceNo,
        int rule,
        size_t listLength,
        RDDI_MEM_READ_OP_EX* memList);

#if DOXYGEN_ACTIVE
/**
    Begin/continue download sequence. This function differs from Debug_MemWrite()
    in that it returns before the memory write has completed, and therefore does
    not return an error on failure. At the completion of a download, a call must
    be made to Debug_MemDownloadEnd(), which provides a completion status. If an
    error occurs during a download, all further calls to Debug_MemDownload() are
    ignored until Debug_MemDownloadEnd() is called.

    This function exists simply as an optimization and gives better throughput
    than Debug_MemWrite().
    Once Debug_MemDownload() has been called, all functions except
    Debug_MemDownload() may be ignored until Debug_MemDownloadEnd() is called.

    \param[in] handle See Debug_MemWrite() for details
    \param[in] deviceNo See Debug_MemWrite() for details
    \param[in] page See Debug_MemWrite() for details
    \param[in] address See Debug_MemWrite() for details
    \param[in] size See Debug_MemWrite() for details
    \param[in] rule See Debug_MemWrite() for details
    \param[in] check See Debug_MemWrite() for details
    \param[in] count See Debug_MemWrite() for details
    \param[in] pValues See Debug_MemWrite() for details

    <B>Example</B> - To download two 200-byte blocks at 0x8000 and 80C8:

    \dontinclude memory.c
    \skip memset(pData, 0xF0, MEMBUFLEN);
    \until the error offset
*/
#endif
RDDI int Debug_MemDownload(
        RDDIHandle handle,
        int deviceNo,
        uint32 page,
        uint32 address,
        int size,
        int rule,
        int check,
        int count,
        const uint8 *pValues);

#if DOXYGEN_ACTIVE
/**
    Completes a download sequence (of Debug_MemDownload() calls). This function
    will not return until all downloads started by Debug_MemDownload() calls are
    complete - depending on the amount downloaded and the JTAG clock speed this
    may represent a sizeable delay. For an example of usage see Debug_MemDownload().

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID.
    \param[out] pErrorValue If an error occurs: the data value being written when
        the first failure occurred. Ignored if download is successful.
    \param[out] pErrorPage If an error occurs: the page being written when the
        first failure occurred. Ignored if download is successful.
    \param[out] pErrorAddr If an error occurs: the base address of the block
        being written when the first failure occurred.  Ignored if download is
        successful.
    \param[out] pErrorOffset If an error occurs: the offset into the block at
        which the first failure occurred.  Ignored if download is successful.

    pErrorValue, pErrorPage, pErrorAddr and pErrorOffset may be passed as NULL.
 */
#endif
RDDI int Debug_MemDownloadEnd(
        RDDIHandle handle,
        int deviceNo,
        int *pErrorValue,
        uint32 *pErrorPage,
        uint32 *pErrorAddr,
        uint32 *pErrorOffset);

#if DOXYGEN_ACTIVE
/**
    Fill memory with a pattern. Some ICEs may not support fills, or may only
    support a subset (such as fill with zeroes or fill with fixed size values).
    For an example of usage see Debug_MemWrite().

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID. To access memory on a device it is necessary
        to have first connected using Debug_OpenConn()
    \param[in] page See Debug_MemWrite() for details
    \param[in] addr See Debug_MemWrite() for details
    \param[in] accessSize Indicates the size of accesses to use for writing the
        pattern (and therefore the size of the pattern), specified in number of
        bytes per access. This is similar to the field used in other memory
        operations, but with additional limitations. Since the pattern itself
        is specified as a 32 bit integer, access sizes greater than 4 are not
        allowed.
        The 'default' access size is also not allowed, since the ICE is unable
        to determine the pattern size. The allowed values are:
        \li RDDI_ACC_BYTE (1) Byte pattern (low 8 bits of pattern field)
        \li RDDI_ACC_HALF (2) Half word pattern (low 16 bits of pattern field)
        \li RDDI_ACC_WORD (4) Long word pattern (whole pattern field)
    \param[in] rule See Debug_MemWrite for details
    \param[in] check See Debug_MemWrite for details
    \param[in] iterations The number of times to duplicate the pattern. The
        total size of the fill depends on the selected access size.
    \param[in] pattern Specifies the pattern to fill to memory.
 */
#endif
RDDI int Debug_MemFill(
        RDDIHandle handle,
        int deviceNo,
        uint32 page,
        uint32 addr,
        int accessSize,
        int rule,
        int check,
        int iterations,
        uint32 pattern);

#if DOXYGEN_ACTIVE
/**
    Copy a block from one area to another in target memory. Note that the source
    and destination blocks may not overlap.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID. To access memory on a device it is necessary
        to have first connected using Debug_OpenConn()
    \param[in] sourcePage The page on the target from which to copy data. The
        meaning of page is target specific and is 0 for targets that do not use
        page.
    \param[in] sourceAddr The address of the first location from which to copy.
    \param[in] accessSize Indicates the size of accesses to be used for reading
        and writing, specified in number of bytes per access. See
        Debug_MemWrite() for details.
    \param[in] rule Bitfield specifies special access rules for specific ICEs
        and processors. See Debug_MemWrite() for details.
    \param[in] check If non-zero this indicates that the ICE should verify that
        the copy was successful, by reading back the destination addresses and
        checking against the source data.
    \param[in] byteCopySize Specifies the number of bytes to copy. The size is
        always specified in bytes, regardless of the specified access size.
        However, the byte size must be a multiple of the access size.
    \param[in] destPage The page on the target to copy data to.
    \param[in] destAddr The address of the first location to copy to.

    <B>Example</B> - To copy an area of 200 bytes from 0x8000 to 0xA000:

    \dontinclude memory.c
    \skipline MEMBUFLEN
    \skip = Debug_MemCopy
    \until 0x8000));

 */
#endif
RDDI int Debug_MemCopy(
        RDDIHandle handle,
        int deviceNo,
        uint32 sourcePage,
        uint32 sourceAddr,
        int accessSize,
        int rule,
        int check,
        int byteCopySize,
        uint32 destPage,
        uint32 destAddr);

#if DOXYGEN_ACTIVE
/**
    Read one or more registers in a contiguous block. The ICE may read these
    one at a time, so assumptions should not be made regarding the coherency of
    data in a changing system. Further, some systems may read more than requested
    and cache the values for efficiency.

    Registers are sent in little-endian format regardless of target. A 64-bit
    register must be read as two adjacent 32-bit register ID positions in
    low-high format (and so on for larger registers).

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID. To access registers on a device it is
        necessary to have first connected using Debug_OpenConn()
    \param[in] startID ID of the first register to read
    \param[in] count Number of 32-bit registers to read.
    \param[out] pValues A 32-bit buffer to receive the register values.
    \param[in] valuesLen The number of 32-bit values that can fit into the
        client supplied buffer. If the buffer is not big enough to take the
        requested number of register values then the error RDDI_BUFFER_OVERFLOW
        is returned.

    <B>Example</B> - To read the first 16 registers:

    \dontinclude register.c
    \skip startID
    \until valuesLen
    \skip Debug_RegReadBlock
    \until );

For details on accessing coprocessor registers see \ref CP15Regs
 */
#endif
RDDI int Debug_RegReadBlock(
        RDDIHandle handle,
        int deviceNo,
        uint32 startID,
        int count,
        uint32 *pValues, size_t valuesLen);

#if DOXYGEN_ACTIVE
/**
    Write one or more registers in a contiguous block. The ICE may write these
    one at a time, so assumptions should not be made regarding the coherency of
    data in a changing system. Further, some systems may not write the values
    until execution starts.

    The ICE returns an error if any writes failed and will most likely stop
    writing the sequence at that point, although exact behaviour is not defined
    and should not be relied upon. If writing is delayed, no error occurs until
    execution starts.

    Registers are sent in little-endian format regardless of target. A 64-bit
    register must be treated as two adjacent register ID positions in low-high
    format (and so on for larger registers). It is not permitted to write only
    part of a multi-word register.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID. To access registers on a device it is
        necessary to have first connected using Debug_OpenConn()
    \param[in] startID ID of the first register to write
    \param[in] pValues An array of 32-bit values to write to registers.
    \param[in] count Number of 32-bit register values to write.

    <B>Example</B> - To set the processor PC to 0x8000:

    \dontinclude register.c
    \skip pcID
    \until pcValue
    \skip Debug_RegReadBlock
    \until );

 */
#endif
RDDI int Debug_RegWriteBlock(
        RDDIHandle handle,
        int deviceNo,
        uint32 startID,
        int count,
        const uint32 *pValues);

#if DOXYGEN_ACTIVE
/**
    Access data in the device by running specified sequences of instructions.
    The function varies between devices -
    the most common uses of code sequences are:
        \li Reading or writing special registers in the target that are not
            exposed in the ICE's register map
        \li Reading or writing memory using special commands
        \li Running special instructions to modify the state of the processor
    Note that the code sequence itself is specified in the same format as for
    memory data, whereas the input and output data is specified as an array of
    32-bit words similar to register data. This is intended to correspond
    with the most common use of code sequence requests, which is to read or
    write register values.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID. To run a code sequence on a device it is
        necessary to have first connected using Debug_OpenConn()
    \param[in] type Bitfield providing generic information about the code sequence:
        \li RDDI_CODESEQ_READ: Use this if the code sequence causes data to be
            read from the target
        \li RDDI_CODESEQ_WRITE: Use this if the code sequence causes data to be
            written to the target
        \li Type, classifies the code sequence. One of:
                  RDDI_CODESEQ_UNKNOWN (unclassified);
                  RDDI_CODESEQ_COPRO (coprocessor register access);
                  RDDI_CODESEQ_COPROMEM (coprocessor memory access);
                  RDDI_CODESEQ_MEMORY (special memory access);
                  RDDI_CODESEQ_MONITOR (monitor extension).
    \param[in] instSet Specifies an instruction set for the code sequence, for
        processor architectures that support multiple instruction sets:
        \li RDDI_ARM_ISET_ARM: Arm instruction set
        \li RDDI_ARM_ISET_THUMB: Alternative (THUMB &reg;) instruction set
        \li RDDI_ARM_ISET_JAVA: Java
    \param[in] flags Bitfield: specifies particular processor conditions that
    the code sequence requires.
    Composed of three fields:
        \li Core mode flag, one of RDDI_ARM_IMODE_ANYPRI, RDDI_ARM_IMODE_USR,
        RDDI_ARM_IMODE_FIQ, RDDI_ARM_IMODE_IRQ, RDDI_ARM_IMODE_SVC,
        RDDI_ARM_IMODE_ABT, RDDI_ARM_IMODE_UND, RDDI_ARM_IMODE_SYS,
        RDDI_ARM_IMODE_ANY
        \li Optional IRQ flags: RDDI_ARM_IIRQ_CARE, RDDI_ARM_IIRQ.
        \li Optional FIQ flags: RDDI_ARM_IFIQ_CARE, RDDI_ARM_IFIQ.
    \param[in] dirtyMap A bitfield indicating which processor registers are
    affected by the code sequence. The mapping between bits and registers is
    architecture-specific. The ICE should make sure the contents of these
    registers are preserved across the code sequence, using whatever method
    is appropriate for that ICE.
    \param[in] pCode An array of bytes specifying the instructions to run. As for
    memory write requests, the data is in target order. The format varies
    according to the chosen instruction set.
    \param[in] codeLen The length of the instructions code, in bytes.
    \param[in] pInData An array of 32-bit words specifying the input data to
    the code sequence. This can be NULL if there is no input data. Rules
    governing where to put this data and its format are defined for each
    processor architecture, and may also vary depending on instruction set
    and processor flags.
    \param[in] inDataLen The number of 32-bit words of input data.
    \param[in] outCount The number of 32-bit words of data to return from the
    code sequence. Rules governing where (which registers) to get this data from
    and its format are defined for each processor architecture, and may also
    vary depending on instruction set and processor flags.
    \param[out] pOutData A 32-bit array to receive the output data generated by
    the code sequence. This can be NULL if the code sequence produces no output
    data.
    \param[in] outDataLen The number of 32-bit words that can be accommodated by
    the output data buffer. If the buffer is too small to receive all the
    requested data then an error is returned.
 */
#endif
RDDI int Debug_RegCodeSequence(
        RDDIHandle handle,
        int deviceNo,
        int type,
        int instSet,
        int flags,
        uint32 dirtyMap,
        const uint8 *pCode, size_t codeLen,
        const uint32 *pInData, size_t inDataLen,
        int outCount,
        uint32 *pOutData, size_t outDataLen);

#if DOXYGEN_ACTIVE
/**
    Read a non-contiguous set of registers

    \param[in] handle See Debug_RegReadBlock() for details
    \param[in] deviceNo See Debug_RegReadBlock() for details
    \param[in] pRegisterList An array of register IDs to read
    \param[in] registerListLen The number of register IDs to read
    \param[out] pValues See Debug_RegReadBlock() for details
    \param[in] valuesLen See Debug_RegReadBlock() for details

    <B>Example</B> - To read the contents of register IDs 1, 3 and 5:

    \dontinclude register.c
    \skip pValues
    \until pRegisterList
    \skip pRegisterList
    \until );
 */
#endif
RDDI int Debug_RegReadList(
        RDDIHandle handle,
        int deviceNo,
        const uint32 *pRegisterList, size_t registerListLen,
        uint32 *pValues, size_t valuesLen);

#if DOXYGEN_ACTIVE
/**
    Write to a non-contiguous set of registers.

    \param[in] handle See Debug_RegWriteBlock() for details
    \param[in] deviceNo See Debug_RegWriteBlock() for details
    \param[in] pRegisterList An array of register IDs to write
    \param[in] registerListLen The number of register IDs to write
    \param[in] pValues See Debug_RegWriteBlock() for details

    <B>Example</B> - To write 0x0F0F0F0F, 0xABCDEF01 and 0x00000000 to register
    IDs 1, 3 and 5 respectively:

    \dontinclude register.c
    \skipline pValues
    \skipline pRegisterList
    \skip pRegisterList
    \until = 5;
    \skip pValues[0]
    \until );
 */
#endif
RDDI int Debug_RegWriteList(
        RDDIHandle handle,
        int deviceNo,
        const uint32 *pRegisterList, size_t registerListLen,
        const uint32 *pValues);

#if DOXYGEN_ACTIVE
/**
    Set up a register list. Register lists can be used to improve performance when reading multiple registers from the target.

    \param[in] handle See Debug_RegWriteBlock() for details
    \param[in] deviceNo See Debug_RegWriteBlock() for details
    \param[in] listNo The list identifier. Currently only RDDI_REG_LIST_DEFAULT is defined - this is a list of registers to be returned when the target halts
    \param[in] pRegisterOpList An array of register IDs to be included in the register list
    \param[in] registerOpListLen The number of register IDs to be adde to the list

    <B>Example</B> - To write 0x0F0F0F0F, 0xABCDEF01 and 0x00000000 to register
    IDs 1, 3 and 5 respectively:

    \dontinclude register.c
    \skipline pValues
    \skipline pRegisterList
    \skip pRegisterList
    \until = 5;
    \skip pValues[0]
    \until );
 */
#endif
RDDI int Debug_RegSetList(
        RDDIHandle handle,
        int deviceNo,
        int listNo,
        const uint32 *pRegisterOpList,
        size_t registerOpListLen);

#if DOXYGEN_ACTIVE
/**
    Perform a set of register operations, that will report their individual
    success or failure. Read and write operations may be mixed with

    \param[in] handle See Debug_RegWriteBlock() for details
    \param[in] deviceNo See Debug_RegWriteBlock() for details
    \param[in] pRegisterAccList The array of RDDI_REG_ACC_OP structs which forms
               the list of memory accesses to perform
    \param[in] registerAccListLength The number of enties in the list and so the
               number of entries in the regList array

    <p>Note that although the result returned by this function may be RDDI_SUCCESS
    even if all the individual register operations fail and the individual result
    codes in each of the pRegList entries will need to be examined to determine
    if the entire set of operations has been successful.
    <p>Also note that the register operations are guaranteed to be performed in the
    order specified but no further assumptions about the coherency of the set of
    register operations should be inferred.

 */
#endif
RDDI int Debug_RegRWList(RDDIHandle handle,
                         int deviceNo,
                         RDDI_REG_ACC_OP* pRegisterAccList,
                         size_t registerAccListLength);

#if DOXYGEN_ACTIVE
/**
    Reset system

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID.  If appropriate for the specified resetType,
               and supported by the implementation of RDDI, this parameter may
               be used to direct the reset to a particular device.

               For RDDI_Debug_RVI, this parameter should be zero, and the reset
               applies to the whole system.

               For RDDI_Debug_CADI, this parameter may be zero, to reset the
               system, or may be non-zero to reset the specified device.

               For RDDI_Debug_GDB, this function causes the remote process being
               debugged by gdbserver to be restarted.  The deviceNo parameter
               should be non-zero.

    \param[in] resetType one of the ::RDDI_RST_FLAGS values
  */
#endif
RDDI int Debug_SystemReset(
        RDDIHandle handle,
        int deviceNo,
        int resetType);

#if DOXYGEN_ACTIVE
/**
    TAP Reset

    \param[in] handle Session handle previously created by RDDI_Open()
 */
#endif
RDDI int Debug_TAPReset(
        RDDIHandle handle);

#if DOXYGEN_ACTIVE
/**
    Step/run/stop multiple devices

    The capability information indicates whether synchronization operations are
    tightly coupled, fairly tightly coupled (within a millisecond), or loosely
    coupled. For JTAG emulators, it is normally tightly coupled.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] operation Specifies the operation to perform:
        \li RDDI_SYNCH_TEST Test for support. This allows the client to check
        whether synchronized operations on a particular set of devices are
        supported/possible.
        \li RDDI_SYNCH_STOP Synchronized stop (stop all devices together)
        \li RDDI_SYNCH_GO Synchronized execution (start all devices together)
        \li RDDI_SYNCH_STEP Synchronized step (step all devices once)
    \param[in] pDeviceList An array of device IDs - specifies the group of
    devices to perform the synchronized operation on.
    \param[in] deviceListLen The number of devices on which to perform the
    synchronized operation.

    In order to perform synchronised operations on a device, a debugger must
    have an active connection to that device. The debugger with the active
    connection does not need to be the debugger calling the Debug_Synchronize()
    function.
    This is the only situation where a debugger may perform operations on a
    device without first obtaining an active connection.
 */
#endif
RDDI int Debug_Synchronize(
        RDDIHandle handle,
        int operation,
        const int *pDeviceList, size_t deviceListLen);

#if DOXYGEN_ACTIVE
/**
    Set a hardware breakpoint

    An ICE connected to a processor with dedicated debugging hardware
    implements hardware breakpoints by programming the registers of the debug
    hardware.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID. To set hardware breakpoints it is first
        necessary to connect to a device using Debug_OpenConn().
    \param[in] breakpointType Indicates in broad terms what sort of breakpoint
        this is. This determines which of the other fields are relevant:
        \li RDDI_HWBRK_EXEC Instruction breakpoint
        \li RDDI_HWBRK_READ Data read breakpoint
        \li RDDI_HWBRK_WRITE Data write breakpoint
        \li RDDI_HWBRK_ACCESS Data access (read or write) breakpoint
    \param[in] addressMatch Specifies what sort of address match to use for
        this point:
        \li RDDI_HWBRK_ANONE No address match
        \li RDDI_HWBRK_ASINGLE Single address match
        \li RDDI_HWBRK_ARANGE Address match range
        \li RDDI_HWBRK_AMASK Address masked value match
    \param[in] page The memory page that this breakpoint must match addresses in
    \param[in] addr Specifies an address. Interpretation depends on the value of
        addressMatch:
        \li RDDI_HWBRK_ASINGLE The single address to match
        \li RDDI_HWBRK_ARANGE The start of the address range (inclusive)
        \li RDDI_HWBRK_AMASK The value to match after applying the mask
    \param[in] addr2 Specifies a second address. This is only used for these
        values of addressMatch:
        \li RDDI_HWBRK_ARANGE The end of the address range (inclusive)
        \li RDDI_HWBRK_AMASK The mask value (set bits are part of the match)
    \param[in] count A count qualifier (number of times the breakpoint must match
        before it hits), for systems that support hardware pass counts. Setting
        this parameter to zero or one indicates no pass count.
    \param[in] thenHandle The ID of the next breakpoint in a chain, for systems
        that support hardware breakpoint chaining.
    \param[in] dataMatch Specifies what sort of data match to use for this point:
        \li RDDI_HWBRK_DNONE No data match
        \li RDDI_HWBRK_DSINGLE Single data value match
        \li RDDI_HWBRK_DRANGE Data range match
        \li RDDI_HWBRK_DMASK Data masked value match
    \param[in] data Specifies a data value. Interpretation depends on the value
        of dataMatch:
        \li RDDI_HWBRK_DSINGLE The single value to match
        \li RDDI_HWBRK_DRANGE The lowest value in the range (inclusive)
        \li RDDI_HWBRK_DMASK The value to match after applying the mask
    \param[in] data2 Specifies a second data value. This is only used for these
        values of dataMatch:
        \li RDDI_HWBRK_DRANGE The highest value in the range (inclusive)
        \li RDDI_HWBRK_DMASK The mask value (set bits are part of the match)
    \param[in] input A bitfield to enable other tests specific to the ICE or the
        device
    \param[in] output A bitfield indicating actions to take when the breakpoint
        is hit
    \param[in] flags A bitfield specifying options for the behaviour of the
        breakpoint:
        \li RDDI_HWBFLG_AFTER If set, this indicates that the thenHandle field
        specifies a breakpoint chain in reverse. So this breakpoint is
        automatically enabled when the chained one is hit, rather than the
        other way round.
        \li RDDI_HWBFLG_NOT_ADDR If set, indicates an inverse image range
        (match on all addresses outside the range). Ignored unless the
        addressMatch field is set to RDDI_HWBRK_ARANGE.
        \li RDDI_HWBFLG_NOT_DATA If set, indicates an inverse data range
        (match on all data values outside the range). Ignored unless the
        dataMatch field is set to RDDI_HWBRK_DRANGE.
        \li RDDI_HWBFLG_NOT_THEN If set, indicates the chained breakpoint
        should be disabled when this one is hit.  Ignored unless the thenHandle
        field is non-zero.
        \li RDDI_HWBFLG_AND If set, indicates the chained breakpoint AND this
        breakpoint must both have been hit before activating the next breakpoint
        in the chain. Ignored unless the thenHandle field is non-zero.
        \li RDDI_HWBFLG_COUNTCLR If set, indicates that the pass count should
        reset automatically each time the processor starts. Ignored unless the
        count field is non-zero.
        \li RDDI_HWBFLG_THENDIS If set, indicates that the chained breakpoint
        (not this one) should reset to being disabled each time the processor
        starts. Ignored unless the thenHandle field is non-zero.
        \li RDDI_HWBFLG_ISDIS If set, indicates that this breakpoint is
        initially disabled. Typically another breakpoint would then be defined
        that chains to this one, automatically enabling it when hit.
    \param[out] pBreakpointID A unique identifier for the breakpoint which can
        be used for clearing it and can be returned as part of a stop
        notification message to identify the specific cause.
        Note that IDs are numbered from one upwards (zero is not a valid ID).
        It should also be noted that breakpoint IDs may not be unique between
        hardware and software:
         i.e. a software breakpoint may share an ID with a hardware breakpoint.
        The client must therefore use the breakpoint ID and the breakpoint type
        to produce a composite ID for each breakpoint.

    <B>Example</B> - To set and then clear a hardware instruction breakpoint at
        0x8000:

    \dontinclude break.c
    \skipline breakpointID
    \skip = Debug_SetHWBreak
    \until ));
    \until ));
 */
#endif
RDDI int Debug_SetHWBreak(
        RDDIHandle handle,
        int deviceNo,
        int breakpointType,
        int addressMatch,
        uint32 page,
        uint32 addr,
        uint32 addr2,
        int count,
        int thenHandle,
        int dataMatch,
        uint32 data,
        uint32 data2,
        uint32 input,
        uint32 output,
        int flags,
        int *pBreakpointID);

#if DOXYGEN_ACTIVE
/**
    Clear a hardware breakpoint. For example usage see Debug_SetHWBreak().

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID.
    \param[in] breakpointID Unique breakpoint ID previously returned by
        Debug_SetHWBreak()
 */
#endif
RDDI int Debug_ClearHWBreak(
        RDDIHandle handle,
        int deviceNo,
        int breakpointID);

#if DOXYGEN_ACTIVE
/**
    Retrieve the current pass count for a given hardware breakpoint. The pass
    count is the number of times that the breakpoint's conditions must be met
    before the breakpoint is activated.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID.
    \param[in] breakpointID Specifies the ID of the hardware breakpoint for
    which to retrieve the current pass count. This breakpoint ID is created by
    Debug_SetHWBreak().
    \param[out] pCount The current value of the breakpoint's pass count, for
    systems that support hardware pass counts
 */
#endif
RDDI int Debug_GetHWBreakPassCount(
        RDDIHandle handle,
        int deviceNo,
        int breakpointID,
        int *pCount);

#if DOXYGEN_ACTIVE
/**
    Reset a hardware breakpoint's pass count to its original value (i.e. the
    pass count specified when the breakpoint was created using Debug_SetHWBreak()).

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID.
    \param[in] breakpointID Unique breakpoint ID previously returned by
    Debug_SetHWBreak()
 */
#endif
RDDI int Debug_ResetHWBreakPassCount(
        RDDIHandle handle,
        int deviceNo,
        int breakpointID);

#if DOXYGEN_ACTIVE
/**
    Enable a processor event breakpoint

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID.
    \param[in] breakID Specifies a processor breakpoint to enable.

    <B>Example</B> - To enable the SWI, data abort and prefetch abort
    processor breaks:

    \dontinclude break.c
    \skip = Debug_SetProcBreak
    \until ));

    Note that all breakpoint identifiers are four characters, and are
    automatically padded with trailing spaces where necessary. Therefore
    "SWI" and "SWI " can both be used to enable the software interrupt
    processor breakpoint.

 */
#endif
RDDI int Debug_SetProcBreak(
        RDDIHandle handle,
        int deviceNo,
        ProcBreakID breakID);

#if DOXYGEN_ACTIVE
/**
    Disable a processor event breakpoint

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID.
    \param[in] breakID Specifies a processor breakpoint to disable.

    <B>Example</B> - To disable the SWI, data abort and prefetch abort processor
    breaks:

    \dontinclude break.c
    \skip = Debug_ClearProcBreak
    \until ));

    Note that all breakpoint identifiers are four characters, and are
    automatically  padded with trailing spaces where necessary.
    Therefore "SWI" and "SWI " can both be used to disable the software
    interrupt processor breakpoint.
 */
#endif
RDDI int Debug_ClearProcBreak(
        RDDIHandle handle,
        int deviceNo,
        ProcBreakID breakID);

#if DOXYGEN_ACTIVE
/**
    Returns a list of processor breakpoints implemented on the device.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
    \param[out] pBreakIDList Client-provided array to receive a list of
    implemented processor breakpoints. This can be NULL is the client if only
    interested in the number of breakpoints.
    \param[in,out] pBreakpointCount For input, specifies the size of the
    pBreakIDList array. For output, specifies the number of breakpoint IDs
    copied into the array (or the total number of processor breaks available,
    if the array is NULL).

    <B>Example</B> - To retrieve a list of all the processor breakpoints
    implemented on the device, and display the details of each breakpoint:

    \dontinclude break.c
    \skipline breakpointCount
    \skip = Debug_ListProcBreaks
    \until }
    \until }
    \until }
    \until }

    <B>Note</B> If pBreakIDList is non-NULL and breakpointCount is less that
    the number of breakpoints available on the device then RDDI_BUFFOVER will
    be returned.
  */
#endif
RDDI int Debug_ListProcBreaks(
        RDDIHandle handle,
        int deviceNo,
        ProcBreakID *pBreakIDList, size_t *pBreakpointCount);

#if DOXYGEN_ACTIVE
/**
    Retrieves details of a specified processor breakpoint.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
    \param[in] breakID ID of the breakpoint for which details are required.
    \param[out] pEnabled If non-NULL this is set to true(1) if the breakpoint
                is enabled, false(0) otherwise
    \param[out] pNameBuffer Client-supplied buffer to receive the breakpoint's name
    \param[in] nameBufferLen The length of pNameBuffer in characters
    \param[out] pDescriptionBuffer Client-supplied buffer to receive the
        breakpoint's descriptive text
    \param[in] descriptionBufferLen The length of pDescriptionBuffer in characters

    If the specified breakpoint is not implemented on the device then
    RDDI_BREAKFAIL is returned. Note that all breakpoint identifiers are four
    characters, and are automatically padded with trailing spaces where
    necessary. Therefore "SWI" and "SWI " can both be used to request details
    of the software interrupt processor breakpoint.

    For example usage see Debug_ListProcBreaks().

    <B>Note</B> If pNameBuffer or pDescriptionBuffer are too small to receive
    the breakpoint's name or description then RDDI_BUFFOVER will be returned.
    nameBufferLen and descriptionBufferLen give the length of their respective
    buffers in characters including the terminating nul - ie one more than the
    number of characters that can actually be accommodated in the buffer. However,
    both the name and description are optional - if either is passed as NULL or as
    zero length they are then ignored without error. If nothing can be returned by this
    call (name and description buffers NULL or zero length, and pEnabled passed as NULL)
    then RDDI_BADARG is returned.
  */
#endif
RDDI int Debug_GetProcBreakDetail(
        RDDIHandle handle,
        int deviceNo,
        ProcBreakID breakID,
        int *pEnabled,
        char *pNameBuffer, size_t nameBufferLen,
        char *pDescriptionBuffer, size_t descriptionBufferLen);

#if DOXYGEN_ACTIVE
/**
    Set a software instruction breakpoint

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
    \param[in] page Specifies the page to set the breakpoint on.
    \param[in] addr Specifies the address of an instruction to break on. The
        breakpoint is hit if an instruction at that address is executed.
    \param[in] flags Bitfield specifying options for the breakpoint:
        \li Type: Type of breakpoint to set, one of:
                  RDDI_BRUL_STD (Default for emulator/ICE);
                  RDDI_BRUL_ALT (Alternate breakpoint (Thumb));
                  RDDI_BRUL_JAVA (Java breakpoint).
    \param[out] pBreakpointID A unique identifier for the breakpoint which can
    be used for clearing it and can be returned as part of a stop notification
    message to identify the specific cause.
    Note that IDs are numbered from one upwards (zero is not a valid ID). It
    should also be noted that breakpoint IDs may not be unique between hardware
    and software:
    i.e. a software breakpoint may share an ID with a hardware breakpoint. The
    client must therefore use the breakpoint ID and the breakpoint type to
    produce a composite ID for each breakpoint.

    <B>Example</B> - To set and then clear a software breakpoint at 0x8000:

    \dontinclude break.c
    \skipline breakpointID
    \skip = Debug_SetSWBreak
    \until ));
 */
#endif
RDDI int Debug_SetSWBreak(
        RDDIHandle handle,
        int deviceNo,
        uint32 page,
        uint32 addr,
        int flags,
        int *pBreakpointID);

#if DOXYGEN_ACTIVE
/**
    Clear a software breakpoint by ID. For example usage see Debug_SetSWBreak().

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
    \param[in] breakpointID The ID of the breakpoint to clear (as returned by
    Debug_SetSWBreak()).
 */
#endif
RDDI int Debug_ClearSWBreak(
        RDDIHandle handle,
        int deviceNo,
        int breakpointID);

#if DOXYGEN_ACTIVE
/**
    Clears all the breakpoints on the specified device of the specified types.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
    \param[in] type A bitmap specifying one or more of the following breakpoint
    types:
        <ul><li> RDDI_BCLASS_SW: If specified, the ICE clears all breakpoints set
        using Debug_SetSWBreak()</li>
        <li> RDDI_BCLASS_HW: If specified, the ICE clears all breakpoints set
        using Debug_SetHWBreak()</li>
        <li> RDDI_BCLASS_PROC: If specified, the ICE clears all breakpoints set
        using Debug_SetProcBreak()</li>
        <li> RDDI_BCLASS_ALL: Equivalent to RDDI_BCLASS_SW | RDDI_BCLASS_HW |
        RDDI_BCLASS_PROC. The ICE clears all three types of breakpoint.</li></ul>

    <B>Example</B> - To clear all software and hardware breaks but leave
    processor breaks intact:

    \dontinclude break.c
    \skipline breakpointID
    \skip = Debug_ClearAllBreaks
    \until ));
 */
#endif
RDDI int Debug_ClearAllBreaks(
        RDDIHandle handle,
        int deviceNo,
        int type);

#if DOXYGEN_ACTIVE
/**
    Steps the target one or more times by instruction. This function implements
    the simplest form of low level step, executing the current instruction and
    then returning with the processor stopped at the next instruction.
    Optionally this can be done more than once using just one command.

    The ICE will leave all breakpoints enabled during the step. If there is an
    instruction breakpoint set on the step destination address, then the ICE
    will report the stop cause as STOP_SWBRK or STOP_HWBRK as appropriate. If a
    data breakpoint or processor event breakpoint is hit during a step, or if
    some sort of exception occurs, then that will be reported as the stop cause,
    with the processor stopped at the appropriate address.

    If the processor is currently stopped at a breakpointed instruction, then
    the breakpoint is hit as a result of the step, and the processor does not
    advance its program counter. The client must be aware of this, and arrange
    for breakpoints to be temporarily removed as necessary to avoid execution
    getting 'stuck'.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
    \param[in] count Number of instructions or cycles to step. Zero and one
    have the same meaning (a single step).
    \param[in] flags Specifies how to handle interrupts while stepping:
        \li \link RDDI_STEP_FLAGS.RDDI_STEP_DISINT RDDI_STEP_DISINT \endlink
        Disable interrupts during step. If an interrupt occurs during the step
        then it is ignored and the interrupt handler is not called.
        \li \link RDDI_STEP_FLAGS.RDDI_STEP_SKIPINT RDDI_STEP_SKIPINT \endlink
        Execute interrupts during step. If an interrupt occurs during a step
        then the handler is called. Once the handler has returned the step
        proceeds as before, leaving the processor stopped on the next
        instruction.
        \li \link RDDI_STEP_FLAGS.RDDI_STEP_ENAINT RDDI_STEP_ENAINT \endlink
        Service interrupts during step. If an interrupt occurs during the step
        then the processor is halted on the first instruction of the interrupt
        handler routine.
    \param[out] pInstrStepped The number of instructions that were stepped. This parameter
        is optional and may be passed as NULL.
 */
#endif
RDDI int Debug_Step(
        RDDIHandle handle,
        int deviceNo,
        int count,
        int flags,
        int *pInstrStepped);

#if DOXYGEN_ACTIVE
/**
    Steps the target one or more times by instruction in the reverse direction.
    This function implements the simplest form of low level step, executing the
    current instruction and then returning with the processor stopped at the
    next instruction.
    Optionally this can be done more than once using just one command.

    The ICE will leave all breakpoints enabled during the step. If there is an
    instruction breakpoint set on the step destination address, then the ICE
    will report the stop cause as STOP_SWBRK or STOP_HWBRK as appropriate. If a
    data breakpoint or processor event breakpoint is hit during a step, or if
    some sort of exception occurs, then that will be reported as the stop cause,
    with the processor stopped at the appropriate address.

    If the processor is currently stopped at a breakpointed instruction, then
    the breakpoint is hit as a result of the step, and the processor does not
    advance its program counter. The client must be aware of this, and arrange
    for breakpoints to be temporarily removed as necessary to avoid execution
    getting 'stuck'.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
    \param[in] count Number of instructions or cycles to step. Zero and one
    have the same meaning (a single step).
    \param[in] flags Specifies how to handle interrupts while stepping:
        \li \link RDDI_STEP_FLAGS.RDDI_STEP_DISINT RDDI_STEP_DISINT \endlink
        Disable interrupts during step. If an interrupt occurs during the step
        then it is ignored and the interrupt handler is not called.
        \li \link RDDI_STEP_FLAGS.RDDI_STEP_SKIPINT RDDI_STEP_SKIPINT \endlink
        Execute interrupts during step. If an interrupt occurs during a step
        then the handler is called. Once the handler has returned the step
        proceeds as before, leaving the processor stopped on the next
        instruction.
        \li \link RDDI_STEP_FLAGS.RDDI_STEP_ENAINT RDDI_STEP_ENAINT \endlink
        Service interrupts during step. If an interrupt occurs during the step
        then the processor is halted on the first instruction of the interrupt
        handler routine.
    \param[out] pInstrStepped The number of instructions that were stepped. This
        is a count of the number of instructions that have stepped and WILL NOT
        use the sign of the value to indicate backwards stepping. This parameter
        is optional and may be passed as NULL.
 */
#endif
RDDI int Debug_ReverseStep(
        RDDIHandle handle,
        int deviceNo,
        int count,
        int flags,
        int *pInstrStepped);

#if DOXYGEN_ACTIVE
/**
    Starts execution of the specified device. If the processor is currently
    stopped at a breakpointed instruction, then the breakpoint is hit as soon
    as execution resumes, and the processor does not advance its program
    counter.  The client must be aware of this, and arrange for breakpoints to
    be temporarily removed and stepped off as necessary to avoid execution
    getting 'stuck'.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
 */
#endif
RDDI int Debug_Go(
        RDDIHandle handle,
        int deviceNo);

#if DOXYGEN_ACTIVE
/**
    Starts execution of the specified device but in reverse. If the processor is
    currently stopped at a breakpointed instruction, then the breakpoint is hit
    as soon as execution resumes, and the processor does not advance its program
    counter.  The client must be aware of this, and arrange for breakpoints to
    be temporarily removed and stepped off as necessary to avoid execution
    getting 'stuck'.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
 */
#endif

RDDI int Debug_ReverseGo(
        RDDIHandle handle,
        int deviceNo);

#if DOXYGEN_ACTIVE
/**
    Requests that device execution be halted. This is an asynchronous request
    and may not have been carried out before the function returns. If the client
    has registered a callback function via the Debug_RegisterCallback()
    function then the callback will receive confirmation when the processor
    stops. If no callback function has been registered then the client must poll
    for confirmation using Debug_Poll() or Debug_GetExecStatus().

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID

    <B>Example</B> - To request that a target stop, and poll until the execution
    status has changed to RDDI_PROC_STATE_STOPPED:

    \dontinclude exec.c
    \skip static void RequestStop(RDDIHandle sesHandle, int deviceNo, int bAsync
    \line garbage
    \skip static void RequestStop(RDDIHandle sesHandle, int deviceNo, int bAsync
    \skip ;
    \until could not stop device

    \skip static int GetExecStatus
    \until }
    \until }
    \until }
 */
#endif
RDDI int Debug_Stop(
        RDDIHandle handle,
        int deviceNo);

#if DOXYGEN_ACTIVE
/**
    Retrieve the execution status of the target.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
    \param[out] pState The current execution status of the processor:
        \li RDDI_PROC_STATE_UNKNOWN The current execution status of the
            processor is unknown
        \li RDDI_PROC_STATE_RUNNING The processor is running
        \li RDDI_PROC_STATE_STOPPED The processor is stopped. The pCause and
            pDetail parameters contain more information.
    \param[out] pCause If the processor is stopped, specifies the reason:
        \li RDDI_STOP_UNKNOWN Stopped for unknown reason
        \li RDDI_STOP_HALTED Stopped due to client stop request
        \li RDDI_STOP_STEP Stopped after a step
        \li RDDI_STOP_TIMEOUT The ICE forced a stop as a result of a timeout
            (typically a step operation ran away and had to be stopped)
        \li RDDI_STOP_SWBRK Stopped due to hitting a software breakpoint
            (pDetail gives the ID of the breakpoint)
        \li RDDI_STOP_HWBRK_ANY Stopped due to hitting a hardware breakpoint,
            but it is unknown which one specifically
        \li RDDI_STOP_HWBRK Stopped due to a particular hardware breakpoint
            (pDetail gives the ID of the breakpoint)
        \li RDDI_STOP_PROCEVENT Stopped due to a processor event not covered by
            a specific stop cause (pDetail gives the ID of the processor event
            breakpoint).
        \li RDDI_STOP_TARGSTOP The target stopped itself (using a HALT
            instruction or similar)
        \li RDDI_STOP_UNDEF Stopped due to an undefined instruction
        \li RDDI_STOP_ABORT Stopped due to memory access exception
        \li RDDI_STOP_NOPOWER The emulator has detected no power or clock
        \li RDDI_STOP_BUSHANG The processor became locked in a wait state due to
            the bus not responding, so the ICE stopped it.
        \li RDDI_STOP_BUSERR Stopped due to an error on the processor's bus
        \li RDDI_STOP_RESET Stopped due to a reset
        \li RDDI_STOP_INVSTATE The processor is in an invalid state
        \li RDDI_STOP_INT Stopped due to an interrupt
        \li RDDI_STOP_SWI Stopped due to software interrupt/trap call
        \li RDDI_STOP_SYNCH Stopped due to synchronized stop request
        \li RDDI_STOP_WP_SYNC Stopped due to hitting a synchronous, precise
                watchpoint
        \li RDDI_STOP_WP_SYNC_ANY Stopped due to hitting a synchronous,
                imprecise watchpoint
        \li RDDI_STOP_WP_ASYNC Stopped due to hitting an asynchronous, precise
                watchpoint
        \li RDDI_STOP_WP_ASYNC_ANY Stopped due to hitting an asynchronous,
                imprecise watchpoint
    \param[out] pDetail Gives more information for specific stop reasons if the
    processor is stopped and further information is available. This parameter is
    ignored except for the specific cases listed above.
    \param[out] pTripPage If the processor is stopped, the page containing the
    address that caused the stop.
    \param[out] pTripAddress If the processor is stopped, the address that
    caused the stop.

    pCause, pDetail, pTripPage and pTripAddress are optional and may be passed as NULL
    if the details are not required.

    <B>Example</B> - To retrieve the current execution state and display details
    if the processor is stopped:

    \dontinclude device.c
    \skip int state;
    \until tripAddress
    \skip Debug_GetExecStatus
    \until }
 */
#endif
RDDI int Debug_GetExecStatus(
        RDDIHandle handle,
        int deviceNo,
        int *pState,
        int *pCause,
        uint32 *pDetail,
        uint32 *pTripPage,
        uint32 *pTripAddress);

#if DOXYGEN_ACTIVE
/**
    Retrieve the execution status of the target, including any cached register values

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
    \param[out] pState The current execution status of the processor:
        \li RDDI_PROC_STATE_UNKNOWN The current execution status of the
            processor is unknown
        \li RDDI_PROC_STATE_RUNNING The processor is running
        \li RDDI_PROC_STATE_STOPPED The processor is stopped. The pCause and
            pDetail parameters contain more information.
    \param[out] pCause If the processor is stopped, specifies the reason:
        \li RDDI_STOP_UNKNOWN Stopped for unknown reason
        \li RDDI_STOP_HALTED Stopped due to client stop request
        \li RDDI_STOP_STEP Stopped after a step
        \li RDDI_STOP_TIMEOUT The ICE forced a stop as a result of a timeout
            (typically a step operation ran away and had to be stopped)
        \li RDDI_STOP_SWBRK Stopped due to hitting a software breakpoint
            (pDetail gives the ID of the breakpoint)
        \li RDDI_STOP_HWBRK_ANY Stopped due to hitting a hardware breakpoint,
            but it is unknown which one specifically
        \li RDDI_STOP_HWBRK Stopped due to a particular hardware breakpoint
            (pDetail gives the ID of the breakpoint)
        \li RDDI_STOP_PROCEVENT Stopped due to a processor event not covered by
            a specific stop cause (pDetail gives the ID of the processor event
            breakpoint).
        \li RDDI_STOP_TARGSTOP The target stopped itself (using a HALT
            instruction or similar)
        \li RDDI_STOP_UNDEF Stopped due to an undefined instruction
        \li RDDI_STOP_ABORT Stopped due to memory access exception
        \li RDDI_STOP_NOPOWER The emulator has detected no power or clock
        \li RDDI_STOP_BUSHANG The processor became locked in a wait state due to
            the bus not responding, so the ICE stopped it.
        \li RDDI_STOP_BUSERR Stopped due to an error on the processor's bus
        \li RDDI_STOP_RESET Stopped due to a reset
        \li RDDI_STOP_INVSTATE The processor is in an invalid state
        \li RDDI_STOP_INT Stopped due to an interrupt
        \li RDDI_STOP_SWI Stopped due to software interrupt/trap call
        \li RDDI_STOP_SYNCH Stopped due to synchronized stop request
        \li RDDI_STOP_WP_SYNC Stopped due to hitting a synchronous, precise
                watchpoint
        \li RDDI_STOP_WP_SYNC_ANY Stopped due to hitting a synchronous,
                imprecise watchpoint
        \li RDDI_STOP_WP_ASYNC Stopped due to hitting an asynchronous, precise
                watchpoint
        \li RDDI_STOP_WP_ASYNC_ANY Stopped due to hitting an asynchronous,
                imprecise watchpoint
    \param[out] pDetail Gives more information for specific stop reasons if the
    processor is stopped and further information is available. This parameter is
    ignored except for the specific cases listed above.
    \param[out] pTripPage If the processor is stopped, the page containing the
    address that caused the stop.
    \param[out] pTripAddress If the processor is stopped, the address that
    caused the stop.
    \param[in,out] pRegValuesLen The size of the pRegValues buffer in 32-bit words.
    \param[in] pRegValues Buffer to take register values.

    pCause, pDetail, pTripPage pTripAddress, regValuesLen and pRegValues are optional and may be passed as NULL
    if the details are not required.

    regValuesLen and pRegValues are only used by the RVI flavour of RDDI, and only then if the target is halted,
    in which case they receive the current values of the registers contained in the default register list
    RDDI_REG_LIST_DEFAULT (these register values are returned from the target when it halts and are cached at that point).
    If the register values have been invalidated since the halt (by a register or memory write) then zero register values
    will be returned.

    If regValuesLen is NULL then both regValuesLen and pRegValues are ignored. Otherwise if pRegValues is
    NULL or undersized the call will return RDDI_BUFFER_OVERFLOW but pState, pCause, pDetail, pTripPage and
    pTripAddress will have been filled as normal.

    <B>Example</B> - To retrieve the current execution state and display details
    if the processor is stopped:

 */
#endif
RDDI int Debug_GetExecStatusRegs(
        RDDIHandle handle,
        int deviceNo,
        int *pState,
        int *pCause,
        uint32 *pDetail,
        uint32 *pTripPage,
        uint32 *pTripAddress,
        size_t *pRegValuesLen,
        uint32 *pRegValues);

#if DOXYGEN_ACTIVE
/**
    Register a callback function to receive asynchronous events. Until this
    function has been called, asynchronous events are queued inside RDDI and
    must be retrieved using Debug_Poll(). If any asynchronous events are in the
    queue when this function is called then they are delivered via the
    callback. The callback will receive asynchronous events for all connected
    devices and the vehicle itself - i.e. the callback is global and not device
    specific.

    There can be at most one callback associated with an RDDI connection.
    Attempting to call Debug_RegisterCallback() a second time for the same
    RDDIHandle will result in the RDDI_CALLBACK_EXISTS error being returned.
    Once a callback is registered, it persists until the RDDI connection is
    closed with RDDI_Close(). After RDDI_Close() returns, there will be no
    further calls to the callback function.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] pFn Pointer to callback function to receive asynchronous events.
    See Debug_Poll() for details of the callback function parameters (pDetails
    is supplied by RDDI and not by the client).
    \param[in] pContext User specific context to be returned in callback. If context
    is not required this may be passed as NULL.

    <B>Example</B> - To register a callback function:

    \dontinclude exec.c
    \skipline = Debug_RegisterCallback

    \skipline AsyncCallback
    \line {
    \skip it wasn't for us
    \skipline }
 */
#endif
    RDDI int Debug_RegisterCallback(
            RDDIHandle handle,
            void (*pFn)(void *pContext, RDDI_EVENT_TYPE event, const RDDI_Event *pDetails),
            void* pContext);

#if DOXYGEN_ACTIVE
/**
    Poll for asynchronous events. This function retrieves the first event placed
    in RDDI's internal event queue -
    if a callback function has been registered using Debug_RegisterCallback()
    then asynchronous events are delivered via the callback instead of being
    placed on the queue.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[out] pEvent The event type. One of:
        \li RDDI_PROC_STATE_RUNNING Processor is running
        \li RDDI_PROC_STATE_STOPPED Processor is stopped
        \li RDDI_ASYNC_MESSAGE Asynchronous ASCII message
        \li RDDI_ASYNC_COMMS Asynchronous channel data
    \param[out] pDetails A packed structure to receive additional details for
    the async event. This structure contains an additional buffer to receive
    descriptive strings: if this buffer is insufficient to receive all the
    necessary details then the call fails with error RDDI_BUFFER_OVERFLOW.

    If no asynchronous events are ready to be collected then this function
    returns RDDI_NO_ASYNC_EVENTS - this should not be treated as a critical
    failure.

    <B>Example</B> - To poll for an async message and display the results:

    \dontinclude dcc.c
    \skip static int AsyncPoll(RDDIHandle handle)
    \line garbage
    \skip static int AsyncPoll(RDDIHandle handle)
    \skipline RDDI_EVENT_TYPE
    \until free

    \skip static void DecodeEvent(RDDI_EVENT_TYPE eventType, const RDDI_Event *eventDetails)
    \until }
    \until }
    \until }
 */
#endif
RDDI int Debug_Poll(
        RDDIHandle handle,
        RDDI_EVENT_TYPE *pEvent,
        RDDI_Event *pDetails);

#if DOXYGEN_ACTIVE
/**
    Get the set of currently active threads. Only implemented for the GDB server
    version of RDDI.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
    \param[out] pThreadIDs An array for the returned thread IDs
    \param[in]  idsLen size of the threadIds array
    \param[out] pThreadCount the number of threads in the returned list
 */
#endif
RDDI int Debug_GetThreadList(
        RDDIHandle handle,
        int deviceNo,
        uint32* pThreadIDs, size_t idsLen,
        size_t *pThreadCount);

#if DOXYGEN_ACTIVE
/**
    Get the ID of the current thread as returned by the debug server. For
    GDB server this is the ID of the thread that caused the program to stop.
    Not implemented for other versions of RDDI.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
    \param[out] pThreadID the ID of the thread
 */
#endif
RDDI int Debug_GetCurrentThread(
        RDDIHandle handle,
        int deviceNo,
        uint32* pThreadID);

#if DOXYGEN_ACTIVE
/**
    Change the thread used for register and execution operations.
    Only implemented for the GDB server version of RDDI.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
    \param[in] threadID the ID of the thread to use
 */
#endif
RDDI int Debug_SetCurrentThread(
        RDDIHandle handle,
        int deviceNo,
        uint32 threadID);

#if DOXYGEN_ACTIVE
/**
    Gets a list of symbols the vehicle needs resolving

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
    \param[out] pSymbolList An ASCII string containing the list of symbols to be resolved. This is a
                comma separated list of symbol names. The string must be set empty if no symbols
                need resolving at this time.
    \param[in]  symbolListLen size of the pSymbolList array
 */
#endif
RDDI int Debug_GetResolveSymbolsList(
        RDDIHandle handle,
        int deviceNo,
        char *pSymbolList, size_t symbolListLen);

#if DOXYGEN_ACTIVE
/**
    Sets a list of resolved symbols - the list of symbols may have previously been established
    by a call to Debug_GetResolveSymbolRequestList() or via an asynchronous RDDI_RESOLVE_SYMBOLS
    message request.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo Device ID
    \param[out] pResolvedSymbolList An string containing the list of resolved symbols. This is a comma
            separated list of "<symbol>;<value>" pairs.
 */
#endif
RDDI int Debug_SetResolvedSymbolsList(
        RDDIHandle handle,
        int deviceNo,
        const char *pResolvedSymbolList);

#if DOXYGEN_ACTIVE
/**
    Perform an RDDI implementation specific transaction.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] pDataIn data supplied by client for this transaction
    \param[in] dataInLen length in bytes of the data supplied by the client for this transaction
    \param[out] pDataOut Client-supplied buffer to take the results of the transaction
    \param[in] dataOutLen length in bytes of the data buffer supplied by the client for the results of this transaction
    \param[out] dataOutUsedLen The number of bytes of transaction data returned.

 */
#endif
RDDI int Debug_Transaction(
    RDDIHandle handle,
    const uint8* pDataIn,
    size_t dataInLen,
    uint8 *pDataOut,
    size_t dataOutLen,
    size_t* dataOutUsedLen);

#if DOXYGEN_ACTIVE
/**
    Notify a device (or multiple devices) with generic data. The purpose of this method is for
    device-aware components to be notified of debugger events and manage their state
    appropriately.

    \param[in] handle Session handle previously created by RDDI_Open()
    \param[in] deviceNo The device to send the notificaton to, or 0 to broadcast to all devices
    \param[in] messageType An identifier describing the context of this notification
    \param[in] data Generic data to pass to the notification handler
    \param[in] dataLen The length of the data buffer
*/
#endif
RDDI int Debug_Notify(
    RDDIHandle handle,
    int deviceNo,
    uint32 messageType,
    const uint8* data,
    size_t dataLen);

#endif /* def RDDI_DEBUG_H */


/* end of file rddi_debug.h */

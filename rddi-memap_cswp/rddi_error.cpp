// rddi_error.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/*! \file rddi_error.cpp
 *  \brief Descriptive strings for RDDI errors
 */

#include <string>

#include "rddi.h"
#include "rddi_debug.h"
#include "rddi_trace.h"

struct ErrorDetails
{
    unsigned int code;
    const char *pDescription;
};

/*
 * Error descriptive strings
 */

ErrorDetails pErrorDetails[] = {
{RDDI_SUCCESS,                            "Success - no error"},
{RDDI_BADARG,                             "Bad argument to command."},
{RDDI_INVHANDLE,                          "Supplied handle is invalid."},
{RDDI_FAILED,                             ""},
{RDDI_TOOMANYCONNECTIONS,                 "Too many connections are already open."},
{RDDI_BUFFER_OVERFLOW,                    "The supplied buffer is too small."},
{RDDI_BROWSE_FAILED,                      "Browse Failed."},
{RDDI_INTERNAL_ERROR,                     "Unspecified internal error."},
{RDDI_PARSE_FAILED,                       "Failed to parse configuration file."},
{RDDI_NO_ASYNC_EVENTS,                    "No async events available to poll."},
{RDDI_ITEMNOTSUP,                         "Configuration item not supported."},
{RDDI_REGACCESS,                          "Error accessing register."},
{RDDI_BUSERR,                             "Bus error on memory operation."},
{RDDI_TIMEOUT,                            "Timeout on memory operation."},
{RDDI_VERIFY,                             "Verify error on memory operation."},
{RDDI_BADACC,                             "Bad access size for address."},
{RDDI_RWFAIL,                             "General error on memory or register access."},
{RDDI_MEMACCESS,                          "Error accessing memory."},
{RDDI_BADMODE,                            "Bad mode for step/go."},
{RDDI_BADPC,                              "Bad PC address for step/go."},
{RDDI_BADINSTR,                           "Bad instruction for step/go."},
{RDDI_BADSTACK,                           "Bad stack for step/go."},
{RDDI_NOBREAKS,                           "No breakpoints available."},
{RDDI_BREAKFAIL,                          "Failed to set breakpoint."},
{RDDI_CANNOTSTOP,                         "Cannot stop target."},
{RDDI_EXETIMEOUT,                         "Timed out waiting for execution operation to complete."},
{RDDI_INRESET,                            "Target is in reset."},
{RDDI_RUNNING,                            "Target is running, cannot access."},
{RDDI_NOTRUNNING,                         "Target is not running, cannot access."},
{RDDI_CORESTATE_DEBUGPRIV,                "Debug privilege prevents restoring core state for restart."},
{RDDI_SWBRK_MEMERR,                       "Unable to write software breakpoint to memory."},
{RDDI_BRKCLR_BADID,                       "Bad breakpoint ID when clearing a breakpoint."},
{RDDI_SWBRK_NOHWRSRC,                     "Insufficient hardware resources for software breakpoints."},
{RDDI_PROCBRK_NOHWRSRC,                   "Insufficient hardware resources for software breakpoints used to implement processor events."},
{RDDI_SWBRK_DEBUGPRIV,                    "Failed to set/clear software breakpoint due to insufficient debug privilege."},
{RDDI_HWBRK_NORESRC,                      "No hardware resource available to set breakpoint."},
{RDDI_HWBRK_HWFAIL,                       "Failed to access hardware breakpoint resource."},
{RDDI_PROCBRK_HWFAIL,                     "Failed to access processor event hardware."},
{RDDI_DEVUNKNOWN,                         "Device unknown."},
{RDDI_DEVINUSE,                           "Device already has an active connection."},
{RDDI_NOCONN,                             "Connection to device failed or is not allowed."},
{RDDI_NODRIVER,                           "Cannot find requested driver."},
{RDDI_COMMS,                              "Communications error."},
{RDDI_BADDLL,                             "Cannot load DLL."},
{RDDI_STATEFAIL,                          "Cannot attain state requested."},
{RDDI_NOREMOTE,                           "Remote target status unknown."},
{RDDI_ENDIAN,                             "Cannot determine endianess."},
{RDDI_HARDWAREINITFAIL,                   "Remote hardware failed to initialise."},
{RDDI_DEVBUSY,                            "Device busy."},
{RDDI_NOINIT,                             "No connection to target."},
{RDDI_LOSTCONN,                           "Connection lost."},
{RDDI_NOVCC,                              "No voltage detected on target."},
{RDDI_CMDUNKNOWN,                         "Unknown command."},
{RDDI_CMDUNSUPP,                          "Unsupported command."},
{RDDI_TARGFAULT,                          "Target failure. Please check and retry."},
{RDDI_TARGSTATE,                          "Target in wrong state/mode."},
{RDDI_NORESPONSE,                         "Target not responding."},
{RDDI_OUTOFMEM,                           "Out of heap."},
{RDDI_INCOMP_RVMSG,                       "Incompatible protocol version."},
{RDDI_INCOMP_CLIENT,                      "Incompatible client."},
{RDDI_DEBUGPRIV,                          "Debug privileges do not permit operation."},
{RDDI_WRONGIR,                            "Wrong IR length found."},
{RDDI_WRONGDEV,                           "Wrong device count found."},
{RDDI_NOJTAG,                             "No JTAG response."},
{RDDI_OVERWRITE,                          "Attempt to overwrite monitor (code/data)."},
{RDDI_EMUTIMEOUT,                         "Timed out waiting for emulator."},
{RDDI_CHANOVERFLOW,                       "Channel communication buffer overflowed."},
{RDDI_BADCHANNELID,                       "Unknown channel ID."},
{RDDI_NO_VEH_INIT,                        "ICE/Debug controller not initialised."},
{RDDI_CMD_FAILED,                         "Command failed."},
{RDDI_QUEUE_FULL,                         "Async event queue is full."},
{RDDI_QUEUE_EMPTY,                        "Async event queue is empty."},
{RDDI_UNKNOWN_MEMORY,                     "An unknown register or memory error was received."},
{RDDI_STEPRUN,                            "Unable to stop after step - core is now running."},
{RDDI_NOCOREPOWER,                        "Core has powered down."},
{RDDI_COREOSLOCK,                         "OS has locked out debug during OS save restore."},
{RDDI_MONITOR_DEBUG,                      "Operation not possible due to monitor debug mode being selected."},
{RDDI_NODEBUGPOWER,                       "Debug system is not powered."},
{RDDI_UNKNOWN_EXEC,                       "An unknown execution error was received."},
{RDDI_UNKNOWN,                            "An unknown error was received."},
{RDDI_UNKNOWN_GENERIC,                    "An unknown generic/common error was received."},
{RDDI_UNKNOWN_EMU,                        "An unknown emulator error was received."},
{RDDI_UNKNOWN_SIM,                        "An unknown simulator error was received."},
{RDDI_UNKNOWN_RTOS,                       "An unknown RTOS specific error was received."},
{RDDI_FUTURE,                             "An error was received that has been reserved for future use."},
{RDDI_UNKNOWN_INIT,                       "An unknown initialisation/connection error was received."},
{RDDI_ME_LINUX,                           "RVI-ME is not available under Linux."},
{RDDI_ME_NOT_FOUND,                       "The RVI-ME server could not be found"},
{RDDI_ME_NOT_PRESENT,                     "The RVI-ME server does not exist or could not be accessed."},
{RDDI_ME_NOT_STARTED,                     "The RVI-ME server could not be started."},
{RDDI_NO_CONFIG_FILE,                     "Unable to find or open specified configuration file."},
{RDDI_NO_CONFIG,                          "The connection has not been configured."},
{RDDI_NO_CS_ASSOCIATIONS,                 "CoresightAssociations tag not found in configuration file."},
{RDDI_UNKNOWN_CONFIG,                     "An unknown configuration error was received."},
{RDDI_UNKNOWN_BREAK,                      "An unknown breakpoint error was received."},
{RDDI_UNKNOWN_CAP,                        "Unsupported capability block"},
{RDDI_STEP_ABORT,                         "The step failed - possibly because a breakpoint was hit."},
{RDDI_EXTERNAL_CMD_FAILED,                "An external command or application has failed."},
{RDDI_CALLBACK_EXISTS,                    "Only one callback allowed per RDDI connection."},
{RDDI_SESSION_PAUSED,                     "Debug session is paused - no access to target."},
{RDDI_COREOSDLK,                          "OS has locked out debug during powerdown (OS Double Lock)."},
{RDDI_AUTOTUNE_FAILURE,                   "Autotune failure. Please try to reboot target and select a lower frequency using the AutotuneMaxFreq config item."},

// CADI-specific errors
{RDDI_CADI_INFO,                          "Failed to read CADI server information."},
{RDDI_CADI_INIT,                          "Failed to initialise internal CADI interface."},
{RDDI_CADI_TARGET_INFO,                   "Failed to obtain target information from CADI."},
{RDDI_CADI_TARGET_CONNECT,                "Could not connect to the requested target via CADI."},
{RDDI_CADI_TARGET_EXEC,                   "Failed to start execution."},
{RDDI_CADI_TARGET_FEATURES,               "Failed to obtain target feature information."},
{RDDI_CADI_TARGET_REG_GROUPS,             "Failed to obtain register group information."},
{RDDI_CADI_TARGET_REG_UNSUP,              "Unsupported register."},
{RDDI_CADI_TARGET_MEM_REGIONS,            "Failed to obtain memory region information."},
{RDDI_CADI_CALLBACK_FAIL,                 "Failed to set/clear a CADI callback."},
{RDDI_CADI_BAD_BREAK_ID,                  "Unknown breakpoint ID."},
{RDDI_CADI_MODEL_MISMATCH,                "A CADI model server is already running using a different library to the one specified."},
{RDDI_TRACE_COMMSERROR,                   "A communications error occurred between the host and the trace unit."},
{RDDI_TRACE_STREAM_UNSUPP,                "Streaming trace operation unsupported."},
{RDDI_TRACE_INTERFACE_NOT_INIT,           "Trace interface not initialised."},
{RDDI_MAX_ERR,                            ""}
};

std::string GetErrorText(unsigned int errorCode)
{
    std::string defaultStr = "Unknown error";
    std::string result = "";
    bool bFound = false;

    ErrorDetails *pErrorDetail = pErrorDetails;

    while ((!bFound) && (pErrorDetail->code != RDDI_MAX_ERR))
    {
        pErrorDetail++;
        if (errorCode == pErrorDetail->code)
        {
            result = pErrorDetail->pDescription;
            bFound = true;
        }
    }

    if ((!bFound) || (result.length() == 0))
        result = defaultStr;

    return result;
}

/* end of file rddi_error.cpp */


/*
 * rddi_event.h - Event structure for RDDI
 * Copyright (C) 2006 Arm Limited. All rights reserved.
 */


#ifndef RDDI_EVENT_H
#define RDDI_EVENT_H

/**  \file
     \brief This file describes the events which can be delivered to a debug
            client.via Debug_Poll() or via the user call-back function 
            registered using the Debug_RegisterCallback() fuction.

*/
#include "rddi.h"

/**
 * \enum RDDI_EVENT_TYPE 
 * Indicates the type of event which is being delivered to the client
 */
typedef enum
{
    RDDI_ASYNC_MESSAGE,        /*!< a text message (RDDI_Event::pMessage) has
                                    been received from the device */
    RDDI_PROC_STATE_STOPPED,   /*!< a device (RDDI_Event::deviceNo) is now 
                                    stopped. RDDI_Event::stop_details holds stop
                                    information */
    RDDI_PROC_STATE_RUNNING,   /*!< a device (RDDI_Event::deviceNo) is now 
                                    running */
    RDDI_ASYNC_COMMS,          /*!< a comms block has been received from a device
                                    (RDDI_Event::deviceNo) */
    RDDI_STOP_FAILED,          /*!< a stop request to device 
                                    (RDDI_Event::deviceNo) has failed */
    RDDI_PROC_STATE_UNKNOWN,   /*!< that due to error/failure the current 
                                    execution state of the device (RDDI_Event::deviceNo) 
                                    is unknown */
    RDDI_CONNECTION_LOST,      /*!< that the device (or physical interface) is 
                                    no longer responding */
    RDDI_NO_POWER,             /*!< no target power detected */
    RDDI_HELD_IN_RESET,        /*!< target is being held in reset 
                                    (probably nSRST) */
    RDDI_MEM_CPU_ABORT,        /*!< device took an abort exception during a 
                                    memory access */
    RDDI_CSEQ_MEM_AREA,        /*!< a problem has been encountered with the area
                                    of memory used for code sequences */
    RDDI_CSEQ_BPT,             /*!< a code sequence breakpoint is out of range */
    RDDI_CSEQ_N_STOP,          /*!< a code sequence failed to halt */
    RDDI_REG_ID_UNKNOWN,       /*!< an invalid register id was requested */
    RDDI_DBG_WAIT_TIMEOUT,     /*!< a timeout has occurred waiting for debug 
                                    state */
    RDDI_DCC_R_N_READY,        /*!< DCC read failure - data not ready */
    RDDI_DCC_W_FULL,           /*!< DCC write failure - register not empty */
    RDDI_REG_ACC_FAIL,         /*!< register access failed */
    RDDI_TAP_SEQ_RETRIES,      /*!< a TAP sequence has been retried the maximum 
                                    number of times */
    RDDI_INSTR_UNDEF,          /*!< an instruction has caused an undefined 
                                    exception */
    RDDI_PROC_RESET,           /*!< the target has reset */
    RDDI_NULL_EVENT,           /*!< no event has occurred (used as an 
                                    initialisation value) */
    RDDI_PROCESS_TERMINATED,   /*!< the process being debugged has terminated */
    RDDI_RESOLVE_SYMBOLS,      /*!< request to resolve a comma separated list of
                                    symbols */
    RDDI_POWERED_DOWN,         /*!< device has powered down */
    RDDI_SESSION_PAUSE,        /*!< debug session is paused */
} RDDI_EVENT_TYPE;

/**
 * \enum RDDI_EVENT_STOP_CAUSE
 * Defines the value held in the RDDI_Event.stop_details.cause field
 * when the RDDI_Event::eventType is RDDI_EVENT_TYPE::RDDI_PROC_STATE_STOPPED
 */
typedef enum
{
    RDDI_STOP_UNKNOWN,                  /*!< unknown why stopped */
    RDDI_STOP_HALTED,                   /*!< client (or the ICE) stopped it */
    RDDI_STOP_STEP,                     /*!< normal stop on step-inst */
    RDDI_STOP_TIMEOUT,                  /*!< stopped after timeout (typically 
                                             step took off) */
    RDDI_STOP_SWBRK,                    /*!< SW break */
    RDDI_STOP_HWBRK_ANY,                /*!< a HW break but unknown which */
    RDDI_STOP_HWBRK,                    /*!< one of HW breaks by number */
    RDDI_STOP_PROCEVENT,                /*!< processor event by ID */
    RDDI_STOP_TARGSTOP,                 /*!< detected stop (HALT/IDLE instr) */
    RDDI_STOP_UNDEF,                    /*!< illegal opcode/instruction */
    RDDI_STOP_ABORT,                    /*!< memory access violation */
    RDDI_STOP_NOPOWER,                  /*!< detected no power or clock */
    RDDI_STOP_BUSHANG,                  /*!< hung on bus or CPU busy */
    RDDI_STOP_BUSERR,                   /*!< general bus error */
    RDDI_STOP_RESET,                    /*!< detected reset */
    RDDI_STOP_INVSTATE,                 /*!< CPU in invalid state */
    RDDI_STOP_INT,                      /*!< interrupt */
    RDDI_STOP_SWI,                      /*!< software interrupt/trap */
    RDDI_STOP_SYNCH,                    /*!< synch stop */
    RDDI_STOP_SIGNAL,                   /*!< Stopped due to a signal (details indicates signal number)*/
    RDDI_STOP_UNDODB,                   /*!< Stopped due to UndoDB specific cause (detail indicates reason)*/
    RDDI_STOP_WP_SYNC,                  /*!< synchronous watchpoint of known ID */
    RDDI_STOP_WP_SYNC_ANY,              /*!< synchronous watchpoint but unknown which */
    RDDI_STOP_WP_ASYNC,                 /*!< asynchronous watchpoint of known ID */
    RDDI_STOP_WP_ASYNC_ANY              /*!< asynchronous watchpoint but unknown which */
} RDDI_EVENT_STOP_CAUSE;

/**
 * \typedef RDDI_EVENT_CAUSE
 * For backward compatibility, RDDI_EVENT_CAUSE and RDDI_EVENT_STOP_CAUSE are 
   synonymous.
 *
 */
typedef RDDI_EVENT_STOP_CAUSE RDDI_EVENT_CAUSE;

/**
 * \enum RDDI_STOP_UNDODB_DETAIL
 * Used to provide extra information about a stop event when the cause is RDDI_STOP_UNDODB. 
 */
typedef enum
{
  RDDI_STOP_UNDODB_HISTORY_START, /*!< The start of recorded execution history was reached */
  RDDI_STOP_UNDODB_HISTORY_FULL,  /*!< The execution history buffer is full */
  RDDI_STOP_UNDODB_MODE_CHANGE,   /*!< The execution mode changed from replay to record */
  RDDI_STOP_UNDODB_APP_EXIT,      /*!< The debuggee application has reached the end of execution */
} RDDI_STOP_UNDODB_DETAIL;

/**
 * Holds the event information associated with an RDDI_PROC_STATE_STOPPED event
 */
typedef struct 
{
    uint32 cause;               /*!< The reason why the target stopped. See RDDI_EVENT_STOP_CAUSE. */
    uint32 detail;              /*!< Target dependant extra stop information. This is used only for stop
                                     causes RDDI_STOP_SWBRK, RDDI_STOP_HWBRK_ANY and RDDI_STOP_HWBRK, where it
                                     carries the ID of the breakpoint that caused the target to stop. 
                                     When stop cause is RDDI_STOP_SIGNAL this is the number of the signal that
                                     caused the process to stop.
                                     When cause is RDDI_STOP_UNDODB this is set to one of the values from the
                                     RDDI_STOP_UNDODB_DETAIL enum.*/
    uint32 page;                /*!< The page associated with the stop */
    uint32 address;             /*!< The address associated with the stop e.g. S/W break PC or H/W address. */
    uint32 thread;              /*!< The ID of the current thread. */
} RDDI_STOP_EVENT_DETAILS;

/**
 * \enum RDDI_EVENT_START_CAUSE
 * Defines the value held in the RDDI_Event.start_details.cause field
 * when the RDDI_Event::eventType is RDDI_EVENT_TYPE::RDDI_PROC_STATE_RUNNING
 */
typedef enum
{
  RDDI_START_UNKNOWN,           /*!< The target started for an unknown reason.  */
  RDDI_START_GO,                /*!< The target was started by another client. */
  RDDI_START_SYNCH              /*!< The target started as part of a synchronised group. */
} RDDI_EVENT_START_CAUSE;

/**
 * Holds the event information associated with an RDDI_PROC_STATE_RUNNING event
 */
typedef struct
{
    uint32 cause;           /*!< The reason why the target started running. This is of type RDDI_EVENT_START_CAUSE. */
} RDDI_RUNNING_EVENT_DETAILS;

/**
 * Holds the event information associated with an RDDI_PROCESS_TERMINATED event.
 * This event is can only be generated by an RDDI connection to a gdbserver 
 * session; it indicates that the program under debug has finished execution.  
 * The process terminated normally if and only if the signalNumber field is zero.
 * 
 * If this event is received, the device connection should be closed.  All
 * further operations on the device connection will yield the error 
 * RDDI_LOSTCONN.
 */
typedef struct
{
    int exitCode;             /*!< If signalNumber is zero, holds the normal 
                                   exit status of the process. */
    int signalNumber;         /*!< If nonzero, the number of the signal which 
                                   terminated the process.  The meaning of
                                   specific signal numbers is defined by the
                                   target system. */

} RDDI_PROCESS_TERMINATED_EVENT_DETAILS;

/**
 * \enum RDDI_CHAN_STAT
 * Used to provide extra channel status information. 
 */
typedef enum
{
  RDDI_CHSTAT_TXEMPTY =0x0001,  /*!< transmit buffer empty */
  RDDI_CHSTAT_RXEMPTY =0x0002,  /*!< receive buffer empty */
  RDDI_CHSTAT_TXFULL  =0x0004,  /*!< transmit buffer full */
  RDDI_CHSTAT_RXFULL  =0x0008   /*!< receive buffer empty */
} RDDI_CHAN_STAT;


/**
 * Holds the event information associated with an RDDI_ASYNC_COMMS event
 */
typedef struct 
{
    int channelID;     /*!< The channel ID which sourced the message block (see Debug_ChanCtrl()) */
    int outBufSpace;   /*!< The space left in the channel's output buffer. i.e. the number of bytes
                            of data that the channel may currently accept from the client via Debug_ChanSend().*/
    int inBufData;     /*!< The number of bytes of channel data currently queued for the client. This data will be delivered
                            via RDDI_ASYNC_COMMS events if the client is using async callback. Otherwise it may be collected
                            via Debug_Poll() calls.*/
    uint32 flags;      /*!< Flags indicating additional information about the channel status (see RDDI_CHAN_STAT) */
} RDDI_COMMS_EVENT_DETAILS;

/**
 * Holds the event information associated with an RDDI_STOP_FAILED event
 */
typedef struct
{
    uint32 reason;              /*!< reason for failure to stop */
} RDDI_STOP_FAILED_EVENT_DETAILS;

/**
 * Holds a union of all the possible event details
 */
typedef union
{
    RDDI_STOP_EVENT_DETAILS    stop_details;    /*!< holds the event information associated with an RDDI_PROC_STATE_STOPPED event. */
    RDDI_RUNNING_EVENT_DETAILS running_details; /*!< holds the event information associated with an RDDI_PROC_STATE_RUNNING event. */
    RDDI_COMMS_EVENT_DETAILS   comms_details;   /*!< holds the event information associated with an RDDI_ASYNC_COMMS event. */
    RDDI_STOP_FAILED_EVENT_DETAILS stop_failed_details; /*!< holds the event information associated with an RDDI_STOP_FAILED event. */
    RDDI_PROCESS_TERMINATED_EVENT_DETAILS process_terminated_details;  /*!< Holds the event information associated with an RDDI_PROCESS_TERMINATED event. */
} RDDI_EVENT_DETAILS;

/*!
 * \struct RDDI_Event
 * Defines the content delivered within an event
 */
typedef struct
{
    RDDI_EVENT_TYPE   eventType;    /*!< Identifies the type of event being delivered.
                                    This should be ignored as it is used to carry context information that may
                                    be useful to Arm technical staff. The event type is carried by the event parameter
                                    of an asynchronous callback, and the pEvent parameter of Debug_Poll(). */
    int               deviceNo;     /*!< The source of the event */
    uint8            *pMessage;     /*!< Text or raw buffer being delivered. This parameter (and the associated length and used
                                         parameters) are used only for three event types: RDDI_ASYNC_MESSAGE (ASCII message),  
                                         RDDI_RESOLVE_SYMBOLS (ASCII message, comma seperated list of symbols) and 
                                         RDDI_ASYNC_COMMS (raw binary data). */
    size_t            messageLen;   /*!< The size of the buffer area pointed to by RDDI_Event::pMessage. This parameter
                                         has no meaning when async callbacks are used. However, when using Debug_Poll() the 
                                         client must allocate the pMessage buffer and declare the size (in bytes) here. If
                                         the pMessage buffer is not big enough to receive the waiting data then Debug_Poll()
                                         will return RDDI_BUFFER_OVERFLOW. */
    size_t            messageUsed;  /*!< The number of bytes written into RDDI_Event::pMessage. Beware that ASCII strings
                                         may not be NUL-terminated.*/
    RDDI_EVENT_DETAILS details;     /*!< The details event associated with the RDDI_Event::eventType value. These details
                                         vary with the event type. */
} RDDI_Event;

#endif /* def RDDI_EVENT_H */


/* end of file rddi_event.h */

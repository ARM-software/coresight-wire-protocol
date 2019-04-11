// coresight_trace.cpp
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef _UAPI__LINUX_USB_CORESIGHT_TRACE_H
#define _UAPI__LINUX_USB_CORESIGHT_TRACE_H

#define USB_CSTRACE_INTF_REQ_STREAM_TMC_INFO		0

/* Stream configuration commands */
#define USB_CSTRACE_EP_REQ_GET_TMC          0
#define USB_CSTRACE_EP_REQ_SET_TMC          1
#define USB_CSTRACE_EP_REQ_GET_TX_TIMEOUT   2
#define USB_CSTRACE_EP_REQ_SET_TX_TIMEOUT   3
#define USB_CSTRACE_EP_REQ_GET_BUF_SIZE     4
#define USB_CSTRACE_EP_REQ_SET_BUF_SIZE     5
#define USB_CSTRACE_EP_REQ_GET_WATER_MARK   6
#define USB_CSTRACE_EP_REQ_SET_WATER_MARK   7

/* Stream session control commands */
#define USB_CSTRACE_EP_REQ_ATTACH_STREAM    16
#define USB_CSTRACE_EP_REQ_DETACH_STREAM    17
#define USB_CSTRACE_EP_REQ_START_STREAM	    18
#define USB_CSTRACE_EP_REQ_FLUSH_STREAM     19
#define USB_CSTRACE_EP_REQ_STOP_STREAM      20

struct cs_trace_ctl_msg {
    uint16_t streamIdx;
    uint16_t status;
    uint32_t value;
};

enum {
    CS_STREAM_DETACHED,
    CS_STREAM_BUSY,
    CS_STREAM_DETACHING,
    CS_STREAM_ATTACHED,
    CS_STREAM_PREPARE_SESSION,
    CS_STREAM_END_SESSION,
};

#endif

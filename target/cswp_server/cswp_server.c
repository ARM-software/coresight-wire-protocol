// cswp_server.c
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#define _DEFAULT_SOURCE /* for endian.h */

#include <endian.h>
#include <errno.h>
#include <fcntl.h>
#include <pthread.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <poll.h>
#include <signal.h>
#include <strings.h>
#include <arpa/inet.h>
#include <netdb.h>

#include "linux/usb/functionfs.h"

#include "cswp_impl.h"
#include "cswp_server_cmdint.h"
#include "cswp_server_commands.h"
#include "cswp_server_impl.h"
#include "cswp_buffer.h"

#include "common_tcp.h"

#define cpu_to_le16(x)  htole16(x)
#define cpu_to_le32(x)  htole32(x)
#define le32_to_cpu(x)  le32toh(x)
#define le16_to_cpu(x)  le16toh(x)

#define BUFFER_SIZE 32768

#define STR_INTERFACE_ "CSWP"

#define PORT "8192"
#define BACKLOG 1

#define INVALID_FD (-1)

// get sockaddr, IPv4 or IPv6:
static void *get_in_addr(struct sockaddr *sa)
{
    if (sa->sa_family == AF_INET)
        return &(((struct sockaddr_in*)sa)->sin_addr);

    return &(((struct sockaddr_in6*)sa)->sin6_addr);
}

/*
 * Setup USB descriptors
 */
static void ep0_init(int fd)
{
    /*
     * USB descriptors
     */
    const struct {
        struct usb_functionfs_descs_head_v2 header;
        __le32 fs_count;
        __le32 hs_count;
        struct {
            struct usb_interface_descriptor intf;
            struct usb_endpoint_descriptor_no_audio sink;
            struct usb_endpoint_descriptor_no_audio source;
        } __attribute__((packed)) fs_descs, hs_descs;
    } __attribute__((packed)) descriptors = {
        .header = {
            .magic = cpu_to_le32(FUNCTIONFS_DESCRIPTORS_MAGIC_V2),
            .flags = cpu_to_le32(FUNCTIONFS_HAS_FS_DESC |
                                 FUNCTIONFS_HAS_HS_DESC),
            .length = cpu_to_le32(sizeof descriptors),
        },
        .fs_count = cpu_to_le32(3),
        .fs_descs = {
            .intf = {
                .bLength = sizeof descriptors.fs_descs.intf,
                .bDescriptorType = USB_DT_INTERFACE,
                .bNumEndpoints = 2,
                .bInterfaceClass = USB_CLASS_VENDOR_SPEC,
                .iInterface = 1,
            },
            .sink = {
                .bLength = sizeof descriptors.fs_descs.sink,
                .bDescriptorType = USB_DT_ENDPOINT,
                .bEndpointAddress = 11 | USB_DIR_OUT,
                .bmAttributes = USB_ENDPOINT_XFER_BULK,
                /* .wMaxPacketSize = autoconfiguration (kernel) */
            },
            .source = {
                .bLength = sizeof descriptors.fs_descs.source,
                .bDescriptorType = USB_DT_ENDPOINT,
                .bEndpointAddress = 12 | USB_DIR_IN,
                .bmAttributes = USB_ENDPOINT_XFER_BULK,
                /* .wMaxPacketSize = autoconfiguration (kernel) */
            },
        },
        .hs_count = cpu_to_le32(3),
        .hs_descs = {
            .intf = {
                .bLength = sizeof descriptors.fs_descs.intf,
                .bDescriptorType = USB_DT_INTERFACE,
                .bNumEndpoints = 2,
                .bInterfaceClass = USB_CLASS_VENDOR_SPEC,
                .iInterface = 1,
            },
            .sink = {
                .bLength = sizeof descriptors.hs_descs.sink,
                .bDescriptorType = USB_DT_ENDPOINT,
                .bEndpointAddress = 11 | USB_DIR_OUT,
                .bmAttributes = USB_ENDPOINT_XFER_BULK,
                .wMaxPacketSize = cpu_to_le16(512),
            },
            .source = {
                .bLength = sizeof descriptors.hs_descs.source,
                .bDescriptorType = USB_DT_ENDPOINT,
                .bEndpointAddress = 12 | USB_DIR_IN,
                .bmAttributes = USB_ENDPOINT_XFER_BULK,
                .wMaxPacketSize = cpu_to_le16(512),
                .bInterval = 1, /* NAK every 1 uframe */
            },
        },
    };

    ssize_t ret;
    ret = write(fd, &descriptors, sizeof descriptors);

    if (ret < 0) {
        fprintf(stderr, "Failed to write USB descriptors\n");
        exit(EXIT_FAILURE);
    }

    const struct {
        struct usb_functionfs_strings_head header;
        struct {
            __le16 code;
            const char str1[sizeof STR_INTERFACE_];
        } __attribute__((packed)) lang0;
    } __attribute__((packed)) strings = {
        .header = {
            .magic = cpu_to_le32(FUNCTIONFS_STRINGS_MAGIC),
            .length = cpu_to_le32(sizeof strings),
            .str_count = cpu_to_le32(1),
            .lang_count = cpu_to_le32(1),
        },
        .lang0 = {
            cpu_to_le16(0x0409), /* en-us */
            STR_INTERFACE_,
        },
    };

    ret = write(fd, &strings, sizeof strings);
    if (ret < 0) {
        fprintf(stderr, "Failed to write USB strings\n");
        exit(EXIT_FAILURE);
    }
}

/*
 * Server state
 */
typedef struct
{
    int active;
    int outFd;
    int inFd;
    ssize_t (*read_msg)(int fd, void* buf, size_t sz);
    ssize_t (*write_msg)(int fd, void* buf, ssize_t sz);
    pthread_t cmdThreadId;
} server_state_t;

server_state_t gServerState = {
    .active = 0,
    .outFd = INVALID_FD,
    .inFd = INVALID_FD,
};

static void hex_dump(const uint8_t* buf, size_t sz)
{
    int i;

    while (sz > 8)
    {
        for (i = 0; i < 7; ++i)
            vlog(V_TRACE, "%02X ", *buf++);
        vlog(V_TRACE, "%02X\n", *buf++);
        sz -= 8;
    }

    while (sz > 1) {
        vlog(V_TRACE, "%02X ", *buf++);
        sz--;
    }
    if (sz > 0)
        vlog(V_TRACE, "%02X\n", *buf++);
}

static ssize_t write_msg_usb(int fd, void* buf, ssize_t sz)
{
    ssize_t bytesSent = 0;
    const uint8_t* pWrite = buf;
    while (sz - bytesSent > 0)
    {
        ssize_t bytesWritten = write(fd, pWrite, sz-bytesSent);
        if (bytesWritten <= 0)
        {
            int err = errno;
            fprintf(stderr, "Error writing to USB IN endpoint: %lu\n", bytesWritten);
            return -1;
        }

        pWrite += bytesWritten;
        bytesSent += bytesWritten;
    }

    return bytesSent;
}

static int process_commands(server_state_t* state)
{
    CSWP_BUFFER* cmd = cswp_buffer_alloc(BUFFER_SIZE);
    CSWP_BUFFER* rsp = cswp_buffer_alloc(BUFFER_SIZE);

    cswp_server_state_t cswpServer = {0};

    cswpServer.impl = &cswpServerImpl;

    vlog(V_INFO, "Command thread start\n");

    while (state->active)
    {
        cswp_buffer_clear(cmd);

        /* Read command size from bulk OUT endpoint */
        vlog(V_DEBUG, "Waiting for command\n");

        ssize_t bytesRead = state->read_msg(state->outFd, cmd->buf, cmd->size);
        vlog(V_DEBUG, "Read %lu\n", bytesRead);
        if (bytesRead == -1)
        {
            switch (errno)
            {
            case ESHUTDOWN:
                /* USB endpoint has shutdown - e.g. disconnected, go back and wait */
                /* for next command */
                continue;

            default:
                fprintf(stderr, "Error reading data from client: %d: %s\n", errno, strerror(errno));
                break;
            }
        }
        else if (bytesRead == 0)
        {
            /* Client closed connection and read was cancelled, 0 bytes were read */
            /* No error occurred during read */
            vlog(V_INFO, "Read 0 bytes, will try to accept new connection\n");
            break;
        }

        cmd->used = bytesRead;
        hex_dump(cmd->buf, cmd->used);

        /* Check the reported command size matches the amount of data read */
        cswp_buffer_seek(cmd, 0);
        uint32_t cmdSize;
        cswp_buffer_get_uint32(cmd, &cmdSize);
        vlog(V_DEBUG, "Command size: %lu\n", cmd->used);
        if (cmdSize != cmd->used)
        {
            fprintf(stderr, "Warning! expected %u bytes, but read buffer contains %lu\n", cmdSize, cmd->used);
        }

        /* Get the rest of the header */
        varint_t numCmds;
        uint8_t abortOnError;
        cswp_buffer_get_varint(cmd, &numCmds);
        cswp_buffer_get_uint8(cmd, &abortOnError);

        /* Initialise response buffer */
        cswp_buffer_clear(rsp);
        /*   reserve space for response size */
        cswp_buffer_put_uint32(rsp, 0);
        /*   write number of responses */
        cswp_buffer_put_varint(rsp, numCmds);

        /* Process command */
        unsigned c;
        int res = CSWP_SUCCESS;
        for (c = 0; c < numCmds && cmd->pos < cmd->used; ++c)
        {
            res = cswp_handle_command(&cswpServer, cmd, rsp);
            if (res != CSWP_SUCCESS && abortOnError)
                break;
        }

        /* Generate cancelled errors for subsequent commands if abort on error */
        if (res != CSWP_SUCCESS && abortOnError)
        {
            for (; c < numCmds; ++c)
                cswp_encode_error_response(rsp, 0, CSWP_CANCELLED,
                                           "Cancelled");
        }

        vlog(V_DEBUG, "Response size: %lu\n", rsp->used);

        /* Update response size */
        uint8_t* pLen = rsp->buf;
        *pLen++ = (rsp->used & 0xFF);
        *pLen++ = ((rsp->used >> 8) & 0xFF);
        *pLen++ = ((rsp->used >> 16) & 0xFF);
        *pLen++ = ((rsp->used >> 24) & 0xFF);

        hex_dump(rsp->buf, rsp->used);

        /* Send response */
        if (state->write_msg(state->inFd, rsp->buf, rsp->used) == -1)
        {
            fprintf(stderr, "write(%d): %s", errno, strerror(errno));
            break;
        }
    }

    cswp_server_term(&cswpServer);

    cswp_buffer_free(cmd);
    cswp_buffer_free(rsp);

    vlog(V_INFO, "Command thread exit\n");
    fflush(stdout);

    return EXIT_SUCCESS;
}

/*
 * Command processing thread
 *
 * Read commands from OUT endpoint and send responses on IN endpoint
 */
static void* command_thread(void* arg)
{
    server_state_t* state = (server_state_t*)arg;
    return process_commands(state);
}

static void start_command_thread(void)
{
    if (gServerState.active == 0)
    {
        /* Open bulk EPs */
        gServerState.outFd = open("ep1", O_RDWR);
        if (gServerState.outFd < 0) {
            fprintf(stderr, "Failed to open ep1\n");
            exit(1);
        }

        gServerState.inFd = open("ep2", O_RDWR);
        if (gServerState.inFd < 0)
        {
            fprintf(stderr, "Failed to open ep2\n");
            exit(1);
        }

        gServerState.read_msg = read;
        gServerState.write_msg = write_msg_usb;

        gServerState.active = 1;

        if (pthread_create(&gServerState.cmdThreadId, NULL, command_thread, &gServerState) != 0)
        {
            fprintf(stderr, "Failed to start command thread\n");
            exit(1);
        }

        vlog(V_DEBUG, "Command thread started\n");
    }
}

static void stop_command_thread(void)
{
    vlog(V_DEBUG, "Command thread stop\n");

    if (gServerState.active) {
        close(gServerState.outFd);
        close(gServerState.inFd);

        pthread_join(gServerState.cmdThreadId, NULL);

        vlog(V_INFO, "Command thread stopped: %d\n", gServerState.active);
        gServerState.active = 0;
    }
}

static void handle_setup(const struct usb_ctrlrequest *setup)
{
    /* No setup packets defined */
}

/**
 * Kernel driver sends events on EP0 fd
 */
static void ep0_handle(int fd)
{
    static const char *const names[] = {
		[FUNCTIONFS_BIND] = "BIND",
		[FUNCTIONFS_UNBIND] = "UNBIND",
		[FUNCTIONFS_ENABLE] = "ENABLE",
		[FUNCTIONFS_DISABLE] = "DISABLE",
		[FUNCTIONFS_SETUP] = "SETUP",
		[FUNCTIONFS_SUSPEND] = "SUSPEND",
		[FUNCTIONFS_RESUME] = "RESUME",
	};

    ssize_t res;
    struct usb_functionfs_event event;

    /* Read event */
    res = read(fd, &event, sizeof(event));
    if (res < sizeof(event))
    {
        fprintf(stderr, "Failed to read from EP0\n");
        exit(1);
    }

    if (event.type > FUNCTIONFS_RESUME)
    {
        vlog(V_INFO, "Event %03u (unknown)\n", event.type);
        return;
    }

    vlog(V_DEBUG, "Event %s\n", names[event.type]);

    switch (event.type) {
    case FUNCTIONFS_BIND:
        /* Bind to driver */
        start_command_thread();
        break;

    case FUNCTIONFS_UNBIND:
        /* Unbind */
        stop_command_thread();
        break;

    case FUNCTIONFS_ENABLE:
        /* No action: thread is already running */
        break;

    case FUNCTIONFS_DISABLE:
        /* No action: leave thread running until disable */
        break;

    case FUNCTIONFS_SETUP:
        /* Pass to setup function */
        handle_setup(&event.u.setup);
        break;

    case FUNCTIONFS_SUSPEND:
    case FUNCTIONFS_RESUME:
    default:
        break;
    }
}

static int tcp_init(void)
{
    struct addrinfo hints = {0}, *res = 0;
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    int err = getaddrinfo(NULL, PORT, &hints, &res);
    if (err)
    {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(err));
        return INVALID_FD;
    }

    int sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
    if (sockfd == INVALID_FD)
    {
        err = errno;
        fprintf(stderr, "socket errno=%d: %s\n", err, strerror(err));
        goto handle_err_clean_all;
    }

    int yes = 1;
    if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1)
    {
        err = errno;
        fprintf(stderr, "setsockopt errno=%d: %s\n", err, strerror(err));
        goto handle_err_clean_all;
    }

    if (bind(sockfd, res->ai_addr, res->ai_addrlen) == -1)
    {
        err = errno;
        fprintf(stderr, "bind errno=%d: %s\n", err, strerror(err));
        goto handle_err_clean_all;
    }

    freeaddrinfo(res);

    if (listen(sockfd, BACKLOG) == -1)
    {
        err = errno;
        fprintf(stderr, "listen errno=%d: %s\n", err, strerror(err));
        goto handle_err_clean_socket;
    }

    return sockfd;

handle_err_clean_all:
    freeaddrinfo(res);

handle_err_clean_socket:
    if (sockfd != INVALID_FD)
        close(sockfd);

    return INVALID_FD;
}

int main(int argc, char **argv)
{
    /*
     * We need to ignore SIGPIPE for the sockets because this is generated for client shutdown
     * when in a write() calls and is default handled by shutting down the application.
     */
    signal(SIGPIPE, SIG_IGN);

    int ep0Fd;
    int a;
    const char* logFile = 0;
    const char* transport = "";

    int verbose = 0;

    if (argc > 1)
        chdir(argv[1]);
    for (a = 2; a < argc; ++a)
    {
        if (strcmp("-v", argv[a]) == 0)
            ++verbose;
        else if (strcmp("--logfile", argv[a]) == 0 &&
                 a < argc-1)
        {
            logFile = argv[a+1];
            ++a;
        }
        else if (strcmp("--transport", argv[a]) == 0 &&
                 a < argc-1)
        {
            transport = argv[a+1];
            ++a;
        }
    }

    setup_logging(verbose, logFile);

    vlog(V_INFO, "CSWP %s server\n", transport);

    if (strcasecmp(transport, "usb") == 0)
    {
        /* Open EP0 and configure */
        ep0Fd = open("ep0", O_RDWR);
        if (ep0Fd < 0)
        {
            fprintf(stderr, "Failed to open ep0\n");
            exit(EXIT_FAILURE);
        }
        ep0_init(ep0Fd);

        /* Read events from EP0 */
        /* Other EPs will be handled by threads (select/poll doesn't seem to
         * work, preventing a single threaded implementation) */
        while (1)
            ep0_handle(ep0Fd);

    }
    else if (strcasecmp(transport, "tcp") == 0)
    {
        int sockfd = tcp_init();
        if (sockfd == INVALID_FD)
        {
            fprintf(stderr, "Failed to open TCP socket\n");
            exit(EXIT_FAILURE);
        }

        struct sockaddr_storage theirs = {0};
        while (1)
        {
            vlog(V_DEBUG, "Waiting for connections...\n");

            socklen_t sinSz = sizeof(theirs);
            int newfd = accept(sockfd, &theirs, &sinSz);
            if (newfd == INVALID_FD)
            {
                int err = errno;
                if (err == ETIMEDOUT)
                {
                    vlog(V_INFO, "Timeout during accept, will retry\n");
                }
                else
                {
                    fprintf("accept errno=%d: %s\n", err, strerror(err));
                    exit(EXIT_FAILURE);
                }
            }

            char s[INET_ADDRSTRLEN] = {0};
            inet_ntop(theirs.ss_family, get_in_addr(&theirs), s, sizeof(s));
            vlog(V_INFO, "Got connection from %s\n", s);

            if (gServerState.active == 0)
            {
                gServerState.outFd = newfd;
                gServerState.inFd = newfd;
                gServerState.read_msg = cswp_read_msg_tcp;
                gServerState.write_msg = cswp_write_msg_tcp;
            }

            gServerState.active = 1;

            process_commands(&gServerState);

            close(newfd);
            gServerState.active = 0;
            gServerState.inFd = gServerState.outFd = INVALID_FD;
        }

        close(sockfd);
    }
    else
    {
        vlog(V_INFO, "Unrecognized transport\n");
    }

    vlog(V_INFO, "Exiting\n");

    close_logging();

    return 0;
}


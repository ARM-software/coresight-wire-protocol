// common_tcp_test.c
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include <stdlib.h>
#include <stdint.h>
#include <limits.h>

#include "greatest.h"
#include "fff.h"
DEFINE_FFF_GLOBALS;

#ifdef _WIN32
#include <windows.h>
#include <BaseTsd.h>
typedef SSIZE_T ssize_t;

FAKE_VALUE_FUNC(int, recv, int, char*, int, int);
FAKE_VALUE_FUNC(int, send, int, const char*, int, int);
FAKE_VOID_FUNC(WSASetLastError, int);

#define read_fake recv_fake
#define write_fake send_fake
#define read_reset recv_reset
#define write_reset send_reset

#define RWRET int
#else // linux

FAKE_VALUE_FUNC(ssize_t, read, int, void*, size_t);
FAKE_VALUE_FUNC(ssize_t, write, int, const void*, size_t);

#define RWRET ssize_t

#endif

#define FAKE_FD 999

#define SETUP_N_RUN(x) setup();RUN_TEST(x);

/* List of fakes used by this unit tester */
#define FFF_FAKES_LIST(FAKE)            \
  FAKE(read)       \
  FAKE(write)


void setup()
{
    FFF_FAKES_LIST(RESET_FAKE);
    FFF_RESET_HISTORY();
}


TEST test_cswp_readn(void)
{
    ssize_t bytesReadBase = INT_MAX / 512;
    read_fake.return_val = bytesReadBase;

    for (ssize_t sz = bytesReadBase; sz < INT_MAX; sz += bytesReadBase)
    {
        read_fake.call_count = 0;
        ASSERT_EQ(cswp_readn(FAKE_FD, NULL, sz), sz);
        ASSERT_EQ(sz, bytesReadBase * read_fake.call_count);
    }

    RESET_FAKE(read);
    RWRET returnSeq[] = {4, 1, 12};
    int seqLen = sizeof(returnSeq) / sizeof(returnSeq[0]);
    size_t sz = 0;
    for (int i = 0; i < seqLen; ++i)
        sz += returnSeq[i];
    SET_RETURN_SEQ(read, returnSeq, seqLen);

    ASSERT_EQ(cswp_readn(FAKE_FD, NULL, sz), sz);

    PASS();
}


TEST test_cswp_readn__no_read(void)
{
    read_fake.return_val = 0;

    ssize_t b = cswp_readn(FAKE_FD, NULL, INT_MAX);
    ASSERT_EQ(b, read_fake.return_val);
    ASSERT_EQ(read_fake.call_count, 1);

    read_fake.return_val = -1;
    read_fake.call_count = 0;

    b = cswp_readn(FAKE_FD, NULL, INT_MAX);
    ASSERT_EQ(b, read_fake.return_val);
    ASSERT_EQ(read_fake.call_count, 1);

    PASS();
}


TEST test_cswp_write_msg_tcp(void)
{
    ssize_t bytesWriteBase = INT_MAX / 512;
    write_fake.return_val = bytesWriteBase;

    for (ssize_t sz = bytesWriteBase; sz < INT_MAX; sz += bytesWriteBase)
    {
        write_fake.call_count = 0;
        ASSERT_EQ(cswp_write_msg_tcp(FAKE_FD, NULL, sz), sz);
        ASSERT_EQ(write_fake.call_count, sz / bytesWriteBase);
    }

    RESET_FAKE(write);
    RWRET returnSeq[] = {4, 1, 12};
    int seqLen = sizeof(returnSeq) / sizeof(returnSeq[0]);
    size_t sz = 0;
    for (int i = 0; i < seqLen; ++i)
        sz += returnSeq[i];
    SET_RETURN_SEQ(write, returnSeq, seqLen);

    ASSERT_EQ(cswp_write_msg_tcp(FAKE_FD, NULL, sz), sz);
    PASS();
}


TEST test_cswp_write_msg_tcp__no_write(void)
{
    write_fake.return_val = -1;

    ASSERT_EQ(cswp_write_msg_tcp(FAKE_FD, NULL, SIZE_MAX), -1);
    ASSERT_EQ(write_fake.call_count, 1);

    PASS();
}


TEST test_cswp_read_msg_tcp(void)
{
    uint32_t msgLen = sizeof(uint32_t) * 2;
    void* fakeMsgBuf = &msgLen;
    read_fake.return_val = sizeof(msgLen);

    for (; msgLen < UINT16_MAX * 2; msgLen += 1024)
        ASSERT_EQ(cswp_read_msg_tcp(FAKE_FD, fakeMsgBuf, SIZE_MAX), msgLen);

    PASS();
}


TEST test_cswp_read_msg_tcp__edge_cases(void)
{
    uint32_t msgLen = sizeof(uint32_t) * 2;
    void* fakeMsgBuf = &msgLen;

    read_fake.return_val = -1;
    ASSERT_EQ(cswp_read_msg_tcp(FAKE_FD, fakeMsgBuf, SIZE_MAX), -1);

    read_fake.return_val = 0;
    ASSERT_EQ(cswp_read_msg_tcp(FAKE_FD, fakeMsgBuf, SIZE_MAX), 0);

    read_fake.return_val = 1;
    msgLen = UINT32_MAX;

    ASSERT_EQ(cswp_read_msg_tcp(FAKE_FD, fakeMsgBuf, UINT16_MAX), UINT16_MAX);

    PASS();
}


SUITE(s) {
    SETUP_N_RUN(test_cswp_readn);
    SETUP_N_RUN(test_cswp_readn__no_read);
    SETUP_N_RUN(test_cswp_write_msg_tcp);
    SETUP_N_RUN(test_cswp_write_msg_tcp__no_write);

    SETUP_N_RUN(test_cswp_read_msg_tcp);
    SETUP_N_RUN(test_cswp_read_msg_tcp__edge_cases);
}


GREATEST_MAIN_DEFS();


int main(int argc, char** argv)
{
    GREATEST_MAIN_BEGIN();
    RUN_SUITE(s);
    GREATEST_MAIN_END();
    return EXIT_SUCCESS;
}


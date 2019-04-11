// cswp_buffer_test.c
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "cswp_buffer.h"
#include "cswp_test.h"
#include <string.h>


static void test_init()
{
    uint8_t* rawbuf = malloc(1024);
    CSWP_BUFFER* buf = (CSWP_BUFFER*)rawbuf;
    cswp_buffer_init(buf, 100);
    CHECK_EQUAL(100, buf->size);
    CHECK_EQUAL(0, buf->used);
    free(rawbuf);
}

/*
 * Check that buffers are allocated and indexes are set-up correctly
 */
static void test_allocate()
{
    CSWP_BUFFER *buf1, *buf2, *buf3;

    buf1 = cswp_buffer_alloc(1);
    CHECK_EQUAL(1, buf1->size);
    CHECK_EQUAL(0, buf1->used);
    cswp_buffer_free(buf1);

    buf2 = cswp_buffer_alloc(2);
    CHECK_EQUAL(2, buf2->size);
    CHECK_EQUAL(0, buf2->used);
    cswp_buffer_free(buf2);

    buf3 = cswp_buffer_alloc(1024);
    CHECK_EQUAL(1024, buf3->size);
    CHECK_EQUAL(0, buf3->used);
    cswp_buffer_free(buf3);
}

/*
 * Check that buffers are encoded correctly
 */
static void test_encode()
{
    CSWP_BUFFER* buf0 = cswp_buffer_alloc(0);
    CSWP_BUFFER* buf1 = cswp_buffer_alloc(1);
    CSWP_BUFFER* buf3 = cswp_buffer_alloc(3);
    CSWP_BUFFER* buf7 = cswp_buffer_alloc(7);
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);
    char* big_string;

    // uint8

    // full buffer
    CHECK_EQUAL(CSWP_BUFFER_FULL, cswp_buffer_put_uint8(buf0, 1));
    CHECK_EQUAL(0, buf0->pos);
    CHECK_EQUAL(0, buf0->used);

    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_put_uint8(buf, 1));
    CHECK_EQUAL(1, buf->pos);
    CHECK_EQUAL(1, buf->used);
    CHECK_CONTENTS("\x01", buf->buf, buf->used);

    // now full
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_put_uint8(buf1, 1));
    CHECK_EQUAL(CSWP_BUFFER_FULL, cswp_buffer_put_uint8(buf1, 1));
    CHECK_EQUAL(1, buf1->pos);
    CHECK_EQUAL(1, buf1->used);

    cswp_buffer_clear(buf1);
    cswp_buffer_clear(buf);
    CHECK_EQUAL(0, buf1->pos);
    CHECK_EQUAL(0, buf1->used);
    CHECK_EQUAL(0, buf->pos);
    CHECK_EQUAL(0, buf->used);

    // uint32

    // full / insufficient space buffers
    CHECK_EQUAL(CSWP_BUFFER_FULL, cswp_buffer_put_uint32(buf0, 0x12345678));
    CHECK_EQUAL(0, buf0->pos);
    CHECK_EQUAL(0, buf0->used);
    CHECK_EQUAL(CSWP_BUFFER_FULL, cswp_buffer_put_uint32(buf1, 0x12345678));
    CHECK_EQUAL(0, buf1->pos);
    CHECK_EQUAL(0, buf1->used);
    CHECK_EQUAL(CSWP_BUFFER_FULL, cswp_buffer_put_uint32(buf3, 0x12345678));
    CHECK_EQUAL(0, buf3->pos);
    CHECK_EQUAL(0, buf3->used);

    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_put_uint32(buf, 0x12345678));
    CHECK_EQUAL(4, buf->pos);
    CHECK_EQUAL(4, buf->used);
    CHECK_CONTENTS("\x78\x56\x34\x12", buf->buf, buf->used);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_put_uint32(buf, 0xAA55AA55));
    CHECK_EQUAL(8, buf->pos);
    CHECK_EQUAL(8, buf->used);
    CHECK_CONTENTS("\x78\x56\x34\x12\x55\xAA\x55\xAA", buf->buf, buf->used);

    // now full
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_put_uint32(buf7, 1));
    CHECK_EQUAL(CSWP_BUFFER_FULL, cswp_buffer_put_uint32(buf7, 2));
    CHECK_EQUAL(4, buf7->pos);
    CHECK_EQUAL(4, buf7->used);

    cswp_buffer_clear(buf7);
    cswp_buffer_clear(buf1);
    cswp_buffer_clear(buf);

    // uint64

    // full / insufficient space buffers
    CHECK_EQUAL(CSWP_BUFFER_FULL, cswp_buffer_put_uint64(buf0, 0x12345678));
    CHECK_EQUAL(0, buf0->pos);
    CHECK_EQUAL(0, buf0->used);
    CHECK_EQUAL(CSWP_BUFFER_FULL, cswp_buffer_put_uint64(buf1, 0x12345678));
    CHECK_EQUAL(0, buf1->pos);
    CHECK_EQUAL(0, buf1->used);
    CHECK_EQUAL(CSWP_BUFFER_FULL, cswp_buffer_put_uint64(buf7, 0x12345678));
    CHECK_EQUAL(0, buf7->pos);
    CHECK_EQUAL(0, buf7->used);

    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_put_uint64(buf, 0x123456789ABCDEF0));
    CHECK_EQUAL(8, buf->pos);
    CHECK_EQUAL(8, buf->used);
    CHECK_CONTENTS("\xF0\xDE\xBC\x9A\x78\x56\x34\x12", buf->buf, buf->used);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_put_uint64(buf, 0xAA55AA55AA55AA55));
    CHECK_EQUAL(16, buf->pos);
    CHECK_EQUAL(16, buf->used);
    CHECK_CONTENTS("\xF0\xDE\xBC\x9A\x78\x56\x34\x12\x55\xAA\x55\xAA\x55\xAA\x55\xAA", buf->buf, buf->used);

    cswp_buffer_clear(buf7);
    cswp_buffer_clear(buf1);
    cswp_buffer_clear(buf);

    // varint

    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_put_varint(buf, 0x01));
    CHECK_EQUAL(1, buf->pos);
    CHECK_EQUAL(1, buf->used);
    CHECK_CONTENTS("\x01", buf->buf, buf->used);
    cswp_buffer_clear(buf);

    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_put_varint(buf, 0x7F));
    CHECK_EQUAL(1, buf->pos);
    CHECK_EQUAL(1, buf->used);
    CHECK_CONTENTS("\x7F", buf->buf, buf->used);
    cswp_buffer_clear(buf);

    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_put_varint(buf, 0x80));
    CHECK_EQUAL(2, buf->pos);
    CHECK_EQUAL(2, buf->used);
    CHECK_CONTENTS("\x80\x01", buf->buf, buf->used);
    cswp_buffer_clear(buf);

    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_put_varint(buf, 0x3FFF));
    CHECK_EQUAL(2, buf->pos);
    CHECK_EQUAL(2, buf->used);
    CHECK_CONTENTS("\xFF\x7F", buf->buf, buf->used);
    cswp_buffer_clear(buf);

    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_put_varint(buf, 0x4000));
    CHECK_EQUAL(3, buf->pos);
    CHECK_EQUAL(3, buf->used);
    CHECK_CONTENTS("\x80\x80\x01", buf->buf, buf->used);
    cswp_buffer_clear(buf);

    //   full 32 bits
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_put_varint(buf, 0xFFFFFFFF));
    CHECK_EQUAL(5, buf->pos);
    CHECK_EQUAL(5, buf->used);
    CHECK_CONTENTS("\xFF\xFF\xFF\xFF\x0F", buf->buf, buf->used);
    cswp_buffer_clear(buf);

    //   buffer too small
    CHECK_EQUAL(CSWP_BUFFER_FULL, cswp_buffer_put_varint(buf0, 0x7F));
    CHECK_EQUAL(0, buf0->pos);
    CHECK_EQUAL(0, buf0->used);
    CHECK_EQUAL(CSWP_BUFFER_FULL, cswp_buffer_put_varint(buf1, 0x80));
    CHECK_EQUAL(0, buf1->pos);
    CHECK_EQUAL(0, buf1->used);

    // strings
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_put_string(buf, "Hello"));
    CHECK_EQUAL(6, buf->pos);
    CHECK_EQUAL(6, buf->used);
    CHECK_CONTENTS("\x05Hello", buf->buf, buf->used);
    cswp_buffer_clear(buf);

    //  2 bytes used for length
    big_string = malloc(129);
    memset(big_string, 'A', 128);
    big_string[128] = '\0';
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_put_string(buf, big_string));
    CHECK_EQUAL(2 + 128, buf->pos);
    CHECK_EQUAL(2 + 128, buf->used);
    CHECK_CONTENTS("\x80\x01""AAAAAAAA", buf->buf, 10);
    cswp_buffer_clear(buf);
    free(big_string);

    //   buffer too small
    CHECK_EQUAL(CSWP_BUFFER_FULL, cswp_buffer_put_string(buf0, "A"));
    CHECK_EQUAL(0, buf0->pos);
    CHECK_EQUAL(0, buf0->used);
    CHECK_EQUAL(CSWP_BUFFER_FULL, cswp_buffer_put_string(buf1, "A"));
    CHECK_EQUAL(1, buf1->pos); // encoded length, but not data
    CHECK_EQUAL(1, buf1->used);
    CHECK_EQUAL(CSWP_BUFFER_FULL, cswp_buffer_put_string(buf3, "ABC"));
    CHECK_EQUAL(1, buf3->pos);
    CHECK_EQUAL(1, buf3->used);

    // arrays
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_put_data(buf, "Hello", 5));
    CHECK_EQUAL(5, buf->pos);
    CHECK_EQUAL(5, buf->used);
    CHECK_CONTENTS("Hello", buf->buf, buf->used);
    cswp_buffer_clear(buf);

    cswp_buffer_free(buf0);
    cswp_buffer_free(buf1);
    cswp_buffer_free(buf3);
    cswp_buffer_free(buf7);
    cswp_buffer_free(buf);
}

/*
 * Check that buffers can be decoded correctly
 */
static void test_decode()
{
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);
    uint8_t u8_1, u8_2, u8_3, u8_4;
    uint32_t u32_1, u32_2;
    uint64_t u64_1;
    varint_t v_1, v_2;
    char strBuf[1024];
    void *pDirect;
    uint8_t u8_data[9];

    // uint8
    cswp_buffer_set(buf, "\x01", 1);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_uint8(buf, &u8_1));
    CHECK_EQUAL(1, u8_1);
    CHECK_EQUAL(1, buf->pos);

    // no more data
    CHECK_EQUAL(CSWP_BUFFER_EMPTY, cswp_buffer_get_uint8(buf, &u8_1));
    CHECK_EQUAL(1, buf->pos);

    cswp_buffer_set(buf, "\x02", 1);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_uint8(buf, &u8_1));
    CHECK_EQUAL(2, u8_1);
    CHECK_EQUAL(1, buf->pos);

    // multiple entries
    cswp_buffer_set(buf, "\x01\x02\x03\x04", 4);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_uint8(buf, &u8_1));
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_uint8(buf, &u8_2));
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_uint8(buf, &u8_3));
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_uint8(buf, &u8_4));
    CHECK_EQUAL(1, u8_1);
    CHECK_EQUAL(2, u8_2);
    CHECK_EQUAL(3, u8_3);
    CHECK_EQUAL(4, u8_4);
    // no more data
    CHECK_EQUAL(CSWP_BUFFER_EMPTY, cswp_buffer_get_uint8(buf, &u8_1));
    CHECK_EQUAL(4, buf->pos);

    // uint32
    cswp_buffer_set(buf, "\x01\x02\x03\x04", 4);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_uint32(buf, &u32_1));
    CHECK_EQUAL(0x04030201, u32_1);
    CHECK_EQUAL(4, buf->pos);

    cswp_buffer_set(buf, "\xFF\xFE\xFD\xFC", 4);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_uint32(buf, &u32_1));
    CHECK_EQUAL(0xFCFDFEFF, u32_1);

    cswp_buffer_set(buf, "\x01\x02\x03\x04\xFF\xFE\xFD\xFC", 8);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_uint32(buf, &u32_1));
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_uint32(buf, &u32_2));
    CHECK_EQUAL(0x04030201, u32_1);
    CHECK_EQUAL(0xFCFDFEFF, u32_2);
    CHECK_EQUAL(8, buf->pos);

    // no more data
    CHECK_EQUAL(CSWP_BUFFER_EMPTY, cswp_buffer_get_uint32(buf, &u32_1));
    CHECK_EQUAL(8, buf->pos);

    // insufficient data
    cswp_buffer_set(buf, "\x01", 1);
    CHECK_EQUAL(CSWP_BUFFER_EMPTY, cswp_buffer_get_uint32(buf, &u32_1));
    CHECK_EQUAL(0, buf->pos);
    cswp_buffer_set(buf, "\x01\x02", 2);
    CHECK_EQUAL(CSWP_BUFFER_EMPTY, cswp_buffer_get_uint32(buf, &u32_1));
    CHECK_EQUAL(0, buf->pos);
    cswp_buffer_set(buf, "\x01\x02\x03", 3);
    CHECK_EQUAL(CSWP_BUFFER_EMPTY, cswp_buffer_get_uint32(buf, &u32_1));
    CHECK_EQUAL(0, buf->pos);

    // uint64
    cswp_buffer_set(buf, "\x01\x02\x03\x04\xFF\xFE\xFD\xFC", 8);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_uint64(buf, &u64_1));
    CHECK_EQUAL(0xFCFDFEFF04030201, u64_1);
    CHECK_EQUAL(8, buf->pos);

    // no more data
    CHECK_EQUAL(CSWP_BUFFER_EMPTY, cswp_buffer_get_uint64(buf, &u64_1));
    CHECK_EQUAL(8, buf->pos);

    // insufficient data
    cswp_buffer_set(buf, "\x01", 1);
    CHECK_EQUAL(CSWP_BUFFER_EMPTY, cswp_buffer_get_uint64(buf, &u64_1));
    CHECK_EQUAL(0, buf->pos);
    cswp_buffer_set(buf, "\x01\x02", 2);
    CHECK_EQUAL(CSWP_BUFFER_EMPTY, cswp_buffer_get_uint64(buf, &u64_1));
    CHECK_EQUAL(0, buf->pos);
    cswp_buffer_set(buf, "\x01\x02\x03\x04\x05\x06\x07", 7);
    CHECK_EQUAL(CSWP_BUFFER_EMPTY, cswp_buffer_get_uint64(buf, &u64_1));
    CHECK_EQUAL(0, buf->pos);

    // varint
    cswp_buffer_set(buf, "\x01", 1);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_varint(buf, &v_1));
    CHECK_EQUAL(0x01, v_1);
    CHECK_EQUAL(1, buf->pos);

    cswp_buffer_set(buf, "\x01\x01", 2);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_varint(buf, &v_1));
    CHECK_EQUAL(0x01, v_1);
    CHECK_EQUAL(1, buf->pos);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_varint(buf, &v_2));
    CHECK_EQUAL(0x01, v_2);
    CHECK_EQUAL(2, buf->pos);

    cswp_buffer_set(buf, "\x80\x01", 2);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_varint(buf, &v_1));
    CHECK_EQUAL(0x80, v_1);
    CHECK_EQUAL(2, buf->pos);

    cswp_buffer_set(buf, "\xFF\xFF\xFF\x01\x01", 5);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_varint(buf, &v_1));
    CHECK_EQUAL(0x3FFFFF, v_1);
    CHECK_EQUAL(4, buf->pos);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_varint(buf, &v_2));
    CHECK_EQUAL(0x01, v_2);
    CHECK_EQUAL(5, buf->pos);

    // unterminated varint
    cswp_buffer_set(buf, "\x80\x80\x80", 3);
    CHECK_EQUAL(CSWP_BUFFER_EMPTY, cswp_buffer_get_varint(buf, &v_1));
    CHECK_EQUAL(3, buf->pos);

    // string
    strcpy(strBuf, "BAD");
    cswp_buffer_set(buf, "\x00", 1);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_string(buf, strBuf, sizeof(strBuf)));
    CHECK_EQUAL('\0', strBuf[0]);
    CHECK_EQUAL(0, strcmp(strBuf, ""));
    CHECK_EQUAL(1, buf->pos);

    cswp_buffer_set(buf, "\x01""A", 2);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_string(buf, strBuf, sizeof(strBuf)));
    CHECK_EQUAL('A', strBuf[0]);
    CHECK_EQUAL('\0', strBuf[1]);
    CHECK_EQUAL(0, strcmp(strBuf, "A"));
    CHECK_EQUAL(2, buf->pos);

    cswp_buffer_set(buf, "\x01""A\x01""B", 4);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_string(buf, strBuf, sizeof(strBuf)));
    CHECK_EQUAL('A', strBuf[0]);
    CHECK_EQUAL('\0', strBuf[1]);
    CHECK_EQUAL(0, strcmp(strBuf, "A"));
    CHECK_EQUAL(2, buf->pos);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_string(buf, strBuf, sizeof(strBuf)));
    CHECK_EQUAL('B', strBuf[0]);
    CHECK_EQUAL('\0', strBuf[1]);
    CHECK_EQUAL(0, strcmp(strBuf, "B"));
    CHECK_EQUAL(4, buf->pos);

    cswp_buffer_set(buf, "\x06""ABCDEF\x01""X", 9);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_string(buf, strBuf, sizeof(strBuf)));
    CHECK_EQUAL('\0', strBuf[6]);
    CHECK_EQUAL(0, strcmp(strBuf, "ABCDEF"));
    CHECK_EQUAL(7, buf->pos);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_string(buf, strBuf, sizeof(strBuf)));
    CHECK_EQUAL('X', strBuf[0]);
    CHECK_EQUAL('\0', strBuf[1]);
    CHECK_EQUAL(0, strcmp(strBuf, "X"));
    CHECK_EQUAL(9, buf->pos);

    // unterminated string
    cswp_buffer_set(buf, "\x03""AB", 3);
    CHECK_EQUAL(CSWP_BUFFER_EMPTY, cswp_buffer_get_string(buf, strBuf, sizeof(strBuf)));

    // empty buffer
    cswp_buffer_set(buf, "", 0);
    CHECK_EQUAL(CSWP_BUFFER_EMPTY, cswp_buffer_get_string(buf, strBuf, sizeof(strBuf)));

    // output overflow
    cswp_buffer_set(buf, "\x02""AB", 3);
    CHECK_EQUAL(CSWP_OUTPUT_BUFFER_OVERFLOW, cswp_buffer_get_string(buf, strBuf, 1));
    cswp_buffer_set(buf, "\x02""AB", 3);
    CHECK_EQUAL(CSWP_OUTPUT_BUFFER_OVERFLOW, cswp_buffer_get_string(buf, strBuf, 2));

    // direct access
    cswp_buffer_set(buf, "\x06""ABCDEF\x01""X", 9);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_varint(buf, &v_1));
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_direct(buf, &pDirect, 6));
    CHECK_EQUAL(0, memcmp(pDirect, "ABCDEF", 6));
    CHECK_EQUAL(7, buf->pos);

    // data
    cswp_buffer_set(buf, "\x01\x02\x03\x04\x05\x06\x07\x08\x09", 9);
    CHECK_EQUAL(CSWP_SUCCESS, cswp_buffer_get_data(buf, u8_data, 9));
    CHECK_EQUAL(0, memcmp(u8_data, "\x01\x02\x03\x04\x05\x06\x07\x08\x09", 9));
    CHECK_EQUAL(9, buf->pos);
    cswp_buffer_free(buf);
}

static void test_seek()
{
    CSWP_BUFFER* buf = cswp_buffer_alloc(1024);

    cswp_buffer_set(buf, "\x01\x02\x03\x04\x05\x06\x07", 7);

    cswp_buffer_seek(buf, 0);
    CHECK_EQUAL(0, buf->pos);

    cswp_buffer_seek(buf, 6);
    CHECK_EQUAL(6, buf->pos);

    cswp_buffer_free(buf);
}

void test_buffer()
{
    test_init();
    test_allocate();
    test_encode();
    test_decode();
    test_seek();
}

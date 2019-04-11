// cswp_buffer.c
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "cswp_buffer.h"

#include <string.h>

/* TODO: skip fields if null is passed */
void cswp_buffer_init(CSWP_BUFFER* buffer, size_t size)
{
    buffer->size = size;
    buffer->used = 0;
    buffer->pos = 0;
}

CSWP_BUFFER* cswp_buffer_alloc(size_t size)
{
    void* p = malloc(sizeof(CSWP_BUFFER) + size);
    CSWP_BUFFER* buf = (CSWP_BUFFER*)p;
    cswp_buffer_init(buf, size);
    return buf;
}

void cswp_buffer_free(CSWP_BUFFER* buf)
{
    free(buf);
}

void cswp_buffer_clear(CSWP_BUFFER* buf)
{
    buf->used = 0;
    buf->pos = 0;
}

void cswp_buffer_seek(CSWP_BUFFER* buf, size_t pos)
{
    buf->pos = pos;
}

void cswp_buffer_skip(CSWP_BUFFER* buf, size_t count)
{
    buf->pos += count;
}

#define __CSWP_REQUIRE_W(b, s) if ((b)->size - (b)->used < (s)) return CSWP_BUFFER_FULL;
#define __CSWP_REQUIRE_R(b, s) if ((b)->used - (b)->pos < (s)) return CSWP_BUFFER_EMPTY;
#define __CSWP_PUT_BYTE(b, v) (b)->buf[(b)->pos++] = (v);
#define __CSWP_GET_BYTE(b) (b)->buf[(b)->pos++]

int cswp_buffer_put_uint8(CSWP_BUFFER* buf, uint8_t val)
{
    __CSWP_REQUIRE_W(buf, 1);
    __CSWP_PUT_BYTE(buf, val);
    buf->used = buf->pos;
    return CSWP_SUCCESS;
}

int cswp_buffer_put_uint32(CSWP_BUFFER* buf, uint32_t val)
{
    __CSWP_REQUIRE_W(buf, 4);
    __CSWP_PUT_BYTE(buf, (val & 0xFF));
    __CSWP_PUT_BYTE(buf, ((val >> 8) & 0xFF));
    __CSWP_PUT_BYTE(buf, ((val >> 16) & 0xFF));
    __CSWP_PUT_BYTE(buf, ((val >> 24) & 0xFF));
    buf->used = buf->pos;
    return CSWP_SUCCESS;
}

int cswp_buffer_put_uint64(CSWP_BUFFER* buf, uint64_t val)
{
    __CSWP_REQUIRE_W(buf, 8);
    __CSWP_PUT_BYTE(buf, (val & 0xFF));
    __CSWP_PUT_BYTE(buf, ((val >> 8) & 0xFF));
    __CSWP_PUT_BYTE(buf, ((val >> 16) & 0xFF));
    __CSWP_PUT_BYTE(buf, ((val >> 24) & 0xFF));
    __CSWP_PUT_BYTE(buf, ((val >> 32) & 0xFF));
    __CSWP_PUT_BYTE(buf, ((val >> 40) & 0xFF));
    __CSWP_PUT_BYTE(buf, ((val >> 48) & 0xFF));
    __CSWP_PUT_BYTE(buf, ((val >> 56) & 0xFF));
    buf->used = buf->pos;
    return CSWP_SUCCESS;
}

int cswp_buffer_put_varint(CSWP_BUFFER* buf, varint_t val)
{
    size_t rq = 1;
    varint_t tmp;

    // calculate space required
    for (tmp = val; tmp > 0x7F; tmp >>= 7)
        ++rq;
    __CSWP_REQUIRE_W(buf, rq);

    while (val > 0x7F)
    {
        __CSWP_PUT_BYTE(buf, 0x80 | (val & 0x7F));
        val >>= 7;
    }
    __CSWP_PUT_BYTE(buf, val & 0x7F);
    buf->used = buf->pos;
    return CSWP_SUCCESS;
}

int cswp_buffer_put_string(CSWP_BUFFER* buf, const char* str)
{
    size_t len = strlen(str);
    int res = cswp_buffer_put_varint(buf, len);
    if (res != CSWP_SUCCESS)
        return res;

    __CSWP_REQUIRE_W(buf, len);
    memcpy(&buf->buf[buf->pos], str, len);
    buf->pos += len;
    buf->used = buf->pos;
    return CSWP_SUCCESS;
}

int cswp_buffer_put_data(CSWP_BUFFER* buf, const void* data, size_t size)
{
    __CSWP_REQUIRE_W(buf, size);
    memcpy(&buf->buf[buf->pos], data, size);
    buf->pos += size;
    buf->used = buf->pos;
    return CSWP_SUCCESS;
}

int cswp_buffer_get_uint8(CSWP_BUFFER* buf, uint8_t* val)
{
    __CSWP_REQUIRE_R(buf, 1);
    *val = __CSWP_GET_BYTE(buf);
    return CSWP_SUCCESS;
}

int cswp_buffer_get_uint32(CSWP_BUFFER* buf, uint32_t* val)
{
    uint32_t v;
    __CSWP_REQUIRE_R(buf, 4);
    v = __CSWP_GET_BYTE(buf);
    v |= (__CSWP_GET_BYTE(buf)) << 8;
    v |= (__CSWP_GET_BYTE(buf)) << 16;
    v |= (__CSWP_GET_BYTE(buf)) << 24;
    *val = v;
    return CSWP_SUCCESS;
}

int cswp_buffer_get_uint64(CSWP_BUFFER* buf, uint64_t* val)
{
    uint64_t v;
    __CSWP_REQUIRE_R(buf, 8);
    v = __CSWP_GET_BYTE(buf);
    v |= ((uint64_t)__CSWP_GET_BYTE(buf)) << 8;
    v |= ((uint64_t)__CSWP_GET_BYTE(buf)) << 16;
    v |= ((uint64_t)__CSWP_GET_BYTE(buf)) << 24;
    v |= ((uint64_t)__CSWP_GET_BYTE(buf)) << 32;
    v |= ((uint64_t)__CSWP_GET_BYTE(buf)) << 40;
    v |= ((uint64_t)__CSWP_GET_BYTE(buf)) << 48;
    v |= ((uint64_t)__CSWP_GET_BYTE(buf)) << 56;
    *val = v;
    return CSWP_SUCCESS;
}

int cswp_buffer_get_varint(CSWP_BUFFER* buf, varint_t* val)
{
    uint8_t b;
    varint_t v = 0;
    unsigned s = 0;
    while (1)
    {
        __CSWP_REQUIRE_R(buf, 1);
        b = __CSWP_GET_BYTE(buf);
        v |= (b & 0x7F) << s;
        if ((b & 0x80) == 0)
            break;
        s += 7;
    }
    *val = v;
    return CSWP_SUCCESS;
}

int cswp_buffer_get_string(CSWP_BUFFER* buf, char* str, size_t strSize)
{
    varint_t len;
    int res;
    res = cswp_buffer_get_varint(buf, &len);
    if (res != CSWP_SUCCESS)
        return res;
    __CSWP_REQUIRE_R(buf, len);
    if (len >= strSize) // leave room for null terminator
        return CSWP_OUTPUT_BUFFER_OVERFLOW;
    memcpy(str, &buf->buf[buf->pos], len);
    str[len] = '\0';
    buf->pos += len;
    return CSWP_SUCCESS;
}

int cswp_buffer_get_direct(CSWP_BUFFER* buf, void** data, size_t len)
{
    __CSWP_REQUIRE_R(buf, len);
    *data = &buf->buf[buf->pos];;
    buf->pos += len;
    return CSWP_SUCCESS;
}

int cswp_buffer_get_data(CSWP_BUFFER* buf, void* data, size_t size)
{
    __CSWP_REQUIRE_R(buf, size);
    memcpy(data, &buf->buf[buf->pos], size);
    buf->pos += size;
    return CSWP_SUCCESS;
}

/* end of file cswp_buffer.c */

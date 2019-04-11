// cswp_test.c
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#include "cswp_test.h"

#include "cswp_buffer.h"
#include <stdio.h>
#include <string.h>

extern void test_buffer();
extern void test_commands();
extern void test_server();

static int failures;

void __fail(const char* msg, uint64_t b, const char* f, int l)
{
    fprintf(stderr, "At %s:%d: %s (%lu)\n", f, l, msg, b);
    ++failures;
}

void __check_contents(void *a, void* b, size_t n, const char* f, int l)
{
    if (memcmp(a, b, n) != 0)
    {
        unsigned int i;
        fprintf(stderr, "At %s:%d:\n", f, l);
        for (i = 0; i < n; ++i)
            fprintf(stderr, "%02X ", ((uint8_t*)b)[i]);
        fprintf(stderr, "\n");
        ++failures;
    }
}

/*
 * Helper function to set the contents of a buffer
 */
void cswp_buffer_set(CSWP_BUFFER* buf, const char* contents, size_t length)
{
    memcpy(buf->buf, contents, length);
    buf->pos = 0;
    buf->used = length;
}

int main(int argc, char *argv[])
{
    failures = 0;

    test_buffer();
    test_commands();
    test_server();

    if (failures != 0)
    {
        fprintf(stderr, "%d failures\n", failures);
        return 1;
    }
    else
        return 0;
}

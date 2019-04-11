// cswp_test.c
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

#ifndef CSWP_TEST_H
#define CSWP_TEST_H

#include <stdint.h>
#include <stdlib.h>
#include "cswp_buffer.h"


void __fail(const char* msg, uint64_t b, const char* f, int l);
void __check_contents(void *a, void* b, size_t n, const char* f, int l);

#define CHECK_EQUAL(a, b) if ((a) != (b)) __fail(#a " != " #b, b, __FILE__, __LINE__)
#define CHECK_NOT_EQUAL(a, b) if ((a) == (b)) __fail(#a " == " #b, b, __FILE__, __LINE__)

#define CHECK_CONTENTS(a, b, n) __check_contents((a), (b), (n), __FILE__, __LINE__)

/*
 * Helper function to set the contents of a buffer
 */
void cswp_buffer_set(CSWP_BUFFER* buf, const char* contents, size_t length);

#endif // CSWP_TEST_H

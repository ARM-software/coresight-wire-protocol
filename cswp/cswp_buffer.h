// cswp_buffer.h
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.

/**
 * @file cswp_buffer.h
 * @brief CSWP buffer encoding and decoding
 */

#ifndef CSWP_BUFFER_H
#define CSWP_BUFFER_H

#include "cswp_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Initialise a CSWP_BUFFER
 *
 * Initialise a buffer already allocated by client
 * buffer.size is set to size
 * buffer.used and buffer.pos are set to 0
 * The buffer data is after the CSWP_BUFFER in memory
 *
 * @param buffer The CSWP buffer to initialise
 * @param size The size of the data buffer
 */
void cswp_buffer_init(CSWP_BUFFER* buffer, size_t size);

/**
 * Allocate a CSWP_BUFFER
 *
 * buffer.size is set to size
 * buffer.used and buffer.pos are set to 0
 *
 * Must be released by cswp_buffer_free()
 *
 * @param size The size of buffer to allocate
 * @return pointer to CSWP_BUFFER
 */
CSWP_BUFFER* cswp_buffer_alloc(size_t size);

/**
 * Free a CSWP_BUFFER
 *
 * Release resources allocated by cswp_buffer_alloc()
 *
 * @param buf The buffer to free
 */
void cswp_buffer_free(CSWP_BUFFER* buf);

/**
 * Clear a CSWP_BUFFER
 *
 * buffer.used and buffer.pos are set to 0
 *
 * @param buf The buffer to clear
 */
void cswp_buffer_clear(CSWP_BUFFER* buf);

/**
 * Set the read position of a CSWP_BUFFER
 *
 * @param buf CSWP_BUFFER
 * @param pos The position
 */
void cswp_buffer_seek(CSWP_BUFFER* buf, size_t pos);


/**
 * Advance the read position of a CSWP_BUFFER
 *
 * @param buf CSWP_BUFFER
 * @param count Number of bytes to advance by
 */
void cswp_buffer_skip(CSWP_BUFFER* buf, size_t count);

/**
 * Add a uint8 entry to a buffer
 *
 * Append the uint8_t value to the buffer
 * buffer.used is increased by 1
 *
 * @param buf The buffer
 * @param val The value to add
 * @return CSWP_SUCCESS on success, CSWP_BUFFER_FULL if insufficient space
 */
int cswp_buffer_put_uint8(CSWP_BUFFER* buf, uint8_t val);

/**
 * Add a uint32 entry to a buffer
 *
 * Append the uint32_t value to the buffer
 * buffer.used is increased by 4
 *
 * @param buf The buffer
 * @param val The value to add
 * @return CSWP_SUCCESS on success, CSWP_BUFFER_FULL if insufficient space
 */
int cswp_buffer_put_uint32(CSWP_BUFFER* buf, uint32_t val);

/**
 * Add a uint64 entry to a buffer
 *
 * Append the uint64_t value to the buffer
 * buffer.used is increased by 8
 *
 * @param buf The buffer
 * @param val The value to add
 * @return CSWP_SUCCESS on success, CSWP_BUFFER_FULL if insufficient space
 */
int cswp_buffer_put_uint64(CSWP_BUFFER* buf, uint64_t val);

/**
 * Add a varint entry to a buffer
 *
 * Append the varint value to the buffer
 * buffer.used is increased by the number of bytes required to encode val
 *
 * @param buf The buffer
 * @param val The value to add
 * @return CSWP_SUCCESS on success, CSWP_BUFFER_FULL if insufficient space
 */
int cswp_buffer_put_varint(CSWP_BUFFER* buf, varint_t val);

/**
 * Add a string to a buffer
 *
 * Append the string value to the buffer
 * Value is encoded as a varint length, followed by data bytes (not null terminated)
 * buffer.used is increased by the number of bytes required
 *
 * @param buf The buffer
 * @param str The string to add
 * @return CSWP_SUCCESS on success, CSWP_BUFFER_FULL if insufficient space
 */
int cswp_buffer_put_string(CSWP_BUFFER* buf, const char* str);

/**
 * Copy data to buffer
 *
 * Append the sequence to the buffer
 * buffer.used is increased by the number of bytes required
 *
 * @param buf The buffer
 * @param data The data to add
 * @param size The number of bytes to add
 * @return CSWP_SUCCESS on success, CSWP_BUFFER_FULL if insufficient space
 */
int cswp_buffer_put_data(CSWP_BUFFER* buf, const void* data, size_t size);

/**
 * Get a uint8 entry from a buffer
 *
 * Extract the uint8_t value from the buffer
 * buffer.pos is increased by 1
 *
 * @param buf The buffer
 * @param val Receives the data value
 * @return CSWP_SUCCESS on success, CSWP_BUFFER_EMPTY if insufficient data
 */
int cswp_buffer_get_uint8(CSWP_BUFFER* buf, uint8_t* val);

/**
 * Get a uint32 entry from a buffer
 *
 * Extract the uint32_t value from the buffer
 * buffer.pos is increased by 4
 *
 * @param buf The buffer
 * @param val Receives the data value
 * @return CSWP_SUCCESS on success, CSWP_BUFFER_EMPTY if insufficient data
 */
int cswp_buffer_get_uint32(CSWP_BUFFER* buf, uint32_t* val);

/**
 * Get a uint64 entry from a buffer
 *
 * Extract the uint64_t value from the buffer
 * buffer.pos is increased by 8
 *
 * @param buf The buffer
 * @param val Receives the data value
 * @return CSWP_SUCCESS on success, CSWP_BUFFER_EMPTY if insufficient data
 */
int cswp_buffer_get_uint64(CSWP_BUFFER* buf, uint64_t* val);

/**
 * Get a varint entry from a buffer
 *
 * Extract the varint value from the buffer
 * buffer.pos is increased by the number of bytes required
 *
 * @param buf The buffer
 * @param val Receives the data value
 * @return CSWP_SUCCESS on success, CSWP_BUFFER_EMPTY if insufficient data
 */
int cswp_buffer_get_varint(CSWP_BUFFER* buf, varint_t* val);

/**
 * Get a string entry from a buffer
 *
 * Extract the string value from the buffer
 * buffer.pos is increased by the number of bytes required
 *
 * @param buf The buffer
 * @param str Receives the string data
 * @param strSize The size of the str buffer
 * @return CSWP_SUCCESS on success, CSWP_BUFFER_EMPTY if insufficient data
 */
int cswp_buffer_get_string(CSWP_BUFFER* buf, char* str, size_t strSize);

/**
 * Get a direct pointer to data in a buffer
 *
 * Return a pointer to the current read position
 * buffer.pos is increased by the number of bytes required
 *
 * @param buf The buffer
 * @param ptr Receives the pointer to data
 * @param len The number of bytes accessed
 * @return CSWP_SUCCESS on success, CSWP_BUFFER_EMPTY if insufficient data
 */
int cswp_buffer_get_direct(CSWP_BUFFER* buf, void** ptr, size_t len);

/**
 * Copy data from buffer
 *
 *
 * @param buf The buffer
 * @param data The read data from the buffer
 * @param size The number of bytes to add
 * @return CSWP_SUCCESS on success
 */
int cswp_buffer_get_data(CSWP_BUFFER* buf, void* data, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* CSWP_BUFFER_H */

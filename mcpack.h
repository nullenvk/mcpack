#pragma once
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

/*
 * allocated_size - the amount of memory allocated for the data buffer
 * size - amount of data to parse in data (for unpacking), automatically filled by packing
 * read_ptr - index of the last element that was parsed by unpacking
 */

typedef struct {
    unsigned char *data;
    size_t allocated_size, size, read_ptr;

    bool allocate, used;
} mc_buffer_t;

int mc_pack(mc_buffer_t *buffer, const char *fmt, ...);
int mc_packv(mc_buffer_t *buffer, const char *fmt, va_list ap); 

int mc_unpack(mc_buffer_t buffer, const char *fmt, ...);
int mc_unpackv(mc_buffer_t buffer, const char *fmt, va_list ap);
int mc_unpack_raw(unsigned char *buf, size_t sz, const char *fmt, ...);


/*
 * new - create a buffer for new data
 * make  - create a buffer containing existing data
 */ 

mc_buffer_t     mc_buffer_new();
mc_buffer_t     mc_buffer_new_static(unsigned char *data, size_t size);
mc_buffer_t     mc_buffer_make(unsigned char *data, size_t size);
void            mc_buffer_free(mc_buffer_t *buffer);

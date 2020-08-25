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
} MCBuffer;

int mc_pack(MCBuffer *buffer, const char *fmt, ...);
int mc_unpack(MCBuffer buffer, const char *fmt, ...);
int mc_unpack_raw(unsigned char *buf, size_t sz, const char *fmt, ...);


/*
 * new - create a buffer for new data
 * make  - create a buffer containing existing data
 */ 

MCBuffer    mcbuffer_new();
MCBuffer    mcbuffer_new_static(unsigned char *data, size_t size);
void        mcbuffer_free(MCBuffer *buffer);

#define VARINT_LIMIT 5
#define VARLONG_LIMIT 10

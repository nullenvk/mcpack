#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

#include "mcpack.h"

#define htonll(x) ((1==htonl(1)) ? (x) : ((uint64_t)htonl((x) & 0xFFFFFFFF) << 32) | htonl((x) >> 32))
#define ntohll(x) ((1==ntohl(1)) ? (x) : ((uint64_t)ntohl((x) & 0xFFFFFFFF) << 32) | ntohl((x) >> 32))

#define VARINT_LIMIT 5
#define VARLONG_LIMIT 10

mc_buffer_t mc_buffer_new() {
    mc_buffer_t buffer; 

    buffer.allocate = true;
    buffer.used = false;
    buffer.data = NULL;
    buffer.size = 0;
    buffer.allocated_size = 0;
    buffer.read_ptr = 0;

    return buffer;
}

mc_buffer_t mc_buffer_new_static(unsigned char *data, size_t size) {
    mc_buffer_t buffer;

    buffer.allocate = false;
    buffer.used = false;
    buffer.data = data;
    buffer.size = 0;
    buffer.allocated_size = size;
    buffer.read_ptr = 0;

    return buffer;
}

mc_buffer_t mc_buffer_make(unsigned char *data, size_t size) {
    mc_buffer_t buffer = mc_buffer_new_static(data, size);

    buffer.size = size;

    return buffer;
}

static void mc_buffer_init_alloc(mc_buffer_t *buffer) {
    if (buffer->allocate)
        buffer->data = malloc(1);

    buffer->used = 1;
}

int mc_buffer_require_size(mc_buffer_t *buffer, size_t size) {
    if(size > buffer->allocated_size) {
        if(buffer->allocate) {
            buffer->data = realloc(buffer->data, size);
            buffer->allocated_size = size;
        } else {
            return -1;
        }
    }

    return 0;
}

void mc_buffer_free(mc_buffer_t *buffer) {
    if (buffer->used && buffer->allocate) {
        free(buffer->data);
        buffer->allocated_size = 0;
    }

    buffer->used = 0;
    buffer->size = 0;
}

static int mc_buffer_append(mc_buffer_t *buf, unsigned char byte) {
    if (buf->size >= buf->allocated_size) {
        if (buf->allocate) {
            //buf->allocated_size = buf->allocated_size != 0 ? buf->allocated_size * 2 : 1;
            buf->allocated_size++; 
            buf->data = realloc(buf->data, buf->allocated_size);
        } else {
            return -1;
        }
    }

    buf->data[buf->size] = byte;
    buf->size++;
    return 1;
}

static int mc_buffer_appendx(mc_buffer_t *buf, unsigned long long x, size_t n) {

    size_t orig_pos = buf->size;

    for (size_t i = 0; i < n; i++) {
        if (mc_buffer_append(buf, x) == -1) {
            buf->size = orig_pos;
            return -1;
        }

        x = x >> 8;
    }

    return n;
}

static int var_append(mc_buffer_t *buf, long long value, int limit) {
    unsigned char temp;
    int buf_err;

    for(int i=0; i < limit; i++) {
        temp = (unsigned char)(value & 0b01111111);

        value >>= 7;
        if (value != 0) {
            temp |= 0b10000000;
        }

        buf_err = mc_buffer_append(buf, temp);

        if (buf_err == -1)
            return -1;

        if(value == 0) 
            break;
    }

    if(value != 0)
        return -1;

    return 0;
}

int mc_var_read(mc_buffer_t *buf, long long *result, int limit) {
    int num_read = 0;
    *result = 0;

    if ((buf->size - buf->read_ptr) == 0)
        return -1;

    unsigned char read;

    do {
        if (buf->read_ptr + num_read >= buf->size)
            return -1;

        read = buf->data[buf->read_ptr + num_read];
        int value = (read & 0b01111111);
        *result |= (value << (7 * num_read));

        num_read++;
        if (num_read > limit)
            return -1;

    } while ((read & 0b10000000) != 0);

    buf->read_ptr += num_read;
    return num_read;
}

static int mc_packv(mc_buffer_t *buffer, const char *fmt, va_list ap) {
    mc_buffer_init_alloc(buffer);

    for(size_t i = 0; i < strlen(fmt); i++) {

        //curflags = fmtflags;
        //memset(&fmtflags, 0, sizeof(fmtflags));

        int buf_err = 0;
        uint8_t  arg1;
        uint16_t arg2;
        uint32_t arg4;
        uint64_t arg8;

        const char *args;
        size_t args_len;

        switch(fmt[i]) {

            // Bool
            case 'b':
                arg1 = va_arg(ap, unsigned int) ? 0x01 : 0x00;
                buf_err = mc_buffer_append(buffer, arg1);
                break;

                // Byte
            case '1':
                arg1 = va_arg(ap, unsigned int);
                buf_err = mc_buffer_append(buffer, arg1);
                break;

                // Short
            case '2':
                arg2 = htons((uint16_t)va_arg(ap, unsigned int));
                buf_err = mc_buffer_appendx(buffer, arg2, 2);
                break;

                // Int
            case '4':
                arg4 = htonl(va_arg(ap, uint32_t));
                buf_err = mc_buffer_appendx(buffer, arg4, 4);
                break;

                // Long 
            case '8':
                //arg8 = htonll(va_arg(ap, uint64_t));
                arg8 = va_arg(ap, uint64_t);
                arg8 = htonll(arg8);
                buf_err = mc_buffer_appendx(buffer, arg8, 8);
                break;

                // VarInt
            case 'i':
                buf_err = var_append(buffer, va_arg(ap, int), VARINT_LIMIT);
                break;

                // VarLong
            case 'I':
                buf_err = var_append(buffer, va_arg(ap, long long), VARLONG_LIMIT);
                break;

                // String
            case 's':
                args = va_arg(ap, const char *);
                args_len = strlen(args);
                if(var_append(buffer, args_len, VARINT_LIMIT) == -1)
                    return -1;
                mc_buffer_require_size(buffer, args_len);
                memcpy(buffer->data + buffer->size, args, args_len); 
                buffer->size += args_len;
                break;

                // Raw bytes (size + pointer)
                //case '=':
                //break;
        };

        if(buf_err == -1)
            return -1;
    }

    return buffer->size;
}


int mc_pack(mc_buffer_t *buffer, const char *fmt, ...) {
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = mc_packv(buffer, fmt, ap);
    va_end(ap);

    return ret;

}

static uint64_t mcbuffer_read_int(mc_buffer_t buffer, int bytes) {
    uint64_t value = 0;

    for (int i = 0; i < bytes; i++) {
        //value += buffer.data[buffer.pos + i] << (i * 8);
        value = value << 8;
        value += buffer.data[buffer.read_ptr + i];
    }

    return value;
}

static size_t get_left_data_read(mc_buffer_t buffer) {
    return buffer.size - buffer.read_ptr;
}

static int mc_unpackv(mc_buffer_t buffer, const char *fmt, va_list ap) {
    for(size_t i = 0; i < strlen(fmt); i++) {

        //curflags = fmtflags;
        //memset(&fmtflags, 0, sizeof(fmtflags));

        uint8_t  *arg1;
        uint16_t *arg2;
        uint32_t *arg4;
        uint64_t *arg8;
        int      *argi;
        long long *argI;
        char     *args;
        size_t   args_len;

        long long argi_val, string_len;

        // TODO: Test whether endianness conversion in 1,2,4,8 is OK

        switch(fmt[i]) {

            // Bool
            // Byte
            case 'b':
            case '1':
                if (get_left_data_read(buffer) < 1)
                    return -1;

                if ((arg1 = va_arg(ap, uint8_t *)) != NULL) {
                    *arg1 = mcbuffer_read_int(buffer, 1);
                }

                buffer.read_ptr++;
                break;

                // Short
            case '2':
                if (get_left_data_read(buffer) < 2)
                    return -1;

                if ((arg2 = va_arg(ap, uint16_t *)) != NULL) {
                    *arg2 = mcbuffer_read_int(buffer, 2);
                }

                buffer.read_ptr += 2;
                break;

                // Int
            case '4':
                if (get_left_data_read(buffer) < 4)
                    return -1;

                if ((arg4 = va_arg(ap, uint32_t *)) != NULL) {
                    *arg4 = mcbuffer_read_int(buffer, 4);
                }

                buffer.read_ptr += 4;
                break;

                // Long 
            case '8':
                if (get_left_data_read(buffer) < 8)
                    return -1;

                if ((arg8 = va_arg(ap, uint64_t *)) != NULL) {
                    *arg8 = mcbuffer_read_int(buffer, 8);
                }

                buffer.read_ptr += 8;
                break;

                // VarInt
            case 'i':
                if (mc_var_read(&buffer, &argi_val, VARINT_LIMIT) == -1)
                    return -1;

                if ((argi = va_arg(ap, int *)) != NULL) {
                    *argi = argi_val;
                }
                break;

                // VarLong
            case 'I':
                if(mc_var_read(&buffer, &argi_val, VARLONG_LIMIT) == -1)
                    return -1;

                if ((argI = va_arg(ap, long long *)) != NULL) {
                    *argI = argi_val;
                }
                break;

                // String
            case 's':
                args_len = va_arg(ap, size_t);
                args = va_arg(ap, char *);

                if (mc_var_read(&buffer, &string_len, VARINT_LIMIT) == -1)
                    return -1;

                if (get_left_data_read(buffer) < (size_t)string_len)
                    return -1;

                if (args != NULL)
                    strncpy(args, (const char*)buffer.data + buffer.size, args_len);

                buffer.read_ptr += string_len;

                break;

                // Raw bytes (size + pointer)
                //case '=':
                //break;
        };

        if(buffer.read_ptr > buffer.size) {
            return -1;
        }
    }

    return buffer.read_ptr;

}

int mc_unpack(mc_buffer_t buffer, const char *fmt, ...) {
    va_list ap;
    int ret;

    va_start(ap, fmt);
    ret = mc_unpackv(buffer, fmt, ap);
    va_end(ap);

    return ret;
}

int mc_unpack_raw(unsigned char *buf, size_t sz, const char *fmt, ...) {
    va_list ap;
    int ret;
    mc_buffer_t buffer;

    buffer.data = buf;
    buffer.allocated_size = sz;
    buffer.size = sz;
    buffer.used = 0;
    buffer.read_ptr = 0;

    va_start(ap, fmt);
    ret = mc_unpackv(buffer, fmt, ap);
    va_end(ap);

    return ret;
}

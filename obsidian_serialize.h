#pragma once
#include <stddef.h>
#include <stdarg.h>
#include <stdbool.h>

typedef struct {
    unsigned char *data;
    size_t allocated_size, size;

    bool allocate, used;
} ByteBuffer;

typedef struct {
    int sockfd;

    int compression_threshold;

    bool encryption_enabled;
} MCConnection;

int mc_pack(ByteBuffer *buffer, const char *fmt, ...);
int mc_unpack(ByteBuffer buffer, const char *fmt, ...);
int mc_unpack_raw(unsigned char *buf, size_t sz, const char *fmt, ...);

int mcpkt_pack(ByteBuffer *buffer, int id, const char *fmt, ...);
int mcpkt_send(MCConnection *conn, int id, const char *fmt, ...);
int mcpkt_recv(MCConnection *conn, ByteBuffer *buffer);

void bytebuffer_free(ByteBuffer *buffer);

MCConnection mcconnection_new(const char *addr, const char *port);

#define VARINT_LIMIT 5
#define VARLONG_LIMIT 10

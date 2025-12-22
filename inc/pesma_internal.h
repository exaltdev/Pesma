#ifndef PESMA_INTERNAL_H
#define PESMA_INTERNAL_H

#include "pesma.h"
#include <stdio.h>
#include <stdlib.h>

/* Shared read/write buffer */
typedef struct {
    uint8_t* data;
    size_t   buffer_size;
    size_t   buffer_used;
    size_t   buffer_pos;
} PBuffer;

/* Socket backend */
typedef struct {
    int         socket_fd;
    char        ip_address[46];
    uint16_t    port;
    PSocketType socket_type;
    bool        is_connected;
} PSocketInternal;

/* File backend */
typedef struct {
    FILE* file_pointer;
    char* file_path;
} PFileInternal;

/* Full PHandle struct — opaque to user */
struct PHandle {
    PType   type;
    PBuffer read_buffer;
    PBuffer write_buffer;
    union {
        PSocketInternal socket_backend;
        PFileInternal   file_backend;
    } backend;
};

/* Internal helper functions */
int internal_flush (PHandle* handle);
ssize_t pesma_write_buffer_load (PHandle* handle, const void* data, size_t len); // append to write buffer

#endif

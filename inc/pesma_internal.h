#ifndef PESMA_INTERNAL_H
#define PESMA_INTERNAL_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef PESMA_TYPES
#define PType uint8_t
#define PSocketType uint8_t // shutting up the IDE (include preprocessor directive is copy paste anyway)
#endif

#define P_SOCKET_BUFFER_SIZE 65536

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
    int         ip_address;
    uint16_t    port;
    PSocketType socket_type;
    bool        is_connected;
} PSocketInternal;

/* File backend */
typedef struct {
    FILE* file_pointer;
    char* file_path;
} PFileInternal;

/* Handle struct */
struct PHandle {
    PType   type;
    PBuffer read_buffer;
    PBuffer write_buffer;
    union {
        PSocketInternal socket;
        PFileInternal   file;
    } backend;
};

typedef struct PHandle PHandle;

/* Internal helper functions */
int                    internal_flush (PHandle* handle);
ssize_t
pesma_write_buffer_load (PHandle* handle, const void* data, size_t len); // append to write buffer
int pesma_internal_socket_create (bool type, uint16_t port);
PBuffer pesma_internal_buffer_create(size_t size);

#endif

#ifndef PESMA_INTERNAL_H
#define PESMA_INTERNAL_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <stdlib.h>

#ifndef PESMA_TYPES
    #define PType uint8_t
    #define PSocketType uint8_t // shutting up the IDE (internal include remedy)
#endif

#define P_SOCKET_BUFFER_SIZE 65536

/* Shared read/write buffer */
typedef struct {
    uint8_t* data;
    size_t   size;
    size_t   used;
    size_t   pos;
} PBuffer;

/* Socket backend */
typedef struct {
    int         fd;
    int         ip_address;
    uint16_t    port;
    PSocketType type;
    bool        is_connected;
} PSocketInternal;

/* File backend */
typedef struct {
    int file_descriptor;
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
int internal_flush (PHandle* handle);
ssize_t
pesma_write_buffer_load (PHandle* handle, const void* data, size_t len); // append to write buffer
int     pesma_internal_socket_create (bool type, uint16_t port);
void pesma_internal_buffers_create (PHandle* handle, size_t size);
ssize_t pesma_internal_write(PHandle* handle, size_t size, void* value, const char* message);
uint64_t pesma_internal_read(PHandle* handle, size_t size, const char* message);


#endif

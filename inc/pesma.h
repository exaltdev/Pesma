#ifndef PESMA_H
#define PESMA_H

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include <sys/types.h>
#include <string.h>

/* Handle type enums */
typedef enum {
    P_TYPE_SOCKET,
    P_TYPE_FILE,
    P_TYPE_BUFFER,
    P_TYPE_FIFO
} PType;

typedef enum {
    P_TCP_SERVER,
    P_TCP_CLIENT,
    P_TCP_CONNECTION,
    P_UDP
} PSocketType;
#define PESMA_TYPES // include here for internal file

#include "pesma_internal.h"

#define SOCKET_BUFFER_SIZE 65536

/* TCP
Create TCP client/server and perform I/O */
PHandle* pesma_tcp_client_create (const char* dns_address, uint16_t port); // create TCP client
int      pesma_tcp_connect (PHandle* handle);                              // connect TCP client
PHandle* pesma_tcp_server_create (uint16_t port);                          // create TCP server
PHandle* pesma_tcp_accept (PHandle* handle);                             // accept client connection
bool     pesma_handle_connected (PHandle* handle);                       // check connection
ssize_t  pesma_tcp_send (PHandle* handle, size_t len); // send TCP data
ssize_t  pesma_tcp_receive (PHandle* handle, size_t len);  // receive TCP data

/* UDP
Create UDP handle and perform I/O */
PHandle* pesma_udp_create (const char* dns_address, uint16_t port);      // create UDP handle
int      pesma_udp_connect (PHandle* handle);                            // optional UDP connect
ssize_t  pesma_udp_send (PHandle* handle, size_t len); // send UDP data
ssize_t  pesma_udp_receive (PHandle* handle, size_t len);  // receive UDP data

/* File
Open, read/write, seek, size operations */
PHandle* pesma_file_open (const char* path, const char* mode);        // open or create file
size_t   pesma_file_size (PHandle* handle);                           // get file size
int      pesma_file_seek (PHandle* handle, long offset, int whence);  // seek in file
long     pesma_file_tell (PHandle* handle);                           // get current file position
ssize_t  pesma_file_read (PHandle* handle, size_t len); // read from file
ssize_t  pesma_file_write (PHandle* handle, size_t len); // write to file

/* Memory buffer
Initializers and read/write into buffers*/
PHandle*  pesma_buffer_create (size_t capacity);                           // create memory buffer
ssize_t  pesma_buffer_get (PHandle* handle, size_t len);       // read from buffer
ssize_t  pesma_buffer_set (PHandle* handle, size_t len); // write to buffer

/* Utilities
Query type/state, network wait, buffer operations */
PType pesma_handle_get_type (PHandle* handle); // get handle type
int   pesma_network_wait (PHandle* handle, bool wait_for_read, bool wait_for_write, int timeout_ms); // wait until readable/writable
int   pesma_handle_free (PHandle* handle); // close socket/file/buffer and free
int   pesma_buffer_clear (PHandle* handle, bool type); // clears selected buffer in handle

/* Typed read/write helpers
Write basic types or strings */
ssize_t pesma_write_int8 (PHandle* handle, int8_t value);
ssize_t pesma_write_int16 (PHandle* handle, int16_t value);
ssize_t pesma_write_int32 (PHandle* handle, int32_t value);
ssize_t pesma_write_int64 (PHandle* handle, int64_t value);

ssize_t pesma_write_uint8 (PHandle* handle, uint8_t value);
ssize_t pesma_write_uint16 (PHandle* handle, uint16_t value);
ssize_t pesma_write_uint32 (PHandle* handle, uint32_t value);
ssize_t pesma_write_uint64 (PHandle* handle, uint64_t value);

ssize_t pesma_write_char (PHandle* handle, char value);
ssize_t pesma_write_string (PHandle* handle, const char* str);

/* Typed read helpers
Read basic types or strings */
ssize_t pesma_read_int8 (PHandle* handle, int8_t* value);
ssize_t pesma_read_int16 (PHandle* handle, int16_t* value);
ssize_t pesma_read_int32 (PHandle* handle, int32_t* value);
ssize_t pesma_read_int64 (PHandle* handle, int64_t* value);

ssize_t pesma_read_uint8 (PHandle* handle, uint8_t* value);
ssize_t pesma_read_uint16 (PHandle* handle, uint16_t* value);
ssize_t pesma_read_uint32 (PHandle* handle, uint32_t* value);
ssize_t pesma_read_uint64 (PHandle* handle, uint64_t* value);

ssize_t pesma_read_char (PHandle* handle, char* value);
ssize_t pesma_read_string (PHandle* handle, char* dst, size_t max_len);

#endif

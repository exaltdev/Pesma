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

#define PATH_MAX 4096

#define SOCKET_BUFFER_SIZE 65536
#define FILE_BUFFER_SIZE   65536

/* TCP
Create TCP client/server and perform I/O */
PHandle* pesma_tcp_connect (const char* host, uint16_t port); // resolve + socket + connect
PHandle* pesma_tcp_listen (uint16_t port);                    // socket + reuseaddr + bind + listen
PHandle* pesma_tcp_accept (PHandle* server);                  // accept pending client

/* UDP
Create UDP handle and perform I/O */
PHandle* pesma_udp_connect (const char* host, uint16_t port); // connected UDP: send/recv
PHandle* pesma_udp_bind (uint16_t port);                      // bound UDP: recv from anyone
ssize_t  pesma_udp_send_to (PHandle* h, const char* host, uint16_t port);
ssize_t pesma_udp_recv_from (PHandle* h, char* out_host, size_t out_len, uint16_t* out_port);

/* Socket general
General purpose socket operations */
int  pesma_socket_shutdown (PHandle* handle, int how);          // SHUT_RD, SHUT_WR, SHUT_RDWR
int  pesma_socket_set_reuseaddr (PHandle* handle, bool enable); // SO_REUSEADDR
int  pesma_socket_set_nonblock (PHandle* handle);               // SO_NONBLOCk
int  pesma_socket_set_keepalive (PHandle* handle, bool enable); // TCP keepalive
int  pesma_socket_set_nodelay (PHandle* handle, bool enable);   // TCP_NODELAY
int  pesma_socket_get_error (PHandle* handle);                  // getsockopt SO_ERROR
int  pesma_socket_get_peer (PHandle* handle, char* ip_str, size_t ip_len, uint16_t* port);
int  pesma_socket_get_local (PHandle* handle, char* ip_str, size_t ip_len, uint16_t* port);
bool pesma_socket_is_connected (PHandle* handle);

/* File
Open, read/write, seek, size operations */
PHandle* pesma_file_open (const char* path, const char* mode);       // open or create file
PHandle* pesma_fifo_create (const char* path, const char* mode);     // open or create pipe
off_t    pesma_file_size (PHandle* handle);                          // get file size
off_t    pesma_file_seek (PHandle* handle, long offset, int whence); // seek in file
off_t    pesma_file_tell (PHandle* handle);                          // get current file position

/* Memory buffer
Initializers and read/write into buffers*/
PHandle* pesma_buffer_create (size_t capacity); // create memory buffer
void     pesma_buffer_sync (PHandle* handle);   // syncs buffer usage

/* Utilities
Query type/state, network wait, buffer operations */
PType pesma_handle_get_type (PHandle* handle); // get handle type
int   pesma_network_wait (PHandle* handle,
                          bool     wait_for_read,
                          bool     wait_for_write,
                          int      timeout_ms);             // wait until readable/writable
int   pesma_handle_free (PHandle* handle);             // close socket/file/buffer and free
int   pesma_buffer_clear (PHandle* handle, char type); // clears selected buffer in handle

/* Universal send/recv functions
Read/write to socket/file/buffer/etc...*/
ssize_t pesma_send (PHandle* handle, size_t len);
ssize_t pesma_recv (PHandle* handle, size_t len);

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

ssize_t pesma_write_float (PHandle* handle, float value);
ssize_t pesma_write_double (PHandle* handle, double value);

ssize_t pesma_write_char (PHandle* handle, char value);
ssize_t pesma_write_string (PHandle* handle, const char* str, size_t length);

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

ssize_t pesma_read_float (PHandle* handle, float* value);
ssize_t pesma_read_double (PHandle* handle, double* value);

ssize_t pesma_read_char (PHandle* handle, char* value);
ssize_t pesma_read_string (PHandle* handle, char* dst, size_t length);

#endif

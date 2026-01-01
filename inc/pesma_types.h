#ifndef PESMA_TYPES_H
#define PESMA_TYPES_H

/* Handle type enums */
typedef enum {
    P_TYPE_SOCKET,
    P_TYPE_FILE,
    P_TYPE_BUFFER,
    P_TYPE_FIFO
} PType;

typedef enum {
    P_SOCKET_TCP_CLIENT,
    P_SOCKET_TCP_SERVER,
    P_SOCKET_TCP_CONNECTION,
    P_SOCKET_UDP
} PSocketType;

typedef enum {
    PESMA_BUFFER_READ,
    PESMA_BUFFER_WRITE
} PBufferType;

#endif

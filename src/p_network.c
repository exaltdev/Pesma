#include "pesma.h"
#include <fcntl.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>

/* TCP */
int pesma_internal_socket_create(bool type, uint16_t port)
{
    uint32_t sock;
    if(type) {
       sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    else {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }
    int err;
    if(fcntl(sock, F_SETFL, O_NONBLOCK) != 0) {
        exit(1);
    }

    return sock;
}

PHandle* pesma_tcp_client_create(const char* dns_address, uint16_t port)
{
    uint32_t sockServ;
    PHandle* handle;

    sockServ = pesma_internal_socket_create(1, port);
    handle = malloc(sizeof(*handle));
    handle->type = P_TYPE_SOCKET;
    sockServ = pesma_internal_socket_create(0, port);
    return NULL;
}

int pesma_tcp_connect(PHandle* handle)
{
    return 0;
}

PHandle* pesma_tcp_server_create(uint16_t port)
{
    return NULL;
}

PHandle* pesma_tcp_accept(PHandle* handle)
{
    return NULL;
}

bool pesma_handle_connected(PHandle* handle)
{
    return false;
}

ssize_t pesma_tcp_send(PHandle* handle, const void* data, size_t len)
{
    return 0;
}

ssize_t pesma_tcp_receive(PHandle* handle, void* buffer, size_t len)
{
    return 0;
}

/* UDP */

PHandle* pesma_udp_create(const char* dns_address, uint16_t port)
{
    return NULL;
}

int pesma_udp_connect(PHandle* handle)
{
    return 0;
}

ssize_t pesma_udp_send(PHandle* handle, const void* data, size_t len)
{
    return 0;
}

ssize_t pesma_udp_receive(PHandle* handle, void* buffer, size_t len)
{
    return 0;
}

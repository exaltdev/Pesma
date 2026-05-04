#include "pesma.h"

#include <fcntl.h>
#include <netinet/in.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

/* TCP */
int pesma_internal_socket_create(PSocketType type, uint16_t port)
{
    int sock;

    struct sockaddr_in addr;

    memset(&addr, 0, sizeof(struct sockaddr));

    if(type == P_TCP_SERVER || type == P_TCP_CLIENT) {
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    else {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }
    if(fcntl(sock, F_SETFL, O_NONBLOCK) != 0) {
        exit(1);  //checkerr
    }
    if(type != P_TCP_CLIENT) {
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if(bind(sock, (struct sockaddr*) &addr, sizeof(struct sockaddr)) != 0) {
            exit(1);  //checkerr
        }
        if(type == P_TCP_SERVER) {
            if(listen(sock, SOMAXCONN) < 0)
                exit(1);  //checkerr
        }
    }
    return sock;
}

PHandle* pesma_tcp_client_create(const char* dns_address, uint16_t port)
{
    uint32_t sockCli;
    PHandle* handle;

    handle = malloc(sizeof(*handle));
    memset(handle, 0, sizeof(PHandle));
    handle->type = P_TYPE_SOCKET;
    sockCli = pesma_internal_socket_create(0, port);

    handle->backend.socket.port = port;
    handle->backend.socket.type = P_TCP_CLIENT;
    handle->backend.socket.fd = sockCli;
    handle->backend.socket.ip_address = 0;
    handle->backend.socket.is_connected = false;

    //TODO: Unfinished, setup similar logic to the server create.
    return handle;
}

int pesma_tcp_connect(PHandle* handle)
{
    return 0;
}

PHandle* pesma_tcp_server_create(uint16_t port)
{
    PHandle* handle;
    uint32_t sockServ;

    sockServ = pesma_internal_socket_create(true, port);

    memset(handle, 0, sizeof(PHandle));

    handle->type = P_TYPE_SOCKET;

    pesma_internal_buffers_create(handle, P_SOCKET_BUFFER_SIZE);

    handle->backend.socket.port = port;
    handle->backend.socket.type = P_TCP_SERVER;
    handle->backend.socket.fd = sockServ;
    handle->backend.socket.ip_address = 0;
    handle->backend.socket.is_connected = false;

    return handle;
}

PHandle* pesma_tcp_accept(PHandle* handle)
{
    PHandle* client_handle;
    uint32_t sockCli;
    uint32_t addr_size;
    struct sockaddr_in client_addr;

    addr_size = sizeof(client_addr);

    if((sockCli = accept(handle->backend.socket.fd, (struct sockaddr*) &client_addr, &addr_size)) !=
        0) {
        exit(1);  //checkerr
    }
    
    client_handle = malloc(sizeof(PHandle));

    handle->type = P_TYPE_SOCKET;

    pesma_internal_buffers_create(handle, P_SOCKET_BUFFER_SIZE);

    handle->backend.socket.type = P_TCP_SERVER;
    handle->backend.socket.fd = sockCli;
    handle->backend.socket.ip_address = 0;
    handle->backend.socket.is_connected = true;

    return client_handle;
}

bool pesma_handle_connected(PHandle* handle)
{
    return false;
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

ssize_t pesma_udp_send(PHandle* handle, size_t len)
{
    return 0;
}

ssize_t pesma_udp_receive(PHandle* handle, size_t len)
{
    return 0;
}

#include "pesma.h"

#include <arpa/inet.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>

/* TCP */

PHandle* pesma_tcp_connect(const char* host, uint16_t port)
{
    return NULL;
}

PHandle* pesma_tcp_listen(uint16_t port)
{
    PHandle* handle;
    int sockServ;
    struct sockaddr_in* addr;

    handle = malloc(sizeof(*handle));
    memset(handle, 0, sizeof(PHandle));
    handle->type = P_TYPE_SOCKET;
    //sockServ = pesma_internal_socket_create(P_TCP_SERVER, port);

    //TODO: do this in generic socket builder since the info is build there anyway
    addr = (struct sockaddr_in*) &handle->backend.socket.addr;
    addr->sin_family = AF_INET;
    addr->sin_port = htons(port);
    addr->sin_addr.s_addr = htonl(INADDR_ANY);
    handle->backend.socket.addr_len = sizeof(struct sockaddr_in);

    handle->backend.socket.type = P_TCP_SERVER;
    handle->backend.socket.fd = sockServ;
    handle->backend.socket.is_connected = false;

    return handle;
}

PHandle* pesma_tcp_accept(PHandle* handle)
{
    PHandle* client_handle;
    int sockCli;
    socklen_t addr_size;
    struct sockaddr_in client_addr;

    addr_size = sizeof(client_addr);
    sockCli = accept(handle->backend.socket.fd, (struct sockaddr*) &client_addr, &addr_size);
    if(sockCli == -1) {
        perror("accept fuckup");
        exit(1);  //checkerr
    }

    client_handle = malloc(sizeof(*handle));
    memset(client_handle, 0, sizeof(PHandle));
    client_handle->type = P_TYPE_SOCKET;

    pesma_internal_buffers_create(client_handle, P_SOCKET_BUFFER_SIZE);

    memcpy(&client_handle->backend.socket.addr, &client_addr, addr_size);
    client_handle->backend.socket.addr_len = addr_size;

    client_handle->backend.socket.type = P_TCP_CONNECTION;
    client_handle->backend.socket.fd = sockCli;
    client_handle->backend.socket.is_connected = true;

    return client_handle;
}

/* UDP */

PHandle* pesma_udp_create(const char* dns_address, uint16_t port)
{
    fprintf(stderr, "[PESMA] %s not implemented\n", __func__);
    return NULL;
}

PHandle* pesma_udp_bind(uint16_t port)
{
    fprintf(stderr, "[PESMA] %s not implemented\n", __func__);
    return NULL;
}

ssize_t pesma_udp_send_to(PHandle* handle, const char* host, uint16_t port)
{
    fprintf(stderr, "[PESMA] %s not implemented\n", __func__);
    return -1;
}

ssize_t pesma_udp_recv_from(PHandle* handle, char* out_host, size_t out_len, uint16_t* out_port)
{
    fprintf(stderr, "[PESMA] %s not implemented\n", __func__);
    return -1;
}

/* Socket utils */

int pesma_socket_shutdown(PHandle* handle, int how)
{
    fprintf(stderr, "[PESMA] %s not implemented\n", __func__);
    return -1;
}

int pesma_socket_set_reuseaddr(PHandle* handle, bool enable)
{
    int optval = enable;
    int ret;
    ret = setsockopt(handle->backend.socket.fd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));
    if(ret != 0)
        perror("Reuseaddr error");
    return ret;
}

int pesma_socket_set_keepalive(PHandle* handle, bool enable)
{
    if(handle->backend.socket.type == P_UDP) {
        printf("Cannot enable KEEPALIVE on UDP socket");
        return -1;
    }
    int optval = enable;
    return setsockopt(handle->backend.socket.fd, SOL_SOCKET, SO_KEEPALIVE, &optval, sizeof(optval));
}

int pesma_socket_set_nonblock(PHandle* handle)
{
    return fcntl(handle->backend.socket.fd, F_SETFL, O_NONBLOCK);
}

int pesma_socket_set_nodelay(PHandle* handle, bool enable)
{
    if(handle->backend.socket.type == P_UDP) {
        printf("Cannot enable TCP_NODELAY on UDP socket");
        return -1;
    }
    int optval = enable;
    return setsockopt(handle->backend.socket.fd, IPPROTO_TCP, TCP_NODELAY, &optval, sizeof(optval));
}

int pesma_socket_get_peer(PHandle* handle, char* ip_str, size_t ip_len, uint16_t* port)
{
    struct sockaddr_in* addr = (struct sockaddr_in*) &handle->backend.socket.addr;
    inet_ntop(AF_INET, &addr->sin_addr, ip_str, ip_len);
    if(port)
        *port = ntohs(addr->sin_port);
    return 0;
}

int pesma_socket_get_local(PHandle* handle, char* ip_str, size_t ip_len, uint16_t* port)
{
    struct sockaddr_storage addr;
    socklen_t addr_len = sizeof(addr);
    getsockname(handle->backend.socket.fd, (struct sockaddr*) &addr, &addr_len);

    struct sockaddr_in* addr_in = (struct sockaddr_in*) &addr;
    inet_ntop(AF_INET, &addr_in->sin_addr, ip_str, ip_len);
    if(port)
        *port = ntohs(addr_in->sin_port);
    return 0;
}

int pesma_socket_get_error(PHandle* handle)
{
    fprintf(stderr, "[PESMA] %s not implemented\n", __func__);
    return -1;
}

bool pesma_socket_is_connected(PHandle* handle)
{
    fprintf(stderr, "[PESMA] %s not implemented\n", __func__);
    return -1;
}

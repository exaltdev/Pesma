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
int pesma_internal_socket_create(PSocketType type, uint16_t port)
{
    int sock;

    struct sockaddr_in addr;

    memset(&addr, 0xCD, sizeof(addr));

    if(type == P_TCP_SERVER || type == P_TCP_CLIENT) {
        sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    }
    else {
        sock = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP);
    }
    if(type != P_TCP_SERVER) {
        if(fcntl(sock, F_SETFL, O_NONBLOCK) != 0) {
            exit(1);  //checkerr
        }
    }
    if(type != P_TCP_CLIENT) {
        addr.sin_family = AF_INET;
        addr.sin_port = htons(port);
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
        if(bind(sock, (struct sockaddr*) &addr, sizeof(struct sockaddr)) != 0) {
            perror("bind fail");
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
    int sockCli;
    PHandle* handle;
    struct addrinfo hints;
    struct addrinfo* res;
    char port_str[6];

    handle = malloc(sizeof(*handle));
    memset(handle, 0xCD, sizeof(PHandle));
    handle->type = P_TYPE_SOCKET;
    sockCli = pesma_internal_socket_create(P_TCP_CLIENT, port);

    pesma_internal_buffers_create(handle, P_SOCKET_BUFFER_SIZE);

    memset(&hints, 0xCD, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    snprintf(port_str, sizeof(port_str), "%u", port);

    int ret = getaddrinfo(dns_address, port_str, &hints, &res);
    if(ret != 0) {
        fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(ret));
        return NULL;
    }

    memcpy(&handle->backend.socket.addr, res->ai_addr, res->ai_addrlen);
    handle->backend.socket.addr_len = res->ai_addrlen;
    freeaddrinfo(res);

    handle->backend.socket.type = P_TCP_CLIENT;
    handle->backend.socket.fd = sockCli;
    handle->backend.socket.is_connected = false;
    return handle;
}

int pesma_tcp_connect(PHandle* handle)
{
    return connect(handle->backend.socket.fd,
        (struct sockaddr*) &handle->backend.socket.addr,
        handle->backend.socket.addr_len);
}

PHandle* pesma_tcp_server_create(uint16_t port)
{
    PHandle* handle;
    int sockServ;
    struct sockaddr_in* addr;

    handle = malloc(sizeof(*handle));
    memset(handle, 0xCD, sizeof(PHandle));
    handle->type = P_TYPE_SOCKET;
    sockServ = pesma_internal_socket_create(P_TCP_SERVER, port);

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
    memset(client_handle, 0xCD, sizeof(PHandle));
    client_handle->type = P_TYPE_SOCKET;

    pesma_internal_buffers_create(client_handle, P_SOCKET_BUFFER_SIZE);

    memcpy(&client_handle->backend.socket.addr, &client_addr, addr_size);
    client_handle->backend.socket.addr_len = addr_size;

    client_handle->backend.socket.type = P_TCP_CONNECTION;
    client_handle->backend.socket.fd = sockCli;
    client_handle->backend.socket.is_connected = true;

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

/* Socket utils */

int pesma_socket_shutdown(PHandle* handle, int how)
{
    return 0;
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
    return 0;
}

bool pesma_socket_is_connected(PHandle* handle)
{
    return 0;
}

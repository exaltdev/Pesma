#include "pesma.h"

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netdb.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

/* TCP */

PHandle* pesma_tcp_connect(const char* host, uint16_t port)
{
    PHandle* handle;
    int sockCli = -1;
    struct addrinfo hints, *servinfo, *p;
    int rv;
    char port_str[6];
    struct sockaddr_storage peer_addr;
    socklen_t peer_addr_len = 0;

    snprintf(port_str, sizeof(port_str), "%u", port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;

    if((rv = getaddrinfo(host, port_str, &hints, &servinfo)) != 0) {
        fprintf(stderr, "[PESMA] connect: getaddrinfo: %s\n", gai_strerror(rv));
        return NULL;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if((sockCli = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }

        if(connect(sockCli, p->ai_addr, p->ai_addrlen) == -1) {
            fprintf(stderr, "[PESMA] connect: %s\n", strerror(errno));
            close(sockCli);
            sockCli = -1;
            continue;
        }

        memcpy(&peer_addr, p->ai_addr, p->ai_addrlen);
        peer_addr_len = p->ai_addrlen;
        break;
    }

    freeaddrinfo(servinfo);

    if(p == NULL || sockCli == -1) {
        fprintf(stderr, "[PESMA] connect: failed to connect to %s:%u\n", host, port);
        return NULL;
    }

    handle = malloc(sizeof(*handle));
    if(handle == NULL) {
        fprintf(stderr, "[PESMA] connect: malloc failed\n");
        close(sockCli);
        return NULL;
    }

    memset(handle, 0, sizeof(*handle));

    pesma_internal_buffers_create(handle, P_SOCKET_BUFFER_SIZE);

    handle->type = P_TYPE_SOCKET;
    handle->backend.socket.fd = sockCli;
    memcpy(&handle->backend.socket.addr, &peer_addr, peer_addr_len);
    handle->backend.socket.addr_len = peer_addr_len;
    handle->backend.socket.type = P_TCP_CLIENT;
    handle->backend.socket.is_connected = true;

    return handle;
}

PHandle* pesma_tcp_listen(uint16_t port)
{
    PHandle* handle;
    int sockServ = -1;
    struct addrinfo hints, *servinfo, *p;
    int yes = 1;
    int rv;
    char port_str[6];
    struct sockaddr_storage bound_addr;
    socklen_t bound_addr_len = 0;

    snprintf(port_str, sizeof(port_str), "%u", port);

    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;

    if((rv = getaddrinfo(NULL, port_str, &hints, &servinfo)) != 0) {
        fprintf(stderr, "[PESMA] listen: getaddrinfo: %s\n", gai_strerror(rv));
        return NULL;
    }

    for(p = servinfo; p != NULL; p = p->ai_next) {
        if((sockServ = socket(p->ai_family, p->ai_socktype, p->ai_protocol)) == -1) {
            continue;
        }

        if(setsockopt(sockServ, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes)) == -1) {
            fprintf(stderr, "[PESMA] listen: setsockopt: %s\n", strerror(errno));
            close(sockServ);
            sockServ = -1;
            continue;
        }

        if(bind(sockServ, p->ai_addr, p->ai_addrlen) == -1) {
            fprintf(stderr, "[PESMA] listen: bind: %s\n", strerror(errno));
            close(sockServ);
            sockServ = -1;
            continue;
        }

        memcpy(&bound_addr, p->ai_addr, p->ai_addrlen);
        bound_addr_len = p->ai_addrlen;
        break;
    }

    freeaddrinfo(servinfo);

    if(p == NULL || sockServ == -1) {
        fprintf(stderr, "[PESMA] listen: failed to bind any address\n");
        return NULL;
    }

    if(listen(sockServ, P_SOCKET_MAX_LISTENERS) == -1) {
        fprintf(stderr, "[PESMA] listen: %s\n", strerror(errno));
        close(sockServ);
        return NULL;
    }

    handle = malloc(sizeof(*handle));
    if(handle == NULL) {
        fprintf(stderr, "[PESMA] listen: malloc failed\n");
        close(sockServ);
        return NULL;
    }

    memset(handle, 0, sizeof(*handle));

    pesma_internal_buffers_create(handle, P_SOCKET_BUFFER_SIZE);

    handle->type = P_TYPE_SOCKET;
    handle->backend.socket.fd = sockServ;
    memcpy(&handle->backend.socket.addr, &bound_addr, bound_addr_len);
    handle->backend.socket.addr_len = bound_addr_len;
    handle->backend.socket.type = P_TCP_SERVER;
    handle->backend.socket.is_connected = false;

    return handle;
}

PHandle* pesma_tcp_accept(PHandle* handle)
{
    PHandle* client_handle;
    int sockCli;
    socklen_t addr_size;
    struct sockaddr_in client_addr;

    if(handle->type != P_TYPE_SOCKET || handle->backend.socket.type != P_TCP_SERVER) {
        fprintf(stderr, "[PESMA] accept: handle isn't a listening socket\n");
        return NULL;
    }

    addr_size = sizeof(client_addr);
    sockCli = accept(handle->backend.socket.fd, (struct sockaddr*) &client_addr, &addr_size);
    if(sockCli == -1) {
        fprintf(stderr, "[PESMA] accept: %s\n", strerror(errno));
        return NULL;
    }

    client_handle = malloc(sizeof(*client_handle));
    if(client_handle == NULL) {
        fprintf(stderr, "[PESMA] accept: malloc fail\n");
        close(sockCli);
        return NULL;
    }

    memset(client_handle, 0, sizeof(*client_handle));
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


PHandle* pesma_udp_connect(const char* host, uint16_t port)
{
    (void) host;
    (void) port;
    fprintf(stderr, "[PESMA] %s not implemented\n", __func__);
    return NULL;
}

PHandle* pesma_udp_bind(uint16_t port)
{
    (void) port;
    fprintf(stderr, "[PESMA] %s not implemented\n", __func__);
    return NULL;
}

ssize_t pesma_udp_send_to(PHandle* handle, const char* host, uint16_t port)
{
    (void) handle;
    (void) host;
    (void) port;
    fprintf(stderr, "[PESMA] %s not implemented\n", __func__);
    return -1;
}

ssize_t pesma_udp_recv_from(PHandle* handle, char* out_host, size_t out_len, uint16_t* out_port)
{
    (void) handle;
    (void) out_host;
    (void) out_len;
    (void) out_port;
    fprintf(stderr, "[PESMA] %s not implemented\n", __func__);
    return -1;
}

/* Socket utils */

int pesma_socket_shutdown(PHandle* handle, int how)
{
    (void) handle;
    (void) how;
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
    (void) handle;
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
    (void) handle;
    fprintf(stderr, "[PESMA] %s not implemented\n", __func__);
    return -1;
}

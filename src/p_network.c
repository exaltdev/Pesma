#include "pesma_internal.h"
#include <stdlib.h>
#include <stdbool.h>

/* TCP */

PHandle* pesma_tcp_client_create(const char* dns_address, uint16_t port) {
    return NULL;
}

int pesma_tcp_connect(PHandle* handle) {
    return 0;
}

PHandle* pesma_tcp_server_create(uint16_t port) {
    return NULL;
}

PHandle* pesma_tcp_accept(PHandle* handle) {
    return NULL;
}

bool pesma_handle_connected(PHandle* handle) {
    return false;
}

ssize_t pesma_tcp_send(PHandle* handle, const void* data, size_t len) {
    return 0;
}

ssize_t pesma_tcp_receive(PHandle* handle, void* buffer, size_t len) {
    return 0;
}

/* UDP */

PHandle* pesma_udp_create(const char* dns_address, uint16_t port) {
    return NULL;
}

int pesma_udp_connect(PHandle* handle) {
    return 0;
}

ssize_t pesma_udp_send(PHandle* handle, const void* data, size_t len) {
    return 0;
}

ssize_t pesma_udp_receive(PHandle* handle, void* buffer, size_t len) {
    return 0;
}

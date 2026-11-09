#include "pesma.h"
#include <stdbool.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

/* Utilities */

PType pesma_handle_get_type(PHandle* handle)
{
    return handle->type;
}

int pesma_handle_free(PHandle* handle)
{
    //TODO: Cleaner logic here.
    if(handle->type == P_TYPE_FILE || handle->type == P_TYPE_FIFO) {
        free(handle->backend.file.path);
        close(handle->backend.file.fd);
    }
    if(handle->type == P_TYPE_SOCKET) {
        close(handle->backend.socket.fd);
    }
    free(handle->write_buffer.data);
    if(!(handle->type == P_TYPE_BUFFER))
        free(handle->read_buffer.data);
    free(handle);
    return 0;
}

int pesma_buffer_clear(PHandle* handle, char type)
{
    PBuffer* buf;
    if(type == 'w')
        buf = &(handle->write_buffer);
    else if(type == 'r')
        buf = &(handle->read_buffer);
    else {
        fprintf(stderr, "[PESMA] Attempting to clear nonexistant buffer type\n");
        return -1;
    }

    memset(buf->data, 0, buf->size);
    buf->used = 0;
    buf->front = 0;
    buf->back = 0;
    return 0;
}

void pesma_internal_buffers_create(PHandle* handle, size_t size)
{
    PBuffer* buf = &(handle->write_buffer);
    uint8_t* data = calloc(size, 1);
    buf->data = data;
    buf->size = size;
    buf->used = 0;
    buf->front = 0;
    buf->back = 0;

    buf = &(handle->read_buffer);
    if(handle->type != P_TYPE_BUFFER)
        data = calloc(size, 1);
    buf->data = data;
    buf->size = size;
    buf->used = 0;
    buf->front = 0;
    buf->back = 0;
    return;
}

/* Universal send/recv functions */

ssize_t pesma_recv(PHandle* handle, size_t len)
{
    if(handle->type == P_TYPE_BUFFER) {
        pesma_buffer_sync(handle);
        return 0;
    }

    int fd;
    int is_socket = 0;
    switch(handle->type) {
    case P_TYPE_FILE:
    case P_TYPE_FIFO: fd = handle->backend.file.fd; break;
    case P_TYPE_SOCKET:
        fd = handle->backend.socket.fd;
        is_socket = 1;
        break;
    default: return -1;
    }

    PBuffer* b = &handle->read_buffer;
    size_t space = b->size - b->used;
    size_t to_read = len < space ? len : space;
    if(to_read == 0)
        return 0;

    size_t first = b->size - b->front;
    if(first > to_read)
        first = to_read;

    ssize_t n =
        is_socket ? recv(fd, b->data + b->front, first, 0) : read(fd, b->data + b->front, first);
    if(n <= 0)
        return n;

    b->front = (b->front + (size_t) n) % b->size;
    b->used += (size_t) n;
    ssize_t total = n;

    if((size_t) n == first && to_read > first) {
        size_t second = to_read - first;
        n = is_socket ? recv(fd, b->data, second, 0) : read(fd, b->data, second);
        if(n > 0) {
            b->front = (size_t) n % b->size;
            b->used += (size_t) n;
            total += n;
        }
    }
    return total;
}

ssize_t pesma_send(PHandle* handle, size_t len)
{
    if(handle->type == P_TYPE_BUFFER)
        return 0;

    int fd;
    int is_socket = 0;
    switch(handle->type) {
    case P_TYPE_FILE:
    case P_TYPE_FIFO: fd = handle->backend.file.fd; break;
    case P_TYPE_SOCKET:
        fd = handle->backend.socket.fd;
        is_socket = 1;
        break;
    default: return -1;
    }

    PBuffer* b = &handle->write_buffer;
    size_t to_send = len < b->used ? len : b->used;
    if(to_send == 0)
        return 0;

    size_t first = b->size - b->back;
    if(first > to_send)
        first = to_send;

    ssize_t n =
        is_socket ? send(fd, b->data + b->back, first, 0) : write(fd, b->data + b->back, first);
    if(n <= 0)
        return n;

    b->back = (b->back + (size_t) n) % b->size;
    b->used -= (size_t) n;
    ssize_t total = n;

    if((size_t) n == first && to_send > first) {
        size_t second = to_send - first;
        n = is_socket ? send(fd, b->data, second, 0) : write(fd, b->data, second);
        if(n > 0) {
            b->back = (size_t) n % b->size;
            b->used -= (size_t) n;
            total += n;
        }
    }
    return total;
}

/* Typed write helpers */

ssize_t pesma_internal_write(PHandle* handle, size_t type_size, void* value, const char* message)
{
    uint8_t* data;
    uint8_t* bytes;

    data = handle->write_buffer.data;

    if(handle->write_buffer.size - handle->write_buffer.used < type_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to write %s\n", message);
        return -1;
    }
    bytes = (uint8_t*) value;
    for(size_t i = type_size; i > 0; i--) {
        data[handle->write_buffer.front++] = bytes[i - 1];
        handle->write_buffer.front %= handle->write_buffer.size;
        handle->write_buffer.used++;
    }

    return 0;
}

ssize_t pesma_write_int8(PHandle* handle, int8_t value)
{
    return pesma_internal_write(handle, sizeof(value), &value, "int8_t");
}

ssize_t pesma_write_int16(PHandle* handle, int16_t value)
{
    return pesma_internal_write(handle, sizeof(value), &value, "int16_t");
}

ssize_t pesma_write_int32(PHandle* handle, int32_t value)
{
    return pesma_internal_write(handle, sizeof(value), &value, "int32_t");
}

ssize_t pesma_write_int64(PHandle* handle, int64_t value)
{
    return pesma_internal_write(handle, sizeof(value), &value, "int64_t");
}

ssize_t pesma_write_uint8(PHandle* handle, uint8_t value)
{
    return pesma_internal_write(handle, sizeof(value), &value, "uint8_t");
}

ssize_t pesma_write_uint16(PHandle* handle, uint16_t value)
{
    return pesma_internal_write(handle, sizeof(value), &value, "uint16_t");
}

ssize_t pesma_write_uint32(PHandle* handle, uint32_t value)
{
    return pesma_internal_write(handle, sizeof(value), &value, "uint32_t");
}

ssize_t pesma_write_uint64(PHandle* handle, uint64_t value)
{
    return pesma_internal_write(handle, sizeof(value), &value, "uint64_t");
}

ssize_t pesma_write_float(PHandle* handle, float value)
{
    return pesma_internal_write(handle, sizeof(value), &value, "float");
}

ssize_t pesma_write_double(PHandle* handle, double value)
{
    return pesma_internal_write(handle, sizeof(value), &value, "double");
}

ssize_t pesma_write_char(PHandle* handle, char value)
{
    return pesma_internal_write(handle, sizeof(value), &value, "char");
}

ssize_t pesma_write_string(PHandle* handle, const char* str, size_t length)
{
    uint8_t* data;

    data = handle->write_buffer.data;

    if(handle->write_buffer.size - handle->write_buffer.used < length) {
        fprintf(stderr,
            "[PESMA] The write buffer size is to small to write string length %ld\n",
            length);
        return -1;
    }

    for(size_t i = 0; i < length; i++) {
        data[handle->write_buffer.front++] = str[i];
        handle->write_buffer.front %= handle->write_buffer.size;
        handle->write_buffer.used++;
    }

    return 0;
}

/* Typed read helpers */

ssize_t pesma_internal_read(PHandle* handle, size_t type_size, void* value, const char* message)
{
    uint8_t* data;
    uint8_t* bytes;

    data = handle->read_buffer.data;
    if(type_size > handle->read_buffer.used) {
        fprintf(stderr,
            "[PESMA] Attempting to read %s while there is not enough space in buffer\n",
            message);
        return -1;
    }
    bytes = (uint8_t*) value;

    for(size_t i = type_size; i > 0; i--) {
        bytes[i - 1] = data[handle->read_buffer.back++];
        handle->read_buffer.back %= handle->read_buffer.size;
        handle->read_buffer.used--;
    }
    return 0;
}

ssize_t pesma_read_int8(PHandle* handle, int8_t* value)
{
    return pesma_internal_read(handle, sizeof(int8_t), value, "int8_t");
}

ssize_t pesma_read_int16(PHandle* handle, int16_t* value)
{
    return pesma_internal_read(handle, sizeof(int16_t), value, "int16_t");
}

ssize_t pesma_read_int32(PHandle* handle, int32_t* value)
{
    return pesma_internal_read(handle, sizeof(int32_t), value, "int32_t");
}

ssize_t pesma_read_int64(PHandle* handle, int64_t* value)
{
    return pesma_internal_read(handle, sizeof(int64_t), value, "int64_t");
}

ssize_t pesma_read_uint8(PHandle* handle, uint8_t* value)
{
    return pesma_internal_read(handle, sizeof(uint8_t), value, "uint8_t");
}

ssize_t pesma_read_uint16(PHandle* handle, uint16_t* value)
{
    return pesma_internal_read(handle, sizeof(uint16_t), value, "uint16_t");
}

ssize_t pesma_read_uint32(PHandle* handle, uint32_t* value)
{
    return pesma_internal_read(handle, sizeof(uint32_t), value, "uint32_t");
}

ssize_t pesma_read_uint64(PHandle* handle, uint64_t* value)
{
    return pesma_internal_read(handle, sizeof(uint64_t), value, "uint64_t");
}

ssize_t pesma_read_float(PHandle* handle, float* value)
{
    return pesma_internal_read(handle, sizeof(float), value, "float");
}

ssize_t pesma_read_double(PHandle* handle, double* value)
{
    return pesma_internal_read(handle, sizeof(double), value, "double");
}

ssize_t pesma_read_char(PHandle* handle, char* value)
{
    return pesma_internal_read(handle, sizeof(char), value, "char");
}

ssize_t pesma_read_string(PHandle* handle, char* dst, size_t length)
{
    uint8_t* data;
    data = handle->read_buffer.data;

    if(handle->read_buffer.used < length) {
        fprintf(stderr,
            "[PESMA] Attempting to read string length %ld while there is not enough space in " "buf"
                                                                                               "fer"
                                                                                               "\n",
            length);
        return -1;
    }

    for(size_t i = 0; i < length; i++) {
        dst[i] = data[handle->read_buffer.back++];
        handle->read_buffer.back %= handle->read_buffer.size;
        handle->read_buffer.used--;
    }

    return 0;
}

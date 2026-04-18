#include "pesma.h"
#include <stdbool.h>
#include <stdlib.h>

/* Utilities */

PType pesma_handle_get_type(PHandle* handle)
{
    return handle->type;
}

int pesma_network_wait(PHandle* handle, bool wait_for_read, bool wait_for_write, int timeout_ms)
{
    return 0;
}

int pesma_handle_free(PHandle* handle)
{
    if(handle->type==P_TYPE_FILE) free(handle->backend.file.file_path);
    free(handle->write_buffer.data);
    free(handle->read_buffer.data);
    free(handle);
    return 0;
}

int pesma_buffer_clear(PHandle* handle, char type)
{
    PBuffer* buf;
    if(type == 'w')buf = &(handle->write_buffer);
    else if(type == 'r')buf = &(handle->read_buffer);
    else {
        fprintf(stderr, "[PESMA] Attempting to clear nonexistant buffer type");
        return -1;
    }

    memset(buf->data, 0, buf->size);
    buf->pos = 0;
    buf->used = 0;
    return 0;
}

void pesma_internal_buffers_create (PHandle* handle, size_t size)
{
    PBuffer* buf = &(handle->write_buffer);
    uint8_t* data = calloc(size, 1);
    buf->data = data;
    buf->size = size;
    buf->pos = 0;
    buf->used = 0;
    
    buf = &(handle->read_buffer);
    if(handle->type != P_TYPE_BUFFER) data = calloc(size, 1);
    buf->size = size;
    buf->pos = 0;
    buf->used = 0;
    return;
}

/* Typed write helpers */

ssize_t pesma_internal_write(PHandle* handle, size_t size, void* value, const char* message)
{
    uint8_t* data;
    uint8_t byte;
    uint64_t casted_value;

    data = handle->write_buffer.data;
    if(handle->write_buffer.pos + size >= handle->write_buffer.size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to write %s", message);
        return 1;
    }
    casted_value = *(uint64_t*) value;
    while(--size >= 0) {
        uint8_t byte = (casted_value >> size * 8) & 0xFF;
        data[handle->write_buffer.pos++] = byte;
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

ssize_t pesma_write_string(PHandle* handle, const char* str)
{
    uint8_t* data;
    uint64_t length;
    uint64_t pos;

    data = handle->write_buffer.data;
    for(length = 0; str[length] != 0; length++);
    length++;

    if(handle->write_buffer.used + length > handle->write_buffer.size) {
        fprintf(stderr,
            "[PESMA] The write buffer size is to small to pack the string of length %ld",
            length);
        return 1;
    }

    for(pos = 0; pos < length; pos++) { data[handle->write_buffer.pos++] = str[pos]; }

    return 0;
}

/* Typed read helpers */

uint64_t pesma_internal_read(PHandle* handle, size_t size, const char* message)
{
    uint64_t value;
    uint8_t* data;

    value = 0;
    data = handle->read_buffer.data;
    if(handle->read_buffer.pos + size >= handle->read_buffer.size) {
        fprintf(stderr,
            "[PESMA] Attempting to read %s while there is not enough space in buffer",
            message);
        return 1;
    }
    while(--size >= 0) {
        uint8_t byte = data[handle->read_buffer.pos++];
        value |= byte << size * 8;
    }
    return value;
}

int8_t pesma_read_int8(PHandle* handle)
{
    return (int8_t) pesma_internal_read(handle, sizeof(int8_t), "int8_t");
}

int16_t pesma_read_int16(PHandle* handle)
{
    return (int16_t) pesma_internal_read(handle, sizeof(int16_t), "int16_t");
}

int32_t pesma_read_int32(PHandle* handle)
{
    return (int32_t) pesma_internal_read(handle, sizeof(int32_t), "int32_t");
}

int64_t pesma_read_int64(PHandle* handle)
{
    return (int64_t) pesma_internal_read(handle, sizeof(int64_t), "int64_t");
}

uint8_t pesma_read_uint8(PHandle* handle)
{
    return (uint8_t) pesma_internal_read(handle, sizeof(uint8_t), "uint8_t");
}

uint16_t pesma_read_uint16(PHandle* handle)
{
    return (uint16_t) pesma_internal_read(handle, sizeof(uint16_t), "uint16_t");
}

uint32_t pesma_read_uint32(PHandle* handle)
{
    return (uint32_t) pesma_internal_read(handle, sizeof(uint32_t), "uint32_t");
}

uint64_t pesma_read_uint64(PHandle* handle)
{
    return (uint64_t) pesma_internal_read(handle, sizeof(uint64_t), "uint64_t");
}

float pesma_read_float(PHandle* handle)
{
    return (float) pesma_internal_read(handle, sizeof(float), "float");
}

double pesma_read_double(PHandle* handle)
{
    return (double) pesma_internal_read(handle, sizeof(double), "double");
}

char pesma_read_char(PHandle* handle)
{
    return (char) pesma_internal_read(handle, sizeof(char), "char");
}

ssize_t pesma_read_string(PHandle* handle, char* dst, size_t max_len)
{
    uint8_t* data;
    int pos;
    data = handle->read_buffer.data;

    for(pos = 0; data[pos] != 0; pos++) { dst[pos] = data[pos]; }
    dst[pos] = '\0';
    return 0;
}

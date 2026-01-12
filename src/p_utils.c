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
    return 0;
}

int pesma_buffer_clear(PHandle* handle, bool type)
{
    return 0;
}

/* Typed write helpers */

ssize_t pesma_write_int8(PHandle* handle, int8_t value)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 1 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int8_t");
        return 1;
    }
    *(int8_t*)&(data[handle->write_buffer.buffer_pos]) = value;
    handle->write_buffer.buffer_used += 1;
    handle->write_buffer.buffer_pos += 1;
    return 0;
}

ssize_t pesma_write_int16(PHandle* handle, int16_t value)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 2 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int16_t");
        return 1;
    }
    *(int16_t*)&(data[handle->write_buffer.buffer_pos]) = value;
    handle->write_buffer.buffer_used += 2;
    handle->write_buffer.buffer_used += 2;
    return 0;
}

ssize_t pesma_write_int32(PHandle* handle, int32_t value)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 4 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int32_t");
        return 1;
    }
    *(int32_t*)&(data[handle->write_buffer.buffer_pos]) = value;
    handle->write_buffer.buffer_used += 4;
    handle->write_buffer.buffer_used += 4;
    return 0;
}

ssize_t pesma_write_int64(PHandle* handle, int64_t value)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 8 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int32_t");
        return 1;
    }
    *(int64_t*)&(data[handle->write_buffer.buffer_pos]) = value;
    handle->write_buffer.buffer_used += 8;
    handle->write_buffer.buffer_used += 8;
    return 0;
}

ssize_t pesma_write_uint8(PHandle* handle, uint8_t value)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 1 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int8");
        return 1;
    }
    *(uint8_t*)&(data[handle->write_buffer.buffer_pos]) = value;
    handle->write_buffer.buffer_used += 1;
    handle->write_buffer.buffer_used += 1;
    return 0;
}

ssize_t pesma_write_uint16(PHandle* handle, uint16_t value)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 2 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int8");
        return 1;
    }
    *(uint16_t*)&(data[handle->write_buffer.buffer_pos]) = value;
    handle->write_buffer.buffer_used += 2;
    handle->write_buffer.buffer_used += 2;
    return 0;
}

ssize_t pesma_write_uint32(PHandle* handle, uint32_t value)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 4 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int8");
        return 1;
    }
    *(uint32_t*)&(data[handle->write_buffer.buffer_pos]) = value;
    handle->write_buffer.buffer_used += 4;
    handle->write_buffer.buffer_used += 4;
    return 0;
}

ssize_t pesma_write_uint64(PHandle* handle, uint64_t value)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 8 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int8");
        return 1;
    }
    *(uint64_t*)&(data[handle->write_buffer.buffer_pos]) = value;
    handle->write_buffer.buffer_used += 8;
    handle->write_buffer.buffer_used += 8;
    return 0;
}

ssize_t pesma_write_char(PHandle* handle, char value)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 1 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int8");
        return 1;
    }
    *(char*)&(data[handle->write_buffer.buffer_pos]) = value;
    handle->write_buffer.buffer_used += 1;
    handle->write_buffer.buffer_used += 1;
    return 0;
}

ssize_t pesma_write_string(PHandle* handle, const char* str)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 1 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int8");
        return 1;
    }
    ((char*) data)[handle->write_buffer.buffer_pos] = value;
    handle->write_buffer.buffer_used += 1;
    handle->write_buffer.buffer_used += 1;
    return 0;
}

/* Typed read helpers */

int8_t pesma_read_int8(PHandle* handle)
{
    uint8_t value;
    uint8_t* data;

    data = handle->read_buffer.data;
    value = ((int8_t*) data)[handle->read_buffer.buffer_pos];
    handle->read_buffer.buffer_pos += 1;
    return value;
}

int16_t pesma_read_int16(PHandle* handle)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 1 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int8");
        return 1;
    }
    ((int16_t*) data)[handle->write_buffer.buffer_pos] = value;
    handle->write_buffer.buffer_used += 1;
    handle->write_buffer.buffer_used += 1;
    return 0;
}

int32_t pesma_read_int32(PHandle* handle)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 1 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int8");
        return 1;
    }
    ((int8_t*) data)[handle->write_buffer.buffer_pos] = value;
    handle->write_buffer.buffer_used += 1;
    handle->write_buffer.buffer_used += 1;
    return 0;
}

int64_t pesma_read_int64(PHandle* handle)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 1 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int8");
        return 1;
    }
    ((int8_t*) data)[handle->write_buffer.buffer_pos] = value;
    handle->write_buffer.buffer_used += 1;
    handle->write_buffer.buffer_used += 1;
    return 0;
}

uint8_t pesma_read_uint8(PHandle* handle)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 1 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int8");
        return 1;
    }
    ((int8_t*) data)[handle->write_buffer.buffer_pos] = value;
    handle->write_buffer.buffer_used += 1;
    handle->write_buffer.buffer_used += 1;
    return 0;
}

uint16_t pesma_read_uint16(PHandle* handle)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 1 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int8");
        return 1;
    }
    ((int8_t*) data)[handle->write_buffer.buffer_pos] = value;
    handle->write_buffer.buffer_used += 1;
    handle->write_buffer.buffer_used += 1;
    return 0;
}

uint32_t pesma_read_uint32(PHandle* handle)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 1 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int8");
        return 1;
    }
    ((int8_t*) data)[handle->write_buffer.buffer_pos] = value;
    handle->write_buffer.buffer_used += 1;
    handle->write_buffer.buffer_used += 1;
    return 0;
}

uint64_t pesma_read_uint64(PHandle* handle)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 1 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int8");
        return 1;
    }
    ((int8_t*) data)[handle->write_buffer.buffer_pos] = value;
    handle->write_buffer.buffer_used += 1;
    handle->write_buffer.buffer_used += 1;
    return 0;
}

char pesma_read_char(PHandle* handle)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 1 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int8");
        return 1;
    }
    ((int8_t*) data)[handle->write_buffer.buffer_pos] = value;
    handle->write_buffer.buffer_used += 1;
    handle->write_buffer.buffer_used += 1;
    return 0;
}

ssize_t pesma_read_string(PHandle* handle, char* dst, size_t max_len)
{
    uint8_t* data;
    data = handle->write_buffer.data;

    if(handle->write_buffer.buffer_used + 1 > handle->write_buffer.buffer_size) {
        fprintf(stderr, "[PESMA] The write buffer size is to small to pack int8");
        return 1;
    }
    ((int8_t*) data)[handle->write_buffer.buffer_pos] = value;
    handle->write_buffer.buffer_used += 1;
    handle->write_buffer.buffer_used += 1;
    return 0;
}

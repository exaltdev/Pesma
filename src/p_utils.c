#include "pesma.h"
#include <stdlib.h>
#include <stdbool.h>

/* Utilities */

PType pesma_handle_get_type(PHandle* handle) {
    return P_TYPE_NONE;
}

int pesma_network_wait(PHandle* handle, bool wait_for_read, bool wait_for_write, int timeout_ms) {
    return 0;
}

int pesma_handle_free(PHandle* handle) {
    return 0;
}

int pesma_buffer_clear(PHandle* handle, PBufferType type) {
    return 0; 
}

/* Typed write helpers */

ssize_t pesma_write_int8(PHandle* handle, int8_t value) { return 0; }
ssize_t pesma_write_int16(PHandle* handle, int16_t value) { return 0; }
ssize_t pesma_write_int32(PHandle* handle, int32_t value) { return 0; }
ssize_t pesma_write_int64(PHandle* handle, int64_t value) { return 0; }

ssize_t pesma_write_uint8(PHandle* handle, uint8_t value) { return 0; }
ssize_t pesma_write_uint16(PHandle* handle, uint16_t value) { return 0; }
ssize_t pesma_write_uint32(PHandle* handle, uint32_t value) { return 0; }
ssize_t pesma_write_uint64(PHandle* handle, uint64_t value) { return 0; }

ssize_t pesma_write_char(PHandle* handle, char value) { return 0; }
ssize_t pesma_write_string(PHandle* handle, const char* str) { return 0; }

/* Typed read helpers */

ssize_t pesma_read_int8(PHandle* handle, int8_t* value) { return 0; }
ssize_t pesma_read_int16(PHandle* handle, int16_t* value) { return 0; }
ssize_t pesma_read_int32(PHandle* handle, int32_t* value) { return 0; }
ssize_t pesma_read_int64(PHandle* handle, int64_t* value) { return 0; }

ssize_t pesma_read_uint8(PHandle* handle, uint8_t* value) { return 0; }
ssize_t pesma_read_uint16(PHandle* handle, uint16_t* value) { return 0; }
ssize_t pesma_read_uint32(PHandle* handle, uint32_t* value) { return 0; }
ssize_t pesma_read_uint64(PHandle* handle, uint64_t* value) { return 0; }

ssize_t pesma_read_char(PHandle* handle, char* value) { return 0; }
ssize_t pesma_read_string(PHandle* handle, char* dst, size_t max_len) { return 0; }

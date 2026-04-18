#include "pesma.h"
#include <stdlib.h>

/* Memory buffer (FIFO) */

PHandle* pesma_buffer_create(size_t capacity)
{
    PHandle* handle = malloc(sizeof(PHandle));
    handle->type = P_TYPE_BUFFER;
    pesma_internal_buffers_create(handle, 1024);
    return handle;
}

ssize_t pesma_buffer_get(PHandle* handle, size_t len)
{
    return 0;
}

ssize_t pesma_buffer_set(PHandle* handle, size_t len)
{
    return 0;
}

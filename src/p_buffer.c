#include "pesma.h"
#include <stdlib.h>

/* Memory buffer (FIFO) */

PHandle* pesma_buffer_create(size_t capacity)
{
    PHandle* handle = malloc(sizeof(PHandle));
    handle->type = P_TYPE_BUFFER;
    pesma_internal_buffers_create(handle, capacity);
    return handle;
}

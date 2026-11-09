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

void pesma_buffer_sync(PHandle* handle){
    int front = handle->write_buffer.front;
    int back = handle->write_buffer.back;
    int used = handle->write_buffer.used;
    
    handle->read_buffer.used = used;
    handle->read_buffer.front = front;
    handle->read_buffer.back = back;

    return;
}

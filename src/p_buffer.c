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
    int pos = handle->write_buffer.pos;
    int used = handle->write_buffer.used;
    int size = handle->write_buffer.size;
    if(pos >= used)
        pesma_buffer_clear(handle, 'r');//Not needed?
    else
        memmove(handle->write_buffer.data, handle->write_buffer.data + pos, used - pos);
    
    handle->read_buffer.used = used - pos;
    handle->write_buffer.used = used - pos;
    handle->read_buffer.pos = 0;
    
    return;
}

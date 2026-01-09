#include "pesma.h"
#include <stdio.h>
#include <stdlib.h>

/* File operations */

PHandle* pesma_file_open(const char* path, const char* mode)
{
    return NULL;
}

size_t pesma_file_size(PHandle* handle)
{
    return 0;
}

int pesma_file_seek(PHandle* handle, long offset, int whence)
{
    return 0;
}

long pesma_file_tell(PHandle* handle)
{
    return 0;
}

ssize_t pesma_file_read(PHandle* handle, size_t len)
{
    return 0;
}

ssize_t  pesma_file_write (PHandle* handle, size_t len)
{
    return 0;
}

#include "pesma.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>

/* File operations */

int pesma_internal_parse_mode(const char* mode){
    int flags = 0;
    int index = 0;
    //TODO: Implement fcnt.h open flags from char arr
    
    return flags;
}

PHandle* pesma_file_open(const char* path, const char* mode)
{
    //TODO: Open files with mode, use parse function
    return NULL;
}

PHandle* pesma_fifo_create (const char* path, const char* mode)
{
    //TODO: mkfifo handle
    return 0;
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

ssize_t pesma_file_write(PHandle* handle, size_t len)
{
   return 0; 
}

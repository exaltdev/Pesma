#include "pesma.h"
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>

/* File operations */

int pesma_internal_parse_mode(const char* mode)
{
    int flags = 0;
    int index = 0;
    int rdwr = 0;
    
    //TODO: Tidy and harden logic on not having double chatacters. This works for now
    for(int i = 0; mode[i] != 0; i++) {
        switch(mode[i]) {
        case 'r': rdwr += 1; break;
        case 'w': rdwr += 2; break;
        case 'a': flags |= O_APPEND; break;
        case 'c': flags |= O_CREAT; break;
        default : printf("[PESMA] Invalid file open mode [%c]", mode[i]); return -1;
        }
    }
    // ORing with O_RDONLY doesnt nothing as O_RDONLY is 0.
    if(!rdwr)
        printf("[PESMA] No read/write choise, deafulting to read only.");
    if(rdwr == 2)
        flags |= O_WRONLY;
    if(rdwr == 3)
        flags |= O_RDWR;
    return flags;
}

PHandle* pesma_file_open(const char* path, const char* mode)
{
    int fd = open(path, pesma_internal_parse_mode(mode), S_IRWXU);
    PHandle* handle = malloc(sizeof(PHandle));
    handle->type = P_TYPE_FILE;
    pesma_internal_buffers_create(handle,FILE_BUFFER_SIZE);
    handle->backend.file.fd = fd;
    //TODO: strcmp the path maybe, maybe something safer we will see..
    handle->backend.file.path = malloc(PATH_MAX);
    strcpy(handle->backend.file.path, path);
    return handle;
}

PHandle* pesma_fifo_create(const char* path, const char* mode)
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
    return read(handle->backend.file.fd, handle->read_buffer.data, len);
}

ssize_t pesma_file_write(PHandle* handle, size_t len)
{
    return write(handle->backend.file.fd, handle->write_buffer.data, len);
}

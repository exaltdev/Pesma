#include "pesma.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

/* File operations */

PHandle* pesma_file_open(const char* path, int flags, mode_t mode)
{
    int fd;
    PHandle* handle;

    fd = open(path, flags, mode);
    if(fd == -1)
        return NULL;

    handle = pesma_internal_handle_create(P_TYPE_FILE, FILE_BUFFER_SIZE);

    handle->backend.file.fd = fd;
    handle->backend.file.path = strdup(path);
    if(!handle->backend.file.path) {
        fprintf(stderr, "[PESMA] malloc fail\n");
        close(fd);
        free(handle);
        return NULL;
    }
    return handle;
}

PHandle* pesma_fifo_open(const char* path, int flags, mode_t mode)
{
    PHandle* handle;

    if(mkfifo(path, mode) == -1 && errno != EEXIST) {
        return NULL;  //EACCES, ENOENT, etc.
    }

    int fd = open(path, flags);
    if(fd == -1)
        return NULL;
    handle = pesma_internal_handle_create(P_TYPE_FILE, FILE_BUFFER_SIZE);

    handle->backend.file.fd = fd;
    handle->backend.file.path = strdup(path);
    if(!handle->backend.file.path) {
        fprintf(stderr, "[PESMA] malloc fail\n");
        close(fd);
        free(handle);
        return NULL;
    }
    return handle;
}

off_t pesma_file_size(PHandle* handle)
{
    off_t current = lseek(handle->backend.file.fd, 0, SEEK_CUR);
    if(current == -1)
        return -1;

    off_t size = lseek(handle->backend.file.fd, 0, SEEK_END);
    if(size == -1) {
        lseek(handle->backend.file.fd, current, SEEK_SET);
        return -1;
    }

    lseek(handle->backend.file.fd, current, SEEK_SET);
    return size;
}

off_t pesma_file_seek(PHandle* handle, off_t offset, int whence)
{
    return lseek(handle->backend.file.fd, offset, whence);
}

off_t pesma_file_tell(PHandle* handle)
{
    return lseek(handle->backend.file.fd, 0, SEEK_CUR);
}

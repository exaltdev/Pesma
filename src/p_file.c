#include "pesma.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <unistd.h>

/* File operations */

int pesma_internal_parse_mode(const char* mode)
{
    int flags = 0;
    int has_read = 0, has_write = 0;

    for(int i = 0; mode[i] != 0; i++) {
        switch(mode[i]) {
        case 'r': has_read = 1; break;
        case 'w': has_write = 1; break;
        case 'a': flags |= O_APPEND; break;
        case 'c': flags |= O_CREAT; break;
        default : fprintf(stderr, "[PESMA] Invalid mode char: %c\n", mode[i]); return -1;
        }
    }

    if(has_read && has_write)
        flags |= O_RDWR;
    else if(has_write)
        flags |= O_WRONLY;
    else
        flags |= O_RDONLY;
    return flags;
}

PHandle* pesma_file_open(const char* path, const char* mode)
{
    int flags = pesma_internal_parse_mode(mode);
    if(flags == -1)
        return NULL;

    int fd;

    if(flags & O_CREAT) {
        fd = open(path, flags, S_IRUSR | S_IWUSR);
    }
    else {
        fd = open(path, flags);
    }
    if(fd == -1)
        return NULL;

    PHandle* handle = malloc(sizeof(PHandle));
    handle->type = P_TYPE_FILE;
    pesma_internal_buffers_create(handle, FILE_BUFFER_SIZE);
    handle->backend.file.fd = fd;
    //TODO: strcmp the path maybe, maybe something safer we will see..
    handle->backend.file.path = malloc(PATH_MAX);
    if(!handle->backend.file.path) {
        close(fd);
        free(handle);
        return NULL;
    }
    strcpy(handle->backend.file.path, path);
    return handle;
}

PHandle* pesma_fifo_create(const char* path, const char* mode)
{
    if(mkfifo(path, S_IRUSR | S_IWUSR) == -1) {
        if(errno != EEXIST)
            return NULL;
    }

    int flags = pesma_internal_parse_mode(mode);
    if(flags == -1)
        return NULL;

    int fd = open(path, flags);
    if(fd == -1)
        return NULL;

    PHandle* handle = malloc(sizeof(PHandle));
    handle->type = P_TYPE_FIFO;
    pesma_internal_buffers_create(handle, FILE_BUFFER_SIZE);
    handle->backend.file.fd = fd;
    //TODO: strcmp the path maybe, maybe something safer we will see..
    handle->backend.file.path = malloc(PATH_MAX);
    if(!handle->backend.file.path) {
        close(fd);
        free(handle);
        return NULL;
    }
    strcpy(handle->backend.file.path, path);
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

off_t pesma_file_seek(PHandle* handle, long offset, int whence)
{
    return lseek(handle->backend.file.fd, offset, whence);
}

off_t pesma_file_tell(PHandle* handle)
{
    return lseek(handle->backend.file.fd, 0, SEEK_CUR);
}

ssize_t pesma_file_read(PHandle* handle, size_t len)
{
    ssize_t ret = read(handle->backend.file.fd, handle->read_buffer.data, len);
    pesma_buffer_clear(handle, 'r');
    return ret;
}

ssize_t pesma_file_write(PHandle* handle, size_t len)
{
    int move = len;
    int used = handle->write_buffer.used;
    if(len > used)
        move = used;

    ssize_t ret = write(handle->backend.file.fd, handle->write_buffer.data, move);

    if(move == used)
        pesma_buffer_clear(handle, 'w');
    else
        memmove(handle->write_buffer.data, handle->write_buffer.data + move, used - move);

    handle->write_buffer.used = used - move;
    return ret;
}

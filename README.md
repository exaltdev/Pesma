# Pesma - Portable Endian-Safe Memory Abstraction

A C library providing endian-independent I/O abstractions for sockets, files, and memory buffers. Pesma simplifies working with different data sources by offering a unified interface with buffered, typed read/write operations.

## Features

- Endian-independent I/O - Write and read primitive types without worrying about byte order
- Unified buffer system - All handles use consistent read/write buffers
- Multiple backends - TCP, UDP, file, and memory buffer support through a single interface
- Typed operations - Write/read integers, floats, doubles, characters, and strings directly
- Simple API - Consistent function naming and behavior across all handle types

## Status

- [x] Typed read/write operations
- [x] Memory buffer handles
- [x] File I/O handles
- [x] Unified send/recv interface
- [ ] TCP/UDP networking (WIP)
- [ ] epoll integration (TODO)
- [ ] OpenSSL/TLS support (TODO)

## Building

```bash
git clone https://github.com/exaltdev/Pesma.git
cd Pesma
make
```
This builds both the .so shared library and testing with the pesma_tests executable.
If you have bear installed you can run the build.sh script which runs bear -- make.
```bash
./build.sh
```

## Quick Start

Pesma uses a two-phase I/O model. First build your message in the write buffer using typed functions, then send the buffer to its destination.

```c
#include "pesma.h"

int main() {
PHandle* handle = pesma_buffer_create(1024);

// Phase 1: Build message in buffer
pesma_write_int32(handle, 42);
pesma_write_float(handle, 3.14f);
pesma_write_string(handle, "Hello, World!");

// Phase 2: Send the buffer
pesma_buffer_sync(handle); // syncs usage of the buffer
pesma_send(handle, handle->write_buffer.used);

pesma_handle_free(handle);
return 0;
}
```

Reading reads into the internal read buffer, from which you extract your needed types 

```C
#include "pesma.h"
#include <stdio.h>

int main() {
PHandle* handle = pesma_file_open("data.bin", "r");

// Phase 1: Fill read buffer from source
pesma_recv(handle, 1024);

// Phase 2: Extract typed values
int32_t count = pesma_read_int32(handle);
float temperature = pesma_read_float(handle);

char name[64];
pesma_read_string(handle, name, sizeof(name));

printf("Count: %d, Temperature: %.2f, Name: %s\n", count, temperature, name);

pesma_handle_free(handle);
return 0;
}
```

The library handles endianness internally. You write and read values in native byte order and the library converts to a consistent big-endian byte order in the buffer. Data written on a little-endian machine can be read correctly on a big-endian machine.

## License

Check the LICENSE file for the license.

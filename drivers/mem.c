#include <fcntl.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include "mem.h"

int mem_read(void *buffer, off_t ptr, size_t len)
{
    int errno;
    off_t mapped_offset = ptr & ~(sysconf(_SC_PAGESIZE) - 1);
    size_t mapped_len = len + ptr - mapped_offset;

    int fd = open(MEM_DEV, O_RDONLY, 0);
    if (fd == -1) return -1;

    char *mapped = mmap(NULL, mapped_len, PROT_READ, MAP_SHARED, fd, mapped_offset);
    if (mapped == MAP_FAILED) return -1;

    memcpy(buffer, mapped + ptr - mapped_offset, len);

    errno = munmap(mapped, mapped_len);
    if (errno) return errno;

    errno = close(fd);
    if (errno) return errno;

    return 0;
}

int mem_write(off_t ptr, void *buffer, size_t len)
{
    int errno;
    off_t mapped_offset = ptr & ~(sysconf(_SC_PAGESIZE) - 1);
    size_t mapped_len = len + ptr - mapped_offset;

    int fd = open(MEM_DEV, O_RDWR, 0);
    if (fd == -1) return -1;

    char *mapped = mmap(NULL, mapped_len, PROT_WRITE, MAP_SHARED, fd, mapped_offset);
    if (mapped == MAP_FAILED) return -1;

    memcpy(mapped + ptr - mapped_offset, buffer, len);

    errno = munmap(mapped, mapped_len);
    if (errno) return errno;

    errno = close(fd);
    if (errno) return errno;

    return 0;
}

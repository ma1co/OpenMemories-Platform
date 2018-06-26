#pragma once
#include <sys/types.h>

#define MEM_DEV "/dev/mem"

int mem_read(void *buffer, off_t ptr, size_t len);
int mem_write(off_t ptr, void *buffer, size_t len);

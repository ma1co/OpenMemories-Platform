#pragma once

#define BOOTLOADER_DEV "/dev/nflasha"

typedef struct {
    int block;
    int num_pages;
    int page_size;
    int is_safe;
} bootloader_block;

int bootloader_get_num_blocks();
int bootloader_get_blocks(int fd, bootloader_block *blocks);
int bootloader_read_pages(int fd, bootloader_block *block, int page, char *buffer, int n);
int bootloader_get_block_size(bootloader_block *block);
int bootloader_read_block(int fd, bootloader_block *block, char *buffer);

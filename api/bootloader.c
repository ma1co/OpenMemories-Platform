#include "bootloader.h"
#include "drivers/mms_ioc.h"
#include "drivers/nand_ioc.h"

#if !defined DRIVER_mms_ioc && !defined DRIVER_nand_ioc
    #error No bootloader driver available
#endif

int bootloader_get_num_blocks()
{
#if defined DRIVER_mms_ioc
    return MMS_IOC_NUM_BOOTLOADER_BLOCKS;
#elif defined DRIVER_nand_ioc
    return NAND_IOC_NUM_SAFE_PHYS_BLOCKS + NAND_IOC_NUM_NORMAL_PHYS_BLOCKS;
#endif
}

int bootloader_get_blocks(int fd, bootloader_block *blocks)
{
    int errno;
    int block_size, page_size;

#if defined DRIVER_mms_ioc
    errno = mms_ioc_getblocksize(fd, &block_size);
    if (errno) return errno;
    errno = mms_ioc_getpagesize(fd, &page_size);
    if (errno) return errno;
#elif defined DRIVER_nand_ioc
    errno = nand_ioc_get_devinfo(fd, &block_size, &page_size);
    if (errno) return errno;
#endif

    int n_pages_per_block = block_size / page_size;

#if defined DRIVER_mms_ioc
    for (int i = 0; i < MMS_IOC_NUM_BOOTLOADER_BLOCKS; i++)
        blocks[i] = (bootloader_block) {mms_ioc_bootloader_blocks[i], n_pages_per_block, page_size, 0};
#elif defined DRIVER_nand_ioc
    int safe_blocks[NAND_IOC_NUM_SAFE_PHYS_BLOCKS];
    errno = nand_ioc_get_safe_phys_blocks(fd, safe_blocks);
    if (errno) return errno;

    int normal_blocks[NAND_IOC_NUM_NORMAL_PHYS_BLOCKS];
    errno = nand_ioc_get_normal_phys_blocks(fd, normal_blocks);
    if (errno) return errno;

    int b = 0;
    for (int i = 0; i < NAND_IOC_NUM_SAFE_PHYS_BLOCKS; i++, b++)
        blocks[b] = (bootloader_block) {safe_blocks[i], n_pages_per_block, NAND_IOC_PAGE_SAFE_SIZE, 1};
    for (int i = 0; i < NAND_IOC_NUM_NORMAL_PHYS_BLOCKS; i++, b++)
        blocks[b] = (bootloader_block) {normal_blocks[i], n_pages_per_block, page_size, 0};
#endif

    return 0;
}

int bootloader_read_pages(int fd, bootloader_block *block, int page, char *buffer, int n)
{
    int errno;

#if defined DRIVER_mms_ioc
    int first_page = block->block * block->num_pages + page;
    for (int i = 0; i < n; i++) {
        errno = mms_ioc_pread(fd, first_page + i, MMS_IOC_PAGE, block->page_size, buffer + i * block->page_size);
        if (errno) return errno;
    }
#elif defined DRIVER_nand_ioc
    if (block->is_safe) {
        errno = nand_ioc_pread_safe(fd, block->block, page, n, block->num_pages, buffer);
        if (errno) return errno;
    } else {
        char info[n * NAND_IOC_PAGE_NORMAL_INFO_SIZE];
        errno = nand_ioc_pread_normal(fd, block->block, page, block->page_size, n, buffer, info);
        if (errno) return errno;
    }
#endif

    return 0;
}

int bootloader_get_block_size(bootloader_block *block)
{
    return block->num_pages * block->page_size;
}

int bootloader_read_block(int fd, bootloader_block *block, char *buffer)
{
    return bootloader_read_pages(fd, block, 0, buffer, block->num_pages);
}

#pragma once

#define BOOTROM_BASE 0xffff0000

#if defined(GEN0) || defined(GEN1)
#define BOOTROM_SIZE 0x2000
#else
#define BOOTROM_SIZE 0x6000
#endif

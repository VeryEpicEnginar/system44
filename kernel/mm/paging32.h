#pragma once
#include <stdint.h>

#define PAGE_SIZE 4096

#define PDE_PRESENT  0x001
#define PDE_WRITABLE 0x002
#define PDE_USER     0x004

#define PTE_PRESENT  0x001
#define PTE_WRITABLE 0x002
#define PTE_USER     0x004

void paging_init(void);

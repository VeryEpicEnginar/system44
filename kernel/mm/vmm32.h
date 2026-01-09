#pragma once
#include <stdint.h>

#define VMM_PRESENT  0x1
#define VMM_WRITABLE 0x2
#define VMM_USER     0x4

void vmm_map_page(uint32_t vaddr, uint32_t paddr, uint32_t flags);

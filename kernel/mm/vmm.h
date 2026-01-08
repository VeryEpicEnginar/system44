#pragma once
#include <stdint.h>

void vmm_init(void);
void vmm_map(uint64_t virt, uint64_t phys, uint64_t flags);

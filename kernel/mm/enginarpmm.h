#pragma once
#include <stdint.h>
#include <stddef.h>

#define PAGE_SIZE 0x1000

void     pmm_init(uint64_t* e820_addr, uint16_t e820_count);
uint64_t pmm_alloc_page(void);
void     pmm_free_page(uint64_t phys);

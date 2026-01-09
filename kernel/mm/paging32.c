#include "paging.h"
#include <mm/pmm.h>
#include <lib/memory.h>

static uint32_t *page_directory;

void paging_init(void)
{
    page_directory = (uint32_t*)pmm_alloc_page();
    memset(page_directory, 0, PAGE_SIZE);

    // identity map first 4MB
    uint32_t *first_table = (uint32_t*)pmm_alloc_page();
    memset(first_table, 0, PAGE_SIZE);

    for (int i = 0; i < 1024; i++) {
        first_table[i] = (i * PAGE_SIZE) | PTE_PRESENT | PTE_WRITABLE;
    }

    page_directory[0] =
        ((uint32_t)first_table) | PDE_PRESENT | PDE_WRITABLE;

    asm volatile("mov %0, %%cr3" :: "r"(page_directory));
    uint32_t cr0;
    asm volatile("mov %%cr0, %0" : "=r"(cr0));
    cr0 |= 0x80000000; // PG
    asm volatile("mov %0, %%cr0" :: "r"(cr0));
}

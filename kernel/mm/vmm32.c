#include "vmm.h"
#include <mm/pmm.h>

#define PDE_INDEX(x) (((x) >> 22) & 0x3FF)
#define PTE_INDEX(x) (((x) >> 12) & 0x3FF)

extern uint32_t *page_directory;

void vmm_map_page(uint32_t vaddr, uint32_t paddr, uint32_t flags)
{
    uint32_t pde = PDE_INDEX(vaddr);
    uint32_t pte = PTE_INDEX(vaddr);

    uint32_t *table;

    if (!(page_directory[pde] & 1)) {
        table = (uint32_t*)pmm_alloc_page();
        for (int i = 0; i < 1024; i++) table[i] = 0;
        page_directory[pde] =
            ((uint32_t)table) | flags | VMM_PRESENT;
    } else {
        table = (uint32_t*)(page_directory[pde] & 0xFFFFF000);
    }

    table[pte] =
        (paddr & 0xFFFFF000) | flags | VMM_PRESENT;
}

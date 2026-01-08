#include "vmm.h"
#include "pmm.h"
#include <lib/memory.h>

#define PML4E_PRESENT  (1ULL<<0)
#define PML4E_RW       (1ULL<<1)
#define PML4E_USR      (1ULL<<2)

static uint64_t *pml4;

static uint64_t *alloc_table(void) {
    uint64_t phys = pmm_alloc_page();
    uint64_t *virt = (uint64_t*)(phys);
    memset(virt, 0, PAGE_SIZE);
    return virt;
}

static uint64_t *get_or_alloc(uint64_t *entry) {
    if (!(*entry & PML4E_PRESENT)) {
        uint64_t *table = alloc_table();
        *entry = ((uint64_t)table) | PML4E_PRESENT | PML4E_RW | PML4E_USR;
        return table;
    }
    return (uint64_t*)(*entry & ~0xFFFULL);
}

void vmm_init(void) {
    pml4 = alloc_table();

    for (uint64_t addr = 0; addr < 0x100000000ULL; addr+= PAGE_SIZE) {
        vmm_map(addr, addr, PML4E_PRESENT | PML4E_RW);
    }

    asm volatile("mov %0, %%cr3" :: "r"(pml4));
}

void vmm_map(uint64_t virt, uint64_t phys, uint64_t flags) {
    uint64_t *pdpt = get_or_alloc(&pml4[(virt >> 39) & 0x1FF]);
    uint64_t *pd   = get_or_alloc(&pdpt[(virt >> 30) & 0x1FF]);
    uint64_t *pt   = get_or_alloc(&pd[(virt >> 21) & 0x1FF]);

    pt[(virt >> 12) & 0x1FF] = (phys & ~0xFFFULL) | flags;
}

#include "heap.h"
#include "pmm.h"
#include "vmm.h"
#include <lib/memory.h>

#define HEAP_START 0xFFFF000000000000ULL
#define HEAP_LIMIT 0xFFFF000080000000ULL

static uint64_t heap_next = HEAP_START;

void heap_init(void) {
    heap_next = HEAP_START;
}

void* kmalloc(size_t size) {
    size = (size + 7) & ~7ULL;
    if (heap_next + size > HEAP_LIMIT)
        return NULL;

    uint64_t base = heap_next;
    for (uint64_t addr = heap_next; addr < heap_next + size; addr += PAGE_SIZE) {
        uint64_t phys = pmm_alloc_page();
        vmm_map(addr, phys, PML4E_PRESENT | PML4E_RW | PML4E_USR);
    }

    heap_next += size;
    return (void*)base;
}

void kfree(void* ptr) {
    (void)ptr;
}

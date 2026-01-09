#include <stdint.h>
#include <core/panic.h>
#include <mm/pmm.h>
#include <mm/vmm32.h>
#include <mm/heap.h>

#define KHEAP_START  0x80000000
#define KHEAP_END    0x90000000

void page_fault_handler(uint32_t err)
{
    uint32_t addr;
    asm volatile("mov %%cr2, %0" : "=r"(addr));

    uint32_t present = err & 1;
    uint32_t user    = err & 4;

    /* kernel heap grow */
    if (!present &&
        !user &&
        addr >= KHEAP_START &&
        addr < KHEAP_END)
    {
        uint32_t page = addr & 0xFFFFF000;
        uint32_t phys = pmm_alloc_page();
        if (!phys)
            panic("kheap OOM");

        vmm32_map_page(
            page,
            phys,
            VMM_PRESENT | VMM_WRITABLE
        );
        return;
    }

    /* null pointer guard- */
    if (addr < 0x1000)
        panic("NULL dereference");

    panic("PAGE FAULT addr=%x err=%x", addr, err);
}

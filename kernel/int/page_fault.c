#include <stdint.h>
#include <core/panic.h>
#include <mm/pmm.h>
#include <mm/vmm.h>

void page_fault_handler(uint32_t err)
{
    uint32_t fault_addr;
    asm volatile("mov %%cr2, %0" : "=r"(fault_addr));

    uint32_t present  = err & 0x1;
    uint32_t write    = err & 0x2;
    uint32_t user     = err & 0x4;
    uint32_t reserved = err & 0x8;
    uint32_t fetch    = err & 0x10;

    if (!present) {
        uint32_t virt = fault_addr & 0xFFFFF000;
        uint32_t phys = pmm_alloc_page();

        if (!phys)
            panic("Out of physical memory");

        vmm_map_page(
            virt,
            phys,
            VMM_PRESENT | VMM_WRITABLE
        );
        return;
    }

    panic(
        "PAGE FAULT\n"
        "ADDR=%x ERR=%x P=%d W=%d U=%d R=%d I=%d",
        fault_addr,
        err,
        present,
        write,
        user,
        reserved,
        fetch
    );
}

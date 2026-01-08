#include "page_fault.h"
#include <kernel/mm/vmm.h>
#include <kernel/mm/pmm.h>
#include <kernel/lib/printk.h>

static inline uint32_t read_cr2(void)
{
    uint32_t val;
    asm volatile("mov %%cr2, %0" : "=r"(val));
    return val;
}

void page_fault_handler(uint32_t err_code)
{
    uint32_t fault_addr = read_cr2();

    int present   = err_code & 0x1;
    int write     = err_code & 0x2;
    int user      = err_code & 0x4;
    int reserved  = err_code & 0x8;
    int exec      = err_code & 0x10;

    printk("\n#PF at %x\n", fault_addr);
    printk("P=%d W=%d U=%d R=%d X=%d\n",
           present, write, user, reserved, exec);

    uint32_t page = fault_addr & 0xFFFFF000;

    if (!present)
    {
        uint32_t phys = pmm_alloc_page();
        if (!phys)
        {
            panic("Out of physical memory");
        }

        vmm_map_page(page, phys,
            VMM_PRESENT | VMM_WRITABLE | VMM_GLOBAL);

        return;
    }

    panic("Page protection fault");
}

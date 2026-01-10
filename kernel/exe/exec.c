#include "exec.h"
#include "elf.h"
#include "../mm/pmm.h"
#include "../mm/vmm32.h"
#include "../core/panic.h"
#include "../lib/string.h"
#include <stdint.h>

#define USER_STACK_TOP   0xBFFFE000
#define USER_STACK_SIZE  0x4000      /* 16 KB */

extern void exec_enter_user(uint32_t entry, uint32_t user_stack);

void exec_run(void* elf_image)
{
    uint32_t entry = (uint32_t)elf_load(elf_image);

    uint32_t stack_bottom = USER_STACK_TOP - USER_STACK_SIZE;

    for (uint32_t va = stack_bottom + 0x1000;
         va < USER_STACK_TOP;
         va += 0x1000)
    {
        uint32_t pa = (uint32_t)pmm_alloc_pages(1);

        if (!pa)
            panic("exec: out of memory (stack)");

        vmm32_map_page(
            va,
            pa,
            VMM_PRESENT | VMM_USER | VMM_WRITABLE
        );

        memset((void*)va, 0, 0x1000);
    }

    exec_enter_user(entry, USER_STACK_TOP);

    panic("exec: returned from user mode");
}

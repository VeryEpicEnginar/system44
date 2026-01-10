#include "exec.h"
#include "elf.h"
#include "../mm/pmm.h"
#include "../mm/vmm32.h"

#define USER_STACK_TOP  0xBFFFE000
#define USER_STACK_SIZE 0x4000

void exec(const void *image)
{
    uint32_t entry = elf_load(image);

    for (uint32_t off = 0; off < USER_STACK_SIZE; off += 0x1000) {
        uint32_t phys = pmm_alloc_pages(1);

        vmm32_map_page(
            USER_STACK_TOP - off,
            phys,
            VMM_PRESENT | VMM_USER | VMM_WRITABLE
        );
    }

    exec_enter_user(entry, USER_STACK_TOP);
}

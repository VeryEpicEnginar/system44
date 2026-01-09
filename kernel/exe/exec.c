#include <exe/exec.h>
#include <exe/elf.h>

#include <mm/pmm.h>
#include <mm/vmm.h>
#include <mm/paging.h>

#include <core/panic.h>
#include <lib/string.h>
#include <lib/memory.h>

/*
 * Simple execution:
* - Creates a new page index
* - Maps ELF segments * - Sets up the user stack
* - Returns the entry point
 */

#define USER_STACK_TOP   0xBFFFE000
#define USER_STACK_SIZE  0x4000    /* 16 KB */

uint32_t exec_elf(const char *path)
{
    elf_t elf;

    if (elf_load(path, &elf) != 0)
        panic("exec: invalid ELF");

    page_directory_t *new_dir = vmm_create_directory();
    if (!new_dir)
        panic("exec: no memory for page directory");

    vmm_clone_kernel_space(new_dir);

    for (uint32_t i = 0; i < elf.phnum; i++) {
        elf_phdr_t *ph = &elf.phdrs[i];

        if (ph->type != ELF_PT_LOAD)
            continue;

        uint32_t virt = ph->vaddr & 0xFFFFF000;
        uint32_t end  = ph->vaddr + ph->memsz;

        for (; virt < end; virt += 0x1000) {
            uint32_t phys = pmm_alloc_page();
            if (!phys)
                panic("exec: out of memory");

            vmm_map_page_ex(
                new_dir,
                virt,
                phys,
                VMM_PRESENT | VMM_USER | VMM_WRITABLE
            );
        }

        memcpy(
            (void*)ph->vaddr,
            ph->data,
            ph->filesz
        );
        
        if (ph->memsz > ph->filesz) {
            memset(
                (void*)(ph->vaddr + ph->filesz),
                0,
                ph->memsz - ph->filesz
            );
        }
    }

    /* user stack */
    for (uint32_t off = 0; off < USER_STACK_SIZE; off += 0x1000) {
        uint32_t phys = pmm_alloc_page();
        if (!phys)
            panic("exec: stack alloc failed");

        vmm_map_page_ex(
            new_dir,
            USER_STACK_TOP - off,
            phys,
            VMM_PRESENT | VMM_USER | VMM_WRITABLE
        );
    }

    vmm_switch_directory(new_dir);

    return elf.entry;
}

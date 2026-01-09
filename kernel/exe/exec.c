#include <stdint.h>
#include <stddef.h>

#include <kernel/mm/pmm.h>
#include <kernel/mm/vmm32.h>
#include <kernel/mm/paging32.h>
#include <kernel/int/interrupts.h>
#include <kernel/panic.h>

#include "elf32.h"
#include "exec.h"

#define USER_BASE       0x00400000
#define USER_STACK_TOP  0xBFFFE000
#define USER_STACK_SIZE 0x4000   /* 16 KB */

static void exec_create_user_stack(void);

int exec_load_elf32(void *elf_image)
{
    Elf32_Ehdr *eh = (Elf32_Ehdr *)elf_image;

    if (eh->e_ident[0] != 0x7F ||
        eh->e_ident[1] != 'E' ||
        eh->e_ident[2] != 'L' ||
        eh->e_ident[3] != 'F')
        return -1;

    if (eh->e_ident[4] != ELFCLASS32)
        return -1;

    Elf32_Phdr *ph =
        (Elf32_Phdr *)((uint8_t *)elf_image + eh->e_phoff);

    for (uint32_t i = 0; i < eh->e_phnum; i++) {
        if (ph[i].p_type != PT_LOAD)
            continue;

        uint32_t vaddr = ph[i].p_vaddr;
        uint32_t memsz = ph[i].p_memsz;
        uint32_t filesz = ph[i].p_filesz;
        uint8_t *src =
            (uint8_t *)elf_image + ph[i].p_offset;

        for (uint32_t off = 0; off < memsz; off += 0x1000) {
            uint32_t phys = pmm_alloc_page();
            if (!phys)
                panic("exec: out of physical memory");

            vmm32_map_page(
                (vaddr + off) & 0xFFFFF000,
                phys,
                VMM_PRESENT | VMM_USER | VMM_WRITABLE
            );

            uint32_t copy = 0x1000;
            if (off + copy > filesz)
                copy = (filesz > off) ? filesz - off : 0;

            if (copy)
                memcpy(
                    (void *)(vaddr + off),
                    src + off,
                    copy
                );
        }
    }

    exec_create_user_stack();

    uint32_t entry = eh->e_entry;
    exec_enter_user(entry, USER_STACK_TOP);

    return 0;
}


static void exec_create_user_stack(void)
{

    for (uint32_t off = 0; off < USER_STACK_SIZE; off += 0x1000) {
        uint32_t phys = pmm_alloc_page();
        if (!phys)
            panic("exec: stack alloc failed");

        vmm32_map_page(
            USER_STACK_TOP - off,
            phys,
            VMM_PRESENT | VMM_USER | VMM_WRITABLE
        );
    }
}

/*
 * Switch to ring3 and start execution
 * Implemented in assembly
 */
extern void exec_enter_user(uint32_t entry, uint32_t user_stack);

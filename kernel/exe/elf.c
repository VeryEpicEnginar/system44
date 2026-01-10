#include "elf.h"
#include "../mm/pmm.h"
#include "../mm/vmm32.h"
#include "../lib/string.h"
#include "../core/panic.h"

uint32_t elf_load(const void *image)
{
    Elf32_Ehdr *eh = (Elf32_Ehdr *)image;

    if (eh->magic != ELF_MAGIC)
        panic("ELF: bad magic");

    Elf32_Phdr *ph = (Elf32_Phdr *)((uint8_t *)image + eh->phoff);

    for (uint32_t i = 0; i < eh->phnum; i++, ph++) {
        if (ph->type != ELF_PT_LOAD)
            continue;

        uint32_t start = ph->vaddr & 0xFFFFF000;
        uint32_t end   = (ph->vaddr + ph->memsz + 0xFFF) & 0xFFFFF000;

        for (uint32_t va = start; va < end; va += 0x1000) {
            uint32_t pa = pmm_alloc_pages(1);

            vmm32_map_page(
                va,
                pa,
                VMM_PRESENT | VMM_USER | VMM_WRITABLE
            );

            memset((void *)va, 0, 0x1000);
        }

        memcpy(
            (void *)ph->vaddr,
            (const void *)((uint8_t *)image + ph->offset),
            ph->filesz
        );
    }

    return eh->entry;
}

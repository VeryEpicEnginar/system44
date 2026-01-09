#include "elf.h"
#include "exec.h"

#include "../mm/pmm.h"
#include "../mm/vmm32.h"
#include "../core/panic.h"
#include "../lib/string.h"

void* elf_load(void* elf_image)
{
    Elf32_Ehdr* eh = (Elf32_Ehdr*)elf_image;

    if (eh->magic != ELF_MAGIC) {
        panic("Not a valid ELF");
    }

    Elf32_Phdr* ph =
        (Elf32_Phdr*)((uint8_t*)elf_image + eh->phoff);

    for (uint32_t i = 0; i < eh->phnum; i++, ph++) {
        if (ph->type != ELF_PT_LOAD)
            continue;

        uint32_t va = ph->vaddr & 0xFFFFF000;
        uint32_t end = ph->vaddr + ph->memsz;

        for (; va < end; va += 0x1000) {
            uint32_t pa = (uint32_t)pmm_alloc_pages(1);

            if (!pa)
                panic("Out of physical memory (ELF)");

            vmm32_map_page(
                va,
                pa,
                VMM_PRESENT | VMM_USER | VMM_WRITABLE
            );

            memset((void*)va, 0, 0x1000);
        }

        memcpy(
            (void*)ph->vaddr,
            (uint8_t*)elf_image + ph->offset,
            ph->filesz
        );
    }

    return (void*)eh->entry;
}

#include "elf.h"
#include <mm/pmm.h>
#include <mm/vmm.h>
#include <lib/memory.h>

int elf32_load(void *image)
{
    elf32_header_t *hdr = image;
    if (hdr->magic != ELF_MAGIC) return -1;

    elf32_phdr_t *ph =
        (elf32_phdr_t*)((uint8_t*)image + hdr->phoff);

    for (int i = 0; i < hdr->phnum; i++) {
        if (ph[i].type != 1) continue; // PT_LOAD

        uint32_t v = ph[i].vaddr & 0xFFFFF000;
        uint32_t end = ph[i].vaddr + ph[i].memsz;

        for (; v < end; v += 4096) {
            uint32_t phys = pmm_alloc_page();
            vmm_map_page(v, phys,
                VMM_PRESENT | VMM_WRITABLE | VMM_USER);
        }

        memcpy((void*)ph[i].vaddr,
               image + ph[i].offset,
               ph[i].filesz);
    }

    void (*entry)(void) = (void*)hdr->entry;
    entry();
    return 0;
}

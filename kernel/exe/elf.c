#include "elf.h"

#include <mm/pmm.h>
#include <mm/vmm.h>
#include <lib/string.h>
#include <core/panic.h>

#define ELFCLASS64 2
#define ELFDATA2LSB 1
#define EM_X86_64 62

static int elf_validate(elf64_hdr_t *hdr) {
    if (hdr->magic != ELF_MAGIC) return 0;
    if (hdr->elf_class != ELFCLASS64) return 0;
    if (hdr->data != ELFDATA2LSB) return 0;
    if (hdr->machine != EM_X86_64) return 0;
    return 1;
}

static uint64_t elf_flags_to_vmm(uint32_t flags) {
    uint64_t vmm_flags = VMM_PRESENT | VMM_USER;

    if (flags & 2)  // PF_W
        vmm_flags |= VMM_WRITE;

    if (!(flags & 1)) // PF_X yoksa
        vmm_flags |= VMM_NX;

    return vmm_flags;
}

uint64_t elf_load(void *image) {
    elf64_hdr_t *hdr = (elf64_hdr_t *)image;

    if (!elf_validate(hdr))
        panic("ELF: invalid header");

    elf64_phdr_t *phdrs =
        (elf64_phdr_t *)((uint8_t *)image + hdr->phoff);

    for (uint16_t i = 0; i < hdr->phnum; i++) {
        elf64_phdr_t *ph = &phdrs[i];
        if (ph->type != PT_LOAD)
            continue;

        uint64_t start = ph->vaddr & ~0xFFFULL;
        uint64_t end   = (ph->vaddr + ph->memsz + 0xFFF) & ~0xFFFULL;
        uint64_t flags = elf_flags_to_vmm(ph->flags);

        for (uint64_t va = start; va < end; va += 0x1000) {
            uint64_t pa = pmm_alloc_page();
            if (!pa)
                panic("ELF: out of memory");

            vmm_map_page(va, pa, flags);
            memset((void *)va, 0, 0x1000);
        }

        memcpy(
            (void *)ph->vaddr,
            (uint8_t *)image + ph->offset,
            ph->filesz
        );

        if (ph->memsz > ph->filesz) {
            memset(
                (void *)(ph->vaddr + ph->filesz),
                0,
                ph->memsz - ph->filesz
            );
        }
    }

    return hdr->entry;
}

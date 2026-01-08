#include <exe/elf.h>

void run_init(void *elf_image) {
    uint64_t entry = elf_load(elf_image);
    if (!entry)
        panic("exec: ELF load failed");

    ((void (*)(void))entry)();
}

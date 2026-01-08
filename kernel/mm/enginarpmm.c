#include "pmm.h"
#include <lib/memory.h>
#include <core/log.h>

#define MAX_PHYS_PAGES (1024*1024)

static uint8_t *bitmap;
static uint64_t total_pages;

static inline void set_bit(uint64_t i) {
    bitmap[i >> 3] |= (1 << (i & 7));
}

static inline void clr_bit(uint64_t i) {
    bitmap[i >> 3] &= ~(1 << (i & 7));
}

static inline int test_bit(uint64_t i) {
    return bitmap[i >> 3] & (1 << (i & 7));
}

void pmm_init(uint64_t* e820, uint16_t entries) {
    uint64_t bitmap_size = (MAX_PHYS_PAGES / 8) + PAGE_SIZE;
    bitmap = (uint8_t*)e820[0];

    memset(bitmap, 0xFF, bitmap_size);

    total_pages = MAX_PHYS_PAGES;
    for (uint16_t i = 0; i < entries; i++) {
        uint64_t base = e820[i*2 + 0];
        uint64_t size = e820[i*2 + 1];

        uint64_t start_page = base / PAGE_SIZE;
        uint64_t count = size / PAGE_SIZE;

        for (uint64_t p = 0; p < count; p++) {
            uint64_t idx = start_page + p;
            if (idx < total_pages) {
                clr_bit(idx);
            }
        }
    }
}

uint64_t pmm_alloc_page(void) {
    for (uint64_t i = 0; i < total_pages; i++) {
        if (!test_bit(i)) {
            set_bit(i);
            return i * PAGE_SIZE;
        }
    }
    return 0;
}

void pmm_free_page(uint64_t phys) {
    uint64_t idx = phys / PAGE_SIZE;
    if (idx < total_pages) {
        clr_bit(idx);
    }
}

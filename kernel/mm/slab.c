#include "slab.h"
#include "pmm.h"
#include "vmm.h"

#include <lib/memory.h>
#include <core/panic.h>

#define PAGE_SIZE 4096
#define SLAB_MAGIC 0x51A8BEEF

// Cache sizes
static const size_t cache_sizes[] = {
    32, 64, 128, 256, 512, 1024, 2048
};

#define CACHE_COUNT (sizeof(cache_sizes) / sizeof(size_t))

typedef struct slab {
    uint32_t magic;
    uint32_t used;
    uint32_t capacity;
    struct slab* next;
    void* free_list;
} slab_t;

typedef struct slab_cache {
    size_t obj_size;
    slab_t* slabs;
} slab_cache_t;

static slab_cache_t caches[CACHE_COUNT];

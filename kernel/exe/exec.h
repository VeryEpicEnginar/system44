#pragma once
#include <stdint.h>

/* It runs elf and returns an entry point. */
uint32_t exec_elf(const char *path);

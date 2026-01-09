#pragma once
#include <stdint.h>

/* idt entry */
typedef struct {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t  zero;
    uint8_t  type_attr;
    uint16_t offset_high;
} __attribute__((packed)) idt_entry_t;

/* api */
typedef struct {
    uint16_t limit;
    uint32_t base;
} __attribute__((packed)) idt_ptr_t;

void interrupts_init(void);
void idt_set_gate(
    uint8_t  num,
    uint32_t base,
    uint16_t sel,
    uint8_t  flags
);

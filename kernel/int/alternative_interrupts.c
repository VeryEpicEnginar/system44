extern void isr14(void);

void interrupts_init(void)
{
    ...
    idt_set_gate(
        14,
        (uint32_t)isr14,
        0x08,
        0x8E        // present, ring0, interrupt gate
    );
}

[bits 32]

global isr14
extern page_fault_handler

isr14:
    cli

    pushad              ; registers
    push ds
    push es
    push fs
    push gs

    mov ax, 0x10        ; kernel data segment
    mov ds, ax
    mov es, ax

    push dword [esp + 44]   ; error code
    call page_fault_handler
    add esp, 4

    pop gs
    pop fs
    pop es
    pop ds
    popad

    sti
    iretd

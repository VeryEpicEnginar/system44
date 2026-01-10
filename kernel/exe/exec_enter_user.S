.global exec_enter_user

exec_enter_user:
    cli

    mov 4(%esp), %eax    # entry
    mov 8(%esp), %ebx    # user stack

    mov $0x23, %cx       # user data segment
    mov %cx, %ds
    mov %cx, %es
    mov %cx, %fs
    mov %cx, %gs

    pushl $0x23          # SS
    pushl %ebx           # ESP
    pushfl
    pushl $0x1B          # CS
    pushl %eax           # EIP

    iret

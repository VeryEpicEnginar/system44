.global exec_enter_user

/*
 * void exec_enter_user(uint32_t entry, uint32_t user_stack)
 *
 * entry      -> EIP (ring3)
 * user_stack -> ESP (ring3)
 */

exec_enter_user:
    cli

    mov 4(%esp), %eax    /* entry */
    mov 8(%esp), %ebx    /* user stack */

    /* Load user data segment selectors */
    mov $0x23, %ax       /* USER_DS = 0x20 | 3 */
    mov %ax, %ds
    mov %ax, %es
    mov %ax, %fs
    mov %ax, %gs

    /*
     * iret stack layout:
     * SS
     * ESP
     * EFLAGS
     * CS
     * EIP
     */

    pushl $0x23           /* SS */
    pushl %ebx            /* ESP */
    pushfl
    popl %ecx
    orl $0x200, %ecx      /* IF = 1 */
    pushl %ecx
    pushl $0x1B           /* CS = USER_CS = 0x18 | 3 */
    pushl %eax            /* EIP */

    iret

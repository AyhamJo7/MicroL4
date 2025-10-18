/*
 * Context Switching
 */

.section .text
.global context_switch

/*
 * context_switch(tcb_t **old, tcb_t *new)
 *   rdi = old thread pointer
 *   rsi = new thread pointer
 */
context_switch:
    /* Save old context if not NULL */
    testq %rdi, %rdi
    jz load_new

    /* Get pointer to old TCB */
    movq (%rdi), %rax
    testq %rax, %rax
    jz load_new

    /* Save registers to old TCB */
    movq %rbx, 32(%rax)
    movq %rcx, 40(%rax)
    movq %rdx, 48(%rax)
    movq %rsi, 56(%rax)
    movq %rdi, 64(%rax)
    movq %rbp, 72(%rax)
    movq %r8, 80(%rax)
    movq %r9, 88(%rax)
    movq %r10, 96(%rax)
    movq %r11, 104(%rax)
    movq %r12, 112(%rax)
    movq %r13, 120(%rax)
    movq %r14, 128(%rax)
    movq %r15, 136(%rax)

    /* Save stack pointer */
    movq %rsp, 16(%rax)

    /* Save return address as RIP */
    movq (%rsp), %rcx
    movq %rcx, 8(%rax)

load_new:
    /* Load new thread's CR3 */
    movq 24(%rsi), %rax
    movq %rax, %cr3

    /* Restore stack pointer */
    movq 16(%rsi), %rsp

    /* Restore registers */
    movq 32(%rsi), %rbx
    movq 40(%rsi), %rcx
    movq 48(%rsi), %rdx
    movq 72(%rsi), %rbp
    movq 80(%rsi), %r8
    movq 88(%rsi), %r9
    movq 96(%rsi), %r10
    movq 104(%rsi), %r11
    movq 112(%rsi), %r12
    movq 120(%rsi), %r13
    movq 128(%rsi), %r14
    movq 136(%rsi), %r15

    /* Jump to new RIP */
    movq 8(%rsi), %rax
    jmp *%rax

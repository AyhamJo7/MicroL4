/*
 * Interrupt Service Routines (ISR stubs)
 */

.section .text

/* Macro for exceptions without error code */
.macro ISR_NOERRCODE num
.global isr\num
isr\num:
    pushq $0                /* Dummy error code */
    pushq $\num             /* Interrupt number */
    jmp isr_common
.endm

/* Macro for exceptions with error code */
.macro ISR_ERRCODE num
.global isr\num
isr\num:
    pushq $\num             /* Interrupt number */
    jmp isr_common
.endm

/* Exception handlers */
ISR_NOERRCODE 0     /* Divide by zero */
ISR_NOERRCODE 1     /* Debug */
ISR_NOERRCODE 2     /* NMI */
ISR_NOERRCODE 3     /* Breakpoint */
ISR_NOERRCODE 4     /* Overflow */
ISR_NOERRCODE 5     /* Bound range exceeded */
ISR_NOERRCODE 6     /* Invalid opcode */
ISR_NOERRCODE 7     /* Device not available */
ISR_ERRCODE   8     /* Double fault */
ISR_ERRCODE   10    /* Invalid TSS */
ISR_ERRCODE   11    /* Segment not present */
ISR_ERRCODE   12    /* Stack fault */
ISR_ERRCODE   13    /* General protection fault */
ISR_ERRCODE   14    /* Page fault */
ISR_NOERRCODE 16    /* x87 FPU error */

/* Timer IRQ */
ISR_NOERRCODE 32

/* Common ISR handler */
isr_common:
    /* Save all registers */
    pushq %rax
    pushq %rbx
    pushq %rcx
    pushq %rdx
    pushq %rbp
    pushq %rsi
    pushq %rdi
    pushq %r8
    pushq %r9
    pushq %r10
    pushq %r11
    pushq %r12
    pushq %r13
    pushq %r14
    pushq %r15

    /* Pass stack frame to handler */
    movq %rsp, %rdi

    /* Check if this is timer interrupt */
    movq 120(%rsp), %rax    /* Get interrupt number */
    cmpq $32, %rax
    je timer_isr

    /* Call exception handler */
    call exception_handler
    jmp isr_return

timer_isr:
    call timer_handler

isr_return:
    /* Restore registers */
    popq %r15
    popq %r14
    popq %r13
    popq %r12
    popq %r11
    popq %r10
    popq %r9
    popq %r8
    popq %rdi
    popq %rsi
    popq %rbp
    popq %rdx
    popq %rcx
    popq %rbx
    popq %rax

    /* Remove error code and interrupt number */
    addq $16, %rsp

    iretq

/*
 * IDT Interface
 */

#ifndef KERNEL_IDT_H
#define KERNEL_IDT_H

#include <kernel/types.h>

void idt_init(void);

struct interrupt_frame {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rdi, rsi, rbp, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, rsp, ss;
} __attribute__((packed));

void syscall_handler(struct interrupt_frame *frame);

#endif /* KERNEL_IDT_H */
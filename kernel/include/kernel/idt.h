/*
 * IDT Interface
 */

#ifndef KERNEL_IDT_H
#define KERNEL_IDT_H

void idt_init(void);

struct interrupt_frame;
void syscall_handler(struct interrupt_frame *frame);

#endif /* KERNEL_IDT_H */

/*
 * Interrupt Descriptor Table (IDT)
 */

#include "kernel/console.h"
#include "kernel/idt.h"
#include "kernel/types.h"

#define IDT_ENTRIES 256

struct idt_entry {
    uint16_t offset_low;
    uint16_t selector;
    uint8_t ist;
    uint8_t type_attr;
    uint16_t offset_mid;
    uint32_t offset_high;
    uint32_t reserved;
} __attribute__((packed));

struct idt_ptr {
    uint16_t limit;
    uint64_t base;
} __attribute__((packed));

static struct idt_entry idt[IDT_ENTRIES];
static struct idt_ptr idtr;

/* Exception handlers (implemented in assembly stub) */
extern void isr0(void);
extern void isr1(void);
extern void isr2(void);
extern void isr3(void);
extern void isr4(void);
extern void isr5(void);
extern void isr6(void);
extern void isr7(void);
extern void isr8(void);
extern void isr10(void);
extern void isr11(void);
extern void isr12(void);
extern void isr13(void);
extern void isr14(void);
extern void isr16(void);
extern void isr32(void); /* Timer */
extern void isr128(void); /* Syscall */


static void idt_set_gate(uint8_t num, uint64_t handler, uint16_t selector, uint8_t flags) {
    idt[num].offset_low = handler & 0xFFFF;
    idt[num].offset_mid = (handler >> 16) & 0xFFFF;
    idt[num].offset_high = (handler >> 32) & 0xFFFFFFFF;
    idt[num].selector = selector;
    idt[num].ist = 0;
    idt[num].type_attr = flags;
    idt[num].reserved = 0;
}

void idt_init(void) {
    /* Clear IDT */
    for (int i = 0; i < IDT_ENTRIES; i++) {
        idt[i] = (struct idt_entry){0};
    }

    /* Install exception handlers */
    idt_set_gate(0, (uint64_t)isr0, 0x08, 0x8E);
    idt_set_gate(1, (uint64_t)isr1, 0x08, 0x8E);
    idt_set_gate(2, (uint64_t)isr2, 0x08, 0x8E);
    idt_set_gate(3, (uint64_t)isr3, 0x08, 0x8E);
    idt_set_gate(4, (uint64_t)isr4, 0x08, 0x8E);
    idt_set_gate(5, (uint64_t)isr5, 0x08, 0x8E);
    idt_set_gate(6, (uint64_t)isr6, 0x08, 0x8E);
    idt_set_gate(7, (uint64_t)isr7, 0x08, 0x8E);
    idt_set_gate(8, (uint64_t)isr8, 0x08, 0x8E);
    idt_set_gate(10, (uint64_t)isr10, 0x08, 0x8E);
    idt_set_gate(11, (uint64_t)isr11, 0x08, 0x8E);
    idt_set_gate(12, (uint64_t)isr12, 0x08, 0x8E);
    idt_set_gate(13, (uint64_t)isr13, 0x08, 0x8E);
    idt_set_gate(14, (uint64_t)isr14, 0x08, 0x8E);
    idt_set_gate(16, (uint64_t)isr16, 0x08, 0x8E);
    idt_set_gate(32, (uint64_t)isr32, 0x08, 0x8E);
    idt_set_gate(128, (uint64_t)isr128, 0x08, 0xEE); /* Syscall */


    /* Load IDT */
    idtr.limit = sizeof(idt) - 1;
    idtr.base = (uint64_t)&idt;
    __asm__ volatile("lidt %0" : : "m"(idtr));

    kprintf("[IDT] Exception handlers installed\n");
}

struct interrupt_frame {
    uint64_t r15, r14, r13, r12, r11, r10, r9, r8;
    uint64_t rdi, rsi, rbp, rdx, rcx, rbx, rax;
    uint64_t int_no, err_code;
    uint64_t rip, cs, rflags, rsp, ss;
} __attribute__((packed));

void exception_handler(struct interrupt_frame *frame) {
    kprintf("\n[EXCEPTION] Interrupt %d, error code: %x\n", frame->int_no, frame->err_code);
    kprintf("  RIP: %p\n", frame->rip);
    kprintf("  RSP: %p\n", frame->rsp);
    kprintf("  CS:  %x, SS: %x\n", frame->cs, frame->ss);
    kprintf("  RFLAGS: %p\n", frame->rflags);

    if (frame->int_no == 14) {
        /* Page fault */
        uint64_t cr2;
        __asm__ volatile("mov %%cr2, %0" : "=r"(cr2));
        kprintf("  CR2 (fault address): %p\n", cr2);
    }

    kprintf("[PANIC] Halting system\n");
    while (1) {
        __asm__ volatile("hlt");
    }
}

/* Weak timer handler (can be overridden by scheduler) */
__attribute__((weak)) void timer_handler(struct interrupt_frame *frame) {
    (void)frame;
    /* Do nothing by default */
}

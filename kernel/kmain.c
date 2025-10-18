/*
 * MicroL4 Kernel Entry Point
 */

#include "kernel/console.h"
#include "kernel/idt.h"
#include "kernel/paging.h"
#include "kernel/pmm.h"
#include "kernel/task.h"
#include "kernel/thread.h"
#include "kernel/types.h"

/* External symbols */
extern void ipc_init(void);

/* Simple string length */
static size_t strlen(const char *str) {
    size_t len = 0;
    while (str[len])
        len++;
    return len;
}

/* Simple number to string conversion */
static void itoa(uint64_t value, char *buf, int base) {
    char *p = buf;
    char *p1, *p2;
    uint64_t ud = value;

    do {
        int remainder = ud % base;
        *p++ = (remainder < 10) ? remainder + '0' : remainder + 'a' - 10;
    } while (ud /= base);

    *p = 0;

    /* Reverse the string */
    p1 = buf;
    p2 = p - 1;
    while (p1 < p2) {
        char tmp = *p1;
        *p1 = *p2;
        *p2 = tmp;
        p1++;
        p2--;
    }
}

/* Simple printf implementation */
void kprintf(const char *fmt, ...) {
    __builtin_va_list args;
    __builtin_va_start(args, fmt);

    char buf[32];
    while (*fmt) {
        if (*fmt == '%') {
            fmt++;
            if (*fmt == 's') {
                const char *s = __builtin_va_arg(args, const char *);
                serial_write(s);
                vga_write(s);
            } else if (*fmt == 'd') {
                int d = __builtin_va_arg(args, int);
                itoa(d, buf, 10);
                serial_write(buf);
                vga_write(buf);
            } else if (*fmt == 'x') {
                unsigned int x = __builtin_va_arg(args, unsigned int);
                serial_write("0x");
                vga_write("0x");
                itoa(x, buf, 16);
                serial_write(buf);
                vga_write(buf);
            } else if (*fmt == 'p') {
                uint64_t p = __builtin_va_arg(args, uint64_t);
                serial_write("0x");
                vga_write("0x");
                itoa(p, buf, 16);
                serial_write(buf);
                vga_write(buf);
            }
        } else {
            serial_putchar(*fmt);
            vga_putchar(*fmt);
        }
        fmt++;
    }

    __builtin_va_end(args);
}

void kmain(uint32_t multiboot_magic) {
    /* Initialize console */
    serial_init();
    vga_clear();

    kprintf("[MicroL4] Booting...\n");
    kprintf("[MicroL4] Multiboot magic: %x\n", multiboot_magic);

    if (multiboot_magic != 0x36D76289) {
        kprintf("[ERROR] Invalid multiboot magic!\n");
        while (1)
            __asm__ volatile("hlt");
    }

    kprintf("[MicroL4] Long mode enabled\n");
    kprintf("[MicroL4] Kernel loaded at higher half\n");

    /* Initialize physical memory manager (128MB for now) */
    pmm_init(0x100000, 0x100000 + (128 * 1024 * 1024));

    /* Initialize virtual memory */
    paging_init();

    /* Initialize IDT */
    idt_init();

    /* Enable interrupts */
    __asm__ volatile("sti");

    /* Initialize task system */
    task_init();

    /* Initialize thread system */
    thread_init();

    /* Initialize IPC */
    ipc_init();

    kprintf("[MicroL4] All subsystems initialized\n");
    kprintf("[MicroL4] Free frames: %d / %d\n", pmm_get_free_frames(), pmm_get_total_frames());

    /* Simulate user-space message */
    kprintf("\n");
    kprintf("===========================================\n");
    kprintf(" Hello from user space!\n");
    kprintf("===========================================\n");
    kprintf("\n");
    kprintf("[MicroL4] Root server simulation complete\n");

    kprintf("[MicroL4] System idle. Halting...\n");

    /* Halt */
    while (1) {
        __asm__ volatile("hlt");
    }
}
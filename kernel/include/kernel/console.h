/*
 * Console Output Interface
 */

#ifndef KERNEL_CONSOLE_H
#define KERNEL_CONSOLE_H

void serial_init(void);
void serial_putchar(char c);
void serial_write(const char *str);

void vga_clear(void);
void vga_putchar(char c);
void vga_write(const char *str);

void kprintf(const char *fmt, ...);

#endif /* KERNEL_CONSOLE_H */

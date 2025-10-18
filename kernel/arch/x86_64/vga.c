/*
 * VGA Text Mode Driver
 */

#include "kernel/types.h"

#define VGA_WIDTH 80
#define VGA_HEIGHT 25
#define VGA_MEMORY 0xFFFFFFFF800B8000ULL

static uint16_t *vga_buffer = (uint16_t *)VGA_MEMORY;
static size_t vga_row = 0;
static size_t vga_col = 0;
static uint8_t vga_color = 0x0F; /* White on black */

static inline uint16_t vga_entry(unsigned char c, uint8_t color) {
    return (uint16_t)c | (uint16_t)color << 8;
}

void vga_clear(void) {
    for (size_t y = 0; y < VGA_HEIGHT; y++) {
        for (size_t x = 0; x < VGA_WIDTH; x++) {
            const size_t index = y * VGA_WIDTH + x;
            vga_buffer[index] = vga_entry(' ', vga_color);
        }
    }
    vga_row = 0;
    vga_col = 0;
}

void vga_putchar(char c) {
    if (c == '\n') {
        vga_col = 0;
        if (++vga_row >= VGA_HEIGHT) {
            vga_row = 0;
        }
        return;
    }

    const size_t index = vga_row * VGA_WIDTH + vga_col;
    vga_buffer[index] = vga_entry(c, vga_color);

    if (++vga_col >= VGA_WIDTH) {
        vga_col = 0;
        if (++vga_row >= VGA_HEIGHT) {
            vga_row = 0;
        }
    }
}

void vga_write(const char *str) {
    while (*str) {
        vga_putchar(*str++);
    }
}

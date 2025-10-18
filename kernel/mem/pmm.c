/*
 * Physical Memory Manager (Bitmap Allocator)
 */

#include "kernel/pmm.h"

#include "kernel/console.h"
#include "kernel/types.h"

#define PAGE_SIZE 4096
#define BITMAP_SIZE 32768 /* Support up to 512MB with 4KB pages */

static uint8_t frame_bitmap[BITMAP_SIZE];
static size_t total_frames = 0;
static size_t used_frames = 0;

/* Bitmap operations */
static inline void bitmap_set(size_t frame)
{
    size_t byte = frame / 8;
    size_t bit = frame % 8;
    frame_bitmap[byte] |= (1 << bit);
}

static inline void bitmap_clear(size_t frame)
{
    size_t byte = frame / 8;
    size_t bit = frame % 8;
    frame_bitmap[byte] &= ~(1 << bit);
}

static inline bool bitmap_test(size_t frame)
{
    size_t byte = frame / 8;
    size_t bit = frame % 8;
    return frame_bitmap[byte] & (1 << bit);
}

void pmm_init(uint64_t mem_start, uint64_t mem_end)
{
    /* Initialize all frames as used */
    for (size_t i = 0; i < BITMAP_SIZE; i++) {
        frame_bitmap[i] = 0xFF;
    }

    /* Calculate total frames */
    total_frames = (mem_end - mem_start) / PAGE_SIZE;
    if (total_frames > BITMAP_SIZE * 8) {
        total_frames = BITMAP_SIZE * 8;
    }

    /* Mark available frames as free */
    for (size_t i = 0; i < total_frames; i++) {
        bitmap_clear(i);
    }

    used_frames = 0;

    kprintf("[PMM] Initialized: %d frames (%d MB)\n", total_frames,
            (total_frames * PAGE_SIZE) / (1024 * 1024));
}

void pmm_mark_used(uint64_t phys_addr, size_t count)
{
    size_t frame = phys_addr / PAGE_SIZE;
    for (size_t i = 0; i < count; i++) {
        if (!bitmap_test(frame + i)) {
            bitmap_set(frame + i);
            used_frames++;
        }
    }
}

void pmm_mark_free(uint64_t phys_addr, size_t count)
{
    size_t frame = phys_addr / PAGE_SIZE;
    for (size_t i = 0; i < count; i++) {
        if (bitmap_test(frame + i)) {
            bitmap_clear(frame + i);
            used_frames--;
        }
    }
}

uint64_t pmm_alloc_frame(void)
{
    for (size_t i = 0; i < total_frames; i++) {
        if (!bitmap_test(i)) {
            bitmap_set(i);
            used_frames++;
            return i * PAGE_SIZE;
        }
    }
    return 0; /* Out of memory */
}

void pmm_free_frame(uint64_t phys_addr)
{
    size_t frame = phys_addr / PAGE_SIZE;
    if (frame < total_frames && bitmap_test(frame)) {
        bitmap_clear(frame);
        used_frames--;
    }
}

size_t pmm_get_total_frames(void)
{
    return total_frames;
}

size_t pmm_get_used_frames(void)
{
    return used_frames;
}

size_t pmm_get_free_frames(void)
{
    return total_frames - used_frames;
}

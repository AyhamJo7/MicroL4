/*
 * Physical Memory Manager Interface
 */

#ifndef KERNEL_PMM_H
#define KERNEL_PMM_H

#include "kernel/types.h"

void pmm_init(uint64_t mem_start, uint64_t mem_end);
void pmm_mark_used(uint64_t phys_addr, size_t count);
void pmm_mark_free(uint64_t phys_addr, size_t count);

uint64_t pmm_alloc_frame(void);
void pmm_free_frame(uint64_t phys_addr);

size_t pmm_get_total_frames(void);
size_t pmm_get_used_frames(void);
size_t pmm_get_free_frames(void);

#endif /* KERNEL_PMM_H */

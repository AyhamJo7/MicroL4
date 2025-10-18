/*
 * Paging Interface
 */

#ifndef KERNEL_PAGING_H
#define KERNEL_PAGING_H

#include "kernel/types.h"

typedef uint64_t pte_t;

#define PAGE_FLAG_WRITE (1ULL << 1)
#define PAGE_FLAG_USER (1ULL << 2)

void paging_init(void);
bool paging_map_page(pte_t *pml4, uint64_t virt, uint64_t phys, uint64_t flags);
void paging_unmap_page(pte_t *pml4, uint64_t virt);

pte_t *paging_get_kernel_pml4(void);
pte_t *paging_create_address_space(void);
void paging_switch_address_space(pte_t *pml4);

#endif /* KERNEL_PAGING_H */

/*
 * x86-64 Paging (4-level page tables)
 */

#include "kernel/console.h"
#include "kernel/paging.h"
#include "kernel/pmm.h"
#include "kernel/types.h"

#define PAGE_PRESENT (1ULL << 0)
#define PAGE_WRITE (1ULL << 1)
#define PAGE_USER (1ULL << 2)
#define PAGE_SIZE_BIT (1ULL << 7)

#define PAGE_SIZE 4096
#define ENTRIES_PER_TABLE 512

#define PHYS_TO_VIRT(addr) ((void *)((uint64_t)(addr) + 0xFFFFFFFF80000000ULL))
#define VIRT_TO_PHYS(addr) ((uint64_t)(addr) - 0xFFFFFFFF80000000ULL)

typedef uint64_t pte_t;

static pte_t *kernel_pml4 = NULL;

/* Get page table index for each level */
static inline size_t pml4_index(uint64_t virt) {
    return (virt >> 39) & 0x1FF;
}

static inline size_t pdpt_index(uint64_t virt) {
    return (virt >> 30) & 0x1FF;
}

static inline size_t pd_index(uint64_t virt) {
    return (virt >> 21) & 0x1FF;
}

static inline size_t pt_index(uint64_t virt) {
    return (virt >> 12) & 0x1FF;
}

/* Get physical address from page table entry */
static inline uint64_t pte_get_addr(pte_t pte) {
    return pte & 0x000FFFFFFFFFF000ULL;
}

/* Memory operations */
static void memset64(void *dest, uint64_t val, size_t count) {
    uint64_t *d = (uint64_t *)dest;
    for (size_t i = 0; i < count; i++) {
        d[i] = val;
    }
}

/* Allocate a page table */
static pte_t *alloc_page_table(void) {
    uint64_t phys = pmm_alloc_frame();
    if (!phys)
        return NULL;

    pte_t *table = (pte_t *)PHYS_TO_VIRT(phys);
    memset64(table, 0, ENTRIES_PER_TABLE);
    return table;
}

/* Get or create page table at given level */
static pte_t *get_or_create_table(pte_t *table, size_t index) {
    if (!(table[index] & PAGE_PRESENT)) {
        pte_t *new_table = alloc_page_table();
        if (!new_table)
            return NULL;
        table[index] = VIRT_TO_PHYS((uint64_t)new_table) | PAGE_PRESENT | PAGE_WRITE;
    }
    return (pte_t *)PHYS_TO_VIRT(pte_get_addr(table[index]));
}

void paging_init(void) {
    /* Get current PML4 from CR3 */
    uint64_t cr3;
    __asm__ volatile("mov %%cr3, %0" : "=r"(cr3));
    kernel_pml4 = (pte_t *)PHYS_TO_VIRT(cr3 & ~0xFFF);

    kprintf("[VMM] Paging initialized, PML4 at %p\n", (uint64_t)kernel_pml4);
}

bool paging_map_page(pte_t *pml4, uint64_t virt, uint64_t phys, uint64_t flags) {
    /* Get indices */
    size_t pml4_idx = pml4_index(virt);
    size_t pdpt_idx = pdpt_index(virt);
    size_t pd_idx = pd_index(virt);
    size_t pt_idx = pt_index(virt);

    /* Navigate/create page tables */
    pte_t *pdpt = get_or_create_table(pml4, pml4_idx);
    if (!pdpt)
        return false;

    pte_t *pd = get_or_create_table(pdpt, pdpt_idx);
    if (!pd)
        return false;

    pte_t *pt = get_or_create_table(pd, pd_idx);
    if (!pt)
        return false;

    /* Map the page */
    pt[pt_idx] = phys | flags | PAGE_PRESENT;

    /* Invalidate TLB */
    __asm__ volatile("invlpg (%0)" : : "r"(virt) : "memory");

    return true;
}

void paging_unmap_page(pte_t *pml4, uint64_t virt) {
    size_t pml4_idx = pml4_index(virt);
    size_t pdpt_idx = pdpt_index(virt);
    size_t pd_idx = pd_index(virt);
    size_t pt_idx = pt_index(virt);

    if (!(pml4[pml4_idx] & PAGE_PRESENT))
        return;

    pte_t *pdpt = (pte_t *)PHYS_TO_VIRT(pte_get_addr(pml4[pml4_idx]));
    if (!(pdpt[pdpt_idx] & PAGE_PRESENT))
        return;

    pte_t *pd = (pte_t *)PHYS_TO_VIRT(pte_get_addr(pdpt[pdpt_idx]));
    if (!(pd[pd_idx] & PAGE_PRESENT))
        return;

    pte_t *pt = (pte_t *)PHYS_TO_VIRT(pte_get_addr(pd[pd_idx]));
    pt[pt_idx] = 0;

    __asm__ volatile("invlpg (%0)" : : "r"(virt) : "memory");
}

pte_t *paging_get_kernel_pml4(void) {
    return kernel_pml4;
}

pte_t *paging_create_address_space(void) {
    pte_t *new_pml4 = alloc_page_table();
    if (!new_pml4)
        return NULL;

    /* Copy kernel mappings (higher half) */
    for (size_t i = 256; i < 512; i++) {
        new_pml4[i] = kernel_pml4[i];
    }

    return new_pml4;
}

void paging_switch_address_space(pte_t *pml4) {
    uint64_t phys = VIRT_TO_PHYS((uint64_t)pml4);
    __asm__ volatile("mov %0, %%cr3" : : "r"(phys) : "memory");
}

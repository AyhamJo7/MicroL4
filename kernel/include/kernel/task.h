/*
 * Task/Address Space Management
 */

#ifndef KERNEL_TASK_H
#define KERNEL_TASK_H

#include "kernel/paging.h"
#include "kernel/types.h"

struct task {
    uint64_t task_id;
    pte_t *cr3;
    char name[32];
};

typedef struct task task_t;

void task_init(void);
task_t *task_create(const char *name);
void task_destroy(task_t *task);

#endif /* KERNEL_TASK_H */

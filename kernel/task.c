/*
 * Task Implementation
 */

#include "kernel/task.h"

#include "kernel/console.h"
#include "kernel/paging.h"
#include "kernel/types.h"

#define MAX_TASKS 32

static task_t task_pool[MAX_TASKS];
static uint64_t next_task_id = 1;

static void strcpy(char *dest, const char *src, size_t max)
{
    size_t i = 0;
    while (src[i] && i < max - 1) {
        dest[i] = src[i];
        i++;
    }
    dest[i] = '\0';
}

void task_init(void)
{
    for (int i = 0; i < MAX_TASKS; i++) {
        task_pool[i].task_id = 0;
        task_pool[i].cr3 = NULL;
    }
    kprintf("[TASK] Task system initialized\n");
}

task_t *task_create(const char *name)
{
    /* Find free slot */
    task_t *task = NULL;
    for (int i = 0; i < MAX_TASKS; i++) {
        if (task_pool[i].task_id == 0) {
            task = &task_pool[i];
            break;
        }
    }

    if (!task)
        return NULL;

    /* Create address space */
    pte_t *cr3 = paging_create_address_space();
    if (!cr3)
        return NULL;

    task->task_id = next_task_id++;
    task->cr3 = cr3;
    strcpy(task->name, name, sizeof(task->name));

    kprintf("[TASK] Created task %d (%s)\n", task->task_id, name);

    return task;
}

void task_destroy(task_t *task)
{
    /* TODO: Free page tables */
    task->task_id = 0;
    task->cr3 = NULL;
}

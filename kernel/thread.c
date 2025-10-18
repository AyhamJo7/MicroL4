/*
 * Thread Implementation
 */

#include "kernel/thread.h"

#include "kernel/console.h"
#include "kernel/pmm.h"
#include "kernel/types.h"

#define KERNEL_STACK_SIZE (16 * 4096) /* 64KB */

static tcb_t thread_pool[MAX_THREADS];
static tcb_t *current_thread = NULL;
static tcb_t *ready_queue = NULL;
static uint64_t next_tid = 1;

/* Memory operations */
static void *memset(void *dest, int val, size_t len)
{
    uint8_t *d = (uint8_t *) dest;
    for (size_t i = 0; i < len; i++) {
        d[i] = (uint8_t) val;
    }
    return dest;
}

void thread_init(void)
{
    for (int i = 0; i < MAX_THREADS; i++) {
        thread_pool[i].state = THREAD_DEAD;
        thread_pool[i].tid = 0;
    }
    kprintf("[SCHED] Thread system initialized\n");
}

static tcb_t *alloc_tcb(void)
{
    for (int i = 0; i < MAX_THREADS; i++) {
        if (thread_pool[i].state == THREAD_DEAD) {
            return &thread_pool[i];
        }
    }
    return NULL;
}

tcb_t *thread_create(uint64_t entry, uint64_t cr3, bool user_mode)
{
    tcb_t *thread = alloc_tcb();
    if (!thread) {
        kprintf("[THREAD] Failed to allocate TCB\n");
        return NULL;
    }

    /* Allocate stack */
    uint64_t stack_phys = pmm_alloc_frame();
    if (!stack_phys) {
        thread->state = THREAD_DEAD;
        return NULL;
    }

    /* Map stack to virtual address (for now, use physical + offset) */
    uint64_t stack_virt = stack_phys + 0xFFFFFFFF80000000ULL;
    thread->stack_base = (void *) stack_virt;
    thread->stack_size = 4096;

    /* Initialize TCB */
    thread->tid = next_tid++;
    thread->state = THREAD_READY;
    thread->priority = 1;
    thread->cr3 = cr3;
    thread->rip = entry;
    thread->rsp = stack_virt + 4096 - 16; /* Leave space for alignment */
    thread->rflags = 0x202;               /* IF set */
    thread->ipc_partner = 0;
    thread->ipc_buffer = NULL;

    /* Clear registers */
    thread->rax = thread->rbx = thread->rcx = thread->rdx = 0;
    thread->rsi = thread->rdi = thread->rbp = 0;
    thread->r8 = thread->r9 = thread->r10 = thread->r11 = 0;
    thread->r12 = thread->r13 = thread->r14 = thread->r15 = 0;

    /* Add to ready queue */
    thread->next = ready_queue;
    ready_queue = thread;

    kprintf("[THREAD] Created thread %d (entry: %p)\n", thread->tid, entry);

    return thread;
}

void thread_destroy(tcb_t *thread)
{
    if (thread->stack_base) {
        uint64_t phys = (uint64_t) thread->stack_base - 0xFFFFFFFF80000000ULL;
        pmm_free_frame(phys);
    }
    thread->state = THREAD_DEAD;
    thread->tid = 0;
}

tcb_t *thread_get_current(void)
{
    return current_thread;
}

/* Schedule next thread */
tcb_t *scheduler_get_next(void)
{
    if (!ready_queue)
        return NULL;

    /* Simple round-robin */
    tcb_t *next = ready_queue;
    ready_queue = next->next;

    /* Re-add to end of queue if still ready */
    if (next->state == THREAD_READY) {
        tcb_t **tail = &ready_queue;
        while (*tail) {
            tail = &(*tail)->next;
        }
        *tail = next;
        next->next = NULL;
    }

    return next;
}

extern void context_switch(tcb_t **old, tcb_t *new);

void thread_yield(void)
{
    if (!ready_queue)
        return;

    tcb_t *old = current_thread;
    tcb_t *new = scheduler_get_next();

    if (new &&new != old) {
        if (old) {
            old->state = THREAD_READY;
        }
        new->state = THREAD_RUNNING;
        current_thread = new;
        context_switch(&old, new);
    }
}

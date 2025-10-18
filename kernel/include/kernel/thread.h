/*
 * Thread Management
 */

#ifndef KERNEL_THREAD_H
#define KERNEL_THREAD_H

#include "kernel/types.h"

#define MAX_THREADS 64

typedef enum { THREAD_READY, THREAD_RUNNING, THREAD_BLOCKED, THREAD_DEAD } thread_state_t;

struct thread_control_block {
    uint64_t tid;
    thread_state_t state;
    uint64_t priority;

    /* Saved context */
    uint64_t rsp;
    uint64_t rip;
    uint64_t rflags;
    uint64_t cr3;

    /* Saved registers */
    uint64_t rax, rbx, rcx, rdx;
    uint64_t rsi, rdi, rbp;
    uint64_t r8, r9, r10, r11, r12, r13, r14, r15;

    /* Stack */
    void *stack_base;
    size_t stack_size;

    /* IPC state */
    uint64_t ipc_partner;
    void *ipc_buffer;

    /* Linked list */
    struct thread_control_block *next;
};

typedef struct thread_control_block tcb_t;

void thread_init(void);
tcb_t *thread_create(uint64_t entry, uint64_t cr3, bool user_mode);
void thread_destroy(tcb_t *thread);
tcb_t *thread_get_current(void);
void thread_yield(void);

#endif /* KERNEL_THREAD_H */

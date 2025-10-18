/*
 * Synchronous IPC Implementation
 */

#include "kernel/console.h"
#include "kernel/thread.h"
#include "kernel/types.h"
#include "microl4/syscall.h"

#define IPC_TIMEOUT_INFINITE (-1ULL)

/* Simple IPC: synchronous rendezvous */
int ipc_send(uint64_t dest_tid, ipc_msg_t *msg) {
    /* Find destination thread */
    tcb_t *current = thread_get_current();
    if (!current) {
        return -1;
    }

    /* For now, just store message in current thread's buffer */
    if (!current->ipc_buffer) {
        return -2;
    }

    /* Copy message */
    ipc_msg_t *buf = (ipc_msg_t *)current->ipc_buffer;
    buf->sender = current->tid;
    buf->tag = msg->tag;
    for (int i = 0; i < 4; i++) {
        buf->words[i] = msg->words[i];
    }

    /* Block until receiver is ready (simplified) */
    current->state = THREAD_BLOCKED;
    current->ipc_partner = dest_tid;

    /* Would normally yield here and wait for receiver */
    thread_yield();

    return 0;
}

int ipc_receive(uint64_t *src_tid, ipc_msg_t *msg) {
    tcb_t *current = thread_get_current();
    if (!current) {
        return -1;
    }

    /* Block until sender arrives */
    current->state = THREAD_BLOCKED;

    /* Simplified: just return from buffer */
    if (current->ipc_buffer) {
        ipc_msg_t *buf = (ipc_msg_t *)current->ipc_buffer;
        *src_tid = buf->sender;
        msg->tag = buf->tag;
        for (int i = 0; i < 4; i++) {
            msg->words[i] = buf->words[i];
        }
    }

    thread_yield();

    return 0;
}

/* Combined send-receive (call/reply pattern) */
int ipc_call(uint64_t dest_tid, ipc_msg_t *send_msg, ipc_msg_t *recv_msg) {
    int ret = ipc_send(dest_tid, send_msg);
    if (ret < 0)
        return ret;

    uint64_t src;
    return ipc_receive(&src, recv_msg);
}

void ipc_init(void) {
    kprintf("[IPC] IPC system initialized\n");
}

/* Syscall handler */
void syscall_handler(struct interrupt_frame *frame) {
    uint64_t syscall_num = frame->rax;
    uint64_t arg1 = frame->rdi;
    /* uint64_t arg2 = frame->rsi; */
    /* uint64_t arg3 = frame->rdx; */

    switch (syscall_num) {
    case SYS_DEBUG_PRINT:
        /* For now, just print to kernel console */
        kprintf((const char *)arg1);
        break;
    default:
        kprintf("[KERNEL] Unknown syscall: %d\n", syscall_num);
        break;
    }
}

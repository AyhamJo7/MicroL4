/*
 * MicroL4 System Call Interface
 */

#ifndef MICROL4_SYSCALL_H
#define MICROL4_SYSCALL_H

#include "kernel/types.h"

/* Syscall numbers */
#define SYS_IPC 1
#define SYS_THREAD_CREATE 2
#define SYS_THREAD_DESTROY 3
#define SYS_DEBUG_PRINT 99

/* IPC message structure */
struct ipc_message {
    uint64_t sender;
    uint64_t tag;
    uint64_t words[4];
};

typedef struct ipc_message ipc_msg_t;

/* IPC operations */
#define IPC_SEND 1
#define IPC_RECV 2
#define IPC_CALL (IPC_SEND | IPC_RECV)

#endif /* MICROL4_SYSCALL_H */

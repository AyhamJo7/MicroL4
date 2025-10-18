# ADR-0002: Synchronous Rendezvous-Based IPC

## Status

Accepted

## Context

Microkernel IPC must be:
1. **Fast**: Minimize overhead (context switches, copies)
2. **Simple**: Easy to reason about and verify
3. **Safe**: Prevent resource exhaustion and deadlocks

IPC models considered:
- **Asynchronous messaging**: Buffered, non-blocking
- **Synchronous rendezvous**: Direct transfer, blocking
- **Shared memory**: Zero-copy, but requires synchronization
- **Pipes/channels**: Unix-style, buffered

## Decision

Use **synchronous rendezvous IPC** with direct message transfer.

## Rationale

### L4 Principles

L4 microkernels prioritize IPC performance:
> "All system services are IPC-bound, so IPC must be fast."

Rendezvous IPC minimizes:
- **Context switches**: 2 switches per message (send → receive)
- **Memory copies**: 1 copy (sender registers → receiver registers)
- **Kernel state**: No buffering needed

### Message Format

```c
struct ipc_message {
    uint64_t sender;     // Sender TID
    uint64_t tag;        // Message type
    uint64_t words[4];   // Payload (32 bytes)
};
```

Small messages (≤32 bytes) fit in registers → zero-copy fast path (future).

### Blocking Semantics

- **Send**: Block until receiver is ready
- **Receive**: Block until sender arrives
- **Call**: Send + receive (RPC pattern)

## Implementation

### Send Operation

```c
int ipc_send(uint64_t dest_tid, ipc_msg_t *msg) {
    // 1. Find destination thread
    // 2. If dest blocked in receive → deliver immediately
    // 3. Else block sender until dest calls receive
    // 4. Copy message (registers or buffer)
}
```

### Receive Operation

```c
int ipc_receive(uint64_t *src_tid, ipc_msg_t *msg) {
    // 1. Check if sender waiting
    // 2. If yes → copy message, unblock sender
    // 3. Else block until sender arrives
}
```

### State Machine

```
Sender:  RUNNING → ipc_send() → BLOCKED → READY
Receiver: RUNNING → ipc_receive() → BLOCKED → READY
          (rendezvous point)
```

## Consequences

### Advantages

1. **Performance**: Minimal overhead for small messages
2. **Simplicity**: No buffering → less kernel state
3. **Bounded latency**: No queuing delays
4. **Resource safety**: No unbounded memory consumption

### Disadvantages

1. **Deadlock potential**: Circular wait scenarios possible
   - *Mitigation*: Timeouts (future)
2. **Priority inversion**: Low-priority sender blocks high-priority receiver
   - *Mitigation*: Priority inheritance (future)
3. **No decoupling**: Sender and receiver must synchronize
   - *Mitigation*: Intermediate server for async needs

## Alternatives Considered

### Asynchronous Messaging

- **Pros**: Non-blocking, decouples sender/receiver
- **Cons**: Requires buffering, unbounded memory, complex flow control
- **Verdict**: Contradicts L4 minimality

### Shared Memory

- **Pros**: Zero-copy for large data
- **Cons**: Requires locking, hard to verify, cache coherence issues
- **Verdict**: Use for bulk data, but keep IPC for control messages

## Future Optimizations

### Fast Path (Assembly)

For register-only messages:
```asm
# Sender
mov rdi, dest_tid
mov rsi, msg_tag
int 0x80

# Direct transfer to receiver registers (no memory copy)
```

### Capability-Based Endpoints

Replace TID with capability:
```c
ipc_send(cap_endpoint, msg);
```

Prevents confused deputy attacks.

## References

- [L4 IPC Design](https://os.inf.tu-dresden.de/papers_ps/sosp97.pdf)
- [seL4 IPC](https://sel4.systems/Info/Docs/seL4-manual-latest.pdf)
- [Microkernel Performance: IPC](https://www.usenix.org/legacy/events/osdi2010/tech/full_papers/Baumann.pdf)

## Metadata

- **Author**: @AyhamJo7
- **Date**: 2025-10-18
- **Supersedes**: None
- **Related**: ADR-0003 (Scheduler)

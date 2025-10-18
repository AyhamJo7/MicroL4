# MicroL4 Architecture

## Overview

MicroL4 is a minimalist L4-style microkernel implementing fundamental OS abstractions with a focus on simplicity, performance, and IPC-centric design.

## Design Principles

### 1. Minimality

The kernel provides only essential mechanisms:
- **Address Spaces**: Memory isolation via paging
- **Threads**: Units of execution
- **IPC**: Synchronous message passing
- **Scheduling**: Preemptive round-robin

Everything else (drivers, file systems, networking) belongs in user space.

### 2. IPC-Centric

Communication between components happens via fast, synchronous IPC:
- Rendezvous-based: sender blocks until receiver is ready
- Direct message transfer (no buffering)
- Minimal kernel involvement

### 3. Performance-First

- Direct x86-64 optimizations (no portability layers)
- Minimal context switch overhead
- Fast-path IPC in assembly (future optimization)

## System Architecture

```
┌─────────────────────────────────────────────────┐
│              User Space (Ring 3)                │
│                                                 │
│  ┌─────────────┐  ┌─────────────┐              │
│  │ Root Server │  │   Future    │              │
│  │  (init)     │  │  Services   │              │
│  └──────┬──────┘  └──────┬──────┘              │
│         │                 │                     │
│         └────────┬────────┘                     │
│                  │ IPC                          │
├──────────────────┼──────────────────────────────┤
│                  │                              │
│           Kernel API (syscalls)                 │
│                  │                              │
│  ┌───────────────▼──────────────────┐           │
│  │      MicroL4 Kernel (Ring 0)     │           │
│  │                                  │           │
│  │  ┌───────────┐  ┌─────────────┐ │           │
│  │  │   Thread  │  │   Address   │ │           │
│  │  │    Mgmt   │  │    Space    │ │           │
│  │  └───────────┘  └─────────────┘ │           │
│  │                                  │           │
│  │  ┌─────────────────────────────┐│           │
│  │  │       IPC Engine            ││           │
│  │  └─────────────────────────────┘│           │
│  │                                  │           │
│  │  ┌─────────────────────────────┐│           │
│  │  │  Memory Management          ││           │
│  │  │  - PMM (bitmap allocator)   ││           │
│  │  │  - VMM (4-level paging)     ││           │
│  │  └─────────────────────────────┘│           │
│  │                                  │           │
│  │  ┌─────────────────────────────┐│           │
│  │  │  HW Abstraction             ││           │
│  │  │  - IDT / Exceptions         ││           │
│  │  │  - Context switching        ││           │
│  │  └─────────────────────────────┘│           │
│  └──────────────────────────────────┘           │
│            Kernel Space                         │
└─────────────────────────────────────────────────┘
                     │
              ┌──────▼──────┐
              │  x86-64 HW  │
              └─────────────┘
```

## Memory Layout

### Virtual Address Space

```
0x0000000000000000 - 0x00007FFFFFFFFFFF: User space (128 TB)
0xFFFF800000000000 - 0xFFFFFFFF7FFFFFFF: Kernel heap (future)
0xFFFFFFFF80000000 - 0xFFFFFFFFFFFFFFFF: Kernel code/data (2 GB)
```

### Physical Memory

- **0x0000 - 0x100000**: Reserved (BIOS, bootloader)
- **0x100000+**: Managed by PMM (bitmap allocator)

### Page Tables

- **4-level paging**: PML4 → PDPT → PD → PT
- **Page size**: 4 KB
- **Higher-half kernel**: Mapped at -2GB
- **Identity mapping**: Low 2MB for bootstrap only

## Core Subsystems

### 1. Physical Memory Manager (PMM)

- **Allocator**: Bitmap-based frame allocator
- **Granularity**: 4 KB frames
- **Operations**:
  - `pmm_alloc_frame()`: Allocate physical frame
  - `pmm_free_frame()`: Free physical frame
  - `pmm_mark_used/free()`: Reserve/unreserve regions

### 2. Virtual Memory Manager (VMM)

- **Paging**: x86-64 4-level page tables
- **Operations**:
  - `paging_map_page()`: Map virtual → physical
  - `paging_unmap_page()`: Remove mapping
  - `paging_create_address_space()`: New address space
  - `paging_switch_address_space()`: Load CR3

### 3. Thread Management

- **TCB Structure**:
  ```c
  struct thread_control_block {
      uint64_t tid;
      thread_state_t state;       // READY/RUNNING/BLOCKED/DEAD
      uint64_t rsp, rip, rflags;  // Saved context
      uint64_t cr3;               // Address space
      uint64_t registers[15];     // GPRs
      void *stack_base;
      uint64_t ipc_partner;
  };
  ```

- **Scheduler**: Round-robin (ready queue)
- **Context switch**: Assembly routine saves/restores all registers

### 4. Task (Address Space)

- **Task**: Container for threads sharing an address space
- **Operations**:
  - `task_create()`: Allocate new address space (PML4)
  - `task_destroy()`: Tear down task

### 5. Synchronous IPC

- **Mechanism**: Rendezvous-based
- **Message**: Tag + 4 x 64-bit words
- **Operations**:
  - `ipc_send(dest, msg)`: Send and block
  - `ipc_receive(src, msg)`: Receive and block
  - `ipc_call(dest, send, recv)`: Send + receive

#### IPC State Machine

```
Sender Thread              Receiver Thread
─────────────              ───────────────
RUNNING                    RUNNING
   │                          │
   │ ipc_send(R)              │ ipc_receive(*)
   ▼                          ▼
BLOCKED ───────────────────▶ BLOCKED
   │     (rendezvous)         │
   │                          │
   └──────── READY ◀──────────┘
          (message delivered)
```

### 6. Interrupt Handling

- **IDT**: 256 entries
- **Exceptions**:
  - 0-19: CPU exceptions (divide, page fault, GP fault, etc.)
  - 32+: Hardware IRQs (timer for preemption)
- **Handler**: ISR stubs → common handler → C exception handler

## Boot Sequence

1. **GRUB** loads kernel via Multiboot2
2. **boot.S** (32-bit):
   - Check CPUID and long-mode support
   - Set up temporary page tables (identity + higher-half)
   - Enable PAE, load PML4
   - Set EFER.LME, enable paging
   - Jump to 64-bit code
3. **Long mode** (64-bit):
   - Set up GDT
   - Load higher-half stack
   - Call `kmain()`
4. **kmain()**:
   - Initialize serial + VGA
   - Initialize PMM
   - Initialize paging
   - Install IDT
   - Enable interrupts
   - Initialize tasks, threads, IPC
   - (Future: Load root server ELF)
   - Idle loop

## Context Switch Flow

```
Timer IRQ
   │
   ▼
ISR stub (isr.S)
   │
   ├─ Save interrupt frame
   │
   ▼
timer_handler() (idt.c)
   │
   ▼
scheduler_get_next() (thread.c)
   │
   ├─ Select next ready thread
   │
   ▼
context_switch(old, new) (context.S)
   │
   ├─ Save old TCB (rsp, rip, GPRs)
   ├─ Load new CR3
   ├─ Restore new TCB (rsp, rip, GPRs)
   │
   ▼
iretq → new thread
```

## Syscall Interface

### Syscall Numbers

- `SYS_IPC` (1): IPC operations
- `SYS_THREAD_CREATE` (2): Create thread
- `SYS_THREAD_DESTROY` (3): Destroy thread
- `SYS_DEBUG_PRINT` (99): Debug output (temporary)

### Invocation

```asm
mov rax, syscall_number
mov rdi, arg1
...
int 0x80
```

## Future Enhancements

### Short-term

- Full ELF loader
- Actual user-mode transition (ring 3)
- Syscall handler (int 0x80)
- Timer-based preemption

### Medium-term

- Capability-based access control
- IPC fast-path optimization (assembly)
- User-space pager service
- Memory statistics

### Long-term

- SMP support (per-CPU scheduler)
- IOMMU integration
- User-space device drivers
- Formal verification

## References

- **L4Ka**: Original L4 microkernel
- **seL4**: Formally verified microkernel
- **OSDev Wiki**: x86-64 architecture details

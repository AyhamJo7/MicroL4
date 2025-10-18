# MicroL4 — A Minimalist L4-Style Microkernel

[![CI](https://github.com/AyhamJo7/MicroL4/actions/workflows/ci.yml/badge.svg)](https://github.com/AyhamJo7/MicroL4/actions/workflows/ci.yml)
[![License](https://img.shields.io/badge/License-Apache%202.0-blue.svg)](LICENSE)

**MicroL4** is a minimal, L4-style microkernel for x86-64, demonstrating core microkernel principles: address spaces, threads with preemptive scheduling, and fast synchronous IPC.

## Features

- **Minimal Kernel Surface**: Only essential mechanisms (threads, address spaces, IPC)
- **Fast Synchronous IPC**: Direct rendezvous-based message passing
- **Preemptive Scheduling**: Round-robin scheduler with timer-driven context switches
- **x86-64 Long Mode**: Full 64-bit operation with 4-level paging
- **Multiboot2 Boot**: Standard GRUB bootloader support
- **User-Space Root Server**: Demonstrates kernel-to-user transition and IPC

## Architecture Overview

```
┌─────────────────────────────────────────────┐
│           User Space (Ring 3)               │
│  ┌──────────────┐       ┌──────────────┐   │
│  │ Root Server  │ ←IPC→ │ Future       │   │
│  │              │       │ Services     │   │
│  └──────────────┘       └──────────────┘   │
├─────────────────────────────────────────────┤
│         MicroL4 Kernel (Ring 0)             │
│  ┌──────────┐ ┌──────────┐ ┌──────────┐   │
│  │  Thread  │ │  Address │ │   IPC    │   │
│  │  Mgmt    │ │  Spaces  │ │  Engine  │   │
│  └──────────┘ └──────────┘ └──────────┘   │
│  ┌──────────────────────────────────────┐  │
│  │    Memory Mgmt (PMM + Paging)        │  │
│  └──────────────────────────────────────┘  │
│  ┌──────────────────────────────────────┐  │
│  │    HW Abstraction (IDT, Exceptions)  │  │
│  └──────────────────────────────────────┘  │
└─────────────────────────────────────────────┘
```

## Quick Start

### Prerequisites

- x86_64 cross-compiler toolchain (`x86_64-elf-gcc`, `x86_64-elf-ld`)
- GRUB utilities (`grub-mkrescue`, `xorriso`)
- QEMU (`qemu-system-x86_64`)
- GNU Make

**Or** use the provided Docker development environment:

```bash
docker build -t microl4-dev -f toolchain/Dockerfile .
docker run -it -v $(pwd):/workspace microl4-dev
```

**Or** use VS Code Dev Containers (`.devcontainer/`).

### Build

```bash
# Check toolchain
make toolchain-check

# Build kernel
make build

# Create bootable ISO
make iso
```

### Run

```bash
# Run in QEMU
make run

# Debug with GDB
make debug
# In another terminal: gdb -x .gdbinit
```

Expected output:
```
[MicroL4] Booting...
[MicroL4] PMM initialized: 512 MB
[MicroL4] Paging enabled (higher-half kernel)
[MicroL4] IDT configured
[MicroL4] Scheduler started
[MicroL4] Root server loaded
Hello from user space!
```

## Repository Structure

```
MicroL4/
├── kernel/                 # Kernel source code
│   ├── arch/x86_64/       # Architecture-specific code
│   ├── include/           # Kernel headers
│   └── mem/               # Memory management
├── user/root_server/      # Initial user-space process
├── boot/                  # Boot configuration (GRUB)
├── toolchain/             # Development environment
├── docs/                  # Architecture & design docs
├── .github/               # CI/CD and governance
└── Makefile               # Build system
```

## Development

See [CONTRIBUTING.md](CONTRIBUTING.md) for contribution guidelines.

### Code Standards

- C11 for kernel code
- GAS syntax for assembly
- Format with `clang-format` (see `.clang-format`)
- Run `make format` before committing

### Testing

```bash
# Run all checks
make check

# Lint code
make lint
```

## Documentation

- [Architecture](docs/architecture.md) - System design and principles
- [ADRs](docs/adr/) - Architectural decision records
- [Threat Model](docs/threat-model.md) - Security considerations

## Non-Goals

MicroL4 intentionally excludes:
- Device drivers (belong in user space)
- File systems (user-space services)
- Network stack (user-space services)
- Multi-architecture support (x86-64 only)

## License

Licensed under the Apache License, Version 2.0. See [LICENSE](LICENSE).

## Acknowledgments

Inspired by the L4 microkernel family and seL4 principles.

## Maintainers

- [@AyhamJo7](https://github.com/AyhamJo7)

---

**Project Status**: Active Development

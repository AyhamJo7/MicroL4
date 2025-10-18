# Build Instructions for MicroL4

## Prerequisites

### Required Tools

- **Cross-compiler**: `x86_64-elf-gcc` and `x86_64-elf-binutils`
- **GRUB**: `grub-mkrescue`, `xorriso`
- **QEMU**: `qemu-system-x86_64`
- **Build tools**: `make`, `nasm`
- **Optional**: `gdb` for debugging, `clang-format` for code formatting

### Installation

#### Ubuntu/Debian

```bash
sudo apt-get update
sudo apt-get install -y \
    build-essential \
    nasm \
    xorriso \
    grub-pc-bin \
    grub-common \
    qemu-system-x86 \
    gdb \
    clang-format
```

#### Building Cross-Compiler

If your distribution doesn't have `x86_64-elf-gcc`:

```bash
# See toolchain/Dockerfile for full build script
# Or use Docker:
docker build -t microl4-toolchain -f toolchain/Dockerfile .
docker run -it -v $(pwd):/workspace microl4-toolchain
```

## Build Steps

### 1. Verify Toolchain

```bash
make toolchain-check
```

Expected output:
```
Checking toolchain...
Toolchain OK!
x86_64-elf-gcc (GCC) 12.2.0
grub-mkrescue (GRUB) 2.06
QEMU emulator version 6.2.0
```

### 2. Build Kernel

```bash
make build
```

This compiles all C and assembly files and links them into `build/kernel.elf`.

### 3. Create Bootable ISO

```bash
make iso
```

Generates `microl4.iso` using GRUB.

### 4. Run in QEMU

```bash
make run
```

Expected output:
```
[MicroL4] Booting...
[MicroL4] Multiboot magic: 0x36d76289
[MicroL4] Long mode enabled
[MicroL4] Kernel loaded at higher half
[PMM] Initialized: 32768 frames (128 MB)
[VMM] Paging initialized, PML4 at 0xffff...
[IDT] Exception handlers installed
[TASK] Task system initialized
[SCHED] Thread system initialized
[IPC] IPC system initialized
[MicroL4] All subsystems initialized
[MicroL4] Free frames: 32760 / 32768

===========================================
 Hello from user space!
===========================================

[MicroL4] Root server simulation complete
[MicroL4] System idle. Halting...
```

### 5. Debug with GDB

Terminal 1:
```bash
make debug
```

Terminal 2:
```bash
gdb -x .gdbinit
```

GDB will connect to QEMU and break at `boot_entry` and `kmain`.

## Build Targets

| Target | Description |
|--------|-------------|
| `make all` | Same as `make iso` (default) |
| `make build` | Compile kernel ELF |
| `make iso` | Create bootable ISO |
| `make run` | Run in QEMU with serial output |
| `make debug` | Run QEMU with GDB server (-s -S) |
| `make clean` | Remove all build artifacts |
| `make format` | Format C/H files with clang-format |
| `make lint` | Run static analysis checks |
| `make toolchain-check` | Verify toolchain installation |

## Directory Structure After Build

```
MicroL4/
├── build/                      # Build artifacts
│   ├── kernel.elf             # Kernel binary
│   └── kernel/                # Object files
├── isodir/                    # ISO staging
│   └── boot/
│       ├── kernel.elf
│       └── grub/grub.cfg
└── microl4.iso                # Bootable ISO image
```

## Troubleshooting

### "x86_64-elf-gcc: not found"

Install cross-compiler or use Docker environment:
```bash
docker build -t microl4-dev -f toolchain/Dockerfile .
docker run -it -v $(pwd):/workspace microl4-dev bash
```

### "grub-mkrescue: command not found"

Install GRUB tools:
```bash
sudo apt-get install grub-pc-bin grub-common xorriso
```

### QEMU hangs or triple-faults

Check serial output for error messages. Common issues:
- Stack misalignment (16-byte boundary required)
- Invalid page table entries
- Exception without handler

Enable interrupt debugging:
```bash
qemu-system-x86_64 -cdrom microl4.iso -serial stdio -d int,cpu_reset
```

### Build warnings about missing files

Ensure you're in the project root:
```bash
cd /path/to/MicroL4
make clean && make iso
```

## CI/CD

GitHub Actions automatically builds and tests on every push:
- Builds cross-compiler (cached)
- Compiles kernel
- Creates ISO
- Runs QEMU smoke test
- Checks formatting
- Uploads ISO artifact

See `.github/workflows/ci.yml` for details.

## Next Steps

After successful build:

1. **Modify kernel**: Edit files in `kernel/`
2. **Rebuild**: `make clean && make iso`
3. **Test**: `make run`
4. **Format code**: `make format`
5. **Commit**: `git add . && git commit -m "..."`

## References

- [Makefile](Makefile) - Build system
- [CONTRIBUTING.md](CONTRIBUTING.md) - Development guidelines
- [docs/architecture.md](docs/architecture.md) - System design

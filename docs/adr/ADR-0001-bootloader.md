# ADR-0001: Use GRUB with Multiboot2 for Bootloader

## Status

Accepted

## Context

MicroL4 needs a bootloader to:
1. Load the kernel into memory
2. Transition from BIOS/UEFI to protected mode
3. Provide memory map and hardware information
4. Hand off control to kernel entry point

Options considered:
- **Custom bootloader**: Full control, but significant development effort
- **GRUB + Multiboot**: Industry standard, well-tested
- **UEFI native**: Modern, but adds complexity
- **Limine**: Minimal, but less mature

## Decision

Use **GRUB with Multiboot2 specification**.

## Rationale

### Advantages

1. **Maturity**: GRUB is battle-tested across thousands of systems
2. **Multiboot2 standard**: Well-documented interface between bootloader and kernel
3. **Tooling**: `grub-mkrescue` makes ISO creation trivial
4. **QEMU support**: First-class support in emulators
5. **Minimal kernel complexity**: Kernel doesn't need to handle boot protocols

### Trade-offs

1. **Dependency**: Relies on external GRUB package
2. **Size**: GRUB is relatively large (~1MB)
3. **Boot time**: Slightly slower than minimal bootloaders

### Multiboot2 Benefits

- Provides memory map (E820)
- Framebuffer information (for future graphics)
- Bootloader name and version
- x86-64 long mode support

## Implementation

### Multiboot2 Header

```asm
.section .multiboot
multiboot2_header_start:
    .long 0xE85250D6            # Magic
    .long 0                     # Architecture (i386)
    .long header_end - header_start
    .long -(0xE85250D6 + 0 + (header_end - header_start))
    # Tags...
    .short 0, 0
    .long 8
multiboot2_header_end:
```

### GRUB Configuration

```
set timeout=0
menuentry "MicroL4" {
    multiboot2 /boot/kernel.elf
    boot
}
```

## Consequences

### Positive

- Fast development (no custom bootloader needed)
- Easy ISO generation for testing
- Standard interface reduces bugs

### Negative

- Hard dependency on GRUB in build environment
- Slightly larger boot image

## Alternatives Considered

### Custom Bootloader

- **Pros**: Full control, minimal size
- **Cons**: High development cost, more bugs, reinventing wheel
- **Verdict**: Not worth effort for educational kernel

### Limine

- **Pros**: Modern, minimal
- **Cons**: Less mature, smaller community
- **Verdict**: GRUB more stable

## References

- [Multiboot2 Specification](https://www.gnu.org/software/grub/manual/multiboot2/multiboot.html)
- [OSDev: Multiboot](https://wiki.osdev.org/Multiboot)
- [GRUB Manual](https://www.gnu.org/software/grub/manual/)

## Metadata

- **Author**: @AyhamJo7
- **Date**: 2025-10-18
- **Supersedes**: None
- **Superseded by**: None

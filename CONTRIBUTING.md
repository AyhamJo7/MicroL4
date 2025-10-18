# Contributing to MicroL4

Thank you for your interest in contributing to MicroL4!

## Getting Started

1. Fork the repository
2. Clone your fork: `git clone https://github.com/YOUR_USERNAME/MicroL4.git`
3. Create a feature branch: `git checkout -b feature/your-feature-name`
4. Set up the development environment (see below)

## Development Environment

### Using Docker (Recommended)

```bash
docker build -t microl4-dev -f toolchain/Dockerfile .
docker run -it -v $(pwd):/workspace microl4-dev
```

### Using VS Code Dev Containers

Open the project in VS Code and select "Reopen in Container" when prompted.

### Native Setup

Install the required tools:
- `x86_64-elf-gcc` and `x86_64-elf-binutils`
- QEMU (`qemu-system-x86_64`)
- GRUB tools (`grub-mkrescue`, `xorriso`)
- `clang-format`, `make`, `gdb`

Verify your toolchain:
```bash
make toolchain-check
```

## Code Standards

### C Code

- Follow C11 standard
- Use kernel coding style (see `.clang-format`)
- Format before committing: `make format`
- No standard library (freestanding environment)
- Explicit integer sizes (`uint64_t`, not `unsigned long`)

### Assembly

- Use GAS syntax (AT&T)
- Add comments for non-obvious operations
- Align directives for readability

### Commit Messages

Follow Conventional Commits:

```
type(scope): brief description

Longer explanation if needed.

Fixes #123
```

Types: `feat`, `fix`, `docs`, `style`, `refactor`, `test`, `chore`

Examples:
```
feat(pmm): add bitmap-based physical memory allocator
fix(idt): correct page fault handler stack alignment
docs(architecture): add IPC state machine diagram
```

## Pull Request Process

1. Ensure your code builds: `make clean && make iso`
2. Run formatting: `make format`
3. Test in QEMU: `make run`
4. Update `CHANGELOG.md` if applicable
5. Submit PR with clear description
6. Link related issues

### PR Checklist

- [ ] Code follows style guidelines
- [ ] Builds without warnings
- [ ] Runs in QEMU without crashes
- [ ] Documentation updated
- [ ] Commit messages are clear
- [ ] No merge conflicts

## Areas for Contribution

### High Priority

- IPC fast-path optimization
- Capability-based security model
- SMP support
- User-space pager service

### Medium Priority

- Additional exception handlers
- Improved scheduler (priority-based)
- Memory statistics and debugging
- Unit tests for kernel components

### Documentation

- Architecture diagrams
- API documentation
- Tutorial for adding syscalls
- Performance analysis

## Testing

### Manual Testing

```bash
# Build and run
make iso && make run

# Debug with GDB
make debug
# In another terminal:
gdb -x .gdbinit
```

### Expected Boot Sequence

1. GRUB loads kernel
2. Long-mode transition
3. PMM initialization
4. Paging setup
5. IDT configuration
6. Scheduler start
7. Root server launch
8. "Hello from user space!" message

## Questions?

- Open an issue with the `question` label
- Check existing issues and documentation first

## Code Review

All submissions require review. We aim to respond within 3-5 business days.

## License

By contributing, you agree that your contributions will be licensed under the Apache License 2.0.

---

Thank you for contributing to MicroL4!

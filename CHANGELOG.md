# Changelog

All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/),
and this project adheres to [Semantic Versioning](https://semver.org/spec/v2.0.0.html).

## [Unreleased]

### Added
- Initial repository structure
- Multiboot2 bootloader support with GRUB
- x86-64 long mode bootstrap
- Physical memory manager (bitmap allocator)
- Virtual memory manager (4-level paging)
- IDT and exception handling
- Thread management with TCB
- Preemptive round-robin scheduler
- Task/address space abstraction
- Synchronous IPC mechanism
- Root server (initial user-space process)
- ELF loader for user programs
- Serial console output (COM1)
- VGA text mode output
- Build system with Make
- Docker-based development environment
- VS Code dev container configuration
- CI/CD pipeline with GitHub Actions
- Documentation (architecture, ADRs, threat model)
- Enterprise governance files

### Changed
- N/A

### Deprecated
- N/A

### Removed
- N/A

### Fixed
- N/A

### Security
- N/A

## [0.1.0] - TBD

Initial release.

[Unreleased]: https://github.com/AyhamJo7/MicroL4/compare/v0.1.0...HEAD
[0.1.0]: https://github.com/AyhamJo7/MicroL4/releases/tag/v0.1.0

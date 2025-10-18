# MicroL4 Threat Model

## Overview

This document outlines the security assumptions, threat vectors, and mitigation strategies for MicroL4.

**Note**: MicroL4 is currently a research/educational microkernel. This threat model documents current security posture and planned improvements.

## Trust Boundaries

```
┌─────────────────────────────────────────┐
│         Untrusted User Space            │  ← Potentially malicious
│  ┌──────────┐   ┌──────────┐           │
│  │  Server  │   │  Server  │           │
│  └──────────┘   └──────────┘           │
├─────────────────────────────────────────┤
│         Trusted Kernel Space            │  ← Must remain secure
│  ┌──────────────────────────────────┐   │
│  │   MicroL4 Kernel (TCB)           │   │
│  └──────────────────────────────────┘   │
└─────────────────────────────────────────┘
│         Hardware                        │  ← Assumed trusted
└─────────────────────────────────────────┘
```

### Trusted Computing Base (TCB)

- Kernel code (~5000 SLOC)
- Bootloader (GRUB)
- x86-64 hardware

### Untrusted

- All user-space code
- Future device drivers
- Root server (after initialization)

## Threat Actors

| Actor | Capability | Goal |
|-------|------------|------|
| Malicious user process | Ring 3 code execution | Escalate to ring 0, DoS, data theft |
| Compromised driver | User-space server | Kernel exploit, system DoS |
| Physical attacker | Hardware access | Cold boot, DMA attacks |

## Attack Vectors & Mitigations

### 1. Memory Corruption

**Threat**: User process overwrites kernel memory via buffer overflow, use-after-free, etc.

**Current Mitigations**:
- ✅ **Paging isolation**: Kernel mapped at higher half, user space at low addresses
- ✅ **Privilege enforcement**: Ring 0 (kernel) vs Ring 3 (user)
- ✅ **No-execute (NX)**: Data pages marked non-executable (future)

**Limitations**:
- ❌ No ASLR/KASLR
- ❌ No stack canaries
- ❌ Limited input validation on syscalls

**Roadmap**:
- [ ] Add KASLR for kernel
- [ ] Add stack canaries (`-fstack-protector-strong`)
- [ ] Validate all user pointers before dereferencing

### 2. Privilege Escalation

**Threat**: User process gains kernel privileges via syscall vulnerabilities.

**Current Mitigations**:
- ✅ **CPU privilege rings**: User code runs at ring 3
- ✅ **Syscall validation**: Check syscall numbers (basic)

**Limitations**:
- ❌ No capability-based access control
- ❌ Minimal syscall argument validation
- ❌ No secure syscall interface (using `int 0x80`)

**Roadmap**:
- [ ] Implement capability system
- [ ] Use `syscall/sysret` instructions (faster, more secure)
- [ ] Add per-syscall permission checks

### 3. Information Disclosure

**Threat**: Kernel leaks sensitive data to user space.

**Current Mitigations**:
- ✅ **Address space isolation**: Separate page tables per task

**Limitations**:
- ❌ No scrubbing of returned buffers
- ❌ Kernel pointers visible in error messages
- ❌ No protection against timing attacks

**Roadmap**:
- [ ] Zero buffers before returning to user space
- [ ] Remove kernel addresses from user-visible output
- [ ] Implement constant-time crypto primitives

### 4. Denial of Service (DoS)

**Threat**: Malicious process exhausts resources (CPU, memory).

**Current Mitigations**:
- ✅ **Preemptive scheduling**: Timer-based context switching

**Limitations**:
- ❌ No CPU time quotas
- ❌ No memory limits per task
- ❌ No IPC rate limiting

**Roadmap**:
- [ ] Add per-task resource accounting
- [ ] Implement priority inheritance for IPC
- [ ] Add memory allocation limits

### 5. IPC Attacks

**Threat**: Malicious IPC messages exploit parser bugs, overflow buffers, or cause deadlocks.

**Current Mitigations**:
- ✅ **Synchronous IPC**: Rendezvous prevents unbounded buffering

**Limitations**:
- ❌ No message validation
- ❌ No endpoint capability checks
- ❌ Possible deadlock scenarios

**Roadmap**:
- [ ] Validate message tag and size
- [ ] Implement capability-based IPC endpoints
- [ ] Add timeout mechanism

### 6. Hardware Attacks

**Threat**: Physical access enables DMA, cold boot, or firmware attacks.

**Current Mitigations**:
- ❌ None (not in scope for software-only kernel)

**Roadmap**:
- [ ] IOMMU integration (prevent rogue DMA)
- [ ] Secure boot chain
- [ ] Memory encryption (SME/SEV on AMD)

## Security Assumptions

### What We Assume

1. **Hardware is trustworthy**: CPU, memory controller behave correctly
2. **Bootloader is trusted**: GRUB has not been compromised
3. **No physical attacks**: Attacker has no hardware access
4. **Single-core**: No race conditions from SMP (yet)

### What We Do NOT Assume

1. **User-space code is benign**: All user code is untrusted
2. **Input is valid**: Syscall arguments may be malicious
3. **Drivers are safe**: Future user-space drivers are untrusted

## Current Security Posture

| Category | Status | Notes |
|----------|--------|-------|
| Memory isolation | ✅ Strong | Paging enforced |
| Privilege separation | ✅ Strong | Ring 0/3 enforced |
| Input validation | ⚠️ Weak | Minimal checks |
| Resource limits | ❌ None | No quotas |
| Cryptography | ❌ None | Not implemented |
| Audit logging | ❌ None | Not implemented |

## Vulnerability Reporting

See [SECURITY.md](../SECURITY.md) for reporting procedures.

## References

- [seL4 Security Whitepaper](https://sel4.systems/)
- [Intel SDM: Privilege Levels](https://software.intel.com/content/www/us/en/develop/articles/intel-sdm.html)
- [OWASP Kernel Security Guide](https://owasp.org/)

/*
 * MicroL4 Boot Code
 * Multiboot2 header and long-mode transition
 */

.section .multiboot
.align 8

multiboot2_header_start:
    .long 0xE85250D6                    /* Magic number */
    .long 0                             /* Architecture: i386 */
    .long multiboot2_header_end - multiboot2_header_start
    .long -(0xE85250D6 + 0 + (multiboot2_header_end - multiboot2_header_start))

    /* End tag */
    .align 8
    .short 0                            /* Type */
    .short 0                            /* Flags */
    .long 8                             /* Size */
multiboot2_header_end:

.section .boot_text, "ax"
.code32

.global boot_entry
boot_entry:
    /* Clear interrupts */
    cli

    /* Save multiboot info pointer */
    movl %ebx, %edi

    /* Set up temporary stack */
    movl $boot_stack_top, %esp

    /* Check for long mode support */
    call check_cpuid
    call check_long_mode

    /* Set up paging for long mode */
    call setup_page_tables

    /* Enable PAE */
    movl %cr4, %eax
    orl $0x20, %eax                     /* Set PAE bit */
    movl %eax, %cr4

    /* Load PML4 */
    movl $pml4, %eax
    movl %eax, %cr3

    /* Enable long mode (set EFER.LME) */
    movl $0xC0000080, %ecx              /* EFER MSR */
    rdmsr
    orl $0x100, %eax                    /* Set LME bit */
    wrmsr

    /* Enable paging (set CR0.PG) */
    movl %cr0, %eax
    orl $0x80000000, %eax               /* Set PG bit */
    movl %eax, %cr0

    /* Load 64-bit GDT */
    lgdt gdt64_pointer

    /* Jump to 64-bit code */
    ljmp $0x08, $long_mode_start

check_cpuid:
    pushfl
    popl %eax
    movl %eax, %ecx
    xorl $0x00200000, %eax
    pushl %eax
    popfl
    pushfl
    popl %eax
    pushl %ecx
    popfl
    cmpl %ecx, %eax
    je no_cpuid
    ret

no_cpuid:
    movl $'C', 0xB8000
    hlt

check_long_mode:
    movl $0x80000000, %eax
    cpuid
    cmpl $0x80000001, %eax
    jb no_long_mode

    movl $0x80000001, %eax
    cpuid
    testl $(1 << 29), %edx
    jz no_long_mode
    ret

no_long_mode:
    movl $'L', 0xB8000
    hlt

setup_page_tables:
    /* Zero out page tables */
    movl $pml4, %edi
    movl $0, %eax
    movl $4096, %ecx
    rep stosl

    /* PML4[0] -> PDPT (identity map first 2MB) */
    movl $pml4, %edi
    movl $pdpt, %eax
    orl $0x3, %eax                      /* Present + writable */
    movl %eax, (%edi)

    /* PML4[511] -> PDPT (higher half) */
    movl $pml4, %edi
    addl $(511 * 8), %edi
    movl $pdpt, %eax
    orl $0x3, %eax
    movl %eax, (%edi)

    /* PDPT[0] -> PD */
    movl $pdpt, %edi
    movl $pd, %eax
    orl $0x3, %eax
    movl %eax, (%edi)

    /* PD[0] -> 2MB page (identity map) */
    movl $pd, %edi
    movl $0x83, %eax                    /* Present + writable + huge */
    movl %eax, (%edi)

    ret

.code64
long_mode_start:
    /* Set up segment registers */
    movw $0x10, %ax
    movw %ax, %ds
    movw %ax, %es
    movw %ax, %fs
    movw %ax, %gs
    movw %ax, %ss

    /* Set up kernel stack (higher half) */
    movq $kernel_stack_top, %rsp

    /* Pass multiboot info to kmain */
    movl %edi, %edi                     /* Zero-extend to 64-bit */

    /* Jump to C kernel */
    call kmain

    /* If kmain returns, halt */
halt_loop:
    hlt
    jmp halt_loop

.section .data
.align 16
gdt64:
    .quad 0                             /* Null descriptor */
    .quad 0x00AF9A000000FFFF            /* Code segment */
    .quad 0x00AF92000000FFFF            /* Data segment */
gdt64_pointer:
    .word gdt64_pointer - gdt64 - 1
    .quad gdt64

.section .bss
.align 4096
pml4:
    .space 4096
pdpt:
    .space 4096
pd:
    .space 4096

.align 16
boot_stack:
    .space 16384
boot_stack_top:

.align 16
kernel_stack:
    .space 32768
kernel_stack_top:

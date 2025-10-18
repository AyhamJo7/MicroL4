/* MicroL4 Linker Script */
/* Higher-half kernel at -2GB (0xFFFFFFFF80000000) */

ENTRY(boot_entry)

SECTIONS
{
    /* Kernel is loaded at 1MB physical, but linked at higher half */
    . = 1M;

    /* Boot section (identity mapped + higher half) */
    .boot ALIGN(4K) : AT(ADDR(.boot))
    {
        *(.multiboot)
        *(.boot_text)
    }

    /* Move to higher half: -2GB virtual address */
    . += 0xFFFFFFFF80000000;

    /* Text section */
    .text ALIGN(4K) : AT(ADDR(.text) - 0xFFFFFFFF80000000)
    {
        *(.text)
        *(.text.*)
    }

    /* Read-only data */
    .rodata ALIGN(4K) : AT(ADDR(.rodata) - 0xFFFFFFFF80000000)
    {
        *(.rodata)
        *(.rodata.*)
    }

    /* Initialized data */
    .data ALIGN(4K) : AT(ADDR(.data) - 0xFFFFFFFF80000000)
    {
        *(.data)
        *(.data.*)
    }

    /* BSS (uninitialized data) */
    .bss ALIGN(4K) : AT(ADDR(.bss) - 0xFFFFFFFF80000000)
    {
        *(COMMON)
        *(.bss)
        *(.bss.*)
    }

    /* Kernel end marker */
    . = ALIGN(4K);
    _kernel_end = .;

    /* Discard unnecessary sections */
    /DISCARD/ :
    {
        *(.comment)
        *(.eh_frame)
        *(.note.*)
    }
}

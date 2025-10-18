# GDB initialization for MicroL4 debugging
target remote :1234
symbol-file build/kernel.elf

# Break at kernel entry
break boot_entry
break kmain

# Intel syntax
set disassembly-flavor intel

# Display settings
set print pretty on
set pagination off

# Layout
layout split
focus cmd

continue

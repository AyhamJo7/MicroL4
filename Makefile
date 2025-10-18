# MicroL4 Makefile

# Toolchain
CC := x86_64-elf-gcc
LD := x86_64-elf-ld
AS := x86_64-elf-as
OBJCOPY := x86_64-elf-objcopy

# Directories
KERNEL_DIR := kernel
ARCH_DIR := $(KERNEL_DIR)/arch/x86_64
MEM_DIR := $(KERNEL_DIR)/mem
USER_DIR := user/root_server
BUILD_DIR := build
ISO_DIR := isodir

# Flags
CFLAGS := -ffreestanding -O2 -Wall -Wextra -std=c11 -mno-red-zone \
          -mcmodel=kernel -mno-mmx -mno-sse -mno-sse2 \
          -I$(KERNEL_DIR)/include -fno-stack-protector -nostdlib
LDFLAGS := -T linker.ld -nostdlib -z max-page-size=0x1000
ASFLAGS := --64

# Source files
ASM_SOURCES := $(ARCH_DIR)/boot.S $(ARCH_DIR)/context.S
C_SOURCES := $(KERNEL_DIR)/kmain.c \
             $(KERNEL_DIR)/thread.c \
             $(KERNEL_DIR)/task.c \
             $(KERNEL_DIR)/ipc.c \
             $(ARCH_DIR)/idt.c \
             $(ARCH_DIR)/paging.c \
             $(ARCH_DIR)/serial.c \
             $(ARCH_DIR)/vga.c \
             $(MEM_DIR)/pmm.c

USER_C_SOURCES := $(USER_DIR)/main.c

# Object files
ASM_OBJECTS := $(patsubst %.S,$(BUILD_DIR)/%.o,$(ASM_SOURCES))
C_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(C_SOURCES))
USER_OBJECTS := $(patsubst %.c,$(BUILD_DIR)/%.o,$(USER_C_SOURCES))

ALL_OBJECTS := $(ASM_OBJECTS) $(C_OBJECTS)

# Targets
KERNEL_ELF := $(BUILD_DIR)/kernel.elf
USER_ELF := $(BUILD_DIR)/root_server.elf
ISO_FILE := microl4.iso

.PHONY: all build iso run debug clean format lint toolchain-check

all: iso

# Build kernel ELF
build: $(KERNEL_ELF)

$(KERNEL_ELF): $(ALL_OBJECTS) linker.ld
	@echo "Linking kernel..."
	@mkdir -p $(BUILD_DIR)
	$(LD) $(LDFLAGS) -o $@ $(ALL_OBJECTS)
	@echo "Kernel built: $@"

# Compile assembly files
$(BUILD_DIR)/%.o: %.S
	@echo "Assembling $<..."
	@mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) $< -o $@

# Compile C files
$(BUILD_DIR)/%.o: %.c
	@echo "Compiling $<..."
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) -c $< -o $@

# Build user-space root server
$(USER_ELF): $(USER_OBJECTS)
	@echo "Linking root server..."
	@mkdir -p $(BUILD_DIR)
	$(LD) -T user/root_server/user.ld -o $@ $(USER_OBJECTS)
	@echo "Root server built: $@"

# Create bootable ISO
iso: $(KERNEL_ELF)
	@echo "Creating ISO..."
	@mkdir -p $(ISO_DIR)/boot/grub
	@cp $(KERNEL_ELF) $(ISO_DIR)/boot/kernel.elf
	@cp boot/grub.cfg $(ISO_DIR)/boot/grub/grub.cfg
	grub-mkrescue -o $(ISO_FILE) $(ISO_DIR) 2>/dev/null || \
		grub-mkrescue -o $(ISO_FILE) $(ISO_DIR)
	@echo "ISO created: $(ISO_FILE)"

# Run in QEMU
run: iso
	qemu-system-x86_64 -cdrom $(ISO_FILE) -serial stdio -no-reboot -d int,cpu_reset

# Debug with GDB
debug: iso
	qemu-system-x86_64 -cdrom $(ISO_FILE) -serial stdio -no-reboot -s -S

# Format code
format:
	@echo "Formatting C/H files..."
	@find kernel user -name "*.c" -o -name "*.h" | xargs clang-format -i
	@echo "Formatting complete."

# Lint (basic checks)
lint:
	@echo "Running basic lint checks..."
	@find kernel user -name "*.c" -o -name "*.h" | xargs clang-format --dry-run -Werror || true

# Toolchain check
toolchain-check:
	@echo "Checking toolchain..."
	@which $(CC) > /dev/null || (echo "ERROR: $(CC) not found" && exit 1)
	@which $(LD) > /dev/null || (echo "ERROR: $(LD) not found" && exit 1)
	@which $(AS) > /dev/null || (echo "ERROR: $(AS) not found" && exit 1)
	@which grub-mkrescue > /dev/null || (echo "ERROR: grub-mkrescue not found" && exit 1)
	@which qemu-system-x86_64 > /dev/null || (echo "ERROR: qemu-system-x86_64 not found" && exit 1)
	@echo "Toolchain OK!"
	@$(CC) --version | head -n1
	@grub-mkrescue --version | head -n1
	@qemu-system-x86_64 --version | head -n1

# Clean build artifacts
clean:
	@echo "Cleaning build artifacts..."
	@rm -rf $(BUILD_DIR) $(ISO_DIR) $(ISO_FILE)
	@echo "Clean complete."

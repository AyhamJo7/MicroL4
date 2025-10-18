#!/bin/bash
set -e

# Run QEMU in headless mode and capture output
timeout 10s qemu-system-x86_64 \
    -cdrom microl4.iso \
    -serial file:boot.log \
    -display none \
    -no-reboot || true

# Check for expected boot messages
echo "=== Boot Log ==="
cat boot.log
echo "================"

grep -q "MicroL4" boot.log || (echo "ERROR: Boot message not found" && exit 1)
grep -q "Booting" boot.log || (echo "ERROR: Boot sequence not found" && exit 1)
grep -q "initialized" boot.log || (echo "ERROR: Initialization not complete" && exit 1)
grep -q "Hello from user space" boot.log || (echo "ERROR: User space message not found" && exit 1)

echo "✓ All checks passed"

#!/bin/bash
# Format all C and header files using clang-format

echo "Formatting C and header files..."

# Find and format all .c and .h files
find kernel user -type f \( -name "*.c" -o -name "*.h" \) -print0 | \
    xargs -0 clang-format -i --style=file

echo "Formatting complete!"

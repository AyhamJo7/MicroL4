# MicroL4 Development Environment
FROM ubuntu:22.04

ENV DEBIAN_FRONTEND=noninteractive

# Install build dependencies
RUN apt-get update && apt-get install -y \
    build-essential \
    gcc \
    g++ \
    nasm \
    xorriso \
    grub-pc-bin \
    grub-common \
    qemu-system-x86 \
    gdb \
    git \
    wget \
    bison \
    flex \
    libgmp3-dev \
    libmpc-dev \
    libmpfr-dev \
    texinfo \
    clang-format \
    python3 \
    python3-pip \
    && rm -rf /var/lib/apt/lists/*

# Set up cross-compiler (x86_64-elf)
ENV PREFIX="/usr/local/cross"
ENV TARGET=x86_64-elf
ENV PATH="$PREFIX/bin:$PATH"

WORKDIR /tmp/toolchain

# Download and build binutils
RUN wget https://ftp.gnu.org/gnu/binutils/binutils-2.39.tar.gz && \
    tar -xzf binutils-2.39.tar.gz && \
    mkdir build-binutils && cd build-binutils && \
    ../binutils-2.39/configure --target=$TARGET --prefix="$PREFIX" \
        --with-sysroot --disable-nls --disable-werror && \
    make -j$(nproc) && make install && \
    cd /tmp/toolchain && rm -rf build-binutils binutils-2.39*

# Download and build GCC
RUN wget https://ftp.gnu.org/gnu/gcc/gcc-12.2.0/gcc-12.2.0.tar.gz && \
    tar -xzf gcc-12.2.0.tar.gz && \
    mkdir build-gcc && cd build-gcc && \
    ../gcc-12.2.0/configure --target=$TARGET --prefix="$PREFIX" \
        --disable-nls --enable-languages=c,c++ --without-headers && \
    make -j$(nproc) all-gcc && make -j$(nproc) all-target-libgcc && \
    make install-gcc && make install-target-libgcc && \
    cd /tmp/toolchain && rm -rf build-gcc gcc-12.2.0*

# Clean up
RUN rm -rf /tmp/toolchain

# Set working directory
WORKDIR /workspace

# Verify installation
RUN x86_64-elf-gcc --version && x86_64-elf-ld --version

CMD ["/bin/bash"]

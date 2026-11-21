#!/bin/bash

BINUTILS_VERSION="${BINUTILS_VERSION:-2.47}"
GCC_VERSION="${GCC_VERSION:-16.2.0}"

BINUTILS_URL="https://ftp.gnu.org/gnu/binutils/binutils-${BINUTILS_VERSION}.tar.xz"
GCC_URL="https://ftp.gnu.org/gnu/gcc/gcc-${GCC_VERSION}/gcc-${GCC_VERSION}.tar.xz"

TARGET=i686-elf

# ---------------------------

set -e

# Downloads a file, unless it's already there (e.g. after a build that failed halfway).
# Downloads to a .part file first, so an interrupted download isn't mistaken for a complete one.
download() {
    local file
    file=$(basename "$1")
    if [ -f "$file" ]; then
        echo "Already downloaded: $file"
        return
    fi
    wget -O "$file.part" "$1"
    mv "$file.part" "$file"
}

TOOLCHAINS_DIR=
OPERATION='build'

while test $# -gt 0
do
    case "$1" in
        -c) OPERATION='clean'
            ;;
        *)  TOOLCHAINS_DIR=$(realpath "$1")
            ;;
    esac
    shift
done

if [ -z "$TOOLCHAINS_DIR" ]; then
    echo "Missing arg: toolchains directory"
    exit 1
fi

mkdir -p "$TOOLCHAINS_DIR"
pushd "$TOOLCHAINS_DIR"
TOOLCHAIN_PREFIX="$TOOLCHAINS_DIR/$TARGET"

if [ "$OPERATION" = "build" ]; then

    # Download and build binutils
    BINUTILS_SRC="binutils-${BINUTILS_VERSION}"
    BINUTILS_BUILD="binutils-build-${BINUTILS_VERSION}"

    download ${BINUTILS_URL}
    tar -xf binutils-${BINUTILS_VERSION}.tar.xz

    mkdir -p ${BINUTILS_BUILD}
    pushd ${BINUTILS_BUILD}
    ../binutils-${BINUTILS_VERSION}/configure \
        --prefix="${TOOLCHAIN_PREFIX}"	\
        --target=${TARGET}				\
        --with-sysroot					\
        --disable-nls					\
        --disable-werror
    make -j"$(nproc)"
    make install
    popd

    # Download and build GCC
    GCC_SRC="gcc-${GCC_VERSION}"
    GCC_BUILD="gcc-build-${GCC_VERSION}"

    download ${GCC_URL}
    tar -xf gcc-${GCC_VERSION}.tar.xz
    mkdir -p ${GCC_BUILD}
    pushd ${GCC_BUILD}
    ../gcc-${GCC_VERSION}/configure     \
        --prefix="${TOOLCHAIN_PREFIX}" 	\
        --target=${TARGET}				\
        --disable-nls					\
        --enable-languages=c,c++		\
        --without-headers
    make -j"$(nproc)" all-gcc all-target-libgcc
    make install-gcc install-target-libgcc
    popd

elif [ "$OPERATION" = "clean" ]; then
	rm -rf *
fi
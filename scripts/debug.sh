#!/bin/bash

QEMU_ARGS="-S -gdb stdio -m 32"

if [ "$#" -le 1 ]; then
    echo "Usage: ./debug.sh <image_type> <image>"
    exit 1
fi

case "$1" in
    "floppy")   QEMU_ARGS="${QEMU_ARGS} -fda $2"
    ;;
    "disk")     QEMU_ARGS="${QEMU_ARGS} -hda $2"
    ;;
    *)          echo "Unknown image type $1."
                exit 2
esac


cat > .gdb_script.gdb << EOF
    b *0x7c00
    layout asm
    target remote | qemu-system-i386 $QEMU_ARGS
EOF

gdb -x .gdb_script.gdb
rm -f .gdb_script.gdb
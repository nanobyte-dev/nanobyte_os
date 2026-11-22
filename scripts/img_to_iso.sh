#!/usr/bin/env bash
# Makes a bootable CD image (ISO) out of a floppy image, using El Torito floppy emulation: the BIOS
# boots the CD as if it were a floppy, so the bootloader works unchanged.

set -e

if [ "$#" -ne 2 ]; then
    echo "Usage: img_to_iso.sh <floppy image> <output iso>"
    exit 1
fi

IMAGE="$1"
ISO="$2"

# floppy emulation only works with a standard floppy size
if [ "$(wc -c < "$IMAGE")" -ne 1474560 ]; then
    echo "$IMAGE is not a 1.44 MB floppy image. Build one with: scons imageType=floppy"
    exit 1
fi

if ! command -v xorriso > /dev/null; then
    echo "xorriso not found, install it with your package manager"
    exit 1
fi

# the floppy image has to be a file inside the ISO
STAGING_DIR=$(mktemp -d)
trap 'rm -rf "$STAGING_DIR"' EXIT
cp "$IMAGE" "$STAGING_DIR/floppy.img"

xorriso -as mkisofs -quiet -o "$ISO" -b floppy.img "$STAGING_DIR"
echo "Created $ISO"

#!/bin/bash

set -e

TARGET=$1
SIZE=$2

STAGE1_STAGE2_LOCATION_OFFSET=480

DISK_SECTOR_COUNT=$(( (${SIZE} + 511 ) / 512 ))

DISK_PART1_BEGIN=2048
DISK_PART1_END=$(( ${DISK_SECTOR_COUNT} - 1 ))

# generate image file
echo "Generating disk image ${TARGET} (${DISK_SECTOR_COUNT} sectors)..."
dd if=/dev/zero of=$TARGET bs=512 count=${DISK_SECTOR_COUNT} >/dev/null

# create partition table
echo "Creating partition..."
parted -s $TARGET mklabel msdos
parted -s $TARGET mkpart primary ${DISK_PART1_BEGIN}s ${DISK_PART1_END}s
parted -s $TARGET set 1 boot on

STAGE2_SIZE=$(stat -c%s ${BUILD_DIR}/stage2.bin)
echo ${STAGE2_SIZE}
STAGE2_SECTORS=$(( ( ${STAGE2_SIZE} + 511 ) / 512 ))
echo ${STAGE2_SECTORS}

if [ ${STAGE2_SECTORS} \> $(( ${DISK_PART1_BEGIN} - 1 )) ]; then
    echo "Stage2 too big!!!"
    exit 2
fi

dd if=${BUILD_DIR}/stage2.bin of=$TARGET conv=notrunc bs=512 seek=1 #>/dev/null

# create loopback device
DEVICE=$(losetup -fP --show ${TARGET})
echo "Created loopback device ${DEVICE}"
TARGET_PARTITION="${DEVICE}p1"

# create file system
echo "Formatting ${TARGET_PARTITION}..."
mkfs.fat -n "NBOS" $TARGET_PARTITION >/dev/null

# install bootloader
echo "Installing bootloader on ${TARGET_PARTITION}..."
dd if=${BUILD_DIR}/stage1.bin of=$TARGET_PARTITION conv=notrunc bs=1 count=3 2>&1 >/dev/null
dd if=${BUILD_DIR}/stage1.bin of=$TARGET_PARTITION conv=notrunc bs=1 seek=90 skip=90 2>&1 >/dev/null

# write lba address of stage2 to bootloader
echo "01 00 00 00" | xxd -r -p | dd of=$TARGET_PARTITION conv=notrunc bs=1 seek=$STAGE1_STAGE2_LOCATION_OFFSET
printf "%x" ${STAGE2_SECTORS} | xxd -r -p | dd of=$TARGET_PARTITION conv=notrunc bs=1 seek=$(( $STAGE1_STAGE2_LOCATION_OFFSET + 4 ))

# copy files
echo "Copying files to ${TARGET_PARTITION} (mounted on /tmp/nbos)..."
mkdir -p /tmp/nbos
mount ${TARGET_PARTITION} /tmp/nbos
cp ${BUILD_DIR}/kernel.bin /tmp/nbos
cp test.txt /tmp/nbos
mkdir /tmp/nbos/test
cp test.txt /tmp/nbos/test
umount /tmp/nbos

# destroy loopback device
losetup -d ${DEVICE}

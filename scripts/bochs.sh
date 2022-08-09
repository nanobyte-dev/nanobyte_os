#!/bin/bash

if [ "$#" -le 1 ]; then
    echo "Usage: ./bochs.sh <image_type> <image>"
    exit 1
fi

case "$1" in
    "floppy")   DISK_CFG="floppya: 1_44=\"$2\", status=inserted"
                BOOT_CFG="boot: floppya"
    ;;
    "disk")     DISK_CFG="ata0-master: type=disk, path=\"$2\", heads=32, spt=63"
                BOOT_CFG="boot: disk"
    ;;
    *)          echo "Unknown image type $1."
                exit 2
esac


cat > .bochs_config << EOF
megs: 128
romimage: file=/home/tibi/Downloads/bochs-2.7.pre1/bios/BIOS-bochs-latest
vgaromimage: file=/home/tibi/Downloads/bochs-2.7.pre1/bios/VGABIOS-elpin-2.40
mouse: enabled=0
display_library: x, options="gui_debug"

$DISK_CFG
boot: $BOOT_CFG
EOF

bochs -f .bochs_config
rm -f .bochs_config
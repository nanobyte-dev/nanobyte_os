ENTRY(start)
OUTPUT_FORMAT("binary")

/* The BIOS loads us at 0x7C00, but the first thing we do is copy ourselves to 0x600, to make room
   for the partition's boot sector. Everything after the copy runs from there. */
phys = 0x600;

SECTIONS
{
    . = phys;

    .text               : { *(.text)            }
    .data               : { *(.data)            }
    .rodata             : { *(.rodata)          }

    /* The disk signature starts at offset 440, followed by the partition table at 446. The
       image script only writes our code into the first 440 bytes and leaves those alone. */
    ASSERT(. <= phys + 440, "MBR code doesn't fit in 440 bytes")
}

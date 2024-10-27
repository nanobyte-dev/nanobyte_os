#pragma once

typedef struct {
    // 0x00	1	Drive attributes (bit 7 set = active or bootable)
    uint8_t Attributes;

    // 0x01	3	CHS Address of partition start
    uint8_t ChsStart[3];

    // 0x04	1	Partition type
    uint8_t PartitionType;

    // 0x05	3	CHS address of last partition sector
    uint8_t ChsEnd[3];

    // 0x08	4	LBA of partition start
    uint32_t LbaStart;

    // 0x0C	4	Number of sectors in partition
    uint32_t Size;

} __attribute__((packed)) MBREntry;

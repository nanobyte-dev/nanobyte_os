#pragma once

#include <hal/disk/PartitioningScheme.hpp>

class MBRPartitioningScheme : public hal::disk::PartitioningScheme
{
public:
    bool Probe(hal::fs::File* device, ErrorChain& err) override;
    void GetPartitions(hal::fs::File* device, etl::ivector<hal::disk::Partition>& outPartitions, ErrorChain& err) override;

private:
    struct MBREntry
    {
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

    } __attribute__((packed));

    
    enum MBRAttributes
    {
        None = 0,
        Active = 1 << 7
    };

    
    enum PartitionType
    {
        Absent = 0,
        ExtendedCHS = 0x05,
        ExtendedLBA = 0x0B,
        GPT = 0xEE,
    };

    static void GetCHS(uint8_t chs[3], uint32_t* cylinder, uint32_t* head, uint32_t* sector);
    static bool IsPresent(MBREntry& entry);
};
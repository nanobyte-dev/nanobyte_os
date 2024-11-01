#include "MBRPartitioningScheme.hpp"
#include <core/Memory.hpp>

void MBRPartitioningScheme::GetCHS(uint8_t chs[3], uint32_t *cylinder, uint32_t *head, uint32_t *sector)
{
    // 1 byte  Starting head
    *head = chs[0];

    // 6 bits  Starting sector (Bits 6-7 are the upper two bits for the Starting Cylinder field.)
    *sector = chs[1] & 0x3F;

    // 10 bits Starting Cylinder 
    *cylinder = chs[2] | (chs[1] & 0xC0) << 2;
}

bool MBRPartitioningScheme::Probe(hal::fs::File* device, ErrorChain& err)
{
    if (err.Failed()) return false;

    uint8_t buffer[512];
    MBREntry* partitionTable = reinterpret_cast<MBREntry*>(buffer + 446);
    uint8_t* signature = buffer + 510;
    ResultCode rc;

    // Read first sector
    if (device == nullptr)
    {
        err.Fail(ResultCode::ArgumentNull, "Device is null");
        return false;
    }

    device->ReadAbs(0, buffer, sizeof(buffer), err);
    err.FailOnError(ResultCode::IOFailed, "Failed to read MBR");
    if (err.Failed()) return false;
 
    // Check AA55 signature
    if (*signature != 0x55 || *(signature + 1) != 0xAA) {
        return false;
    }

    int activeCount = 0;

    // Check if partition table is valid
    for (int i = 0; i < 4; i++)
    {
        // valid values are 0x80 = active, 0x00 = inactive
        if (partitionTable[i].Attributes != MBRAttributes::None && partitionTable[i].Attributes != MBRAttributes::Active)
            return false;

        if (partitionTable[i].Attributes == MBRAttributes::Active)
            activeCount++;

        // Type is GPT protective - means it's GPT
        if (partitionTable[i].PartitionType == PartitionType::GPT)
            return false;

        if (IsPresent(partitionTable[i]))
        {
            uint32_t startCyl, startHead, startSector;
            uint32_t endCyl, endHead, endSector;
            GetCHS(partitionTable[i].ChsStart, &startCyl, &startHead, &startSector);
            GetCHS(partitionTable[i].ChsEnd, &endCyl, &endHead, &endSector);

            // we don't know the drive geometry, but even if we assume the max, startLBA should be <= endLBA
            // sector is 6 bits => max is 64
            // head is 8 bits => max is 256
            uint32_t startLba = (startCyl * 256 + startHead) * 64 + (startSector - 1);
            uint32_t endLba = (endCyl * 256 + endHead) * 64 + (endSector - 1);
            if (startLba > endLba)
                return false;

            // sector shouldn't be 0
            if (startSector == 0 || endSector == 0)
                return false;

            // todo: if necessary, add more checks
        }
    }

    // Only a single partition should be active
    if (activeCount > 1)
        return false;

    return true;
}

void MBRPartitioningScheme::GetPartitions(hal::fs::File *device, etl::ivector<hal::disk::Partition> &outPartitions, ErrorChain &err)
{
    if (err.Failed()) return;

    uint8_t buffer[512];
    MBREntry primaryPartitionTable[4];
    MBREntry* partitionTable = reinterpret_cast<MBREntry*>(buffer + 446);

    // Read first sector
    if (device == nullptr)
    {
        err.Fail(ResultCode::ArgumentNull, "Device is null");
        return;
    }

    device->ReadAbs(0, buffer, sizeof(buffer), err);
    err.FailOnError(ResultCode::IOFailed, "Failed to read MBR");
    if (err.Failed()) return;

    Memory::Copy(primaryPartitionTable, partitionTable, sizeof(MBREntry) * 4);

    for (int pi = 0; pi < 4; pi++)
    {
        if (!IsPresent(primaryPartitionTable[pi]))
            continue;
        
        hal::disk::Partition part;
        part.LbaStart = primaryPartitionTable[pi].LbaStart;
        part.Size = primaryPartitionTable[pi].Size;
        part.PartitionType = primaryPartitionTable[pi].PartitionType;
        part.Attributes = hal::disk::PartitionAttributes::None;
        part.Parent = nullptr;

        if (primaryPartitionTable[pi].Attributes &= MBRAttributes::Active)
            part.Attributes |= hal::disk::PartitionAttributes::Bootable;
        if (primaryPartitionTable[pi].PartitionType == PartitionType::ExtendedCHS || primaryPartitionTable[pi].PartitionType == PartitionType::ExtendedLBA)
            part.Attributes |= hal::disk::PartitionAttributes::Extended;

        size_t primaryIndex = outPartitions.size();
        if (outPartitions.size() == outPartitions.max_size())
        {
            err.Fail(ResultCode::ContainerFull, "Partition vector full");
            return;
        }
        outPartitions.push_back(part);

        // Add extended partitions
        if (part.Attributes & hal::disk::PartitionAttributes::Extended)
        {
            // Read extended partition table
            device->ReadAbs(part.LbaStart, buffer, sizeof(buffer), err);
            err.FailOnError(ResultCode::IOFailed, "Failed to read extended MBR");
            if (err.Failed()) return;

            for (int ei = 0; ei < 4; ei++)
            {
                if (!IsPresent(partitionTable[ei]))
                    continue;
                
                hal::disk::Partition extendedPart;
                extendedPart.LbaStart = partitionTable[ei].LbaStart;
                extendedPart.Size = partitionTable[ei].Size;
                extendedPart.PartitionType = partitionTable[ei].PartitionType;
                extendedPart.Attributes = hal::disk::PartitionAttributes::None;
                extendedPart.Parent = &outPartitions[primaryIndex];

                if (partitionTable[ei].Attributes &= MBRAttributes::Active)
                    extendedPart.Attributes |= hal::disk::PartitionAttributes::Bootable;

                if (outPartitions.size() == outPartitions.max_size())
                {
                    err.Fail(ResultCode::ContainerFull, "Partition vector full");
                    return;
                }
                outPartitions.push_back(extendedPart);
            }
        }
    }
}

bool MBRPartitioningScheme::IsPresent(MBREntry& entry)
{
    if (entry.PartitionType == PartitionType::Absent)
        return false;

    if (entry.Size == 0)
        return false;

    // Check CHS
    uint32_t startCyl, startHead, startSector;
    GetCHS(entry.ChsStart, &startCyl, &startHead, &startSector);
    if (startCyl == 0 && startHead == 0 && startSector == 0)
        return false;

    uint32_t endCyl, endHead, endSector;
    GetCHS(entry.ChsEnd, &endCyl, &endHead, &endSector);
    if (endCyl == 0 && endHead == 0 && endSector == 0)
        return false;

    return true;
}
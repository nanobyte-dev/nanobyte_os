#pragma once
#include <stdint.h>

namespace hal::disk
{

enum PartitionAttributes
{
    None,
    Bootable = 1 << 0,
    Extended = 1 << 1,
};

struct Partition
{
    uint32_t LbaStart;
    uint32_t Size;
    uint8_t PartitionType;
    int Attributes;
    Partition* Parent;
};

}
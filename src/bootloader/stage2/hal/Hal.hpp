#pragma once
#include <stdint.h>
#include <hal/DeviceTree.hpp>

namespace hal::Hal
{
    void Initialize(uint16_t bootDrive, uint32_t bootPartition);
    DeviceTree& GetDeviceTree();
}
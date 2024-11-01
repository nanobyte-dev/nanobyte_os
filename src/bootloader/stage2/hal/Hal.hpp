#pragma once
#include <stdint.h>
#include <hal/DeviceTree.hpp>
#include <hal/VirtualFileSystem.hpp>

namespace hal::Hal
{
    void Initialize(uint16_t bootDrive, uint32_t bootPartition);
    DeviceTree& GetDeviceTree();
    VirtualFileSystem& GetVFS();
}
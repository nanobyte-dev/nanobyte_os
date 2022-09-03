#include <core/Defs.hpp>
#include <core/arch/i686/VGATextDevice.hpp>
#include <core/arch/i686/E9Device.hpp>
#include <core/dev/TextDevice.hpp>
#include <core/dev/RangeBlockDevice.hpp>
#include <core/Debug.hpp>
#include <arch/i686/BIOSDisk.hpp>
#include <arch/i686/RealMemory.hpp>
#include <dev/MBR.hpp>
#include <stdint.h>

arch::i686::VGATextDevice g_VGADevice;
arch::i686::E9Device g_DebugDevice;

EXPORT void ASMCALL Start(uint16_t bootDrive, uint32_t partition)
{
    g_VGADevice.Clear();
    
    TextDevice screen(&g_VGADevice);
    Debug::AddOutputDevice(&screen, Debug::Level::Info, false);
    TextDevice debug(&g_DebugDevice);
    Debug::AddOutputDevice(&debug, Debug::Level::Debug, true);

    BIOSDisk disk(bootDrive);
    if (!disk.Initialize())
    {
        Debug::Critical("stage2", "Failed to initialize disk!");
    }

    // Handle partitioned disks
    BlockDevice* part;
    RangeBlockDevice partRange;
    if (bootDrive < 0x80) 
    {
        part = &disk;
    }
    else
    {
        MBREntry* entry = ToLinear<MBREntry*>(partition);
        partRange.Initialize(&disk, entry->LbaStart, entry->Size);
        part = &partRange;
    }

    Debug::Info("stage2", "OK!");

    for (;;);
}

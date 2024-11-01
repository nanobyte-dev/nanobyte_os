#include <core/Defs.hpp>
#include <core/Debug.hpp>
#include <arch/i686/RealMemory.hpp>
#include <mem/Stage2Allocator.hpp>
#include <cpp/NewDelete.hpp>
#include <cpp/EtlErrorHandler.hpp>
//#include <core/fs/FATFileSystem.hpp>
#include <stdint.h>
#include <memdefs.h>

#include <hal/Hal.hpp>

Stage2Allocator g_Allocator(reinterpret_cast<void*>(MEMORY_MIN), MEMORY_MAX - MEMORY_MIN);

EXPORT void ASMCALL Start(uint16_t bootDrive, uint32_t bootPartition)
{
    SetCppAllocator(&g_Allocator);
    InstallEtlErrorHandler();
    hal::Hal::Initialize(bootDrive, bootPartition);

    // Handle partitioned disks
    // File* part;
    // RangeBlockDevice partRange;
    // if (bootDrive < 0x80) 
    // {
    //     part = &disk;
    // }
    // else
    // {
    //     MBREntry* entry = ToLinear<MBREntry*>(partition);
    //     partRange.Initialize(&disk, entry->LbaStart, entry->Size);
    //     part = &partRange;
    // }


    // Read partition
    // FATFileSystem fs;
    // if (!fs.Initialize(part))
    // {
    //     Debug::Critical("stage2", "Failed to initialize FAT file system!");
    // }

    // File* kernel = fs.Open("kernel.bin", FileOpenMode::Read);

    Debug::Info("stage2", "OK!");

    for (;;);
}

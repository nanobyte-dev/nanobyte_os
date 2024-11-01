#include "Hal.hpp"
#include <arch/i686/dev/VGATextModeDevice.hpp>
#include <arch/i686/dev/E9Device.hpp>
#include <arch/i686/dev/BIOSDisk.hpp>
#include <hal/fs/TextStream.hpp>
#include <hal/DeviceTree.hpp>
#include <hal/VirtualFileSystem.hpp>
#include <util/ResultCode.hpp>
#include <Debug.hpp>

#define MODULE "HAL"

arch::i686::VGATextModeDevice g_VGATextModeDevice;
arch::i686::E9Device g_DebugDevice;
arch::i686::BIOSDisk g_BootDisk;

hal::DeviceTree g_DeviceTree;
hal::VirtualFileSystem g_VirtualFileSystem;

hal::fs::TextStream g_VGATextStream;
hal::fs::TextStream g_DebugTextStream;

namespace hal::Hal
{
    void InitializeBootDisk(uint16_t bootDrive, uint32_t bootPartition, ErrorChain& err)
    {
        g_BootDisk.Initialize(bootDrive, err);
        err.FailOnError(ResultCode::IOFailed, "Failed to initialize boot disk");
        if (err.Failed()) return;

        g_DeviceTree.AttachDevice(&g_BootDisk, "Disk", err);
    }

    void InitializeDeviceTree(ErrorChain& err)
    {
        g_VirtualFileSystem.Mount("Devices", &g_DeviceTree, err);
        auto screen = g_DeviceTree.AttachDevice(&g_VGATextModeDevice, "Screen", err);
        if (!screen)
            return;
        
        auto debug = g_DeviceTree.AttachDevice(&g_DebugDevice, "Debug", err);
        if (!debug)
            return;
        auto debugFile = debug->Open(fs::FileOpenMode::Write, err);
        g_DebugTextStream.Initialize(debugFile, fs::TextStreamCapabilities::VirtualTerminal, err);
        Debug::AddOutputDevice(&g_DebugTextStream, Debug::Level::Debug);

        auto screenFile = screen->Open(fs::FileOpenMode::Write, err);
        g_VGATextStream.Initialize(screenFile, fs::TextStreamCapabilities::None, err);
        Debug::AddOutputDevice(&g_VGATextStream, Debug::Level::Info);
    }

    void Initialize(uint16_t bootDrive, uint32_t bootPartition)
    {
        ErrorChain err;
        
        g_VGATextModeDevice.Clear();
        InitializeDeviceTree(err);

        if (err.Failed())
        {
            Debug::Critical(MODULE, "Failed to initialize device tree");
            Debug::Critical(MODULE, " %s", err.Message());
            return;
        }
        
        InitializeBootDisk(bootDrive, bootPartition, err);
                if (err.Failed())
        {
            Debug::Critical(MODULE, "Failed to initialize boot disk!");
            Debug::Critical(MODULE, " %s", err.Message());
            return;
        }
    }

    DeviceTree& GetDeviceTree()
    {
        return g_DeviceTree;
    }

    VirtualFileSystem &GetVFS()
    {
        return g_VirtualFileSystem;
    }
}
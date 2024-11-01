#include "Debug.hpp"
#include <etl/vector.h>
#include <core/util/Enum.hpp>

#define MAX_OUTPUT_DEVICES 10

namespace
{
    struct OutputDevice
    {
        hal::fs::TextStream* device;
        Debug::Level logLevel;
        bool colored;
    };

    etl::vector<OutputDevice, MAX_OUTPUT_DEVICES> g_OutputDevices;

    static const char* const g_LogSeverityColors[] =
    {
        [static_cast<int>(Debug::Level::Debug)]        = "\033[2;37m",
        [static_cast<int>(Debug::Level::Info)]         = "\033[37m",
        [static_cast<int>(Debug::Level::Warn)]         = "\033[1;33m",
        [static_cast<int>(Debug::Level::Error)]        = "\033[1;31m",
        [static_cast<int>(Debug::Level::Critical)]     = "\033[1;37;41m",
    };
    
    static const char* const g_ColorReset = "\033[0m";
}

namespace Debug 
{

ResultCode AddOutputDevice(hal::fs::TextStream* device, Level minLogLevel)
{
    // todo
    if (g_OutputDevices.size() == g_OutputDevices.max_size())
        return ResultCode::ContainerFull;

    OutputDevice dev;
    dev.device = device;
    dev.logLevel = minLogLevel;
    dev.colored = Enum::HasFlag(device->Capabilities(), hal::fs::TextStreamCapabilities::VirtualTerminal);
    g_OutputDevices.push_back(dev);
    return ResultCode::Ok;
}

void Log(const char* module, Level logLevel, const char* fmt, va_list args)
{
    for (OutputDevice& dev : g_OutputDevices)
    {
        ErrorChain err;
        
        if (logLevel < dev.logLevel)
            continue;

        // set color depending on level
        if (dev.colored)
            dev.device->Write(g_LogSeverityColors[static_cast<int>(logLevel)], err);

        // write module
        dev.device->Format(err, "[%s] ", module);

        // print log
        dev.device->VFormat(fmt, args, err);

        // reset color
        if (dev.colored)
            dev.device->Write(g_ColorReset, err);

        dev.device->WriteLine(err);
    }
}

}
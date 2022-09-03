#include "Debug.hpp"

#define MAX_OUTPUT_DEVICES 10

namespace
{
    static const char* const g_LogSeverityColors[] =
    {
        [static_cast<int>(Debug::Level::Debug)]        = "\033[2;37m",
        [static_cast<int>(Debug::Level::Info)]         = "\033[37m",
        [static_cast<int>(Debug::Level::Warn)]         = "\033[1;33m",
        [static_cast<int>(Debug::Level::Error)]        = "\033[1;31m",
        [static_cast<int>(Debug::Level::Critical)]     = "\033[1;37;41m",
    };
    
    static const char* const g_ColorReset = "\033[0m";

    struct
    {
        TextDevice* device;
        Debug::Level logLevel;
        bool colored;
    } g_OutputDevices[MAX_OUTPUT_DEVICES];
    
    int g_OutputDevicesCount;
}

namespace Debug 
{

void AddOutputDevice(TextDevice* device, Level minLogLevel, bool colorOutput)
{
    g_OutputDevices[g_OutputDevicesCount].device = device;
    g_OutputDevices[g_OutputDevicesCount].logLevel = minLogLevel;
    g_OutputDevices[g_OutputDevicesCount].colored = colorOutput;
    g_OutputDevicesCount++;
}

static void Log(const char* module, Level logLevel, const char* fmt, va_list args)
{
    for (int i = 0; i < g_OutputDevicesCount; i++)
    {
        if (logLevel < g_OutputDevices[i].logLevel)
            continue;

        // set color depending on level
        if (g_OutputDevices[i].colored)
            g_OutputDevices[i].device->Write(g_LogSeverityColors[static_cast<int>(logLevel)]);

        // write module
        g_OutputDevices[i].device->Format("[%s] ", module);

        // print log
        g_OutputDevices[i].device->VFormat(fmt, args);

        // reset color
        if (g_OutputDevices[i].colored)
            g_OutputDevices[i].device->Write(g_ColorReset);

        g_OutputDevices[i].device->Write('\n');
    }
}

void Debug(const char* module, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Log(module, Level::Debug, fmt, args);
    va_end(args);
}

void Info(const char* module, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Log(module, Level::Info, fmt, args);
    va_end(args);
}

void Warn(const char* module, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Log(module, Level::Warn, fmt, args);
    va_end(args);
}

void Error(const char* module, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Log(module, Level::Error, fmt, args);
    va_end(args);
}

void Critical(const char* module, const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);
    Log(module, Level::Critical, fmt, args);
    va_end(args);
}

}
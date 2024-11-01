#include "Debug.hpp"
#include <stdarg.h>

#define MAX_OUTPUT_DEVICES 10

// namespace
// {
//     struct
//     {
//         TextStream* device;
//         Debug::Level logLevel;
//         bool colored;
//     } g_OutputDevices[MAX_OUTPUT_DEVICES];
    
//     int g_OutputDevicesCount;
// }

namespace Debug 
{

// void AddOutputDevice(TextStream* device, Level minLogLevel, bool colorOutput)
// {
//     g_OutputDevices[g_OutputDevicesCount].device = device;
//     g_OutputDevices[g_OutputDevicesCount].logLevel = minLogLevel;
//     g_OutputDevices[g_OutputDevicesCount].colored = colorOutput;
//     g_OutputDevicesCount++;
// }

void Log(const char* module, Level logLevel, const char* fmt, va_list args)
{
    // todo
    // for (int i = 0; i < g_OutputDevicesCount; i++)
    // {
    //     if (logLevel < g_OutputDevices[i].logLevel)
    //         continue;

    //     // set color depending on level
    //     if (g_OutputDevices[i].colored)
    //         g_OutputDevices[i].device->Write(g_LogSeverityColors[static_cast<int>(logLevel)]);

    //     // write module
    //     g_OutputDevices[i].device->Format("[%s] ", module);

    //     // print log
    //     g_OutputDevices[i].device->VFormat(fmt, args);

    //     // reset color
    //     if (g_OutputDevices[i].colored)
    //         g_OutputDevices[i].device->Write(g_ColorReset);

    //     g_OutputDevices[i].device->Write('\n');
    // }
}


}
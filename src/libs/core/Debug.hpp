#pragma once
#include <core/dev/TextDevice.hpp>

namespace Debug
{
    enum class Level
    {
        Debug = 0,
        Info = 1,
        Warn = 2,
        Error = 3,
        Critical = 4
    };

    void AddOutputDevice(TextDevice* device, Level minLogLevel, bool colorOutput);

    void Debug(const char* module, const char* fmt, ...);
    void Info(const char* module, const char* fmt, ...);
    void Warn(const char* module, const char* fmt, ...);
    void Error(const char* module, const char* fmt, ...);
    void Critical(const char* module, const char* fmt, ...);
}
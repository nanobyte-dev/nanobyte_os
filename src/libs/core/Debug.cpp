
#include "Debug.hpp"
#include <stdarg.h>

namespace Debug 
{

extern void Log(const char* module, Level logLevel, const char* fmt, va_list args);

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
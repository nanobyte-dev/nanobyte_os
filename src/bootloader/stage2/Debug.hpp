#pragma once
#include <core/Debug.hpp>
#include <hal/fs/TextStream.hpp>

namespace Debug
{
    ResultCode AddOutputDevice(hal::fs::TextStream* device, Level minLogLevel);
}
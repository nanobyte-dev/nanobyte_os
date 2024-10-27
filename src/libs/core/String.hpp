#pragma once
#include <core/Defs.hpp>
#include <string.h>

namespace String
{
    constexpr auto Find = strchr;
    constexpr auto Copy = strcpy;
    constexpr auto Length = strlen;
    constexpr auto Compare = strcmp;
}
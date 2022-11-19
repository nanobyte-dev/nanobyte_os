#pragma once
#include <core/Defs.hpp>

EXPORT const char* strchr(const char* str, char chr);
EXPORT char* strcpy(char* dst, const char* src);
EXPORT unsigned strlen(const char* str);
EXPORT int strcmp(const char* a, const char* b);

namespace String
{
    constexpr auto Find = strchr;
    constexpr auto Copy = strcpy;
    constexpr auto Length = strlen;
    constexpr auto Compare = strcmp;
}
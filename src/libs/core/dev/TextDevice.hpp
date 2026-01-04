#pragma once
#include <stdarg.h>
#include "CharacterDevice.hpp"
#include <cpp/TypeTraits.hpp>

class TextDevice 
{
public:
    TextDevice(CharacterDevice* dev);
    bool Write(char c);
    bool Write(const char* str);
    bool VFormat(const char* fmt, va_list args);
    bool Format(const char* fmt, ...);
    bool FormatBuffer(const char* msg, const void* buffer, size_t count);

    template<typename TNumber>
    bool Write(TNumber number, int radix);

private:
    CharacterDevice* m_dev;

    static const char g_HexChars[];
};

template<typename TNumber>
bool TextDevice::Write(TNumber number, int radix)
{
    typename MakeUnsigned<TNumber>::Type unsNumber;
    bool ok = true;

    if (IsSigned<TNumber>() && number < 0)
    {
        ok = ok && Write('-');
        unsNumber = -number;
    }
    else
    {
        unsNumber = number;
    }

    char buffer[32];
    int pos = 0;

    // convert number to ASCII
    do 
    {
        typename MakeUnsigned<TNumber>::Type rem = number % radix;
        number /= radix;
        buffer[pos++] = g_HexChars[rem];
    } while (number > 0);

    // print number in reverse order
    while (--pos >= 0)
        ok = ok && Write(buffer[pos]);

    return ok;
}
#pragma once
#include <stdarg.h>
#include "File.hpp"
#include <core/cpp/TypeTraits.hpp>

namespace hal::fs {

namespace TextStreamCapabilities_
{
    enum TextStreamCapabilities
    {
        None = 0,

        /// @brief Supports VT sequences
        VirtualTerminal = 1 << 0
    };
}

using TextStreamCapabilities = TextStreamCapabilities_::TextStreamCapabilities;

class TextStream 
{
public:
    TextStream();
    void Initialize(File* dev, TextStreamCapabilities capabilities, ErrorChain& err);
    void Write(char c, ErrorChain& err);
    void Write(const char* str, ErrorChain& err);
    void WriteLine(ErrorChain& err);
    void WriteLine(const char* str, ErrorChain& err);
    void VFormat(const char* fmt, va_list args, ErrorChain& err);
    void Format(ErrorChain& err, const char* fmt, ...);
    void FormatBuffer(const char* msg, const void* buffer, size_t count, ErrorChain& err);

    template<typename TNumber>
    void Write(TNumber number, int radix, ErrorChain& err);

    TextStreamCapabilities Capabilities() const;

private:
    File* m_Dev;
    TextStreamCapabilities m_Capabilities;

    static const char g_HexChars[];
    static const char g_NewLine;
};

template<typename TNumber>
void TextStream::Write(TNumber number, int radix, ErrorChain& err)
{
    if (err.Failed()) return;

    typename MakeUnsigned<TNumber>::Type unsNumber;

    if (IsSigned<TNumber>() && number < 0)
    {
        Write('-', err);
        if (err.Failed()) return;

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
        Write(buffer[pos], err);
}

}
#include "TextStream.hpp"

namespace hal::fs {

enum class FormatState 
{
    Normal = 0,
    Length = 1,
    LengthShort = 2,
    LengthLong = 3,
    Spec = 4,
};

enum class FormatLength 
{
    Default       = 0,
    ShortShort    = 1,
    Short         = 2,
    Long          = 3,
    LongLong      = 4,
};

const char TextStream::g_HexChars[] = "0123456789abcdef";
const char TextStream::g_NewLine = '\n';

TextStream::TextStream()
    : m_Dev(nullptr)
{
}

void TextStream::Initialize(File* dev, TextStreamCapabilities capabilities, ErrorChain& err)
{
    if (err.Failed()) return;

    if (dev == nullptr)
    {
        err.Fail(ResultCode::ArgumentNull, "File is null");
        return;
    }

    m_Dev = dev;
    m_Capabilities = capabilities;
}

void TextStream::Write(char c, ErrorChain& err)
{
    if (err.Failed()) return;
    m_Dev->Write(reinterpret_cast<const uint8_t*>(&c), sizeof(c), err);
}

void TextStream::Write(const char* str, ErrorChain& err)
{
    if (err.Failed()) return;

    while(*str)
    {
        Write(*str, err);
        if (err.Failed()) return;

        str++;
    }
}

void TextStream::WriteLine(ErrorChain& err)
{
    if (err.Failed()) return;

    return Write(g_NewLine, err);
}

void TextStream::WriteLine(const char* str, ErrorChain& err)
{
    Write(str, err);
    Write(g_NewLine, err);
}

void TextStream::VFormat(const char* fmt, va_list args, ErrorChain& err)
{
    FormatState state = FormatState::Normal;
    FormatLength length = FormatLength::Default;
    int radix = 10;
    bool sign = false;
    bool number = false;

    while (*fmt && !err.Failed())
    {
        switch (state)
        {
            case FormatState::Normal:
                switch (*fmt)
                {
                    case '%':   state = FormatState::Length;
                                break;
                    default:    Write(*fmt, err);
                                break;
                }
                break;

            case FormatState::Length:
                switch (*fmt)
                {
                    case 'h':   length = FormatLength::Short;
                                state = FormatState::LengthShort;
                                break;
                    case 'l':   length = FormatLength::Long;
                                state = FormatState::LengthLong;
                                break;
                    default:    goto FormatState_Spec;
                }
                break;

            case FormatState::LengthShort:
                if (*fmt == 'h')
                {
                    length = FormatLength::ShortShort;
                    state = FormatState::Spec;
                }
                else goto FormatState_Spec;
                break;

            case FormatState::LengthLong:
                if (*fmt == 'l')
                {
                    length = FormatLength::LongLong;
                    state = FormatState::Spec;
                }
                else goto FormatState_Spec;
                break;

            case FormatState::Spec:
            FormatState_Spec:
                switch (*fmt)
                {
                    case 'c':   Write((char)va_arg(args, int), err);
                                break;

                    case 's':   
                                Write(va_arg(args, const char*), err);
                                break;

                    case '%':   Write('%', err);
                                break;

                    case 'd':
                    case 'i':   radix = 10; sign = true; number = true;
                                break;

                    case 'u':   radix = 10; sign = false; number = true;
                                break;

                    case 'X':
                    case 'x':
                    case 'p':   radix = 16; sign = false; number = true;
                                break;

                    case 'o':   radix = 8; sign = false; number = true;
                                break;

                    // ignore invalid spec
                    default:    break;
                }

                if (number)
                {
                    if (sign)
                    {
                        switch (length)
                        {
                        case FormatLength::ShortShort:
                        case FormatLength::Short:
                        case FormatLength::Default:     Write(va_arg(args, int), radix, err);
                                                        break;

                        case FormatLength::Long:        Write(va_arg(args, long), radix, err);
                                                        break;

                        case FormatLength::LongLong:    Write(va_arg(args, long long), radix, err);
                                                        break;
                        }
                    }
                    else
                    {
                        switch (length)
                        {
                        case FormatLength::ShortShort:
                        case FormatLength::Short:
                        case FormatLength::Default:     Write(va_arg(args, unsigned int), radix, err);
                                                        break;
                                                        
                        case FormatLength::Long:        Write(va_arg(args, unsigned  long), radix, err);
                                                        break;

                        case FormatLength::LongLong:    Write(va_arg(args, unsigned  long long), radix, err);
                                                        break;
                        }
                    }
                }

                // reset state
                state = FormatState::Normal;
                length = FormatLength::Default;
                radix = 10;
                sign = false;
                number = false;
                break;
        }

        fmt++;
    }
}

void TextStream::Format(ErrorChain& err, const char* fmt,  ...)
{
    if (err.Failed()) return;

    va_list args;
    va_start(args, fmt);
    VFormat(fmt, args, err);
    va_end(args);
}

void TextStream::FormatBuffer(const char* msg, const void* buffer, size_t count, ErrorChain& err)
{
    if (err.Failed()) return;

    const uint8_t* u8Buffer = (const uint8_t*)buffer;
    
    Write(msg, err);
    for (uint16_t i = 0; i < count && !err.Failed(); i++)
    {
        Write(g_HexChars[u8Buffer[i] >> 4], err);
        Write(g_HexChars[u8Buffer[i] & 0xF], err);
    }

    Write("\n", err);
}

TextStreamCapabilities TextStream::Capabilities() const
{
    return m_Capabilities;
}

}
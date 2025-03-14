#include <stdint.h>
#include <stdarg.h>
#include "x86.h"

const unsigned int SCREEN_WIDTH = 80;
const unsigned int SCREEN_HEIGHT = 25;
const uint8_t DEFAULT_COLOR = 0x7;

uint8_t* g_ScreenBuffer = (uint8_t*)0xB8000;
int g_ScreenX = 0;
int g_ScreenY = 0;

char getchr(int x, int y)
{
    return (char)g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x)];
}

void putchr(int x, int y, char c)
{
    g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x)] = c;
}

uint8_t getcolor(int x, int y)
{
    return g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1];
}

void putcolor(int x, int y, uint8_t color)
{
    g_ScreenBuffer[2 * (y * SCREEN_WIDTH + x) + 1] = color;
}

void setcursor(int x, int y)
{
    uint16_t pos = y * SCREEN_WIDTH + x;

    x86_outb(0x3D4, 0x0F);
    x86_outb(0x3D5, (uint8_t)(pos & 0xFF));
    x86_outb(0x3D4, 0x0E);
    x86_outb(0x3D5, (uint8_t)((pos >> 8) & 0xFF));
}

void clrscr()
{
    for (int y = 0; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            putchr(x, y, 0);
            putcolor(x, y, DEFAULT_COLOR);
        }

    g_ScreenX = 0;
    g_ScreenY = 0;
    setcursor(0, 0);
}

void scrollback()
{
    for (int y = 1; y < SCREEN_HEIGHT; y++)
        for (int x = 0; x < SCREEN_WIDTH; x++)
        {
            putchr(x, y - 1, getchr(x, y));
            putcolor(x, y - 1, getcolor(x, y));
        }

    for (int x = 0; x < SCREEN_WIDTH; x++)
    {
        putchr(x, SCREEN_HEIGHT - 1, 0);
        putcolor(x, SCREEN_HEIGHT - 1, DEFAULT_COLOR);
    }

    g_ScreenY -= 1;
}

void putc(char c)
{
    x86_outb(0xE9, c);
    switch (c)
    {
        case '\n':  g_ScreenX = 0; g_ScreenY++;
                    break;
    
        case '\t':  for (int i = 0; i < 4 - (g_ScreenX % 4); i++)
                        putc(' ');
                    break;

        case '\r':  g_ScreenX = 0;
                    break;

        default:    putchr(g_ScreenX, g_ScreenY, c);
                    g_ScreenX += 1;
                    break;
    }

    if (g_ScreenX >= SCREEN_WIDTH) {
        g_ScreenX -= SCREEN_WIDTH;
        g_ScreenY++;
    }

    if (g_ScreenY >= SCREEN_HEIGHT)
        scrollback();

    setcursor(g_ScreenX, g_ScreenY);
}

void puts(const char* str)
{
    while (*str)
    {
        putc(*str);
        ++str;
    }
}

unsigned long long pow(const unsigned long base, const unsigned long exp)
{
    unsigned long long r = 1;
    unsigned long count = exp;

    while (count > 0) {
        r *= base;
        count--;
    }

    return r;
}

unsigned int num_digits(const unsigned long long num, const unsigned int radix)
 {
    unsigned int count = 0;
    unsigned long long t_num = num;

    if (num == 0) { return 1; }

    while (1) {
        unsigned long long q, r;

        q = t_num / radix;
        r = t_num % radix;

        if ((q == r) && (q == 0)) {
            break;
        }

        t_num = q;
        count++;
    }

    return count;
}

unsigned long long abs(const long long num)
{
    return (num >= 0) ? num : (-1 * num);
}

#define PRINTF_STATE_NORMAL         0
#define PRINTF_STATE_WIDTH          1
#define PRINTF_STATE_LENGTH         2
#define PRINTF_STATE_LENGTH_SHORT   3
#define PRINTF_STATE_LENGTH_LONG    4
#define PRINTF_STATE_SPEC           5

#define PRINTF_LENGTH_DEFAULT       0
#define PRINTF_WIDTH_DEFAULT        0
#define PRINTF_LENGTH_SHORT_SHORT   1
#define PRINTF_LENGTH_SHORT         2
#define PRINTF_LENGTH_LONG          3
#define PRINTF_LENGTH_LONG_LONG     4


const char g_HexChars[] = "0123456789abcdef";

void printf_unsigned(unsigned long long, int);

void printf_print_width(unsigned long long number, unsigned long long width, unsigned int radix)
{
    unsigned long long t_number = number;
    unsigned long long n_digs = num_digits(number, radix);
    unsigned long long t_width;

    // if no width calculated or # has more digits than the width, just
    // return without printing the padding to the width
    if ((width <= 0) || (width <= n_digs)) { return; }

    t_width = width - n_digs;
        
    while (t_width-- > 0) {
        putc(' ');
    }
}

void printf_unsigned(unsigned long long number, int radix)
{
    unsigned long long num = number;
    char buffer[32];
    int pos = 0;

    do 
    {
        unsigned long long rem = num % radix;
        num /= radix;
        buffer[pos++] = g_HexChars[rem];
    } while (num > 0);

    // print number in reverse order
    while (--pos >= 0)
        putc(buffer[pos]);
}

void printf_signed(long long number, int radix)
{
    if (number < 0)
    {
        putc('-');
        printf_unsigned((unsigned long long)(-number), radix);
    }
    else
    {
        printf_unsigned((unsigned long long)number, radix);
    }
}

void printf(const char* fmt, ...)
{
    va_list args;
    va_start(args, fmt);

    int state = PRINTF_STATE_NORMAL;
    int length = PRINTF_LENGTH_DEFAULT;
    int width = PRINTF_WIDTH_DEFAULT;
    bool left_align_width = false;
    bool number = false;
    bool sign = false;
    int radix = 10;

    while (*fmt)
    {
        switch (state)
        {
            case PRINTF_STATE_NORMAL:
                switch (*fmt)
                {
                    case '%':   state = PRINTF_STATE_WIDTH;
                                break;
                    default:    putc(*fmt);
                                break;
                }
                break;

            case PRINTF_STATE_WIDTH:
                {
                    switch (*fmt) {                    
                        case '-':
                            left_align_width = true;
                            break;

                        case '0':
                        case '1':
                        case '2':
                        case '3':
                        case '4':
                        case '5':
                        case '6':
                        case '7':
                        case '8':
                        case '9':
                            char *wptr = fmt;
                            char w_chars[10];
                            int num_width_chars = 0;

                            while (1) {
                                if (*wptr < '0' || *wptr > '9') {
                                    int pwr = 0;

                                    // compute the width
                                    for (--num_width_chars; num_width_chars >= 0; num_width_chars--) {
                                        width += pow(10, pwr) * w_chars[num_width_chars];
                                        pwr++;
                                    }

                                    // no longer in the width calculation mode
                                    fmt = wptr;
                                    break;
                                }

                                w_chars[num_width_chars++] = (*wptr) - '0';
                                wptr++;
                            }
                            state = PRINTF_STATE_LENGTH;
                            continue;

                        default:
                            state = PRINTF_STATE_LENGTH;
                            continue;
                    }

                    break;
                }
            break;

            case PRINTF_STATE_LENGTH:
                switch (*fmt)
                {
                    case 'h':   length = PRINTF_LENGTH_SHORT;
                                state = PRINTF_STATE_LENGTH_SHORT;
                                break;
                    case 'l':   length = PRINTF_LENGTH_LONG;
                                state = PRINTF_STATE_LENGTH_LONG;
                                break;
                    default:    goto PRINTF_STATE_SPEC_;
                }
                break;

            case PRINTF_STATE_LENGTH_SHORT:
                if (*fmt == 'h')
                {
                    length = PRINTF_LENGTH_SHORT_SHORT;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_LENGTH_LONG:
                if (*fmt == 'l')
                {
                    length = PRINTF_LENGTH_LONG_LONG;
                    state = PRINTF_STATE_SPEC;
                }
                else goto PRINTF_STATE_SPEC_;
                break;

            case PRINTF_STATE_SPEC:
            PRINTF_STATE_SPEC_:
                switch (*fmt)
                {
                    case 'c':   putc((char)va_arg(args, int));
                                break;

                    case 's':   puts(va_arg(args, const char*));
                                break;

                    case '%':   putc('%');
                                break;

                    case 'd':
                    case 'i':   number = true; radix = 10; sign = true;
                                break;

                    case 'u':   number = true; radix = 10; sign = false;
                                break;

                    case 'X':
                    case 'x':
                    case 'p':   number = true; radix = 16; sign = false;
                                break;

                    case 'o':   number = true; radix = 8; sign = false;
                                break;

                    // ignore invalid spec
                    default:    break;
                }

                // handle numbers
                if (number)
                {
                    if (sign) 
                    {
                        switch (length)
                        {
                            case PRINTF_LENGTH_SHORT_SHORT:
                            case PRINTF_LENGTH_SHORT:
                            case PRINTF_LENGTH_DEFAULT:     int si = va_arg(args, int);
                                                            if (left_align_width == true) {
                                                                printf_signed(si, radix);
                                                            }
                                                            printf_print_width(abs(si), width, radix);
                                                            if (left_align_width == false) {
                                                                printf_signed(si, radix);
                                                            }
                                                            break;

                            case PRINTF_LENGTH_LONG:        long sl = va_arg(args, long);
                                                            if (left_align_width == true) {
                                                                printf_signed(sl, radix);
                                                            }
                                                            printf_print_width(abs(sl), width, radix);
                                                            if (left_align_width == false) {
                                                                printf_signed(sl, radix);
                                                            }
                                                            break;

                            case PRINTF_LENGTH_LONG_LONG:   long long sll = va_arg(args, long long);
                                                            if (left_align_width == true) {
                                                                printf_signed(sll, radix);
                                                            }
                                                            printf_print_width(abs(sll), width, radix);
                                                            if (left_align_width == false) {
                                                                printf_signed(sll, radix);
                                                            }
                                                            break;
                        }
                    }
                    else
                    {
                        switch (length)
                        {
                            case PRINTF_LENGTH_SHORT_SHORT:
                            case PRINTF_LENGTH_SHORT:
                            case PRINTF_LENGTH_DEFAULT:     unsigned int ui = va_arg(args, unsigned int);
                                                            if (left_align_width == true) {
                                                                printf_unsigned(ui, radix);
                                                            }
                                                            printf_print_width(ui, width, radix);
                                                            if (left_align_width == false) {
                                                                printf_unsigned(ui, radix);
                                                            }
                                                            break;

                            case PRINTF_LENGTH_LONG:        unsigned long ul = va_arg(args, unsigned long);
                                                            if (left_align_width == true) {
                                                                printf_unsigned(ul, radix);
                                                            }
                                                            printf_print_width(ul, width, radix);
                                                            if (left_align_width == false) {
                                                                printf_unsigned(ul, radix);
                                                            }
                                                            break;

                            case PRINTF_LENGTH_LONG_LONG:   unsigned long long ull = va_arg(args, unsigned long long);
                                                            if (left_align_width == true) {
                                                                printf_unsigned(ull, radix);
                                                            }
                                                            printf_print_width(ull, width, radix);
                                                            if (left_align_width == false) {
                                                                printf_unsigned(ull, radix);
                                                            }
                                                            break;
                        }
                    }
                }

                // reset state
                state = PRINTF_STATE_NORMAL;
                length = PRINTF_LENGTH_DEFAULT;
                width = PRINTF_WIDTH_DEFAULT;
                radix = 10;
                sign = false;
                number = false;
                break;
        }

        fmt++;
    }
    va_end(args);
}


void print_buffer(const char* msg, const void* buffer, uint16_t count)
{
    const uint8_t* u8Buffer = (const uint8_t*)buffer;
    
    puts(msg);
    for (uint16_t i = 0; i < count; i++)
    {
        putc(g_HexChars[u8Buffer[i] >> 4]);
        putc(g_HexChars[u8Buffer[i] & 0xF]);
    }
    puts("\n");
}

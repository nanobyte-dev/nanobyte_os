#include "io.h"

#define UNUSED_PORT         0x80

void i686_iowait()
{
    i686_outb(UNUSED_PORT, 0);
}

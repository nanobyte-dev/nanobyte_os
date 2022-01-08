#include "hal.h"
#include <arch/i686/gdt.h>

void HAL_Initialize()
{
    i686_GDT_Initialize();
}
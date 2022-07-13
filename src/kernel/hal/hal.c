#include "hal.h"
#include <arch/i686/gdt.h>
#include <arch/i686/idt.h>
#include <arch/i686/isr.h>

void HAL_Initialize()
{
    i686_GDT_Initialize();
    i686_IDT_Initialize();
    i686_ISR_Initialize();
}

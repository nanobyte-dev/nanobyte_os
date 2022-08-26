#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include <hal/hal.h>
#include <arch/i686/irq.h>
#include <debug.h>

extern void _init();

void crash_me();

void timer(Registers* regs)
{
    printf(".");
}

void start(uint16_t bootDrive)
{   
    // call global constructors
    _init();

    HAL_Initialize();

    log_debug("Main", "This is a debug msg!");
    log_info("Main", "This is an info msg!");
    log_warn("Main", "This is a warning msg!");
    log_err("Main", "This is an error msg!");
    log_crit("Main", "This is a critical msg!");
    printf("Nanobyte OS v0.1\n");
    printf("This operating system is under construction.\n");
    //i686_IRQ_RegisterHandler(0, timer);

    //crash_me();

end:
    for (;;);
}

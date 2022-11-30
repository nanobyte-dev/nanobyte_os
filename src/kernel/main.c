#include <stdbool.h>
#include <stdint.h>
#include "stdio.h"
#include "memory.h"
#include <hal/hal.h>
#include <arch/i686/irq.h>
#include <debug.h>
#include <boot/bootparams.h>

// For i686_inb/outb.
#include "arch/i686/io.h"

extern void _init();

void crash_me();

void timer(Registers* regs)
{
    printf(".");
}

void keyboard(Registers* regs) {
    uint8_t scancode = i686_inb(0x60);
    
    
    uint8_t al = i686_inb(0x61);    // keyboard control
    uint8_t ah = al;
    al = al | 0x80; // disable bit 7
    i686_outb(0x61, al); // send it back
    al = ah;
    i686_outb(0x61, al); // and send that back
    
    bool released = (scancode & 0x80) != 0; // Was the key released?
    scancode = scancode & 0x7F; // Clip off the key released bit.
    
    if (released) {
        printf("Key released: %d\r\n", scancode);
    } else {
        printf("Key pressed: %d\r\n", scancode);
    }
}

void start(BootParams* bootParams)
{   
    // call global constructors
    _init();

    HAL_Initialize();

    log_debug("Main", "Boot device: %x", bootParams->BootDevice);
    log_debug("Main", "Memory region count: %d", bootParams->Memory.RegionCount);
    for (int i = 0; i < bootParams->Memory.RegionCount; i++) 
    {
        log_debug("Main", "MEM: start=0x%llx length=0x%llx type=%x", 
            bootParams->Memory.Regions[i].Begin,
            bootParams->Memory.Regions[i].Length,
            bootParams->Memory.Regions[i].Type);
    }


    log_info("Main", "This is an info msg!");
    log_warn("Main", "This is a warning msg!");
    log_err("Main", "This is an error msg!");
    log_crit("Main", "This is a critical msg!");
    printf("Nanobyte OS v0.1\n");
    printf("This operating system is under construction.\n");
    i686_IRQ_RegisterHandler(0, timer);
    i686_IRQ_RegisterHandler(1, keyboard);

    //crash_me();

end:
    for (;;);
}

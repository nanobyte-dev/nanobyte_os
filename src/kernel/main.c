#include <stdbool.h>
#include <stdint.h>
#include "ctype.h"
#include "stdio.h"
#include "memory.h"
#include <hal/hal.h>
#include <arch/i686/irq.h>
#include <debug.h>
#include <boot/bootparams.h>

// For i686_inb/outb.
#include "arch/i686/io.h"

#include "arch/i686/i8259.h"

#include "arch/i686/i8042.h"

#include <hal/keyboard.h>

extern void _init();

void crash_me();

void timer(Registers* regs)
{
    printf(".");
}

void ReportPS2Ports() {
    const PS2Driver* driver = i8042_getDriver();
    if (driver->KeyboardPortNumber == -1) {
        printf("...No keyboard detected.\r\n");
    } else {
        printf("...Keyboard detected on PS/2 port %d.\r\n", driver->KeyboardPortNumber);
    }
    if (driver->MousePortNumber == -1) {
        printf("...No mouse detected.\r\n");
    } else {
        printf("...Mouse detected on PS/2 port %d.\r\n", driver->MousePortNumber);
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

    printf("BASIC-OS v0.1, forked from Nanobyte OS v0.1\n");
    printf("This operating system is under construction.\n");
    printf("\n");

    //i686_IRQ_RegisterHandler(0, timer);

    printf("Scanning hardware...\n");
    const PS2Driver* driver = i8042_getDriver();
    Keyboard_initialize();
    ReportPS2Ports();

    printf("\nOK\n");
    printf("> ");

    Keyboard_clearBuffer();
    int lineLength = 0;
    while (true) {
        while (Keyboard_hasKey()) {
            Keyboard_key* key = Keyboard_popKey();
            //printf("[main] key %s: %x, %d\r\n", key->pressed ? "pressed" : "released", key->scancode, key->ascii);
            if (key != NULL) {
                if (key->pressed) {
                    if (key->ascii != 0) {
                        if (key->ascii == '\b') {
                            if (lineLength > 0) {
                                putc(key->ascii);
                                lineLength--;
                            } else {
                                // TODO: Else beep?
                            }
                        } else {
                            putc(key->ascii);
                            lineLength++;
                        }
                        if (key->scancode == 0x1C) { // <Enter>
                            // TODO: Process the line.
                            printf("> "); // Re-print the prompt.
                            lineLength = 0;
                        }
                    }
                }
            }
        }
    }
    //crash_me();

end:
    for (;;);
}

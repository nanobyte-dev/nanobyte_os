#include "irq.h"
#include "pic.h"
#include "io.h"
#include <stddef.h>

#define PIC_REMAP_OFFSET        0x20

IRQHandler g_IRQHandlers[16];

void i686_IRQ_Handler(Registers* regs)
{
    int irq = regs->interrupt - PIC_REMAP_OFFSET;
    
    uint8_t pic_isr = i686_PIC_ReadInServiceRegister();
    uint8_t pic_irr = i686_PIC_ReadIrqRequestRegister();

    if (g_IRQHandlers[irq] != NULL)
    {
        // handle IRQ
        g_IRQHandlers[irq](regs);
    }
    else
    {
        printf("Unhandled IRQ %d  ISR=%x  IRR=%x...\n", irq, pic_isr, pic_irr);
    }

    // send EOI
    i686_PIC_SendEndOfInterrupt(irq);
}

void i686_IRQ_Initialize()
{
    i686_PIC_Configure(PIC_REMAP_OFFSET, PIC_REMAP_OFFSET + 8);

    // register ISR handlers for each of the 16 irq lines
    for (int i = 0; i < 16; i++)
        i686_ISR_RegisterHandler(PIC_REMAP_OFFSET + i, i686_IRQ_Handler);

    // enable interrupts
    i686_EnableInterrupts();
}

void i686_IRQ_RegisterHandler(int irq, IRQHandler handler)
{
    g_IRQHandlers[irq] = handler;
}

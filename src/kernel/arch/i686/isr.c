#include "isr.h"
#include "idt.h"
#include "gdt.h"
#include "io.h"
#include <stdio.h>
#include <stddef.h>
#include <debug.h>

#define MODULE          "ISR"

ISRHandler g_ISRHandlers[256];

static const char* const g_Exceptions[] = {
    "Divide by zero error",
    "Debug",
    "Non-maskable Interrupt",
    "Breakpoint",
    "Overflow",
    "Bound Range Exceeded",
    "Invalid Opcode",
    "Device Not Available",
    "Double Fault",
    "Coprocessor Segment Overrun",
    "Invalid TSS",
    "Segment Not Present",
    "Stack-Segment Fault",
    "General Protection Fault",
    "Page Fault",
    "",
    "x87 Floating-Point Exception",
    "Alignment Check",
    "Machine Check",
    "SIMD Floating-Point Exception",
    "Virtualization Exception",
    "Control Protection Exception ",
    "",
    "",
    "",
    "",
    "",
    "",
    "Hypervisor Injection Exception",
    "VMM Communication Exception",
    "Security Exception",
    ""
};

void i686_ISR_InitializeGates();

void i686_ISR_Initialize()
{
    i686_ISR_InitializeGates();
    for (int i = 0; i < 256; i++)
        i686_IDT_EnableGate(i);

    i686_IDT_DisableGate(0x80);
}

void __attribute__((cdecl)) i686_ISR_Handler(Registers* regs)
{
    if (g_ISRHandlers[regs->interrupt] != NULL)
        g_ISRHandlers[regs->interrupt](regs);

    else if (regs->interrupt >= 32)
        log_err(MODULE, "Unhandled interrupt %d!", regs->interrupt);
    
    else 
    {
        log_crit(MODULE, "Unhandled exception %d %s", regs->interrupt, g_Exceptions[regs->interrupt]);
        
        log_crit(MODULE, "  eax=%x  ebx=%x  ecx=%x  edx=%x  esi=%x  edi=%x",
               regs->eax, regs->ebx, regs->ecx, regs->edx, regs->esi, regs->edi);

        log_crit(MODULE, "  esp=%x  ebp=%x  eip=%x  eflags=%x  cs=%x  ds=%x  ss=%x",
               regs->esp, regs->ebp, regs->eip, regs->eflags, regs->cs, regs->ds, regs->ss);

        log_crit(MODULE, "  interrupt=%x  errorcode=%x", regs->interrupt, regs->error);

        log_crit(MODULE, "KERNEL PANIC!");
        printf("KERNEL PANIC!");

        i686_Panic();
    }
}

void i686_ISR_RegisterHandler(int interrupt, ISRHandler handler)
{
    g_ISRHandlers[interrupt] = handler;
    i686_IDT_EnableGate(interrupt);
}

#include "pic.h"
#include "io.h"

#define PIC1_COMMAND_PORT           0x20
#define PIC1_DATA_PORT              0x21
#define PIC2_COMMAND_PORT           0xA0
#define PIC2_DATA_PORT              0xA1

// Initialization Control Word 1
// -----------------------------
//  0   IC4     if set, the PIC expects to receive ICW4 during initialization
//  1   SGNL    if set, only 1 PIC in the system; if unset, the PIC is cascaded with slave PICs
//              and ICW3 must be sent to controller
//  2   ADI     call address interval, set: 4, not set: 8; ignored on x86, set to 0
//  3   LTIM    if set, operate in level triggered mode; if unset, operate in edge triggered mode
//  4   INIT    set to 1 to initialize PIC
//  5-7         ignored on x86, set to 0

enum {
    PIC_ICW1_ICW4           = 0x01,
    PIC_ICW1_SINGLE         = 0x02,
    PIC_ICW1_INTERVAL4      = 0x04,
    PIC_ICW1_LEVEL          = 0x08,
    PIC_ICW1_INITIALIZE     = 0x10
} PIC_ICW1;


// Initialization Control Word 4
// -----------------------------
//  0   uPM     if set, PIC is in 80x86 mode; if cleared, in MCS-80/85 mode
//  1   AEOI    if set, on last interrupt acknowledge pulse, controller automatically performs 
//              end of interrupt operation
//  2   M/S     only use if BUF is set; if set, selects buffer master; otherwise, selects buffer slave
//  3   BUF     if set, controller operates in buffered mode
//  4   SFNM    specially fully nested mode; used in systems with large number of cascaded controllers
//  5-7         reserved, set to 0
enum {
    PIC_ICW4_8086           = 0x1,
    PIC_ICW4_AUTO_EOI       = 0x2,
    PIC_ICW4_BUFFER_MASTER  = 0x4,
    PIC_ICW4_BUFFER_SLAVE   = 0x0,
    PIC_ICW4_BUFFERRED      = 0x8,
    PIC_ICW4_SFNM           = 0x10,
} PIC_ICW4;


enum {
    PIC_CMD_END_OF_INTERRUPT    = 0x20,
    PIC_CMD_READ_IRR            = 0x0A,
    PIC_CMD_READ_ISR            = 0x0B,
} PIC_CMD;

void i686_PIC_Configure(uint8_t offsetPic1, uint8_t offsetPic2)
{
    // initialization control word 1
    i686_outb(PIC1_COMMAND_PORT, PIC_ICW1_ICW4 | PIC_ICW1_INITIALIZE);
    i686_iowait();
    i686_outb(PIC2_COMMAND_PORT, PIC_ICW1_ICW4 | PIC_ICW1_INITIALIZE);
    i686_iowait();

    // initialization control word 2 - the offsets
    i686_outb(PIC1_DATA_PORT, offsetPic1);
    i686_iowait();
    i686_outb(PIC2_DATA_PORT, offsetPic2);
    i686_iowait();

    // initialization control word 3
    i686_outb(PIC1_DATA_PORT, 0x4);             // tell PIC1 that it has a slave at IRQ2 (0000 0100)
    i686_iowait();
    i686_outb(PIC2_DATA_PORT, 0x2);             // tell PIC2 its cascade identity (0000 0010)
    i686_iowait();

    // initialization control word 4
    i686_outb(PIC1_DATA_PORT, PIC_ICW4_8086);
    i686_iowait();
    i686_outb(PIC2_DATA_PORT, PIC_ICW4_8086);
    i686_iowait();

    // clear data registers
    i686_outb(PIC1_DATA_PORT, 0);
    i686_iowait();
    i686_outb(PIC2_DATA_PORT, 0);
    i686_iowait();
}

void i686_PIC_SendEndOfInterrupt(int irq)
{
    if (irq >= 8)
        i686_outb(PIC2_COMMAND_PORT, PIC_CMD_END_OF_INTERRUPT);
    i686_outb(PIC1_COMMAND_PORT, PIC_CMD_END_OF_INTERRUPT);
}

void i686_PIC_Disable()
{
    i686_outb(PIC1_DATA_PORT, 0xFF);        // mask all
    i686_iowait();
    i686_outb(PIC2_DATA_PORT, 0xFF);        // mask all
    i686_iowait();
}

void i686_PIC_Mask(int irq)
{
    uint8_t port;

    if (irq < 8) 
    {
        port = PIC1_DATA_PORT;
    }
    else
    {
        irq -= 8;
        port = PIC2_DATA_PORT;
    }

    uint8_t mask = i686_inb(PIC1_DATA_PORT);
    i686_outb(PIC1_DATA_PORT,  mask | (1 << irq));
}

void i686_PIC_Unmask(int irq)
{
    uint8_t port;

    if (irq < 8) 
    {
        port = PIC1_DATA_PORT;
    }
    else
    {
        irq -= 8;
        port = PIC2_DATA_PORT;
    }

    uint8_t mask = i686_inb(PIC1_DATA_PORT);
    i686_outb(PIC1_DATA_PORT,  mask & ~(1 << irq));
}

uint16_t i686_PIC_ReadIrqRequestRegister()
{
    i686_outb(PIC1_COMMAND_PORT, PIC_CMD_READ_IRR);
    i686_outb(PIC2_COMMAND_PORT, PIC_CMD_READ_IRR);
    return ((uint16_t)i686_inb(PIC2_COMMAND_PORT)) | (((uint16_t)i686_inb(PIC2_COMMAND_PORT)) << 8);
}

uint16_t i686_PIC_ReadInServiceRegister()
{
    i686_outb(PIC1_COMMAND_PORT, PIC_CMD_READ_ISR);
    i686_outb(PIC2_COMMAND_PORT, PIC_CMD_READ_ISR);
    return ((uint16_t)i686_inb(PIC2_COMMAND_PORT)) | (((uint16_t)i686_inb(PIC2_COMMAND_PORT)) << 8);
}
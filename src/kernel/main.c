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

extern void _init();

void crash_me();

void timer(Registers* regs)
{
    printf(".");
}

#define PS2_DATA_PORT 0x60
#define PS2_REGISTER 0x64                                                       // Read for status, write for command.

/*
 * The Status Register contains various flags that show the state of the PS/2 controller. The meanings for each bit are:
 *
 * Bit	Meaning
 * 0	Output buffer status (0 = empty, 1 = full)
 *      (must be set before attempting to read data from IO port 0x60)
 * 
 * 1	Input buffer status (0 = empty, 1 = full)
 *      (must be clear before attempting to write data to IO port 0x60 or IO port 0x64)
 * 
 * 2	System Flag
 *      Meant to be cleared on reset and set by firmware (via. PS/2 Controller Configuration Byte) if the system passes self tests (POST)
 * 
 * 3	Command/data (0 = data written to input buffer is data for PS/2 device, 1 = data written to input buffer is data for PS/2 controller command)
 * 4	Unknown (chipset specific)
 *      May be "keyboard lock" (more likely unused on modern systems)
 * 
 * 5	Unknown (chipset specific)
 *      May be "receive time-out" or "second PS/2 port output buffer full"
 * 
 * 6	Time-out error (0 = no error, 1 = time-out error)
 * 7	Parity error (0 = no error, 1 = parity error)
 */
enum {
    PS2_STATUS_OUTPUT_BUFFER    = 0x01,
    PS2_STATUS_INPUT_BUFFER     = 0x02,
    PS2_STATUS_SYSTEM_FLAG      = 0x04,
    PS2_STATUS_COMMAND          = 0x08,
    PS2_STATUS_UNKNOWN0         = 0x10,
    PS2_STATUS_UNKNOWN1         = 0x20,
    PS2_STATUS_ERROR_TIMEOUT    = 0x40,
    PS2_STATUS_ERROR_PARITY     = 0x80
} PS2_STATUS;

enum {
    PS2_RESPONSE_ACK    = 0xFA,
    PS2_RESPONSE_RESEND = 0xF3
} PS2_RESPONSE;

enum {
    PS2_COMMAND_DISABLE_FIRST_PORT = 0xAD,
    PS2_COMMAND_ENABLE_FIRST_PORT = 0xAE,
    PS2_COMMAND_SET_LED = 0xED,
    PS2_COMMAND_SCANCODE_SET = 0xF0, // get/set
    PS2_COMMAND_SET_TYPEMATIC_RATE_AND_DELAY = 0xF3
} PS2_COMMAND;

enum {
    PS2_SCANCODE_SET_1 = 0x43,
    PS2_SCANCODE_SET_2 = 0x41,
    PS2_SCANCODE_SET_3 = 0x3f
} PS2_SCANCODE_SET;

bool Keyboard_keyStates[128] = { false };

char kbd_US_lcase[128] = {
    0,  27, '1', '2', '3', '4', '5', '6', '7', '8', '9', '0', '-', '=', '\b',   
  '\t', /* <-- Tab */
  'q', 'w', 'e', 'r', 't', 'y', 'u', 'i', 'o', 'p', '[', ']', '\n',     
    0, /* <-- control key */
  'a', 's', 'd', 'f', 'g', 'h', 'j', 'k', 'l', ';', '\'', '`',  0, '\\', 'z', 'x', 'c', 'v', 'b', 'n', 'm', ',', '.', '/',   0,
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

char kbd_US_ucase[128] = {
    0,  27, '!', '@', '#', '$', '%', '^', '&', '*', '(', ')', '_', '+', '\b',   
  '\t', /* <-- Tab */
  'Q', 'W', 'E', 'R', 'T', 'Y', 'U', 'I', 'O', 'P', '{', '}', '\n',     
    0, /* <-- control key */
  'A', 'S', 'D', 'F', 'G', 'H', 'J', 'K', 'L', ':', '"', '~',  0, '|', 'Z', 'X', 'C', 'V', 'B', 'N', 'M', '<', '>', '?',   0,
  '*',
    0,  /* Alt */
  ' ',  /* Space bar */
    0,  /* Caps lock */
    0,  /* 59 - F1 key ... > */
    0,   0,   0,   0,   0,   0,   0,   0,
    0,  /* < ... F10 */
    0,  /* 69 - Num lock*/
    0,  /* Scroll Lock */
    0,  /* Home key */
    0,  /* Up Arrow */
    0,  /* Page Up */
  '-',
    0,  /* Left Arrow */
    0,
    0,  /* Right Arrow */
  '+',
    0,  /* 79 - End key*/
    0,  /* Down Arrow */
    0,  /* Page Down */
    0,  /* Insert Key */
    0,  /* Delete Key */
    0,   0,   0,
    0,  /* F11 Key */
    0,  /* F12 Key */
    0,  /* All other keys are undefined */
};

typedef struct {
    uint8_t scancode;
    bool pressed;
    char ascii;
} Keyboard_key;

Keyboard_key g_LastKeyEvent;

void Keyboard_waitForBufferEmpty() {
    // Wait until the 8042 is done processing the current command.
    uint16_t count = 0xFFFF;
    do {
        uint8_t status = i686_inb(PS2_REGISTER);
        if ((status & PS2_STATUS_INPUT_BUFFER) != 0) {
            break;
        }
    } while (count--);
}

/*
 * Send the given command to the 8042 keyboard controller.
 *
 * command: The command byte to send.
 */
void Keyboard_setCommand(uint8_t command) {
    i686_DisableInterrupts();

    Keyboard_waitForBufferEmpty();

    // Send the command.
    i686_outb(PS2_REGISTER, command);

    i686_EnableInterrupts();
}

void Keyboard_sendCommand(uint8_t command) {
    // Disable the keyboard.
    Keyboard_setCommand(PS2_COMMAND_DISABLE_FIRST_PORT);

    i686_DisableInterrupts();

    // Wait until the 8042 is done processing the current command.
    Keyboard_waitForBufferEmpty();

    // Enable the keyboard.
    Keyboard_setCommand(PS2_COMMAND_ENABLE_FIRST_PORT);
    
    // Send the data to port 0x60.
    i686_outb(PS2_DATA_PORT, command);

    i686_EnableInterrupts();
}

/**
 * Set the value of the 3 LEDs.
 * 
 * scrollLock: Turn on the Scroll Lock LED.
 * numLock: Turn on the Num Lock LED.
 * capsLock: Turn on the Caps Lock LED.
 */
void Keyboard_setLEDs(bool scrollLock, bool numLock, bool capsLock) {
    Keyboard_sendCommand(PS2_COMMAND_SET_LED);

    // Bit values 0..2 = scroll, num, caps.
    uint8_t value = (scrollLock ? 0x1 : 0x0) + (numLock ? 0x2 : 0x0) + (capsLock ? 0x4 : 0x0);
    Keyboard_sendCommand(value);
}

/**
 * Set how fast keys are repeated, and how long before key repeats begin.
 * 
 * repeatRate: 5-bit number (00000b = 30 Hz, ..., 11111b = 2 Hz)
 * repeatDelay: delay before keys repeat, 2-bit number (00b = 250 ms, 01b = 500 ms, 10b = 750 ms, 11b = 1000 ms)
 */
void Keyboard_setTypematicRateAndDelay(uint8_t repeatRate, uint8_t repeatDelay) {
    uint8_t portValue = (repeatRate & 0x1F) + ((repeatDelay & 0x03) << 5);
    Keyboard_sendCommand(PS2_COMMAND_SET_TYPEMATIC_RATE_AND_DELAY);
    Keyboard_sendCommand(portValue);
}

int Keyboard_getScancodeSet() {
    do {
        Keyboard_sendCommand(PS2_COMMAND_SCANCODE_SET); // Get/set current scancode set.
        Keyboard_sendCommand(0); // Get scancode set.
    } while (i686_inb(PS2_DATA_PORT) == PS2_RESPONSE_RESEND);

    uint8_t set = i686_inb(PS2_DATA_PORT);
    switch (set) {
        case PS2_SCANCODE_SET_1:
            return 1;
        case PS2_SCANCODE_SET_3:
            return 3;
        case PS2_SCANCODE_SET_2:
        default:
            return 2;
    }
}

/*
 * setNumber: Scancode sets 1-3.
 */
void Keyboard_setScancodeSet(uint8_t setNumber) {
    if (Keyboard_getScancodeSet() == setNumber) {
        return;
    }
    
    do {
        Keyboard_sendCommand(PS2_COMMAND_SCANCODE_SET); // Get/set current scancode set.
        Keyboard_sendCommand(setNumber); // Set scancode set.
    } while (i686_inb(PS2_DATA_PORT) == PS2_RESPONSE_RESEND);
}

bool Keyboard_isKeyPressed(uint8_t scancode) {
    return Keyboard_keyStates[scancode];
}

bool Keyboard_isShiftPressed() {
    return Keyboard_isKeyPressed(0x2a) || Keyboard_isKeyPressed(0x36);
}

/**
 * Process the active key event.
 */
Keyboard_key Keyboard_processKey() {
    Keyboard_key k;

    k.scancode = i686_inb(PS2_DATA_PORT);

    // The high bit from the keyboard indicates if the key was pressed or released.
    k.pressed = (k.scancode & 0x80) == 0;

    // The remaining 7 bits tell us the scancode.
    k.scancode = k.scancode & 0x7F;

    if (Keyboard_isShiftPressed()) {
        k.ascii = kbd_US_ucase[k.scancode];
    } else {
        k.ascii = kbd_US_lcase[k.scancode];
    }
    
    return k;
}

void Keyboard_irqHandler(Registers* regs) {
    g_LastKeyEvent = Keyboard_processKey();

    Keyboard_keyStates[g_LastKeyEvent.scancode] = g_LastKeyEvent.pressed;

    if (g_LastKeyEvent.scancode != 0) {
        //printf("Key %s: %x, %d\r\n", k.pressed ? "pressed" : "released", k.scancode, k.ascii);
        if (g_LastKeyEvent.pressed) {
            if (g_LastKeyEvent.ascii != 0) {
                putc(g_LastKeyEvent.ascii);
            }
        }
    }
}

/**
 * Return the active key event.
 * After reading the event, g_LastKeyEvent.scancode will be reset to 0.
 */
/*
Keyboard_key Keyboard_readKey() {
    Keyboard_key nextKey;
    nextKey.scancode = g_LastKeyEvent.scancode;
    nextKey.ascii = g_LastKeyEvent.ascii;
    nextKey.pressed = g_LastKeyEvent.pressed;
    g_LastKeyEvent.scancode = 0;
    g_LastKeyEvent.ascii = 0;
    g_LastKeyEvent.pressed = false;
    return nextKey;
}
*/

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

    //i686_IRQ_RegisterHandler(0, timer);

    Keyboard_sendCommand(PS2_COMMAND_DISABLE_FIRST_PORT);
    //Keyboard_etLEDs(true, true, true); // Doesn't work in QEMU?
    //Keyboard_setTypematicRateAndDelay(0x1F, 0);
    Keyboard_setScancodeSet(2); // Just to be sure.
    i686_IRQ_RegisterHandler(1, Keyboard_irqHandler);

    while (true) {
        /*
        Keyboard_key k = Keyboard_readKey();
        if (k.scancode != 0) {
            //printf("Key %s: %x, %d\r\n", k.pressed ? "pressed" : "released", k.scancode, k.ascii);
            if (k.ascii != 0) {
                putc(k.ascii);
            }
        }
        */
    }
    //crash_me();

end:
    for (;;);
}

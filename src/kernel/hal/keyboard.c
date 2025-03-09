#include "keyboard.h"
#include <arch/i686/i8042.h>

// For i686_inb/outb.
#include <arch/i686/io.h>

#include <arch/i686/irq.h>

#include <stdbool.h>
#include <stdint.h>
#include <stddef.h>

bool Keyboard_keyStates[128] = { false };

static char kbd_US_lcase[128] = {
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

static char kbd_US_ucase[128] = {
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

static uint8_t g_PS2PortKeyboard = 0xFF;

#define SIZE_KEYBUFFER 32
static Keyboard_key g_KeyBuffer[SIZE_KEYBUFFER];
static int g_KeyBufferIndex = SIZE_KEYBUFFER;

/**
 * Set the value of the 3 LEDs.
 * 
 * scrollLock: Turn on the Scroll Lock LED.
 * numLock: Turn on the Num Lock LED.
 * capsLock: Turn on the Caps Lock LED.
 */
void Keyboard_setLEDs(bool scrollLock, bool numLock, bool capsLock) {
    i8042_sendCommand(PS2_COMMAND_SET_LED, g_PS2PortKeyboard);

    // Bit values 0..2 = scroll, num, caps.
    uint8_t value = (scrollLock ? 0x1 : 0x0) + (numLock ? 0x2 : 0x0) + (capsLock ? 0x4 : 0x0);
    i8042_sendCommand(value, g_PS2PortKeyboard);
}

/**
 * Set how fast keys are repeated, and how long before key repeats begin.
 * 
 * repeatRate: 5-bit number (00000b = 30 Hz, ..., 11111b = 2 Hz)
 * repeatDelay: delay before keys repeat, 2-bit number (00b = 250 ms, 01b = 500 ms, 10b = 750 ms, 11b = 1000 ms)
 */
void Keyboard_setTypematicRateAndDelay(uint8_t repeatRate, uint8_t repeatDelay) {
    uint8_t portValue = (repeatRate & 0x1F) + ((repeatDelay & 0x03) << 5);
    i8042_sendCommand(PS2_COMMAND_SET_TYPEMATIC_RATE_AND_DELAY, g_PS2PortKeyboard);
    i8042_sendCommand(portValue, g_PS2PortKeyboard);
}

int Keyboard_getScancodeSet() {
    do {
        i8042_sendCommand(PS2_COMMAND_SCANCODE_SET, g_PS2PortKeyboard); // Get/set current scancode set.
        i8042_sendCommand(0, g_PS2PortKeyboard); // Get scancode set.
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
    i8042_disableScanning(g_PS2PortKeyboard);

    if (Keyboard_getScancodeSet() == setNumber) {
        return;
    }
    
    do {
        i8042_sendCommand(PS2_COMMAND_SCANCODE_SET, g_PS2PortKeyboard); // Get/set current scancode set.
        i8042_sendCommand(setNumber, g_PS2PortKeyboard); // Set scancode set.
    } while (i686_inb(PS2_DATA_PORT) == PS2_RESPONSE_RESEND);

    i8042_enableScanning(g_PS2PortKeyboard);
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

void Keyboard_setKey(int index, Keyboard_key* key) {
    g_KeyBuffer[g_KeyBufferIndex].ascii = key->ascii;
    g_KeyBuffer[g_KeyBufferIndex].pressed = key->pressed;
    g_KeyBuffer[g_KeyBufferIndex].scancode = key->scancode;
}

void Keyboard_pushKey(Keyboard_key* key) {
    if (g_KeyBufferIndex > 0) {
        //printf("[Keyboard_pushKey] key %s: %x, %d\r\n", key->pressed ? "pressed" : "released", key->scancode, key->ascii);
        Keyboard_setKey(g_KeyBufferIndex--, key);
    }
}

Keyboard_key* Keyboard_popKey() {
    if (g_KeyBufferIndex < SIZE_KEYBUFFER) {
        return &g_KeyBuffer[g_KeyBufferIndex++];
    } else {
        return NULL;
    }
}

bool Keyboard_hasKey() {
    return g_KeyBufferIndex < SIZE_KEYBUFFER;
}

bool Keyboard_isBufferFull() {
    return g_KeyBufferIndex == 0;
}

void Keyboard_clearBuffer() {
    while (Keyboard_hasKey()) {
        Keyboard_popKey();
    }
}

void Keyboard_irqHandler(Registers* regs) {
    if (Keyboard_isBufferFull()) {
        return;
    }

    Keyboard_key key = Keyboard_processKey();

    Keyboard_keyStates[key.scancode] = key.pressed;

    if (key.scancode != 0) {
       Keyboard_pushKey(&key);
    }
}

void Keyboard_initialize() {
    g_PS2PortKeyboard = i8042_getDriver()->KeyboardPortNumber;
    //Keyboard_etLEDs(true, true, true); // Doesn't work in QEMU?
    //Keyboard_setTypematicRateAndDelay(0x1F, 0);
    Keyboard_setScancodeSet(1); // Just to be sure.
    i686_IRQ_RegisterHandler(1, Keyboard_irqHandler);
    Keyboard_clearBuffer();
}

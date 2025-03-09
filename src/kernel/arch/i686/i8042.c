#include "i8042.h"

// For i686_inb/outb.
#include "arch/i686/io.h"

static PS2Driver g_PS2Driver = {
    .Name = "8042 PS/2",
    .KeyboardPortNumber = -1,
    .MousePortNumber = -1
};

/**
 * Wait until the 8042 is done processing the current command.
 */
void i8042_waitForBufferEmpty() {
    uint16_t count = 0xFFFF;
    do {
        uint8_t status = i686_inb(PS2_REGISTER);
        if ((status & PS2_STATUS_INPUT_BUFFER) != 0) {
            break;
        }
    } while (count--);
}

/*
 * Send the given command to the 8042 PS/2 controller.
 *
 * command: The command byte to send.
 */
void i8042_setCommand(uint8_t command) {
    i686_DisableInterrupts();

    i8042_waitForBufferEmpty();

    // Send the command.
    i686_outb(PS2_REGISTER, command);

    i686_EnableInterrupts();
}

/**
 * portNumber: [0..1] to specify the first or second PS/2 port.
 */
void i8042_sendCommand(uint8_t command, int portNumber) {
    // Disable the port.
    i8042_setCommand((portNumber == 0) ? PS2_COMMAND_DISABLE_FIRST_PORT : PS2_COMMAND_DISABLE_SECOND_PORT);

    i686_DisableInterrupts();

    // Wait until the 8042 is done processing the current command.
    i8042_waitForBufferEmpty();

    if (portNumber != 0) {
        i8042_setCommand(PS2_COMMAND_SELECT_PORT2); // Address the second PS/2 port.
    }

    // Enable the keyboard.
    i8042_setCommand((portNumber == 0) ? PS2_COMMAND_ENABLE_FIRST_PORT : PS2_COMMAND_ENABLE_SECOND_PORT);
    
    // Send the data to port 0x60.
    i686_outb(PS2_DATA_PORT, command);

    i686_EnableInterrupts();
}

void i8042_disableScanning(int portNumber) {
    do {
        i8042_sendCommand(PS2_COMMAND_DISABLE_SCANNING, portNumber); // Disable Scanning
    } while (i686_inb(PS2_DATA_PORT) == PS2_RESPONSE_RESEND);
}

void i8042_enableScanning(int portNumber) {
    do {
        i8042_sendCommand(PS2_COMMAND_ENABLE_SCANNING, portNumber); // Enable Scanning
    } while (i686_inb(PS2_DATA_PORT) == PS2_RESPONSE_RESEND);
}

/*
 * Inputs:
 *  portNumber: [0, 1] for the first or second PS/2 ports respectively. 
 */
PS2_DEVICE_TYPE i8042_getDeviceType(uint8_t portNumber) {
    i8042_disableScanning(portNumber);
    i8042_waitForBufferEmpty();
    do {
        i8042_sendCommand(PS2_COMMAND_IDENTIFY_HARDWARE, portNumber); // Identify
    } while (i686_inb(PS2_DATA_PORT) == PS2_RESPONSE_RESEND);

    PS2_DEVICE_TYPE type = PS2_DEVICE_TYPE_KEYBOARD_AT;
    for (int n = 0; n < 16; n++) {
        uint8_t response = i686_inb(PS2_DATA_PORT);
        switch (response) {
            case 0x00:
                type = PS2_DEVICE_TYPE_MOUSE_STANDARD;
                break;
            case 0x03:
                type = PS2_DEVICE_TYPE_MOUSE_WHEEL;
                break;
            case 0x04:
                type = PS2_DEVICE_TYPE_MOUSE_5BUTTON;
                break;
            case 0x41:
            case 0xC1:
                type = PS2_DEVICE_TYPE_KEYBOARD_MF2_TRANSLATED;
                break;
            case 0x83:
                type = PS2_DEVICE_TYPE_KEYBOARD_MF2;
                break;
        }
        if (type != PS2_DEVICE_TYPE_KEYBOARD_AT) {
            break;
        }
    }
    
    i8042_enableScanning(portNumber);
    return type;
}

PS2_DEVICE_CATEGORY i8042_getDeviceCategory(uint8_t portNumber) {
    switch (i8042_getDeviceType(portNumber)) {
        case PS2_DEVICE_TYPE_MOUSE_STANDARD:
        case PS2_DEVICE_TYPE_MOUSE_WHEEL:
        case PS2_DEVICE_TYPE_MOUSE_5BUTTON:
            return PS2_DEVICE_CATEGORY_MOUSE;
        default:
        case PS2_DEVICE_TYPE_KEYBOARD_AT:
        case PS2_DEVICE_TYPE_KEYBOARD_MF2:
        case PS2_DEVICE_TYPE_KEYBOARD_MF2_TRANSLATED:
            return PS2_DEVICE_CATEGORY_KEYBOARD;
    }
}

void i8042_Initialize() {
    g_PS2Driver.KeyboardPortNumber = -1;
    g_PS2Driver.MousePortNumber = -1;

    switch (i8042_getDeviceCategory(0)) {
        case PS2_DEVICE_CATEGORY_KEYBOARD:
            g_PS2Driver.KeyboardPortNumber = 0;
            break;
        case PS2_DEVICE_CATEGORY_MOUSE:
            g_PS2Driver.MousePortNumber = 0;
            break;
    }

    switch (i8042_getDeviceCategory(1)) {
        case PS2_DEVICE_CATEGORY_KEYBOARD:
            g_PS2Driver.KeyboardPortNumber = 1;
            break;
        case PS2_DEVICE_CATEGORY_MOUSE:
            g_PS2Driver.MousePortNumber = 1;
            break;
    }
}

const PS2Driver* i8042_getDriver() {
    return &g_PS2Driver;  
}
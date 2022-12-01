#pragma once

#include <stdint.h>
#include "ps2.h"

#define PS2_DATA_PORT 0x60
#define PS2_REGISTER 0x64                                                       // Read for status, write for command.

/*
 * Byte/s	                Device Type
 * None	                    Ancient AT keyboard with translation enabled in the PS/Controller (not possible for the second PS/2 port)
 * 0x00	                    Standard PS/2 mouse
 * 0x03	                    Mouse with scroll wheel
 * 0x04	                    5-button mouse
 * 0xAB, 0x41 or 0xAB, 0xC1 MF2 keyboard with translation enabled in the PS/Controller (not possible for the second PS/2 port)
 * 0xAB, 0x83	            MF2 keyboard
 */
typedef enum {
    PS2_DEVICE_TYPE_KEYBOARD_AT,
    PS2_DEVICE_TYPE_MOUSE_STANDARD,
    PS2_DEVICE_TYPE_MOUSE_WHEEL,
    PS2_DEVICE_TYPE_MOUSE_5BUTTON,
    PS2_DEVICE_TYPE_KEYBOARD_MF2_TRANSLATED,
    PS2_DEVICE_TYPE_KEYBOARD_MF2
} PS2_DEVICE_TYPE;

typedef enum {
    PS2_DEVICE_CATEGORY_MOUSE,
    PS2_DEVICE_CATEGORY_KEYBOARD
} PS2_DEVICE_CATEGORY;

typedef enum {
    PS2_COMMAND_DISABLE_FIRST_PORT = 0xAD,
    PS2_COMMAND_ENABLE_FIRST_PORT = 0xAE,
    PS2_COMMAND_DISABLE_SECOND_PORT = 0xA7,
    PS2_COMMAND_ENABLE_SECOND_PORT = 0xA8,
    PS2_COMMAND_SELECT_PORT2 = 0xD4,
    PS2_COMMAND_SET_LED = 0xED,
    PS2_COMMAND_SCANCODE_SET = 0xF0, // get/set
    PS2_COMMAND_IDENTIFY_HARDWARE = 0xF2,
    PS2_COMMAND_SET_TYPEMATIC_RATE_AND_DELAY = 0xF3,
    PS2_COMMAND_ENABLE_SCANNING = 0xF4,
    PS2_COMMAND_DISABLE_SCANNING = 0xF5
} PS2_COMMAND;

typedef enum {
    PS2_SCANCODE_SET_1 = 0x43,
    PS2_SCANCODE_SET_2 = 0x41,
    PS2_SCANCODE_SET_3 = 0x3f
} PS2_SCANCODE_SET;

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
typedef enum {
    PS2_STATUS_OUTPUT_BUFFER    = 0x01,
    PS2_STATUS_INPUT_BUFFER     = 0x02,
    PS2_STATUS_SYSTEM_FLAG      = 0x04,
    PS2_STATUS_COMMAND          = 0x08,
    PS2_STATUS_UNKNOWN0         = 0x10,
    PS2_STATUS_UNKNOWN1         = 0x20,
    PS2_STATUS_ERROR_TIMEOUT    = 0x40,
    PS2_STATUS_ERROR_PARITY     = 0x80
} PS2_STATUS;

typedef enum {
    PS2_RESPONSE_ACK    = 0xFA,
    PS2_RESPONSE_RESEND = 0xF3
} PS2_RESPONSE;

void i8042_waitForBufferEmpty();
void i8042_setCommand(uint8_t command);
void i8042_sendCommand(uint8_t command, int portNumber);
void i8042_disableScanning(int portNumber);
void i8042_enableScanning(int portNumber);
PS2_DEVICE_TYPE i8042_getDeviceType(uint8_t portNumber);
PS2_DEVICE_CATEGORY i8042_getDeviceCategory(uint8_t portNumber);
void i8042_Initialize();
const PS2Driver* i8042_getDriver();
